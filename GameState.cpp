#include "GameState.h"

#include "RenderTickEvent.h"
#include "SFEvent.h"
#include "UpdateTickEvent.h"

int GameState::getUID()
{
	return m_UID;
}

void GameState::setUID(int uid)
{
	m_UID = uid;
}

void GameState::handleEvents(sf::Event* event)
{
	if(event == NULL)
		return;

	SFEvent e(event);
	dispatchEvent(&e);
}

void GameState::render()
{
	RenderTickEvent event;
	dispatchEvent(&event);
}

void GameState::update(float delta_ms)
{
	UpdateTickEvent event(delta_ms);
	dispatchEvent(&event);
}