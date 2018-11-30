#include <iostream>
#include <thread>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>

using namespace std;
using namespace boost::asio::ip;

class session : public std::enable_shared_from_this<session>
{
public:
	explicit session(tcp::socket socket)
		: socket_(std::move(socket)),
		timer_(socket_.get_io_context()),
		strand_(socket_.get_io_context())
	{
	}

	void req_redis()
	{
		// Make the connection on the IP address we get from a lookup
		socket_.async_connect(tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 6379));
		socket_
		http::async_write(socket, req, yield[ec]);
		if (ec)
			return fail(ec, "write");

		// This buffer is used for reading and must be persisted
		boost::beast::flat_buffer b;

		// Declare a container to hold the response
		http::response<http::dynamic_body> res;

		// Receive the HTTP response
		http::async_read(socket, b, res, yield[ec]);
		if (ec)
			return fail(ec, "read");

		// Write the message to standard out
		std::cout << res << std::endl;
	}

	void go()
	{
		auto self(shared_from_this());
		boost::asio::spawn(strand_, [this, self](boost::asio::yield_context yield)
		{
			try
			{
				char data[128];
				for (;;)
				{
					timer_.expires_from_now(std::chrono::seconds(10));
					std::size_t n = socket_.async_read_some(boost::asio::buffer(data), yield);
					this->req_redis();
					//boost::asio::async_write(socket_, boost::asio::buffer(data, n), yield);
				}
			}
			catch (std::exception& e)
			{
				socket_.close();
				timer_.cancel();
			}
		});

		boost::asio::spawn(strand_,
			[this, self](boost::asio::yield_context yield)
		{
			while (socket_.is_open())
			{
				boost::system::error_code ignored_ec;
				timer_.async_wait(yield[ignored_ec]);
				if (timer_.expires_from_now() <= std::chrono::seconds(0))
					socket_.close();
			}
		});
	}

private:
	boost::asio::ip::tcp::socket socket_;
	boost::asio::steady_timer timer_;
	boost::asio::io_context::strand strand_;
};

void test_spawn()
{
	boost::asio::io_context io_context;

	boost::asio::spawn(io_context, [&](boost::asio::yield_context yield)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		cout << "sleep.." << endl;
	});

	boost::asio::spawn(io_context,
		[&](boost::asio::yield_context yield)
	{
		tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
		for (;;)
		{
			boost::system::error_code ec;
			tcp::socket socket(io_context);
			acceptor.async_accept(socket, yield[ec]);
			if (!ec) std::make_shared<session>(std::move(socket))->go();
		}
	});

	io_context.run();
}


int main()
{
	boost::asio::io_context io_context;



	boost::asio::spawn(io_context,
		[&](boost::asio::yield_context yield)
	{
		tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
		for (;;)
		{
			boost::system::error_code ec;
			tcp::socket socket(io_context);
			acceptor.async_accept(socket, yield[ec]);
			if (!ec) std::make_shared<session>(std::move(socket))->go();
		}
	});

	io_context.run();
}






