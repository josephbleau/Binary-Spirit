#ifndef RENDERTICKEVENT_H
#define RENDERTICKEVENT_H

#include "GameEvent.h"

class RenderTickEvent : public GameEvent
{
private:

public:
	RenderTickEvent() : GameEvent(Event::EVENT_RENDER){}
	~RenderTickEvent(){}
};

#endif