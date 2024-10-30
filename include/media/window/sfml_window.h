#pragma once

#include "i_window.h"

class SFMLWindow : public IWindow {
public:
	using Window = sf::Window;
	using Renderer = SFMLRenderer;

	SFMLWindow();

	void render() override;
	void update() override;
	void poll_event(const IEvent& e) override;

	bool is_open() override;
	void close() override;
	void clear() override;
	void display() override;
	void set_vsync(bool flag) override;
	void set_view() override;
private:
	bool _open;
	bool _vsync;
	std::unique_ptr<Window> _window;
	std::unique_ptr<Renderer> _renderer;
	std::size_t _size;
	std::size_t _position;
	bool _fullscreen;
};
