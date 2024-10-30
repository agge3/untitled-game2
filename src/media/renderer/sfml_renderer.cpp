#include "renderer/sfml_renderer.h"

SFMLRenderer::SFMLRenderer(const sf::Window& window)
{
	_renderer = std::make_unique<sf::RenderWindow>(
		window.getSystemHandle(), window.getSettings());
}

void SFMLRenderer::render()
{
	_renderer->draw();
}

void SFMLRenderer::update()
{
	_renderer->update();
}

void SFMLRenderer::poll_event(const IEvent& e)
{
	_renderer->poll_event(e);
}

void SFMLRenderer::is_open()
{
	_renderer->isOpen();
}

void SFMLRenderer::close()
{
	_renderer->close();
}

void SFMLRenderer::clear()
{
	_renderer->clear();
}

void SFMLRenderer::display()
{
	renderer->display();
}

void SFMLRenderer::set_vsync(bool flag)
{
	_renderer->setVerticalSyncEnabled(flag);
}
