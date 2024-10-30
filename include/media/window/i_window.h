#pragma once

#include "i_renderer.h"

#include <memory>

class IWindow {
public:
	using RendererPtr = std::unique_ptr<IRenderer>();

	virtual ~IWindow() = default;

	virtual void render() = 0;
	virtual void update() = 0;
	virtual void poll_event(const IEvent& e) const = 0;

	virtual bool is_open() const = 0;
	virtual void close() = 0;
	virtual void clear() = 0;
	virtual void display() = 0;
	virtual void set_vsync(bool flag) = 0;
	virtual void set_view() = 0;
private:
	bool _open;
	bool _vsync;
	RendererPtr _renderer;
	std::size_t _size;
	std::size_t _position;
	bool _fullscreen;
};
