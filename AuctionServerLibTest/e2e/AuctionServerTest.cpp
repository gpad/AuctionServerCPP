#include "../pch.h"

#include "../../AuctionServerLib/AuctionServer.h"
#include "../../AuctionServerLib/Auction.h"
#include <thread>

using Port = unsigned short;

namespace e2e {

	class Client {
	public:
		Client(Port port) {

		}
		void Disconnect() {

		}

		std::vector<Auction> GetAuctions() {
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

		void StartServer(){
			_thread = std::thread([this]() {
				_auctionServer.Run(_port);
			});
		}

		void StopServer(){
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
