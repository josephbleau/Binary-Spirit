#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

class GameEvent;
class EventDispatcher;

class EventListener
{
protected:
	EventDispatcher* m_Dispatcher;

public:
	void setDispatcher(EventDispatcher* dispatcher);
	EventDispatcher* getDispatcher();

	virtual void notify(GameEvent* event) = 0;

	EventListener();
	~EventListener();
};

#endif