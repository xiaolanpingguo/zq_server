#pragma once

#include "utility.hpp"
#include <limits>
#include <mutex>
#include <random>
#include <unordered_set>
#include <vector>

#ifdef USE_STANDALONE_ASIO

#include <asio.hpp>
#include <asio/steady_timer.hpp>
namespace zq 
{
	using error_code = std::error_code;
	using errc = std::errc;
	using system_error = std::system_error;
	namespace make_error_code = std;
}
#else
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
namespace zq {

	namespace asio = boost::asio;
	using error_code = boost::system::error_code;
	namespace errc = boost::system::errc;
	using system_error = boost::system::system_error;
	namespace make_error_code = boost::system::errc;

}
#endif


namespace zq {

template <class socket_type>
class Client;

template <class socket_type>
class ClientBase
{
public:
	class Content : public std::istream
	{
		friend class ClientBase<socket_type>;

	public:
		std::size_t size() noexcept
		{
			return streambuf_.size();
		}
		// Convenience function to return std::string. The stream buffer is consumed.
		std::string string() noexcept
		{
			try
			{
				std::string str;
				auto size = streambuf_.size();
				str.resize(size);
				read(&str[0], static_cast<std::streamsize>(size));
				return str;
			}
			catch (...)
			{
				return std::string();
			}
		}

	private:
		asio::streambuf& streambuf_;
		Content(asio::streambuf& streambuf) noexcept 
			:
			std::istream(&streambuf),
			streambuf_(streambuf)
		{}
	};

	class Response
	{
		friend class ClientBase<socket_type>;
		friend class Client<socket_type>;

		Response(std::size_t max_response_streambuf_size) noexcept 
			: streambuf_(max_response_streambuf_size), 
			content_(streambuf_)
		{}

	public:

		std::string http_version_;
		std::string status_code_;
		Content content_;
		CaseInsensitiveMultimap header_;

	private:
		asio::streambuf streambuf_;
	};

	class Config
	{
		friend class ClientBase<socket_type>;

	private:
		Config() noexcept {}

	public:
		// Set timeout on requests in seconds. Default value: 0 (no timeout).
		long timeout = 0;

		// Set connect timeout in seconds. Default value: 0 (Config::timeout is then used instead).
		long timeout_connect = 0;

		// Maximum size of response stream buffer. Defaults to architecture maximum.
		// Reaching this limit will result in a message_size error code.
		std::size_t max_response_streambuf_size = std::numeric_limits<std::size_t>::max();

		// Set proxy server (server:port)
		std::string proxy_server;
	};

protected:
	class Connection : public std::enable_shared_from_this<Connection>
	{
	public:
		template <typename... Args>
		Connection(std::shared_ptr<ScopeRunner> handler_runner, long timeout, Args &&... args) noexcept
			: handler_runner_(std::move(handler_runner)), 
			timeout_(timeout), 
			socket_(new socket_type(std::forward<Args>(args)...)) 
		{}

		void set_timeout(long seconds = 0) noexcept
		{
			if (seconds == 0)
				seconds = timeout;

			if (seconds == 0)
			{
				timer_ = nullptr;
				return;
			}

			timer_ = std::unique_ptr<asio::steady_timer>(new asio::steady_timer(socket_->get_io_service()));
			timer_->expires_from_now(std::chrono::seconds(seconds));
			auto self = this->shared_from_this();
			timer_->async_wait([self](const error_code& ec)
			{
				if (!ec)
				{
					error_code ec;
					self->socket->lowest_layer().cancel(ec);
				}
			});
		}

		void cancel_timeout() noexcept
		{
			if (timer_)
			{
				error_code ec;
				timer_->cancel(ec);
			}
		}

		std::shared_ptr<ScopeRunner> handler_runner_;
		long timeout_;

		// Socket must be unique_ptr since asio::ssl::stream<asio::ip::tcp::socket> is not movable
		std::unique_ptr<socket_type> socket_; 
		bool in_use_ = false;
		bool attempt_reconnect_ = true;

		std::unique_ptr<asio::steady_timer> timer_;
	};

	class Session
	{
	public:
		Session(std::size_t max_response_streambuf_size, std::shared_ptr<Connection> connection, 
			std::unique_ptr<asio::streambuf> request_streambuf) noexcept
			: connection_(std::move(connection)),
			request_streambuf_(std::move(request_streambuf)),
			response_(new Response(max_response_streambuf_size)) {}

