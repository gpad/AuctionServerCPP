#include "stdafx.h"
#include "AuctionServer.h"
#include <string>
#include <iostream>
#include <sstream>

AuctionServer::AuctionServer(unsigned short port)
{
}


AuctionServer::~AuctionServer()
{
}

void AuctionServer::run()
{
	std::string line;
	while (std::getline(std::cin, line))
	{
		std::cout << "READ: '" << line << "'" << std::endl;
		if (line == "exit") {
			return;
		}
	}
}
