/**
* AbstractFactory because:
* 	* A system should be independent of how its products are created, composed,
* 	  and represented.
* 	* A system should be configured with one of multiple families of products.
* 	* A family of related product objects is designed to be used together, and
* 	  you need to enforce this constraint.
* 	* It makes exchanging product families easy.
* - Design Patterns, GoF
*
* Non-parameterized factory vs parameterized factory:
* 	* Non-parameterized factory because the amount of media backends is small
* 	  so unexpected types don't need to be handled. This can save us the
* 	  complexity of needing to handle unexpected types and doesn't have the
* 	  consequence of producing much boilerplate for the backends we have (since,
* 	  again, this list is small).
*
* Prototype for the instance components because:
* 	* Instance components function similarly in behavior; the differences are in
* 	  implementation.
* 	* Their behavior can thus be universally controlled by a common (and
* 	  implementation-independent) interface API.
*/

#pragma once

#include <SFML/Window/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>
#include <optional>

class IWindow {
public:
	virtual ~IWindow() = default;
	virtual void init() = 0;
	virtual bool is_open() const = 0;
	virtual void close() = 0;
	virtual void set_vsync(bool flag) = 0;
};

class SFMLWindow : public IWindow {
public:
	using Window = sf::Window;

	SFMLWindow() : _fullscreen(false), _vsync(false),
		_window(make_unique<Window>()) { init(); }

	void init() override
	{
		_window = std::make_unique<Window>(sf::VideoMode(800, 600),
									 "SFML Window");
	}
	bool is_open() override
	{
		return _window->isOpen();
	}

	void close() override
	{
		_window->close();
	}

	void set_vsync(bool flag) override
	{
		_vsync = flag;
		_window->setVerticalSyncEnabled(_vsync);
	}
private:
	bool _vsync;
	std::unique_ptr<Window> _window;
	std::size_t _size;
	std::size_t _position;
	bool _fullscreen;
};

class SDLWindow : public IWindow {
public:
	void init() override
	bool is_open() const override;
	void close() override;
	void set_vsync() override;
private:
	bool _open;
	bool _vsync;
	std::size_t _size;
	std::size_t _position;
	bool _fullscreen;
};

class IRenderer {
public:
	virtual ~IRenderer() = default;
	virtual void init() = 0;
	virtual void render() = 0;
	virtual void clear() = 0;
	virtual void display() = 0;
private:
	IWindow _window;
};

class SFMLRenderer : public IRenderer {
public:
	using Ptr = std::unique_ptr<sf::RenderWindow>();

	SFMLRenderer();
	SFMLRenderer(sf::Window &window) { init(); }
	
	void init() override
	{
		_renderer = std::make_unique<sf::RenderWindow(
			window.getSystemHandler(), window.getSettings());
	}
	void render() override
	{
		_renderer->draw();
	}
	void clear()
	{
		_renderer->clear();
	}
	void display()
	{
		renderer->display();
	}
	void set_vsync(bool flag)
	{
		_renderer->setVerticalSyncEnabled(flag);
	}
private:
	Ptr _renderer;
};

class IEvent {
public:
	virtual ~IEvent() = default;
};

class IEventHandler {
	virtual ~IEventHandler() = default;
	virtual std::optional<IEvent> poll_event() const = 0;
};

class SFMLEventHandler {
public:
	SFMLEventHandler( sf::RenderWindow& window) : _window(window) {}

	// optional doesn't heap allocate like a pointer would, and heap allocation
	// isn't necessary for in game loop events.
	std::optional<IEvent> poll_event() const override
	{
	  	//sf::Event event;
		//if (_window->pollEvent(&event)) {
		//	return std::optional<IEvent>(event);
		//}
		return std::nullopt;
	}
	bool poll_event(sf::Event &event)
	{
		return _window->pollEvent(&event);
	}
private:
	sf::RenderWindow _window;
};

class SDLEventHandler {
	std::optional<IEvent> poll_event() const override
	{
		//SDL_Event event;
		//if (SDL_PollEvent(&event) != -1) {
		//	return std::optional<SDLEvent>(event);
		//}
		return std::nullopt;
	}
	bool poll_event(SDL_Event *event)
	{
		return SDL_PollEvent(event) != -1;
	}
};

class MediaBackend {
	virtual ~MediaInstance() {}
	virtual std::unique_ptr<IWindow> create_window() = 0;
	virtual std::unique_ptr<IRenderer> create_renderer(IWindow &window) = 0;
	virtual std::unique_ptr<IEventHandler> create_event_handler(IWindow &window) = 0;
};

class MediaBackendFactory {
public:
	virtual ~MediaFactory() {}
	std::unique_ptr<MediaBackend> create() = 0;
};

class SFMLBackend : public MediaBackend {
public:
	// xxx maybe, or maybe builder
	//std::unique_ptr<MediaInstance> create() const override
	//{
	//	_window = create_window();
	//	_renderer = create_renderer();
	//	_event_handler = create_event_handler();
	//}
	
	// xxx maybe, decide whether to abstract this all into IDisplay. Removes
	// window and renderer and combines them. Will be useful for imbedded GUI
	// components that also use this factory. OR maybe they should be
	// indepedent (although, calling them all from the same interface offloads
	// implementation logic, and combines "what should happen" into a common
	// idea).
	// xxx very likely to go for, because it immediately simplifies SFML (and
	// ImGui) and YMMV on SDL at the time to make a different decision.
	// CONS: couples window and renderer, although renderer can be wrapped up
	// into window, so it's really on the client side is the a pro or a con?
	// PROS: easier to use, but how do you want to interact? as one object or
	// as separate window and render calls
	// honestly, it's a game: calling everything the "display" seems sensible
	// and intuitive. would that cause issues with binding things inside an
	// already existing display?
	// Almost definitely want gui to be produced in the media factory, events
	// also wrap up nicely into it. could have some kind of state detection to
	// priorize a gui instance of a render instance if in the gui (like on
	// click-on-map for the map).
	//std::unique_ptr<IDisplay> create_display() const override
	//{
	//	return std::make_unique<SFMLDisplay>();
	//}

	std::unique_ptr<IWindow> create_window() override
	{
		return std::make_unique<SFMLWindow>();
	}
	std::unique_ptr<IRenderer>create_renderer(IWindow &window) override
	{
		return std::make_unique<SFMLRenderer>(window);
	}
	std::unique_ptr<IEventHandler> create_event_handler(IWindow &window) override
	{
		return std::make_unique<SFMLEventHandler>(window);
	}
};

class SDLBackend : public MediaBackend {
public:
	std::unique_ptr<IWindow> create_window() override
	{
		return std::make_unique<SDLWindow>();
	}
	std::unique_ptr<IRenderer> create_renderer(IWindow &window) override
	{
		return std::make_unique<SDLRenderer>();
	}
	std::unique_ptr<IEventHandler> create_event_handler(IWindow &window) override
	{
		return std::make_unique<SDLEventHandler>();
	}
};

class SFMLBackendFactory : public MediaBackendFactory {
public:
	std::unique_ptr<SFMLBackend> create() override
	{
		return std::make_unique<SFMLBackend>();
	}
};

class SDLBackendFactory : public MediaBackendFactory {
public:
	std::unique_ptr<SDLBackend> create() override
	{
		return std::make_unique<SDLBackend>();
	}
};
