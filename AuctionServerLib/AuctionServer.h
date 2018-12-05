#pragma once

class AuctionServerImpl;

using Port = unsigned short;

class DLL_EXPORT AuctionServer
{
	AuctionServerImpl *_impl;
public:
	AuctionServer();
	virtual ~AuctionServer();

	void Run(Port port);
	void Stop();
};

