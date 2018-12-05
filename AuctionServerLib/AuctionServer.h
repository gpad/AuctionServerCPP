#pragma once
class DLL_EXPORT AuctionServer
{
public:
	AuctionServer(unsigned short port);
	virtual ~AuctionServer();

	void run();
};

