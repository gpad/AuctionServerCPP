#include "pch.h"

#include "../AuctionServerLib/Auction.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

TEST(AuctionTest, AreEqualsIfContainsSameNameAndId) {
	EXPECT_TRUE(Auction(42, "gino") == Auction(42, "gino"));
	EXPECT_FALSE(Auction(42, "gino") == Auction(43, "gino"));
	EXPECT_FALSE(Auction(42, "gino") == Auction(42, "pino"));
}

TEST(AuctionTest, AreNotEqualsIfContainsSameNameAndId) {
	EXPECT_FALSE(Auction(42, "gino") != Auction(42, "gino"));
	EXPECT_TRUE(Auction(42, "gino") != Auction(43, "gino"));
	EXPECT_TRUE(Auction(42, "gino") != Auction(42, "pino"));
}

TEST(AuctionTest, AreEqualsAlsoForGTest) {
	EXPECT_EQ(Auction(42, "gino"), Auction(42, "gino"));
	EXPECT_NE(Auction(42, "gino"), Auction(43, "gino"));
	EXPECT_NE(Auction(42, "gino"), Auction(42, "pino"));
}


TEST(AuctionTest, SerializeAnAuction) {
	std::stringstream ss;
	{
		boost::archive::text_oarchive oa{ ss };
		oa << Auction(1, "1") << Auction(2, "2");
	}
	{
		boost::archive::text_iarchive ia{ ss };
		Auction a1, a2;
		ia >> a1 >> a2;
		EXPECT_EQ(a1, Auction(1, "1"));
		EXPECT_EQ(a2, Auction(2, "2"));
	}
}


TEST(AuctionTest, SerializeAVectorOfAuction) {
	std::string s;
	{
		std::stringstream ss;
		boost::archive::text_oarchive oa{ ss };
		std::vector<Auction> write{ Auction(1, "1"), Auction(2, "2"), Auction(3, "3"), Auction(4, "4") };
		oa << write;
		s = ss.str();
	}
	{
		std::stringstream ss(s);
		boost::archive::text_iarchive ia{ ss };
		std::vector<Auction> read;
		ia >> read;
		auto expected_animals = std::vector<Auction>{ Auction(1, "1"), Auction(2, "2"), Auction(3, "3"), Auction(4, "4") };
		EXPECT_EQ(read, expected_animals);
	}
}

