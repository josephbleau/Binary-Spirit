#pragma once

#include "GameEvent.h"

class RenderTickEvent : public GameEvent
{
private:
  
public:
  RenderTickEvent() : GameEvent(Event::EVENT_RENDER){}
  ~RenderTickEvent(){}
};