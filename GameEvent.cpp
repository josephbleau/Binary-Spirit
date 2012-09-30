#include "GameEvent.h"

GameEvent::GameEvent(Event::Type type)
{
  m_EventType = type;
}

GameEvent::~GameEvent()
{

}

Event::Type GameEvent::getEventType()
{
  return m_EventType;
}