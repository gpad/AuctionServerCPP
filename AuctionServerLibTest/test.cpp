#include "pch.h"

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

class Engine {
public:
	virtual void forward() = 0;
	virtual void backward() = 0;
};

class MockEngine : public Engine {
public:
	MOCK_METHOD0(forward, void());
	MOCK_METHOD0(backward, void());
};


using ::testing::AtLeast;                     // #1

TEST(EngineTest, CanGoForwardAndBackward) {
	MockEngine engine;                          // #2
	EXPECT_CALL(engine, forward())              // #3
		.Times(AtLeast(1));

	engine.forward();
	//engine.backward();

}