		std::shared_ptr<Connection> connection_;
		std::unique_ptr<asio::streambuf> request_streambuf_;
		std::shared_ptr<Response> response_;
		std::function<void(const std::shared_ptr<Connection>&, const error_code&)> callback_;
	};

public:
	// Set before calling request
	Config config_;

	// If you have your own asio::io_service, store its pointer here before calling request().
	// When using asynchronous requests, running the io_service is up to the programmer.
	std::shared_ptr<asio::io_service> io_service_;

	// Convenience function to perform synchronous request. The io_service is run within this function.
	// If reusing the io_service for other tasks, use the asynchronous request functions instead.
	// Do not use concurrently with the asynchronous request functions.
	std::shared_ptr<Response> request(const std::string &method, const std::string& path = "/",
		const std::string& content = "", const CaseInsensitiveMultimap& header = CaseInsensitiveMultimap()) 
	{
		std::shared_ptr<Response> response;
		error_code ec;
		request(method, path, content, header, [&response, &ec](std::shared_ptr<Response> response_, const error_code& ec_) 
		{
			response = response_;
			ec = ec_;
		});

		{
			std::unique_lock<std::mutex> lock(concurrent_synchronous_requests_mutex);
			++concurrent_synchronous_requests;
		}
		io_service->run();
		{
			std::unique_lock<std::mutex> lock(concurrent_synchronous_requests_mutex);
			--concurrent_synchronous_requests;
			if (!concurrent_synchronous_requests)
				io_service->reset();
		}

		if (ec)
			throw system_error(ec);

		return response;
	}

	// Convenience function to perform synchronous request. The io_service is run within this function.
	// If reusing the io_service for other tasks, use the asynchronous request functions instead.
	// Do not use concurrently with the asynchronous request functions.
	std::shared_ptr<Response> request(const std::string &method, const std::string &path, std::istream &content,
		const CaseInsensitiveMultimap &header = CaseInsensitiveMultimap())
	{
		std::shared_ptr<Response> response;
		error_code ec;
		request(method, path, content, header, [&response, &ec](std::shared_ptr<Response> response_, const error_code &ec_)
		{
			response = response_;
			ec = ec_;
		});

		{
			std::unique_lock<std::mutex> lock(concurrent_synchronous_requests_mutex);
			++concurrent_synchronous_requests;
		}
		io_service->run();
		{
			std::unique_lock<std::mutex> lock(concurrent_synchronous_requests_mutex);
			--concurrent_synchronous_requests;
			if (!concurrent_synchronous_requests)
				io_service->reset();
		}

		if (ec)
			throw system_error(ec);

		return response;
	}

	// Asynchronous request where setting and/or running Client's io_service is required.
	// Do not use concurrently with the synchronous request functions.
	void request(const std::string &method, const std::string &path, string_view content, const CaseInsensitiveMultimap &header,
		std::function<void(std::shared_ptr<Response>, const error_code &)> &&request_callback_)
	{
		auto session = std::make_shared<Session>(config.max_response_streambuf_size, get_connection(), create_request_header(method, path, header));
		auto response = session->response;
		auto request_callback = std::make_shared<std::function<void(std::shared_ptr<Response>, const error_code &)>>(std::move(request_callback_));
		session->callback = [this, response, request_callback](const std::shared_ptr<Connection> &connection, const error_code &ec)
		{
			{
				std::unique_lock<std::mutex> lock(this->connections_mutex);
				connection->in_use = false;

				// Remove unused connections, but keep one open for HTTP persistent connection:
				std::size_t unused_connections = 0;
				for (auto it = this->connections.begin(); it != this->connections.end();)
				{
					if (ec && connection == *it)
						it = this->connections.erase(it);
					else if ((*it)->in_use)
						++it;
					else {
						++unused_connections;
						if (unused_connections > 1)
							it = this->connections.erase(it);
						else
							++it;
					}
				}
			}

			if (*request_callback)
				(*request_callback)(response, ec);
		};

		std::ostream write_stream(session->request_streambuf.get());
		if (content.size() > 0)
		{
			auto header_it = header.find("Content-Length");
			if (header_it == header.end()) {
				header_it = header.find("Transfer-Encoding");
				if (header_it == header.end() || header_it->second != "chunked")
					write_stream << "Content-Length: " << content.size() << "\r\n";
			}
		}
		write_stream << "\r\n"
			<< content;

		connect(session);
	}

