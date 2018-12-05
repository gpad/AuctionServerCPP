#pragma once

#include <string>

class DLL_EXPORT Auction
{
	int _id;
	std::string _description;
public:
	Auction(int id, std::string description);
	virtual ~Auction();

	bool operator==(const Auction& auction) const {
		return _id == auction._id && _description == auction._description;
	}

	bool operator!=(const Auction& auction) const {
		return !(*this == auction);
	}

	std::string DebugString() const;
};

inline std::ostream& operator<<(std::ostream& os, const Auction& auction) {
	return os << auction.DebugString();  // whatever needed to print bar to os
}
