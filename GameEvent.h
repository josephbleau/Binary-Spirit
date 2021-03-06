#ifndef GAMEEVENT_H
#define GAMEEVENT_H

namespace Event
{
	enum Type
	{ 
		EVENT_SFEVENT, 
		EVENT_CLICK,
		EVENT_RENDER, 
		EVENT_UPDATE, 
		EVENT_PROJECTILE_FIRED,
		EVENT_GAMEOBJECT_TERRAIN_COLLISION,
		EVENT_OBJECTS_COLLIDING
	};
};

class GameEvent
{
protected:
	Event::Type m_EventType;

public:
	GameEvent(Event::Type type);
	~GameEvent();

	Event::Type getEventType();

};

#endif