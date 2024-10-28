#include <gtest/gtest.h>
#include <glog/logging.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

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
