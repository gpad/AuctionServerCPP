#include "pch.h"

#include "../AuctionServerLib/Response.h"
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/serialization/vector.hpp>
#include <boost/asio.hpp>

TEST(ResponseTest, SpykeOnBuffer) {
	std::string payload = "gino";
	int length = payload.size();
	int commandId = 666;

	std::array<int, 2> header { length, commandId };

	boost::asio::const_buffer buf1 = boost::asio::buffer(header);
	EXPECT_EQ(buf1.size(), 8);

	boost::asio::const_buffer buf2 = boost::asio::buffer(payload);
	EXPECT_EQ(buf2.size(), 4);

	std::vector<boost::asio::const_buffer> buffers;
	buffers.push_back(buf1);
	buffers.push_back(buf2);

	std::vector<char> dest(buf1.size() + buf2.size());
	auto buff = boost::asio::buffer(dest);
	int ret1 = boost::asio::buffer_copy(buff, buffers);

	EXPECT_EQ(buf1.size(), 8);
	EXPECT_EQ(buf2.size(), 4);
	EXPECT_EQ(buff.size(), 12);
	EXPECT_EQ(ret1, 12);
}

TEST(ResponseTest, EncodeDataInBuffer) {
	Response res(ResponseCode::Ok, "0123456789");
	EXPECT_EQ(res.Encode().size(), 8 + 10);
}


