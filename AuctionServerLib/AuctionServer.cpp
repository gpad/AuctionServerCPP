#include "stdafx.h"
#include "AuctionServer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Response {

public:
	std::string Encode() { 

		// it's composed by 8 byte of header
		// 4 byte length
		// 4 byte ret code
		// payload


		return "";
	}
	size_t Length() { return 0; }
};

class CommandRouter {
public:
	Response Execute(int commandId, std::string payload) {
		return Response();
	}
};

class TcpSession: public std::enable_shared_from_this<TcpSession>
{
public:
	TcpSession(tcp::socket socket)
		: _socket(std::move(socket))
	{
	}

	void Start()
	{
		DoReadHeader();
	}

private:
	void DoReadHeader()
	{
		auto self(shared_from_this());
		_socket.async_read_some(boost::asio::buffer(_header, header_length),
			[this, self](boost::system::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				DoReadBody();
			}
			else {
				// close connection
			}
		});
	}
	void DoReadBody() {
		auto self(shared_from_this());
		boost::asio::async_read(_socket, boost::asio::buffer(_payload, GetRequiredPayloadLenght()),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				auto response = ExecuteCommand(GetCommandId(), GetStringPayload());
				DoWrite(response);
			}
			else
			{
				_socket.close();
			}
		});

	}

	void DoWrite(Response response)
	{
		auto self(shared_from_this());
		boost::asio::async_write(_socket, boost::asio::buffer(response.Encode(), response.Length()),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				DoReadHeader();
			}
		});
	}

	Response ExecuteCommand(int commandId, std::string payload) {
		return _router.Execute(commandId, payload);
	}

	int GetCommandId() { return _header[1]; }

	size_t GetRequiredPayloadLenght() { return _header[0]; }
	std::string GetStringPayload() {
		return std::string(_payload.begin(), _payload.end());
	}

	tcp::socket _socket;
	enum { header_length = 1024 };
	std::array<int, 2> _header;
	std::vector<char> _payload;
	CommandRouter _router;
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
