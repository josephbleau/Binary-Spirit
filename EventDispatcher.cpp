#include <algorithm>
#include <iostream>

#include "EventDispatcher.h"
#include "EventListener.h"
#include "GameEvent.h"


EventDispatcher::EventDispatcher()
{

}

EventDispatcher::~EventDispatcher()
{

}

void EventDispatcher::registerListener(EventListener* listener)
{
	if(!listener)
		return;

	auto result = std::find(m_Listeners.begin(), m_Listeners.end(), listener);
	if(result == m_Listeners.end())
	{   
		m_Listeners.push_back(listener);
	}
}

void EventDispatcher::unregisterListener(EventListener* listener)
{
	if(!listener)
		return;

	auto result = std::find(m_Listeners.begin(), m_Listeners.end(), listener);
	if(result != m_Listeners.end())
	{
		m_Listeners.erase(result);
	}
}

void EventDispatcher::dispatchEvent(GameEvent* event)
{
	for(size_t i = 0; i < m_Listeners.size(); i++)
	{
		if(m_Listeners[i])
		{
			m_Listeners[i]->notify(event);
		}
	}
}