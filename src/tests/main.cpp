#include <gtest/gtest.h>
#include <glog/logging.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <sol2/sol2.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include <stdcapture/stdcapture.h>

#include <iostream>

// Function to log a message to stderr and return the stderr buffer as a string.
std::string logToString(const std::string& msg)
{
	FLAGS_minloglevel = google::GLOG_INFO;
	FLAGS_logtostderr = true;
	std::string captured;
	{
		stdcapture::CaptureStderr cap([&](const char* buf, size_t szbuf)
		{
			captured += std::string(buf, szbuf);
		});
		LOG(INFO) << msg;
	}
	return captured;
}

// Function to validate that GLog successfully logged a message.
bool validateGlog(const std::string& msg)
{
	std::string s = logToString(msg);
    return s.find(msg) != std::string::npos;
}

TEST(GLogTest, validate)
{
	std::string msg = "This is a test log message.";
	EXPECT_TRUE(validateGlog(msg));
}

//TEST(SFMLBackendFactory, create)
//{
//	#define SFML
//
//	#ifdef SFML
//		SFMLBackendFactory factory;
//		std::unique_ptr<SFMLBackend> backend = factory.create();
//	#else 
//		SDLBackendFactory factory;
//		std::unique_ptr<SDLBackend> backend = factory.create();
//	#endif
//
//	std::unique_ptr<IWindow> window = backend->create_window();
//	std::unique_ptr<IRenderer> renderer = backend->create_renderer(*window);
//	std::unique_ptr<IEventHandler> event_handler = backend->create_event_handler(*window);
//
//	#ifdef SFML
//		sf::Event event;
//		event_handler->poll_event(&event);
//	#else 
//		SDL_Event event;	
//		event_handler->poll_event(&event);
//	#endif
//}

int sol2_test()
{
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	// integer types
	lua.set("number", 24);
	// floating point numbers
	lua["number2"] = 24.5;
	// string types
	lua["important_string"] = "woof woof";
	// is callable, therefore gets stored as a function that can be called
	lua["a_function"] = []() { return 100; };
	// make a table
	lua["some_table"] = lua.create_table_with("value", 24);

	return 0;
}

int imgui_sfml_test()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "testing");
    window.setFramerateLimit(60);
    window.setView(window.getDefaultView());
    if (!ImGui::SFML::Init(window)) {
        return -1;
	}
    ImGui::StyleColorsLight();
    ImVec2 sfml_view(window.getView().getSize().x,
            window.getView().getSize().y);
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    ImVec2 pos(0.f, 0.f);

    sf::Clock delta_clock;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed)
                window.close();
        }
        ImGui::SFML::Update(window, delta_clock.restart());
        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(sfml_view);
        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button!");
        ImGui::End();

        window.clear();
        window.draw(shape);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
	return 0;
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	google::InitGoogleLogging(*argv);
	return RUN_ALL_TESTS();
}
