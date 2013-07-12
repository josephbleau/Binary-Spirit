#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>

#include "EventDispatcher.h"
#include "EventListener.h"

class GameState : public EventDispatcher, public EventListener
{
protected:
	int m_UID;

public:
	void handleEvents(sf::Event* e);
	void render();
	void update(float tick_ms);

	virtual void onLoad() = 0;
	virtual void onDestroy() = 0;

	void setUID(int UID);
	int getUID();

	virtual void notify(GameEvent* event) = 0;

protected: 
	GameState(){}
};

#endif