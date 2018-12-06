#include "stdafx.h"
#include "Response.h"
#include <array>
#include <vector>
#include <boost/asio.hpp>

boost::asio::const_buffer Response::Encode() {

	// it's composed by 8 byte of header
	// 4 byte length
	// 4 byte ret code
	// payload

	int payload_length = _payload.size();
	std::array<int, 2> header{ payload_length, static_cast<int>(_code) };
	boost::asio::const_buffer buf1 = boost::asio::buffer(header);
	boost::asio::const_buffer buf2 = boost::asio::buffer(_payload);
	std::vector<boost::asio::const_buffer> buffers;
	buffers.push_back(buf1);
	buffers.push_back(buf2);
	_dest = std::vector<char>(buf1.size() + buf2.size());
	auto buff = boost::asio::buffer(_dest);
	boost::asio::buffer_copy(buff, buffers);

	return buff;
}
