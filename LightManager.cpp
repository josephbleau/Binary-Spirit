#include "LightManager.h"

#include <iostream>

#include "ResourceLocator.h"
#include "LightSource.h"

LightManager::LightManager()
{
  m_LightMap.create(1024,768);
  m_LightMap.clear();
  m_BaseColor = sf::Color::Black;
}

LightManager::~LightManager()
{
}

void LightManager::setBaseColor(sf::Color color)
{
  m_BaseColor = color;
}

size_t LightManager::getNumLights()
{
  return m_LightSources.size();
}

void LightManager::update(float tick_ms)
{

}

void LightManager::updateLightMap(int x, int y)
{
  sf::Sprite draw_me;
  m_LightMap.clear(m_BaseColor);

  for(size_t i = 0; i < m_LightSources.size(); i++)
  {
    if(m_LightSources[i] == NULL)
    {
      continue;
    }

    int lw = m_LightSources[i]->getW(); 
    int lh = m_LightSources[i]->getH();
    int lx = x + m_LightSources[i]->getX() + m_LightSources[i]->getOffset().x;;
    int ly = 768 - y - m_LightSources[i]->getY() - lh + m_LightSources[i]->getOffset().y;;

    sf::Texture *tex = m_LightSources[i]->getTexture();
    int tex_w = tex->getSize().x;
    int tex_h = tex->getSize().y;

    float scale_w = (float) (1 + lw / tex_w);
    float scale_h = (float) (1 + lh / tex_h);

    draw_me.setColor(m_LightSources[i]->getColor());

    draw_me.setTexture(*tex);
    draw_me.setTextureRect(sf::IntRect(0, 0, 256, 256));
    draw_me.setPosition((float) lx,(float) ly);
    draw_me.scale(scale_w, scale_h);

    m_LightMap.draw(draw_me, sf::RenderStates(sf::BlendAdd));    

    draw_me.scale(1/scale_w, 1/scale_h);
  }
}

void LightManager::addLight(LightSource* light)
{
  if(light)
  {
    m_LightSources.push_back(light);
  }
}

void LightManager::destroyLight(LightSource* light)
{
  if(light)
  {
    auto it = std::find(m_LightSources.begin(), m_LightSources.end(), light);
    m_LightSources.erase(it);
  }
}
  
void LightManager::render(int x, int y)
{
  updateLightMap(x, y);

  sf::Sprite draw_me;
  draw_me.setColor(sf::Color::White);
  draw_me.setTexture(m_LightMap.getTexture());
  draw_me.setTextureRect(sf::IntRect(0,0,1024,1024));
  draw_me.setPosition(0,0);
  ResourceLocator::getDrawSurface()->draw(draw_me, sf::RenderStates(sf::BlendMultiply)); 
}
