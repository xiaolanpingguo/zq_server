#pragma once


#include "interface_header/platform.h"
#include <string>
#include <vector>
#include <unordered_map>

#include "io_service_pool.hpp"
#include "connection.hpp"

namespace zq {


template<class service_pool_policy = io_service_pool>
class http_server_ 
{
	using HttpHandlersT = std::unordered_map<std::string, http_handler>;
public:
	template<class... Args>
	explicit http_server_(Args&&... args) : io_service_pool_(std::forward<Args>(args)...)
#ifdef ZQ_ENABLE_SSL
		, ctx_(boost::asio::ssl::context::sslv23)
#endif
	{
		init_conn_callback();
	}

	template<typename F>
	void init_ssl_context(bool ssl_enable_v3, F&& f, std::string certificate_chain_file,
		std::string private_key_file, std::string tmp_dh_file) 
	{
#ifdef ZQ_ENABLE_SSL
		unsigned long ssl_options = boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::single_dh_use;

		if (!ssl_enable_v3)
			ssl_options |= boost::asio::ssl::context::no_sslv3;

		ctx_.set_options(ssl_options);
		ctx_.set_password_callback(std::forward<F>(f));
		ctx_.use_certificate_chain_file(std::move(certificate_chain_file));
		ctx_.use_private_key_file(std::move(private_key_file), boost::asio::ssl::context::pem);
		ctx_.use_tmp_dh_file(std::move(tmp_dh_file));
#endif
	}

	// address :
	// "0.0.0.0" : ipv4. use 'https://localhost/' to visit
	// "::1" : ipv6. use 'https://[::1]/' to visit
	// "" : ipv4 & ipv6.
	bool listen(const std::string& address, const std::string& port) 
	{
		boost::asio::ip::tcp::resolver::query query(address.data(), port.data());
		return listen(query);
	}

	//support ipv6 & ipv4
	bool listen(const std::string& port)
	{
		boost::asio::ip::tcp::resolver::query query(port.data());
		return listen(query);
	}

	bool listen(const boost::asio::ip::tcp::resolver::query& query) 
	{
		boost::asio::ip::tcp::resolver resolver(io_service_pool_.get_io_service());
		boost::asio::ip::tcp::resolver::iterator endpoints = resolver.resolve(query);

		bool r = false;

		for (; endpoints != boost::asio::ip::tcp::resolver::iterator(); ++endpoints) 
		{
			boost::asio::ip::tcp::endpoint endpoint = *endpoints;

			auto acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(io_service_pool_.get_io_service());
			acceptor->open(endpoint.protocol());
			acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

			try 
			{
				acceptor->bind(endpoint);
				acceptor->listen();
				start_accept(acceptor);
				r = true;
			}
			catch (const std::exception& e) 
			{
				LOG_INFO << e.what();
			}
		}

		return r;
	}

	void stop() 
	{
		io_service_pool_.stop();
	}

	void run() 
	{
		io_service_pool_.run();
	}

	intptr_t run_one() 
	{
		return io_service_pool_.run_one();
	}

	intptr_t poll() 
	{
		return io_service_pool_.poll();
	}

	intptr_t poll_one() 
	{
		return io_service_pool_.poll_one();
	}

	void set_static_dir(std::string&& path) 
	{
		static_dir_ = public_root_path_ + std::move(path)+"/";
	}

	const std::string& static_dir() const 
	{
		return static_dir_;
	}

	void set_max_req_buf_size(std::size_t max_buf_size) 
	{
		max_req_buf_size_ = max_buf_size;
	}

	void set_keep_alive_timeout(long seconds) 
	{
		keep_alive_timeout_ = seconds;
	}

	void set_http_handler(http_method method, const std::string& name, http_handler&& fun)
	{
		httpHandlers_[method][name] = std::move(fun);
	}

	void set_http_error_handler(http_handler&& fun)
	{
		error_handler_ = std::move(fun);
	}

	void set_public_root_directory(const std::string& name)
    {
        if(!name.empty())
		{
			public_root_path_ = "./"+name+"/";
        }
    }

    std::string get_public_root_directory()
    {
        return public_root_path_;
    }

private:
	void start_accept(std::shared_ptr<boost::asio::ip::tcp::acceptor> const& acceptor) 
	{
		auto new_conn = std::make_shared<connection<SocketT>>(
			io_service_pool_.get_io_service(), max_req_buf_size_, keep_alive_timeout_, http_handler_, static_dir_
#ifdef ZQ_ENABLE_SSL
			, ctx_
#endif
			);
		acceptor->async_accept(new_conn->socket(), [this, new_conn, acceptor](const boost::system::error_code& e) 
		{
			if (!e) 
			{
				new_conn->socket().set_option(boost::asio::ip::tcp::no_delay(true));
				new_conn->start();
			}
			else 
			{
				LOG_INFO << "server::handle_accept: " << e.message();
			}

			start_accept(acceptor);
		});
	}

	void init_conn_callback() 
	{
		http_handler_ = [this](request& req, response& res) 
		{
            res.set_url(req.get_url());

			auto method_str = req.get_method();
			const auto&	method = getMethodByStr(method_str);
			auto it_map = httpHandlers_.find(method);
			if (it_map == httpHandlers_.end())
			{
				res.set_status_and_content(status_type::bad_request, "the url is not right");
			}
			else
			{
				const auto& invoke_map = it_map->second;
				auto it = invoke_map.find(req.get_url());
				if (it != invoke_map.end())
				{
					(it->second)(req, res);
				}
				else
				{
					if (error_handler_)
					{
						(error_handler_)(req, res);
					}
					else
					{
						res.set_status_and_content(status_type::bad_request, "the url is not right");
					}
				}
			}
		};
	}

	service_pool_policy io_service_pool_;

	std::size_t max_req_buf_size_ = 3 * 1024 * 1024; //max request buffer size 3M
	long keep_alive_timeout_ = 60; //max request timeout 60s

	std::string static_dir_ = "./public/static/"; //default
    std::string base_path_[2] = {"base_path","/"};
    std::time_t static_res_cache_max_age_ = 0;
    std::string public_root_path_ = "./public/";

#ifdef ZQ_ENABLE_SSL
	boost::asio::ssl::context ctx_;
#endif

	http_handler http_handler_ = nullptr;
	http_handler error_handler_ = nullptr;

	std::map<http_method, HttpHandlersT> httpHandlers_;
};

using http_server_pool = http_server_<io_service_pool>;
using http_server_single_thread = http_server_<io_service_single_thread>;

}