	// Asynchronous request where setting and/or running Client's io_service is required.
	// Do not use concurrently with the synchronous request functions.
	void request(const std::string &method, const std::string &path, string_view content,
		std::function<void(std::shared_ptr<Response>, const error_code &)> &&request_callback)
	{
		request(method, path, content, CaseInsensitiveMultimap(), std::move(request_callback));
	}

	/// Asynchronous request where setting and/or running Client's io_service is required.
	void request(const std::string &method, const std::string &path,
		std::function<void(std::shared_ptr<Response>, const error_code &)> &&request_callback)
	{
		request(method, path, std::string(), CaseInsensitiveMultimap(), std::move(request_callback));
	}

	// Asynchronous request where setting and/or running Client's io_service is required.
	void request(const std::string &method, std::function<void(std::shared_ptr<Response>, const error_code &)> &&request_callback)
	{
		request(method, std::string("/"), std::string(), CaseInsensitiveMultimap(), std::move(request_callback));
	}

	// Asynchronous request where setting and/or running Client's io_service is required.
	void request(const std::string &method, const std::string &path, std::istream &content, const CaseInsensitiveMultimap &header,
		std::function<void(std::shared_ptr<Response>, const error_code &)> &&request_callback_)
	{
		auto session = std::make_shared<Session>(config.max_response_streambuf_size, get_connection(), create_request_header(method, path, header));
		auto response = session->response;
		auto request_callback = std::make_shared<std::function<void(std::shared_ptr<Response>, const error_code &)>>(std::move(request_callback_));
		session->callback = [this, response, request_callback](const std::shared_ptr<Connection> &connection, const error_code &ec)
		{
			{
				std::unique_lock<std::mutex> lock(this->connections_mutex);
				connection->in_use = false;

				// Remove unused connections, but keep one open for HTTP persistent connection:
				std::size_t unused_connections = 0;
				for (auto it = this->connections.begin(); it != this->connections.end();)
				{
					if (ec && connection == *it)
						it = this->connections.erase(it);
					else if ((*it)->in_use)
						++it;
					else
					{
						++unused_connections;
						if (unused_connections > 1)
							it = this->connections.erase(it);
						else
							++it;
					}
				}
			}

			if (*request_callback)
				(*request_callback)(response, ec);
		};

		content.seekg(0, std::ios::end);
		auto content_length = content.tellg();
		content.seekg(0, std::ios::beg);
		std::ostream write_stream(session->request_streambuf.get());
		if (content_length > 0)
		{
			auto header_it = header.find("Content-Length");
			if (header_it == header.end())
			{
				header_it = header.find("Transfer-Encoding");
				if (header_it == header.end() || header_it->second != "chunked")
					write_stream << "Content-Length: " << content_length << "\r\n";
			}
		}
		write_stream << "\r\n";
		if (content_length > 0)
			write_stream << content.rdbuf();

		connect(session);
	}

	// Asynchronous request where setting and/or running Client's io_service is required.
	void request(const std::string &method, const std::string &path, std::istream &content,
		std::function<void(std::shared_ptr<Response>, const error_code &)> &&request_callback)
	{
		request(method, path, content, CaseInsensitiveMultimap(), std::move(request_callback));
	}

	// Close connections
	void stop() noexcept 
	{
		std::unique_lock<std::mutex> lock(connections_mutex);
		for (auto it = connections.begin(); it != connections.end();)
		{
			error_code ec;
			(*it)->socket->lowest_layer().cancel(ec);
			it = connections.erase(it);
		}
	}

	virtual ~ClientBase() noexcept
	{
		handler_runner->stop();
		stop();
	}

protected:
	bool internal_io_service_ = false;

	std::string host_;
	unsigned short port_;
	unsigned short default_port_;

	std::unique_ptr<asio::ip::tcp::resolver::query> query_;

	std::unordered_set<std::shared_ptr<Connection>> connections_;
	std::mutex connections_mutex_;

	std::shared_ptr<ScopeRunner> handler_runner_;

	std::size_t concurrent_synchronous_requests_ = 0;
	std::mutex concurrent_synchronous_requests_mutex_;

