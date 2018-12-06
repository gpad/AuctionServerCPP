#pragma once

#include <string>
#include <boost/serialization/access.hpp>

class DLL_EXPORT Auction
{
	int _id;
	std::string _description;

	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version) { 
		ar & _id;
		ar & _description;
	}
public:
	Auction():_id(-1){ }
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
