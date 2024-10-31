#include "media-backend/abstract-media-factory.h"

#include <memory>

int sfml_backend()
{
	SFMLBackendFactory factory;
	std::unique_ptr<SFMLBackend> backend = factory.create();

	std::unique_ptr<IWindow> window = backend->create_window();
	std::unique_ptr<IRenderer> renderer = backend->create_renderer(*window);
	std::unique_ptr<IEventHandler> event_handler = 
		backend->create_event_handler(*window);

	sf::CircleShape circle(50.f);
	circle.setFillColor(sf::Color(100, 250, 50));

	while (window->is_open()) {
		sf::Event event;
		while (event_handler->poll_event(event)) {
			if (event.type == sf::Event::Closed) {
				window->close();
			}
		
			renderer->clear();
			renderer->render();

}

int main(int argc, char **argv)
{
}
