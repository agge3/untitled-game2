#pragma once

#include "i_window.h"

class SFMLWindow : public IWindow {
public:
	using RendererPtr = std::unique_ptr<IRenderer>();

	void render();
	vvoid update();
	vvoid poll_event(const IEvent& e);

	vbool is_open();
	vvoid close();
	vvoid clear();
	vvoid display();
	vvoid set_vsync();
	vvoid set_view();
private:
	bool _open;
	bool _vsync;
	RendererPtr _renderer;
	std::size_t _size;
	std::size_t _position;
	bool _fullscreen;
};
