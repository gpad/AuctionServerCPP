#include "stdafx.h"
#include "AuctionServer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class TcpSession: public std::enable_shared_from_this<TcpSession>
{
public:
	TcpSession(tcp::socket socket)
		: _socket(std::move(socket))
	{
	}

	void Start()
	{
		DoRead();
	}

private:
	void DoRead()
	{
		auto self(shared_from_this());
		_socket.async_read_some(boost::asio::buffer(data_, max_length),
			[this, self](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				DoWrite(length);
			}
		});
	}

	void DoWrite(std::size_t length)
	{
		auto self(shared_from_this());
		boost::asio::async_write(_socket, boost::asio::buffer(data_, length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				DoRead();
			}
		});
	}

	tcp::socket _socket;
	enum { max_length = 1024 };
	char data_[max_length];
};


class Server
{
public:
	Server(boost::asio::io_context& io_context, short port)
		: _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
	{
		DoAccept();
	}

private:
	void DoAccept()
	{
		_acceptor.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket)
		{
			if (!ec)
			{
				std::make_shared<TcpSession>(std::move(socket))->Start();
			}

			DoAccept();
		});
	}

	tcp::acceptor _acceptor;
};


class AuctionServerImpl
{
	boost::asio::io_context _ioContext;
public:
	void Run(unsigned short port) {
		Server srv(_ioContext, port);

		_ioContext.run();
	}

	void Stop() {
		_ioContext.stop();
	}

};

AuctionServer::AuctionServer()
	:_impl(new AuctionServerImpl())
{
}


AuctionServer::~AuctionServer()
{
	delete _impl;
}

void AuctionServer::Run(unsigned short port)
{
	_impl->Run(port);
}

void AuctionServer::Stop()
{
	_impl->Stop();
}
