#include "media/abstract-media-factory.h"

#include <memory>

class SFMLFactory : public AbstractMediaFactory {
public:
	std::unique_ptr<AbstractSFMLInstance> construct_sfml_instance() const override
	{
		return std::make_unique<AbstractSFMLInstance>();
	}
};

class SDLFactory : public AbstractMediaFactory {
public:
	std::unique_ptr<AbstractSDLInstance> construct_sdl_instance() const override
	{
		return std::make_unique<AbstractSDLInstance>();
	}
};

class SFMLInstance : public AbstractSFMLInstance {
public:
	// xxx maybe, or maybe builder
	//std::unique_ptr<IMediaInstance> construct const override
	//{
	//	_window = construct_window();
	//	_renderer = construct_renderer();
	//	_event_handler = construct_event_handler();
	//}

	std::unique_ptr<IWindow> construct_window() const override
	{
		return std::make_unique<SFMLWindow>();
	}
	std::unique_ptr<IRenderer>construct_renderer() const override
	{
		return std::make_unique<SFMLRenderer>();
	}
	std::unique_ptr<IEventHandler> construct_event_handler() const override
	{
		return std::make_unique<SFMLEventHandler>();
	}
private:
};

class SDLInstance : public AbstractSDLInstance {
public:
	// xxx maybe, or maybe builder
	//std::unique_ptr<IMediaInstance> construct const override
	//{
	//	_window = construct_window();
	//	_renderer = construct_renderer();
	//	_event_handler = construct_event_handler();
	//}

	std::unique_ptr<IWindow> construct_window() const override
	{
		return std::make_unique<SDLWindow>();
	}
	std::unique_ptr<IRenderer> construct_renderer() const override
	{
		return std::make_unique<SDLRenderer>();
	}
	std::unique_ptr<IEventHandler> construct_event_handler() const override
	{
		return std::make_unique<SDLEventHandler>();
	}
};

class IMedia {
public:
	// everything that's in window, renderer, event_handler
};
