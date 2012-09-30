#include "LightSource.h"

#include "ResourceLocator.h"

int LightSource::getX()
{
  return m_Rect.left;
}

int LightSource::getY()
{
  return m_Rect.top;
}

int LightSource::getW()
{
  return m_Rect.width;
}

int LightSource::getH()
{
  return m_Rect.height;
}

sf::IntRect LightSource::getRect()
{
  return LightSource::m_Rect;
}

sf::Color LightSource::getColor()
{
  return m_Color;
}

sf::Texture* LightSource::getTexture()
{
  return m_Texture;
}

sf::Vector2i LightSource::getOffset()
{
  return m_Offset;
}

void LightSource::setOffset(int x, int y)
{
  m_Offset.x = x;
  m_Offset.y = y;
}

void LightSource::setLocation(int x, int y)
{
  m_Rect.left = x;
  m_Rect.top = y;
}

void LightSource::setBrightness(sf::Uint8 brightness)
{
  m_Color.a = brightness;
}

LightSource* LightSource::createRadialLightSource(sf::IntRect rect, sf::Color color)
{
  LightSource *light_source = new LightSource(rect, color, LightSourceType::LIGHT_RADIAL);
  return light_source;
}

LightSource* LightSource::createConicalLightSource(sf::IntRect rect, sf::Color color)
{
  LightSource *light_source = new LightSource(rect, color, LightSourceType::LIGHT_CONICAL);
  return light_source;
}

LightSource::LightSource(sf::IntRect rect, sf::Color color, LightSourceType::Type type)
{
  m_Rect = rect;
  m_Color = color;
  m_Type = type;

  switch(m_Type)
  {
    case LightSourceType::LIGHT_RADIAL:
    {
      m_Texture = ResourceLocator::requestTexture("lightsource_radial");
      break;
    }
    case LightSourceType::LIGHT_CONICAL:
    {
      m_Texture = ResourceLocator::requestTexture("lightsource_conical");
      break;
    }
  }
}

LightSource::~LightSource()
{

}