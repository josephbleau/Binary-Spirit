#include "LitGameObject.h"

#include <iostream>
#include "GameObject.h"
LitGameObject::LitGameObject()
{
  m_LightSource = NULL;
}

LitGameObject::~LitGameObject()
{
  if(m_LightSource)
  {
    
  }
}

void LitGameObject::updateLightLocation(int x, int y)
{
  if(m_LightSource)
  {
    m_LightSource->setLocation(x,y);
  }
}