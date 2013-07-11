#pragma once

#include <SFML/Graphics.hpp>

#include "GameEvent.h"

class ClickEvent : public GameEvent
{
private:
	sf::Vector2i m_Position;

public:
	ClickEvent(int x, int y) : GameEvent(Event::EVENT_CLICK), m_Position(x,y) {}
	~ClickEvent(){}

	sf::Vector2i getPosition(){ return m_Position; }
};