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
			auto x = resolver.resolve("127.0.0.1", std::to_string(port));
			boost::asio::connect(_socket, x);
		}

		void Disconnect() {
			_socket.close();
		}

		std::vector<Auction> GetAuctions() {
			return std::vector<Auction>();
		}

		bool IsConnected() const {
			return _socket.is_open();
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