	ClientBase(const std::string &host_port, unsigned short default_port) noexcept 
		: default_port_(default_port), 
		handler_runner_(new ScopeRunner())
	{
		auto parsed_host_port = parse_host_port(host_port, default_port);
		host_ = parsed_host_port.first;
		port_ = parsed_host_port.second;
	}

	std::shared_ptr<Connection> get_connection() noexcept
	{
		std::shared_ptr<Connection> connection;
		std::unique_lock<std::mutex> lock(connections_mutex);

		if (!io_service)
		{
			io_service = std::make_shared<asio::io_service>();
			internal_io_service = true;
		}

		for (auto it = connections.begin(); it != connections.end(); ++it)
		{
			if (!(*it)->in_use && !connection)
			{
				connection = *it;
				break;
			}
		}
		if (!connection)
		{
			connection = create_connection();
			connections.emplace(connection);
		}
		connection->attempt_reconnect = true;
		connection->in_use = true;

		if (!query)
		{
			if (config.proxy_server.empty())
				query_ = std::unique_ptr<asio::ip::tcp::resolver::query>(new asio::ip::tcp::resolver::query(host_, std::to_string(port_)));
			else
			{
				auto proxy_host_port = parse_host_port(config_.proxy_server, 8080);
				query_ = std::unique_ptr<asio::ip::tcp::resolver::query>(new asio::ip::tcp::resolver::query(proxy_host_port.first, std::to_string(proxy_host_port.second)));
			}
		}

		return connection;
	}

	virtual std::shared_ptr<Connection> create_connection() noexcept = 0;
	virtual void connect(const std::shared_ptr<Session> &) = 0;

	std::unique_ptr<asio::streambuf> create_request_header(const std::string &method, const std::string &path, 
		const CaseInsensitiveMultimap& header) const
	{
		auto corrected_path = path;
		if (corrected_path == "")
			corrected_path = "/";
		if (!config.proxy_server.empty() && std::is_same<socket_type, asio::ip::tcp::socket>::value)
			corrected_path = "http://" + host + ':' + std::to_string(port) + corrected_path;

		std::unique_ptr<asio::streambuf> streambuf(new asio::streambuf());
		std::ostream write_stream(streambuf.get());
		write_stream << method << " " << corrected_path << " HTTP/1.1\r\n";
		write_stream << "Host: " << host;

		if (port != default_port)
			write_stream << ':' << std::to_string(port);

		write_stream << "\r\n";
		for (auto &h : header)
			write_stream << h.first << ": " << h.second << "\r\n";

		return streambuf;
	}

	std::pair<std::string, unsigned short> parse_host_port(const std::string &host_port, unsigned short default_port) const noexcept
	{
		std::pair<std::string, unsigned short> parsed_host_port;
		std::size_t host_end = host_port.find(':');
		if (host_end == std::string::npos)
		{
			parsed_host_port.first = host_port;
			parsed_host_port.second = default_port;
		}
		else
		{
			parsed_host_port.first = host_port.substr(0, host_end);
			parsed_host_port.second = static_cast<unsigned short>(stoul(host_port.substr(host_end + 1)));
		}
		return parsed_host_port;
	}

	void write(const std::shared_ptr<Session> &session)
	{
		session->connection->set_timeout();
		asio::async_write(*session->connection_->socket_, session->request_streambuf_->data(), 
			[this, session](const error_code& ec, std::size_t /*bytes_transferred*/)
		{
			session->connection->cancel_timeout();
			auto lock = session->connection->handler_runner_->continue_lock();
			if (!lock)
				return;

			if (!ec)
				this->read(session);
			else
				session->callback(session->connection, ec);
		});
	}

