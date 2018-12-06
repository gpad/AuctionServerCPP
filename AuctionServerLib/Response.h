#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>

enum class ResponseCode {
	Ok = 200,
	NotFound = 404
};


class DLL_EXPORT Response
{
	ResponseCode _code;
	std::string _payload;
	std::vector<char> _dest;
public:

	Response(ResponseCode code, std::string payload)
		:_code(code), _payload(payload)
	{

	}

	boost::asio::const_buffer Encode();
	//size_t Length() { return 0; }

	static Response CommandNotFound() { return Response(ResponseCode::NotFound, ""); }
};

