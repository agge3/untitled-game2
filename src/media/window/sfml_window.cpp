#pragma once

#include "window/sfml_window.h"
	SFMLWindow::SFMLWindow() :
		_open(true),
 		_vsync(false),
		_window(std::make_unique<Window>()),
		_size(),
		_position(),
		_fullscreen(false)
{
	init();
	_renderer = std::make_unique<Renderer>(_window);
}

void SFMLWindow::init()
{
}

void SFMLWindow::render() 
{
	_renderer->render();
}
void SFMLWindow::update() 
{
	_renderer->update();
}
void SFMLWindow::poll_event(const IEvent& e)
{
	_renderer->poll_event();
}

bool SFMLWindow::is_open()
{
	return _renderer->isOpen();
}

void SFMLWindow::close()
{
	_renderer->close();
}
void SFMLWindow::clear()
{
	_renderer->clear();
}

void SFMLWindow::display()
{
	_renderer->display();
}

void SFMLWindow::set_vsync(bool flag)
{
	_renderer->setVerticalSyncEnabled(flag);
}

void SFMLWindow::set_view()
{

}
