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

#include <memory>

// Forward declarations of instance interfaces.
class IWindow;
class IRenderer;
class IEventHandler;

class MediaBackend {
	virtual ~MediaInstance() {}
	virtual std::unique_ptr<IWindow> create_window() const = 0;
	virtual std::unique_ptr<IRenderer> create_renderer() const = 0;
	virtual std::unique_ptr<IEventHandler> create_event_handler() const = 0;
};

class MediaBackendFactory {
public:
	virtual ~MediaFactory() {}
	std::unique_ptr<MediaBackend> create_instance() const = 0;
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

	std::unique_ptr<IWindow> create_window() const override
	{
		return std::make_unique<SFMLWindow>();
	}
	std::unique_ptr<IRenderer>create_renderer() const override
	{
		return std::make_unique<SFMLRenderer>();
	}
	std::unique_ptr<IEventHandler> create_event_handler() const override
	{
		return std::make_unique<SFMLEventHandler>();
	}
	IEvent create_event() const override
	{	
		return sf::Event;
	}
};

class SDLBackend : public MediaBackend {
public:
	std::unique_ptr<IWindow> create_window() const override
	{
		return std::make_unique<SDLWindow>();
	}
	std::unique_ptr<IRenderer> create_renderer() const override
	{
		return std::make_unique<SDLRenderer>();
	}
	std::unique_ptr<IEventHandler> create_event_handler() const override
	{
		return std::make_unique<SDLEventHandler>();
	}
};

class SFMLBackendFactory : public MediaBackendFactory {
public:
	std::unique_ptr<SFMLBackend> create() const override
	{
		return std::make_unique<SFMLBackend>();
	}
};

class SDLBackendFactory : public MediaBackendFactory {
public:
	std::unique_ptr<SDLBackend> create() const override
	{
		return std::make_unique<SDLBackend>();
	}
};

class IEvent
{
public:
	virtual ~IEvent() = default;
};

class SFMLEvent : public IEvent
{
public:
	sf::Event event;
};

class SDLEvent : public IEvent
{
public:
	SDL_Event event;
};

class IEventHandler {
	virtual ~IEventHandler() {}
	virtual IEvent create_event() = 0;
	virtual bool poll_event(IEvent &event) const = 0;
};

class SFMLEventHandler {
	SFMLEvent create_event() override
	{
		return sf::Event;
	}
	bool poll_event(SFMLEvent &event) const override
	{
		return _window->pollEvent(static_cast<sf::Event>(&event));
	}
};

class SDLEventHandler {
	SDLEvent create_event() override
	{
		return SDL_Event;
	}
	bool poll_event(SDL_Event event) const override
	{
		return SDL_PollEvent(static_cast<SDL_Event>(&event));
			
	}
};
