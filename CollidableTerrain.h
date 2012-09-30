#pragma once

#include <SFML/Graphics.hpp>

namespace CollidableTerrainType
{
  enum Type
  {
    TERRAIN_TYPE_NORMAL,
    TERRAIN_TYPE_SLOPED,
    TERRAIN_TYPE_ONESIDED
  };
};

class CollidableTerrain
{
public:
  sf::FloatRect m_Rect;
  CollidableTerrainType::Type m_Type;
  int m_Sloped;

  CollidableTerrain(sf::FloatRect rect, CollidableTerrainType::Type type)
  {
    m_Rect = rect;
    m_Type = type;
    m_Sloped = 0;
  }
};