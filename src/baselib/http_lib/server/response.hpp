#pragma once


#include <string>
#include <vector>

#include "asio_fwd.hpp"
#include "response_cv.hpp"
#include "nlohmann_json.hpp"
#include "itoa.hpp"
#include "utils.hpp"

namespace zq{

class response 
{
public:

	std::vector<boost::asio::const_buffer> to_buffers() 
	{
		std::vector<boost::asio::const_buffer> buffers;
		add_header("Host", "zq");

		buffers.reserve(headers_.size() * 4 + 5);
		buffers.emplace_back(to_buffer(status_));
		for (auto const& h : headers_) 
		{
			buffers.emplace_back(boost::asio::buffer(h.first));
			buffers.emplace_back(boost::asio::buffer(name_value_separator));
			buffers.emplace_back(boost::asio::buffer(h.second));
			buffers.emplace_back(boost::asio::buffer(crlf));
		}

		buffers.push_back(boost::asio::buffer(crlf));

		if (body_type_ == content_type::string) 
		{
			buffers.emplace_back(boost::asio::buffer(content_.data(), content_.size()));
		}

		return buffers;
	}

	void add_header(std::string&& key, std::string&& value) 
	{
		headers_.emplace_back(std::move(key), std::move(value));
	}

	void set_status(status_type status)
	{
		status_ = status;
	}

	status_type get_status() const 
	{
		return status_;
	}

	void set_delay(bool delay) 
	{
		delay_ = delay;
	}

	void set_status_and_content(status_type status) 
	{
		status_ = status;
		set_content(to_string(status).data());
	}

	void set_status_and_content(status_type status, std::string&& content, res_content_type res_type = res_content_type::none, content_encoding encoding = content_encoding::none)
	{
		status_ = status;
		if(res_type != res_content_type::none)
		{
			auto iter = res_mime_map.find(res_type);
			if(iter != res_mime_map.end())
			{
				add_header("Content-type",std::string(iter->second.data(),iter->second.size()));
			}
		}
#ifdef ZQ_ENABLE_GZIP
		if (encoding == content_encoding::gzip) 
		{
			std::string encode_str;
			bool r = gzip_codec::compress(std::string(content.data(), content.length()), encode_str, true);
			if (!r) 
			{
				set_status_and_content(status_type::internal_server_error, "gzip compress error");
			}
			else 
			{
				add_header("Content-Encoding", "gzip");
				set_content(std::move(encode_str));
			}
		}
		else 
#endif
			set_content(std::move(content));
	}

	void set_status_and_content(status_type status, std::string&& content,std::string&& res_content_type_str, content_encoding encoding = content_encoding::none)
	{
		status_ = status;
		add_header("Content-type",std::move(res_content_type_str));

#ifdef ZQ_ENABLE_GZIP
		if (encoding == content_encoding::gzip) 
		{
			std::string encode_str;
			bool r = gzip_codec::compress(std::string(content.data(), content.length()), encode_str, true);
			if (!r) 
			{
				set_status_and_content(status_type::internal_server_error, "gzip compress error");
			}
			else 
			{
				add_header("Content-Encoding", "gzip");
				set_content(std::move(encode_str));
			}
		}
		else
#endif
		set_content(std::move(content));
	}

	bool need_delay() const 
	{
		return delay_;
	}

	void reset()
	{
		status_ = status_type::init;
		proc_continue_ = true;
		headers_.clear();
		content_.clear();
        cache_data.clear();
	}

	void set_continue(bool con) 
	{
		proc_continue_ = con;
	}

	bool need_continue() const 
	{
		return proc_continue_;
	}

	void set_content(std::string&& content) 
	{
		char temp[20] = {};
		itoa_fwd((int)content.size(), temp);
		add_header("Content-Length", temp);

		body_type_ = content_type::string;
		content_ = std::move(content);
	}

	void set_chunked() 
	{
		//"Transfer-Encoding: chunked\r\n"
		add_header("Transfer-Encoding", "chunked");
	}

	std::vector<boost::asio::const_buffer> to_chunked_buffers(const char* chunk_data, size_t length, bool eof) 
	{
		std::vector<boost::asio::const_buffer> buffers;

		if (length > 0)
		{
			// convert bytes transferred count to a hex string.
			chunk_size_ = to_hex_string(length);

			// Construct chunk based on rfc2616 section 3.6.1
			buffers.push_back(boost::asio::buffer(chunk_size_));
			buffers.push_back(boost::asio::buffer(crlf));
			buffers.push_back(boost::asio::buffer(chunk_data, length));
			buffers.push_back(boost::asio::buffer(crlf));
		}

		//append last-chunk
		if (eof)
		{
			buffers.push_back(boost::asio::buffer(last_chunk));
			buffers.push_back(boost::asio::buffer(crlf));
		}

		return buffers;
	}

	void set_domain(std::string domain) 
	{
		domain_ = domain;
	}

	std::string get_domain() 
	{
		return domain_;
	}

	void set_path(std::string path)
	{
		path_ = path;
	}

	std::string get_path()
	{
		return path_;
	}

	void set_url(std::string url)
	{
		raw_url_ = url;
	}

	std::string get_url(std::string url)
	{
		return raw_url_;
	}

    void render_json(const nlohmann::json& json_data)
    {
#ifdef  ZQ_ENABLE_GZIP
        set_status_and_content(status_type::ok,json_data.dump(),res_content_type::json,content_encoding::gzip);
#else
        set_status_and_content(status_type::ok,json_data.dump(),res_content_type::json,content_encoding::none);
#endif
    }

    void render_string(std::string&& content)
	{
#ifdef  ZQ_ENABLE_GZIP
		set_status_and_content(status_type::ok,std::move(content),res_content_type::string,content_encoding::gzip);
#else
		set_status_and_content(status_type::ok,std::move(content),res_content_type::string,content_encoding::none);
#endif
	}

	std::vector<std::string> raw_content() 
	{
		return cache_data;
	}

	void redirect(const std::string& url, bool is_forever = false)
	{
		add_header("Location", url.c_str());
		is_forever == false ? set_status_and_content(status_type::moved_temporarily) : set_status_and_content(status_type::moved_permanently);
	}

private:
		
	std::string raw_url_;
	std::vector<std::pair<std::string, std::string>> headers_;
	std::vector<std::string> cache_data;
	std::string content_;
	content_type body_type_ = content_type::unknown;
	status_type status_ = status_type::init;
	bool proc_continue_ = true;
	std::string chunk_size_;

	bool delay_ = false;

	std::string domain_;
	std::string path_;
};

}

