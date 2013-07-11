#pragma once

#include <vector>

class EventListener;
class GameEvent;

class EventDispatcher
{
private:
	std::vector<EventListener*> m_Listeners;
	bool m_DroppedItem;

public:
	void registerListener(EventListener* listener);
	void unregisterListener(EventListener* listener);
	void dispatchEvent(GameEvent* event);

	EventDispatcher();
	~EventDispatcher();
};