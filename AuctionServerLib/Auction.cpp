#include "stdafx.h"
#include "Auction.h"

#include <boost/format.hpp>

using boost::format;
using boost::io::group;

Auction::Auction(int id, std::string description)
	:_id(id), _description(description)
{
}

Auction::~Auction()
{
}

std::string Auction::DebugString() const {
	return str(format("Auction id: (%1%) description: (%2%)") % _id % _description);
}
