#pragma once

#include "i_renderer.h"

#include <memory>

class IWindow {
public:
	using RendererPtr = std::unique_ptr<IRenderer>();

	void render();
	void update();
	void poll_event(const IEvent& e) const;

	bool is_open() const;
	void close();
	void clear();
	void display();
	void set_vsync();
	void set_view();
private:
	bool _open;
	bool _vsync;
	RendererPtr _renderer;
	std::size_t _size;
	std::size_t _position;
	bool _fullscreen;
};
