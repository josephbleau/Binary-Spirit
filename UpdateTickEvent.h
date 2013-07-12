#ifndef UPDATETICKEVENT_H
#define UPDATETICKEVENT_H

#include "GameEvent.h"

class UpdateTickEvent : public GameEvent
{
private:
	float m_Delta;
public:
	UpdateTickEvent(float delta_ms) : GameEvent(Event::EVENT_UPDATE), m_Delta(delta_ms) {}
	~UpdateTickEvent(){}

	float getDelta(){ return m_Delta; }
};

#endif