	void read(const std::shared_ptr<Session>& session)
	{
		session->connection_->set_timeout();
		asio::async_read_until(*session->connection_->socket_, session->response_->streambuf_, "\r\n\r\n", 
			[this, session](const error_code& ec, std::size_t bytes_transferred)
		{
			session->connection_->cancel_timeout();
			auto lock = session->connection->handler_runner->continue_lock();
			if (!lock)
				return;
			if ((!ec || ec == asio::error::not_found) && 
				session->response_->streambuf_.size() == session->response_->streambuf_.max_size())
			{
				session->callback_(session->connection_, make_error_code::make_error_code(errc::message_size));
				return;
			}
			if (!ec)
			{
				session->connection->attempt_reconnect = true;
				std::size_t num_additional_bytes = session->response->streambuf.size() - bytes_transferred;

				if (!ResponseMessage::parse(session->response_->content_, session->response_->http_version_,
					session->response_->status_code_, session->response_->header_))
				{
					session->callback_(session->connection_, make_error_code::make_error_code(errc::protocol_error));
					return;
				}

				auto header_it = session->response_->header.find("Content-Length");
				if (header_it != session->response_->header.end())
				{
					auto content_length = stoull(header_it->second);
					if (content_length > num_additional_bytes)
					{
						session->connection_->set_timeout();
						asio::async_read(*session->connection_->socket_, session->response_->streambuf_,
							asio::transfer_exactly(content_length - num_additional_bytes), 
							[session](const error_code &ec, std::size_t /*bytes_transferred*/)
						{
							session->connection->cancel_timeout();
							auto lock = session->connection->handler_runner->continue_lock();
							if (!lock)
								return;
							if (!ec) 
							{
								if (session->response_->streambuf_.size() == session->response_->streambuf_.max_size()) {
									session->callback_(session->connection_, make_error_code::make_error_code(errc::message_size));
									return;
								}
								session->callback_(session->connection_, ec);
							}
							else
								session->callback_(session->connection_, ec);
						});
					}
					else
						session->callback_(session->connection_, ec);
				}
				else if ((header_it = session->response_->header_.find("Transfer-Encoding")) != session->response_->header_.end()
					&& header_it->second == "chunked")
				{
					auto chunks_streambuf = std::make_shared<asio::streambuf>(this->config_.max_response_streambuf_size);
					this->read_chunked_transfer_encoded(session, chunks_streambuf);
				}
				else if (session->response->http_version < "1.1" ||
					((header_it = session->response_->header_.find("Session")) != session->response_->header.end()
						&& header_it->second == "close"))
				{
					session->connection->set_timeout();
					asio::async_read(*session->connection_->socket_, session->response_->streambuf_, 
						[session](const error_code &ec, std::size_t /*bytes_transferred*/)
					{
						session->connection_->cancel_timeout();
						auto lock = session->connection_->handler_runner_->continue_lock();
						if (!lock)
							return;
						if (!ec)
						{
							if (session->response->streambuf.size() == session->response_->streambuf_.max_size())
							{
								session->callback_(session->connection_, make_error_code::make_error_code(errc::message_size));
								return;
							}
							session->callback_(session->connection_, ec);
						}
						else
							session->callback_(session->connection_, ec == asio::error::eof ? error_code() : ec);
					});
				}
				else
					session->callback_(session->connection_, ec);
			}
			else
			{
				if (session->connection_->attempt_reconnect && ec != asio::error::operation_aborted)
				{
					std::unique_lock<std::mutex> lock(connections_mutex);
					auto it = connections.find(session->connection);
					if (it != connections.end()) 
					{
						connections.erase(it);
						session->connection = create_connection();
						session->connection->attempt_reconnect = false;
						session->connection->in_use = true;
						connections.emplace(session->connection);
						lock.unlock();
						this->connect(session);
					}
					else
					{
						lock.unlock();
						session->callback_(session->connection_, ec);
					}
				}
				else
					session->callback_(session->connection_, ec);
			}
		});
	}

