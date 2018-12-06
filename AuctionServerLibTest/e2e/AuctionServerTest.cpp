#include "../pch.h"

#include "../../AuctionServerLib/AuctionServer.h"
#include "../../AuctionServerLib/Auction.h"
#include <thread>
#include <boost/asio.hpp>

using Port = unsigned short;
using boost::asio::ip::tcp;

namespace e2e {

	class Client {
		boost::asio::io_context _ioContext;
		tcp::socket _socket;

	public:
		Client(Port port) : _socket(_ioContext) {
			tcp::resolver resolver(_ioContext);
			boost::asio::connect(_socket, resolver.resolve("127.0.0.1", std::to_string(port)));
		}

		void Disconnect() {
			_socket.close();
		}

		std::vector<Auction> GetAuctions() {
			// 4 byte length
			// 4 byte command 
			// payload

			// CMD 1 -> Get List of Auctions
			{
				std::array<int, 2> data{ 0, 1 };
				boost::asio::write(_socket, boost::asio::buffer(data));
			}

			// read response header
			std::array<int, 2> header = ReadHeader();
			std::string payload = ReadPayload(header);
			return DecodePayload(payload);
		}

		bool IsConnected() const {
			return _socket.is_open();
		}

		std::array<int, 2> ReadHeader()
		{
			std::array<int, 2> header;
			std::size_t expected = sizeof(int) * 2;
			boost::system::error_code error_code;
			std::size_t received = 0;
			boost::asio::async_read(_socket, boost::asio::buffer(header), [&error_code, &received](auto err, auto count) {
				error_code = err;
				received = count;
			});

			_ioContext.run_for(boost::asio::chrono::seconds(5));

			if (error_code || (received < expected)) {
				ADD_FAILURE() << "Timeout on receiving header, expected " << expected << " received: " << received;
			}
			return header;
		}

		std::string ReadPayload(const std::array<int, 2> &header)
		{
			// read response body
			std::size_t expected = header[1];
			boost::system::error_code error_code;
			std::size_t received = 0;
			std::vector<char> payload;
			payload.resize(expected);
			boost::asio::async_read(_socket, boost::asio::buffer(payload), [&error_code, &received](auto err, auto count) {
				error_code = err;
				received = count;
			});

			_ioContext.run_for(boost::asio::chrono::seconds(5));

			if (error_code || (received < expected)) {
				ADD_FAILURE() << "Timeout on receiving data, expected " << expected << " received: " << received;
				return "";
			}

			return std::string(payload.begin(), payload.end());
		}

		std::vector<Auction> DecodePayload(std::string payload) {
			return std::vector<Auction>();
		}
	};

	class AuctionServerTest : public ::testing::Test {
	protected:
		void SetUp() override {
			StartServer();
		}

		void TearDown() override {
			StopServer();
		}

		void StartServer() {
			_thread = std::thread([this]() {
				_auctionServer.Run(_port);
			});
		}

		void StopServer() {
			_auctionServer.Stop();
			_thread.join();
		}

		std::unique_ptr<Client> ConnectToServer() {
			return std::make_unique<Client>(_port);
		}

		Port _port;
		std::thread _thread;
		AuctionServer _auctionServer;
	};

	TEST_F(AuctionServerTest, ConnectAndThenDisconnect) {
		auto client = ConnectToServer();
		EXPECT_TRUE(client->IsConnected());
		client->Disconnect();
	}

	TEST_F(AuctionServerTest, GetListOfAuction) {
		std::vector<int> v{ 1, 2, 3, 4, 5 };
		auto client = ConnectToServer();
		auto auctions = client->GetAuctions();
		auto expected_auctions = std::vector<Auction>{ Auction(1, "example_1"), Auction(2, "example_2"), Auction(3, "example_3") };
		EXPECT_EQ(auctions, expected_auctions);
	}
}
