#include "EventListener.h"
#include "EventDispatcher.h"

EventListener::EventListener()
{
  m_Dispatcher = 0;
}

EventListener::~EventListener()
{
  if(m_Dispatcher)
  {
    m_Dispatcher->unregisterListener(this);
  }
}

void EventListener::setDispatcher(EventDispatcher* dispatcher)
{
  if(m_Dispatcher != 0)
  {
    m_Dispatcher->unregisterListener(this);
  }

  m_Dispatcher = dispatcher;
  m_Dispatcher->registerListener(this);
}

EventDispatcher* EventListener::getDispatcher()
{
  return m_Dispatcher;
}