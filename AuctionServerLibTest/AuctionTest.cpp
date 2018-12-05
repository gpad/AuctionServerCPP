#include "pch.h"

#include "../AuctionServerLib/Auction.h"

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

