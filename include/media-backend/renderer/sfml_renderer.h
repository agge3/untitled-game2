#pragma once

#include <SFML/Window/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class SFMLRenderer : public IRenderer {
public:
	using Ptr = std::unique_ptr<sf::RenderWindow>();

	SFMLRenderer();
	SFMLRenderer(const sf::Window& window);

	void render() override;
	void update();
	void is_open();
	void close();
	void clear();
private:
	Ptr _renderer;
};
