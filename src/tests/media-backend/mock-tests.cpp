#include "media-backend/abstract-media-factory.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <memory>

class MockWindow : public IWindow {
public:
    MOCK_METHOD(void, init, (), (override));
    MOCK_METHOD(bool, is_open, (), (const, override));
    MOCK_METHOD(void, close, (), (override));
    MOCK_METHOD(void, set_vsync, (bool flag), (override));
};

class MockRenderer : public IRenderer {
public:
    MOCK_METHOD(void, init, (), (override));
    MOCK_METHOD(void, render, (), (override));
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(void, display, (), (override));
};

class MockEventHandler : public IEventHandler {
public:
    MOCK_METHOD(std::optional<IEvent>, poll_event, (), (const, override));
};

class MockSFMLBackend : public MediaBackend {
public:
    MOCK_METHOD(std::unique_ptr<IWindow>, create_window, (), (override));
    MOCK_METHOD(std::unique_ptr<IRenderer>, create_renderer, (IWindow& window), (override));
    MOCK_METHOD(std::unique_ptr<IEventHandler>, create_event_handler, (IWindow& window), (override));
};

class MockSFMLBackendFactory : public MediaBackEndFactory {
	MOCK_METHOD(std::unique_ptr<SFMLBackend>, create, (), (override));
};

TEST(SFMLBackendFactoryTest, Create)
{
	MockSFMLBackendFactory factory;

	EXPECT_CALL(factory, create()).WillOnce([]() {
		return std::make_unique<MockMediaBackend>();
	});

	auto backend = factory.create();
}

TEST(SFMLBackendTest, CreateWindow) {
	MockSFMLBackendFactory factory;
	auto backend = factory.create();

    EXPECT_CALL(backend, create_window())
        .WillOnce([]() {
            return std::make_unique<MockSFMLWindow>();
        });

    auto window = backend.create_window();
    ASSERT_TRUE(window != nullptr);
}

TEST(SFMLBackendTest, CreateRenderer) {
	MockSFMLBackendFactory factory;
	auto backend = factory.create();
    MockSFMLWindow window;

    EXPECT_CALL(backend, create_renderer(testing::_))
        .WillOnce(testing::Return(std::make_unique<MockSFMLRenderer>()));

    auto renderer = backend.create_renderer(window);
    ASSERT_TRUE(renderer != nullptr);
}

TEST(SFMLBackendTest, CreateEventHandler) {
	MockSFMLBackendFactory factory;
	auto backend = factory.create();
    MockSFMLWindow window;

    EXPECT_CALL(backend, create_event_handler(testing::_))
        .WillOnce(testing::Return(std::make_unique<MockSFMLEventHandler>()));

    auto event_handler = backend.create_event_handler(window);
    ASSERT_TRUE(event_handler != nullptr);
}

TEST(SFMLBackendTest, CreateBackend)
{
	MockSFMLBackendFactory factory;
	auto backend = factory.create();
	MockSFMLRenderer renderer;
	MockSFMLEventHandler event_handler;

    EXPECT_CALL(backend, create_window())
        .WillOnce([]() {
            return std::make_unique<MockSFMLWindow>();
        });
    EXPECT_CALL(backend, create_renderer(testing::_))
        .WillOnce(testing::Return(std::make_unique<MockSFMLRenderer>()));
    EXPECT_CALL(backend, create_event_handler(testing::_))
        .WillOnce(testing::Return(std::make_unique<MockSFMLEventHandler>()));

	EXPECT_CALL(window, is_open(testing::_))
		.WillOnce(testing::Return(true));

	auto window = backend.create_window();
	ASSERT_TRUE(window != nullptr);
	auto renderer = backend.create_renderer();
	ASSERT_TRUE(window != nullptr);
	ASSERT_TRUE(renderer != nullptr);
	auto event_handler = backend.create_event_handler();
	ASSERT_TRUE(window != nullptr);
	ASSERT_TRUE(renderer != nullptr);
	ASSERT_TRUE(event_handler != nullptr);

	EXPECT_TRUE(window->is_open());
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