	void read_chunked_transfer_encoded(const std::shared_ptr<Session> &session, const std::shared_ptr<asio::streambuf> &chunks_streambuf)
	{
		session->connection_->set_timeout();
		asio::async_read_until(*session->connection_->socket_, session->response_->streambuf_, "\r\n", 
			[this, session, chunks_streambuf](const error_code& ec, size_t bytes_transferred)
		{
			session->connection_->cancel_timeout();
			auto lock = session->connection_->handler_runner_->continue_lock();
			if (!lock)
				return;
			if ((!ec || ec == asio::error::not_found) && 
				session->response_->streambuf_.size() == session->response_->streambuf_.max_size())
			{
				session->callback_(session->connection_, make_error_code::make_error_code(errc::message_size));
				return;
			}
			if (!ec)
			{
				std::string line;
				getline(session->response_->content_, line);
				bytes_transferred -= line.size() + 1;
				line.pop_back();
				unsigned long length = 0;
				try
				{
					length = stoul(line, 0, 16);
				}
				catch (...)
				{
					session->callback(session->connection_, make_error_code::make_error_code(errc::protocol_error));
					return;
				}

				auto num_additional_bytes = session->response_->streambuf_.size() - bytes_transferred;

				if ((2 + length) > num_additional_bytes)
				{
					session->connection->set_timeout();
					asio::async_read(*session->connection_->socket_, session->response_->streambuf_, 
						asio::transfer_exactly(2 + length - num_additional_bytes), 
						[this, session, chunks_streambuf, length](const error_code &ec, size_t /*bytes_transferred*/)
					{
						session->connection_->cancel_timeout();
						auto lock = session->connection_->handler_runner_->continue_lock();
						if (!lock)
							return;
						if (!ec)
						{
							if (session->response->streambuf.size() == session->response->streambuf.max_size())
							{
								session->callback_(session->connection_, make_error_code::make_error_code(errc::message_size));
								return;
							}
							this->read_chunked_transfer_encoded_chunk(session, chunks_streambuf, length);
						}
						else
							session->callback_(session->connection_, ec);
					});
				}
				else
					this->read_chunked_transfer_encoded_chunk(session, chunks_streambuf_, length);
			}
			else
				session->callback_(session->connection_, ec);
		});
	}

	void read_chunked_transfer_encoded_chunk(const std::shared_ptr<Session>& session, 
		const std::shared_ptr<asio::streambuf>& chunks_streambuf, unsigned long length)
	{
		std::ostream tmp_stream(chunks_streambuf.get());
		if (length > 0)
		{
			std::unique_ptr<char[]> buffer(new char[length]);
			session->response_->content_.read(buffer.get(), static_cast<std::streamsize>(length));
			tmp_stream.write(buffer.get(), static_cast<std::streamsize>(length));
			if (chunks_streambuf->size() == chunks_streambuf->max_size())
			{
				session->callback_(session->connection_, make_error_code::make_error_code(errc::message_size));
				return;
			}
		}

		// Remove "\r\n"
		session->response_->content.get();
		session->response_->content.get();

		if (length > 0)
			read_chunked_transfer_encoded(session, chunks_streambuf);
		else
		{
			if (chunks_streambuf_->size() > 0)
			{
				std::ostream ostream(&session->response_->streambuf_);
				ostream << chunks_streambuf.get();
			}
			error_code ec;
			session->callback_(session->connection_, ec);
		}
	}
};

template <class socket_type>
class Client : public ClientBase<socket_type> {};

using HTTP = asio::ip::tcp::socket;

template <>
class Client<HTTP> : public ClientBase<HTTP> {
public:
	Client(const std::string &server_port_path) noexcept : ClientBase<HTTP>::ClientBase(server_port_path, 80) {}

protected:
	std::shared_ptr<Connection> create_connection() noexcept override
	{
		return std::make_shared<Connection>(handler_runner_, config_.timeout, *io_service_);
	}

	void connect(const std::shared_ptr<Session> &session) override
	{
		if (!session->connection_->socket_->lowest_layer().is_open())
		{
			auto resolver = std::make_shared<asio::ip::tcp::resolver>(*io_service_);
			session->connection_->set_timeout(config.timeout_connect);
			resolver->async_resolve(*query_, [this, session, resolver](const error_code &ec, asio::ip::tcp::resolver::iterator it)
			{
				session->connection_->cancel_timeout();
				auto lock = session->connection_->handler_runner_->continue_lock();
				if (!lock)
					return;
				if (!ec)
				{
					session->connection->set_timeout(config.timeout_connect);
					asio::async_connect(*session->connection_->socket, it, [this, session, resolver](const error_code &ec, asio::ip::tcp::resolver::iterator /*it*/)
					{
						session->connection_->cancel_timeout();
						auto lock = session->connection_->handler_runner->continue_lock();
						if (!lock)
							return;
						if (!ec)
						{
							asio::ip::tcp::no_delay option(true);
							error_code ec;
							session->connection_->socket_->set_option(option, ec);
							this->write(session);
						}
						else
							session->callback_(session->connection_, ec);
					});
				}
				else
					session->callback_(session->connection_, ec);
			});
		}
		else
			write(session);
	}
};


} 

