#ifndef SFEVENT_H
#define SFEVENT_H

#include <SFML/Graphics.hpp>
#include "GameEvent.h"

class SFEvent : public GameEvent
{
private:
	sf::Event* m_Event;

public:
	SFEvent(sf::Event* event) : GameEvent(Event::EVENT_SFEVENT), m_Event(event){
		switch(event->type)
		{
		case sf::Event::Closed:
			break;
		}
	}
	~SFEvent(){}

	sf::Event* getEvent(){ return m_Event; }
};

#endif