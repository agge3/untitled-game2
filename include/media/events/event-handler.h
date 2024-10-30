#pragma once

template <typename Event>
class IEvent {
public:
	IEvent(const Event& event);
};

class EventHandler {
public:
	EventHandler();
	void handle(const IEvent& event);
};
