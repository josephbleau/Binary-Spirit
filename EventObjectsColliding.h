#ifndef EVENTOBJECTSCOLLIDING_H
#define EVENTSOBJECTCOLLIDING_H

#include "GameEvent.h"
#include "GameObject.h"

class EventObjectsColliding : public GameEvent
{
public:
	GameObject* first;
	GameObject* second;

public:
	EventObjectsColliding(GameObject* first, GameObject* second) : GameEvent(Event::EVENT_OBJECTS_COLLIDING), first(first), second(second)
	{
	}

	~EventObjectsColliding(){}
};



#endif