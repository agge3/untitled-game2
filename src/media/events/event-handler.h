#include "media/events/event-handler.h"

template <typename Event>
void EventHandler::handle(const Event& event)
{
	_window->handle_event(event);
}
