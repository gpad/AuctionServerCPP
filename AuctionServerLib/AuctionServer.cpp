#include "stdafx.h"
#include "AuctionServer.h"
#include <string>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "Response.h"
#include "Auction.h"

using boost::asio::ip::tcp;
using Action = std::function<Response(int, std::string)>;

class Router {

	std::map<int, Action> _commands;
public:
	Router(std::map<int, Action> commands)
		:_commands(commands)
	{

	}
	Response Execute(int commandId, std::string payload) {
		auto command = _commands.find(commandId);
		if (command != end(_commands)) {
			return command->second(commandId, payload);
		}
		return Response::CommandNotFound();
	}
};

class TcpSession : public std::enable_shared_from_this<TcpSession>
{
public:
	TcpSession(tcp::socket socket, Router router)
		: _socket(std::move(socket)), _router(router), _response(Response::CommandNotFound())
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
				_response = ExecuteCommand(GetCommandId(), GetStringPayload());
				DoWrite();
			}
			else
			{
				_socket.close();
			}
		});

	}

	void DoWrite()
	{
		auto self(shared_from_this());
		boost::asio::async_write(_socket, _response.Encode(),
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
	Router _router;
	Response _response;
};

class Server
{
public:
	Server(boost::asio::io_context& io_context, short port, Router router)
		: _acceptor(io_context, tcp::endpoint(tcp::v4(), port)), _router(router)
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
				std::make_shared<TcpSession>(std::move(socket), _router)->Start();
			}

			DoAccept();
		});
	}

	tcp::acceptor _acceptor;
	Router _router;
};


class RouterBuilder {
	std::map<int, Action> _commands;
public:
	RouterBuilder& AddCommand(int commandId, Action action)
	{
		_commands[commandId] = action;
		return *this;
	}

	std::map<int, Action> Build() {
		return _commands;
	}
};

class AuctionServerImpl
{
	boost::asio::io_context _ioContext;
public:
	void Run(unsigned short port) {

		auto cmd1 = [](auto id, auto payload) {
			auto auctions = std::vector<Auction>{ Auction(1, "ex_1"), Auction(2, "ex_2"), Auction(3, "ex_3") };
			std::stringstream ss;
			boost::archive::text_oarchive oa{ ss };
			oa << auctions;
			return Response(ResponseCode::Ok, ss.str());
		};

		auto commands = RouterBuilder()
			.AddCommand(1, cmd1)
			.Build();

		Server srv(_ioContext, port, Router(commands));

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
