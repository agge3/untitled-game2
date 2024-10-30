using FactoryPtr = std::unique_ptr<BackendFactory>();

/*
* OpenGLFactory
* SFMLFactory
* SDLFactory
* /

App::App(FactoryPtr factory) :
	_factory(std::move(factory))
{
	_window = _factory->make_window();
	_renderer = _factory->make_renderer();
	_event_handler = _factory->make_event_handler();
}
