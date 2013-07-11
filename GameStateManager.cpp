#include "GameStateManager.h"

#include <iostream>

#include "GameState.h"

GameStateManager::GameStateManager(GameState* state)
{
	m_NextUID = 0;

	if(state)
	{ 
		state->onLoad();
		m_CurrentState = state;
		m_States.push_back(state);
	}
	else
	{
		std::cerr << "Error loading gamestate, improper opening state passed to GSM" << std::endl;
	}
}

GameStateManager::~GameStateManager()
{
	for(size_t i = 0; i < m_States.size(); i++)
	{ 
		if(m_States[i])
		{
			m_States[i]->onDestroy();
			delete m_States[i];
		}
		else
		{
			std::cerr << "Error: attempted to delete invalid state pointer in GSM dtor!" << std::endl;
		}
	}
}

GameState* GameStateManager::getCurrentState()
{
	if(!m_CurrentState)
	{
		std::cerr << "Warning, invalid current state requested from GSM" << std::endl;
	}

	return m_CurrentState;
}

bool GameStateManager::switchCurrentState(GameState* state)
{
	if(state == NULL)
	{
		std::cerr << "Error, invalid state passed into GSM's switchCurrentState" << std::endl;
		return false;
	}

	// Check this state against the ones we already know about, 
	// and if we don't already know about it, store it for later use.
	bool found = false;
	for(size_t i = 0; i < m_States.size(); i++)
	{
		if(state == m_States[i])
		{
			found = true;
			break;
		}
	}

	if(found == false)
	{
		state->onLoad();
		m_States.push_back(state);
	}

	m_CurrentState = state;

	return true;
}

GameState* GameStateManager::createState(GameStateType::Type type)
{
	GameState* state = NULL;

	/*
	switch(type)
	{
	default:
	{
	state = NULL;
	break;
	}
	}
	*/

	if(state == NULL)
	{
		std::cerr << "No valid state type provided to GSM for createState, invalid state returned" << std::endl;
	}

	return state;
}