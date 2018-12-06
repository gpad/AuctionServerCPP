#include "pch.h"

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


using ::testing::AtLeast;

TEST(EngineTest, CanGoForwardAndBackward) {
	MockEngine engine;
	EXPECT_CALL(engine, forward()).Times(AtLeast(1));
	EXPECT_CALL(engine, backward()).Times(AtLeast(1));

	engine.forward();
	engine.backward();
}