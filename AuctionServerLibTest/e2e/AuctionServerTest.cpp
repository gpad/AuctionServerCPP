#include "../pch.h"

#include "../../AuctionServerLib/AuctionServer.h"
#include "../../AuctionServerLib/Auction.h"
#include <thread>
#include <boost/asio.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

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
			std::stringstream ss(payload);
			boost::archive::text_iarchive ia{ ss };
			std::vector<Auction> result;
			ia >> result;
			return result;
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

	class animal
	{
	public:
		animal() = default;
		animal(int legs) : legs_{ legs } {}
		int legs() const { return legs_; }

		bool operator == (const animal &other) const {
			return legs_ == other.legs_;
		}

	private:
		friend class boost::serialization::access;

		template <typename Archive>
		void serialize(Archive &ar, const unsigned int version) { ar & legs_; }

		int legs_;
	};

	TEST(BoostSerializationSpyke, TryToSerialize) {
		std::stringstream ss;
		{
			boost::archive::text_oarchive oa{ ss };
			oa << animal(1) << animal(2);
		}
		{
			boost::archive::text_iarchive ia{ ss };
			animal a1, a2;
			ia >> a1 >> a2;
			EXPECT_EQ(a1.legs(), 1);
			EXPECT_EQ(a2.legs(), 2);
		}
	}


	TEST(BoostSerializationSpyke, TryToSerializeVectorOf) {
		std::string s;
		{
			std::stringstream ss;
			boost::archive::text_oarchive oa{ ss };
			std::vector<animal> write{ animal(1), animal(2), animal(3), animal(4) };
			oa << write;
			s = ss.str();
		}
		{
			std::stringstream ss(s);
			boost::archive::text_iarchive ia{ ss };
			std::vector<animal> read;
			ia >> read;
			auto expected_animals = std::vector<animal>{ animal(1), animal(2), animal(3), animal(4) };
			EXPECT_EQ(read, expected_animals);
		}
	}
}
