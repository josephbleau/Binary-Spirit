#include "GameLevel.h"

#include <iostream>
#include <fstream>

#include "ResourceLocator.h"
#include "MiscUtils.h"
#include "EventDispatcher.h"
#include "GameObject.h"
#include "LightSource.h"

#include "GameEvent.h"
#include "CollidableTerrain.h"
#include "SpatialHash.h"
#include "SpecialTerrainObject.h"

GameLevel::GameLevel()
{
  m_DebugFlags = 0;
//  MiscUtils::set_flag(LevelDebug::kDFLAG_SHOWCOLLISION, m_DebugFlags);
  m_Map = NULL;
}

GameLevel::~GameLevel()
{

}

// DEBUG FLAGS
// ----------------------------------------------------------
void GameLevel::setDebugFlag(LevelDebug::Flag flag)
{
  MiscUtils::set_flag(flag, m_DebugFlags);
}

void GameLevel::unsetDebugFlag(LevelDebug::Flag flag)
{
  MiscUtils::unset_flag(flag, m_DebugFlags);
}

bool GameLevel::checkDebugFlag(LevelDebug::Flag flag)
{
  return MiscUtils::is_flag_set(flag, m_DebugFlags);
}


// MAP LOADING
// ----------------------------------------------------------

void GameLevel::init(EventDispatcher* dispatcher, std::string map_name)
{
  m_Map = ResourceLocator::requestMap(map_name);

  if(m_Map)
  {
    std::cout << "Preparing level " << map_name << std::endl;    
    
    int light_level = m_Map->GetProperties().GetNumericProperty("light_level");
    std::cout << "\tLight level: " << light_level << std::endl;
    setLightLevel(light_level);

    m_CollisionHash = new SpatialHash<CollidableTerrain>(
      (float) (m_Map->GetWidth() * m_Map->GetTileWidth()),
      (float) (m_Map->GetHeight() * m_Map->GetTileHeight()),
      1.f,
      1.f
    );

    // Special Tile Layers
    std::cout << "\tParsing special tile layers..." << std::endl;
    for(int i = 0; i < m_Map->GetNumLayers(); i++)
    {
      const Tmx::Layer* layer = m_Map->GetLayer(i);
      
      if(layer->GetName() == "Collision")
      {
        parseCollisionLayer(layer);
        continue;
      }
    }

    // Object Groups
    std::cout << "\tParsing object groups..." << std::endl;
    for(int i = 0; i < m_Map->GetNumObjectGroups(); i++)
    {
      const Tmx::ObjectGroup* object_group = m_Map->GetObjectGroup(i);
      for(int j = 0; j < object_group->GetNumObjects(); j++)
      {
        const Tmx::Object* object = object_group->GetObject(j);
        parseMapObject(object);
      }
    }
    
    std::cout << "\t\t" << m_LightManager.getNumLights() << " lights loaded" << std::endl;
  }

  rebuildCollisionHash();
  std::cout << "\t" << m_CollisionHash->getNumItems() << " total collidables" << std::endl;
  setDispatcher(dispatcher);
  dispatcher->registerListener(this);
}

void GameLevel::parseCollisionLayer(const Tmx::Layer* layer)
{
  if(layer == 0)
  {
    std::cerr << "Error in parseCollisionLayer, invalid layer" << std::endl;
    return;
  }

  int tile_w = m_Map->GetTileWidth();
  int tile_h = m_Map->GetTileHeight();

  for(int x = 0; x < layer->GetWidth(); x++)
  {
    for(int y = 0; y < layer->GetHeight(); y++)
    {
      const Tmx::MapTile& tile = layer->GetTile(x, y);

      if(tile.id != 0)
      {
        int tileset_i = tile.tilesetId;
        const Tmx::Tileset *tileset = m_Map->GetTileset(tileset_i);
        const Tmx::PropertySet& properties = tileset->GetTile(tile.id)->GetProperties();

        int onesided = properties.GetNumericProperty("onesided");
        int slope = properties.GetNumericProperty("slope");

        sf::FloatRect rect;
        rect.left = (float) x * tile_w;
        rect.top = (float) y * tile_h;

        if(onesided == 0)
        {
          rect.width = (float) tile_w;
          rect.height = (float) tile_h;
          if(slope == 1 || slope == -1)
          {
            m_CollidableTerrain.push_back(CollidableTerrain(rect, CollidableTerrainType::TERRAIN_TYPE_SLOPED));
            m_CollidableTerrain[m_CollidableTerrain.size()-1].m_Sloped = slope;
          }
          else
          {
            m_StaticCollidableRects.push_back(rect);
            m_CollidableTerrain.push_back(CollidableTerrain(rect, CollidableTerrainType::TERRAIN_TYPE_NORMAL));
          }
        }
        else if(onesided == 1)
        {
          rect.width = (float) tile_w;
          rect.height = (float) tile_h/8;
          m_OneSidedStaticCollidableRects.push_back(rect);
          m_CollidableTerrain.push_back(CollidableTerrain(rect, CollidableTerrainType::TERRAIN_TYPE_ONESIDED));
        }
      }
    }
  }
}

void GameLevel::rebuildCollisionHash()
{
  m_CollisionHash->clear();
  for(size_t i = 0; i < m_CollidableTerrain.size(); i++)
  {
    CollidableTerrain& collidable = m_CollidableTerrain[i]; 
    m_CollisionHash->insert(
      &collidable,
      collidable.m_Rect.left,
      collidable.m_Rect.top,
      collidable.m_Rect.width,
      collidable.m_Rect.height
    );
  }
}

std::vector<CollidableTerrain*> GameLevel::getNearbyCollidables(sf::FloatRect rect)
{
  return m_CollisionHash->getItems(rect.left, rect.top, rect.width, rect.height);
}
  
void GameLevel::parseMapObject(const Tmx::Object* object)
{
  if(object)
  {
    if(object->GetName() == "player_spawn")
    {
      m_PlayerSpawnPoint.x = (float) object->GetX();
      m_PlayerSpawnPoint.y = (float) object->GetY();

      std::cout << "\t\tPlayer spawn point found" << std::endl;
    }
    else if(object->GetName() == "special_terrain")
    {
      if(object->GetType() == "moving")
      {
        int gid = object->GetGid();
        sf::Texture* texture = ResourceLocator::requestMapTexture(this->m_Map, gid);
        
        sf::Vector2f location;
        location.x = (float) object->GetX();
        location.y = (float) object->GetY();

        sf::IntRect collision_rect;
        collision_rect.left = collision_rect.top = 0;
        collision_rect.width = object->GetWidth();
        collision_rect.height = object->GetHeight();

        SpecialTerrainObject* moving_platform = 
          new SpecialTerrainObject(
            SpecialTerrainType::STERRAIN_MOVING,
            texture,
            location,
            collision_rect
          );

        
      }
    }
    else if(object->GetName() == "light")
    {
      LightSource *ls = NULL;

      int x = object->GetX();
      int y = object->GetY();
      int w = object->GetWidth();
      int h = object->GetHeight();
      
      sf::Uint8 r = object->GetProperties().GetNumericProperty("r");
      sf::Uint8 g = object->GetProperties().GetNumericProperty("g");
      sf::Uint8 b = object->GetProperties().GetNumericProperty("b");
      sf::Uint8 a = object->GetProperties().GetNumericProperty("brightness");
      
      // If somebody leaves brightness property blank it will be set to zero, we want the default brightness to be max if it's not set
      // Although this is admittedly counter-intuitive, and I apologize if you are now reading this comment cursing at me.
      if(a == 0) 
        a = 255;

      std::string light_type = object->GetType();

      if(light_type == "radial")
      {
        m_LightManager.addLight(LightSource::createRadialLightSource(sf::IntRect(x,y,w,h), sf::Color(r,g,b,a)));
      }
      else if(light_type == "cone")
      {
        m_LightManager.addLight(LightSource::createConicalLightSource(sf::IntRect(x,y,w,h), sf::Color(r,g,b,a)));
      }
    }
    else
    {
      std::cout << "Unkown object named: " << object->GetName() << " of type: " << object->GetType() << std::endl;
    }
  }
}

int GameLevel::getMapHeight()
{
  return m_Map->GetHeight() * m_Map->GetTileHeight();
}

int GameLevel::getMapWidth()
{
  return m_Map->GetWidth() * m_Map->GetTileWidth();
}

// OBJECTS / COLLISION
// ----------------------------------------------------------
bool GameLevel::resolveCollision(GameObject* obj, CollidableTerrain* terrain, Axis::Axis axis, float xdelta, float ydelta)
{
  switch(terrain->m_Type)
  {
    case CollidableTerrainType::TERRAIN_TYPE_NORMAL:
    {
      return resolveCollisionWithNormal(obj, terrain, axis, xdelta, ydelta);
      break;
    }
    case CollidableTerrainType::TERRAIN_TYPE_SLOPED:
    {
      return resolveCollisionWithSloped(obj, terrain, axis, xdelta, ydelta);
      break;
    }
    case CollidableTerrainType::TERRAIN_TYPE_ONESIDED:
    {
      return resolveCollisionWithOneSided(obj, terrain, xdelta, ydelta);
      break;
    }
    default:
    {
      break;
    }
  }

  return false;
}

bool GameLevel::resolveCollisionWithNormal(GameObject* obj, CollidableTerrain* terrain, Axis::Axis axis, float xdelta, float ydelta)
{

  sf::FloatRect r1 = obj->getCollisionRect();
  r1.top  += ydelta;
  r1.left += xdelta; 

  sf::FloatRect r2 = terrain->m_Rect;

  if(r1.intersects(r2))
  {
    if(axis == Axis::X)
    {
      r1.left -= xdelta;
      if(r1.intersects(r2) == false && !obj->isOnSlope())
      {
        return true;
      }

      r1.left+= xdelta*2;
      if(r1.intersects(r2) == false && !obj->isOnSlope())
      {
        return true;
      }
    }

    if(axis == Axis::Y)
    {
      r1.top -= ydelta;
      if(r1.intersects(r2) == false )
      {
        return true;
      }

      r1.top += ydelta*2;
      if(r1.intersects(r2) == false && !obj->isOnSlope())
      {
        return true;
      }
    }
  }
  
  return false;
}

bool GameLevel::resolveCollisionWithOneSided(GameObject* obj, CollidableTerrain* terrain, float xdelta, float ydelta)
{
  sf::FloatRect r1 = obj->getCollisionRect();
  r1.top += ydelta;
  r1.left += xdelta;

  sf::FloatRect r2 = terrain->m_Rect;

  if(r1.intersects(r2))
  {
    r1.height -= ydelta;
    if(r1.intersects(r2) == false && !obj->isIgnoringOneSiders())
    {
      return true;
    }
  }
  
  return false;
}

bool GameLevel::resolveCollisionWithSloped(GameObject* obj, CollidableTerrain* terrain, Axis::Axis axis, float xdelta, float ydelta)
{
  sf::FloatRect r1 = obj->getCollisionRect();
  
  r1.left += xdelta;
  r1.top += ydelta;
  r1.height += 4;

  sf::FloatRect r2 = terrain->m_Rect;
  int slope = terrain->m_Sloped;

  if(r1.intersects(r2))
  {
    float diff = r2.left - r1.left;

    obj->setOnSlope(true);

    if(axis == Axis::X )
    {  
      if(diff <= 0)
      {
        if(slope == 1)
        {
          if(r1.top  >= r2.top  + diff - 15 && obj->getVelocity().y >= 0)
          {
            obj->setLocation(obj->getLocation().x, r2.top  + diff - 15);
          }
        }
        else if(slope == -1 )
        {
          if(r1.top >= r2.top - r2.height - diff - 15 && obj->getVelocity().y >= 0)
          {
            obj->setLocation(obj->getLocation().x, r2.top - r2.height - diff - 15);
          }
        }
      }
    }
    else
    {
      if(slope == 1)
      {
        if(r1.top  >= r2.top  + diff - 15 && obj->getVelocity().y >= 0)
        {
          return true;
        }
      }
      else if(slope == -1 )
      {
        if(r1.top >= r2.top - r2.height - diff - 15 && obj->getVelocity().y >= 0)
        {
          return true;
        }
      }
    }
  }
  

  return false;
}

// RENDER / UPDATE / NOTIFY
// ----------------------------------------------------------
void GameLevel::renderLayerByName(std::string layer_name, sf::IntRect camera_bounds)
{
  sf::Sprite draw_me;
  draw_me.setColor(sf::Color(m_LightLevel,m_LightLevel,m_LightLevel,255));

  int tile_w = m_Map->GetTileWidth();
  int tile_h = m_Map->GetTileHeight();
  
  int x = -camera_bounds.left;
  int y = -camera_bounds.top;
  camera_bounds.left /= tile_w;
  camera_bounds.top /= tile_h;
  camera_bounds.width /= tile_w;
  camera_bounds.height /= tile_h;
  camera_bounds.height+=2;
  camera_bounds.width+=2;

  sf::Texture* tile_tex = NULL;
  int last_tiles_id = 0;

  for(size_t i = 0; i < m_Map->GetNumLayers();  i++)
  {
    const Tmx::Layer* layer = m_Map->GetLayer(i);
    
    if(layer->GetName() != layer_name)
      continue;

    for(int tx = camera_bounds.left; tx < camera_bounds.left + camera_bounds.width; tx++)
    {
      for(int ty = camera_bounds.top; ty < camera_bounds.top + camera_bounds.height; ty++)
      { 
        if(tx >= m_Map->GetWidth() ||
           ty >= m_Map->GetHeight())
           continue;

        const Tmx::MapTile& tile = layer->GetTile(tx, ty);
        int tileset_index = layer->GetTileTilesetIndex(tx, ty);
        
        if(tileset_index == -1)
          continue;

        int tile_id_offset = m_Map->GetTileset(tileset_index)->GetFirstGid()-1;
        int this_tiles_id = tile_id_offset + tile.id;

        if(this_tiles_id == 0)
        {
          continue;
        }

        if(this_tiles_id != last_tiles_id)
        {
          tile_tex = ResourceLocator::requestMapTexture(m_Map, this_tiles_id);
          last_tiles_id = this_tiles_id;
        }
        

        if(tile_tex)
        {   
          draw_me.setTexture(*tile_tex);
          draw_me.setPosition((float)(x + tx * tile_w),(float)(y + ty * tile_h));

          ResourceLocator::getDrawSurface()->draw(draw_me);    
        }
      }
    }
  }
}

void GameLevel::renderLightMap(sf::IntRect camera_bounds)
{
  int tile_w = m_Map->GetTileWidth();
  int tile_h = m_Map->GetTileHeight();
  
  int x = -camera_bounds.left;
  int y = -camera_bounds.top;
  camera_bounds.left /= tile_w;
  camera_bounds.top /= tile_h;
  camera_bounds.width /= tile_w;
  camera_bounds.height /= tile_h;
  camera_bounds.height+=2;
  camera_bounds.width+=2;

  m_LightManager.render(x, y);
}

void GameLevel::renderDebugCollisionMapAt(int x, int y)
{
  for(size_t i = 0; i < m_StaticCollidableRects.size(); i++)
  {
    sf::RectangleShape shape;
    shape.setSize(sf::Vector2f((float) m_StaticCollidableRects[i].width, (float) m_StaticCollidableRects[i].height));
    shape.setPosition((float) m_StaticCollidableRects[i].left, (float) m_StaticCollidableRects[i].top);
    shape.move((float) x, (float) y);
    
    ResourceLocator::getDrawSurface()->draw(shape);
  }
}

void GameLevel::notify(GameEvent* event)
{

}

// ACCESSORS
// ----------------------------------------------------------
const sf::Vector2f& GameLevel::getPlayerSpawnPoint()
{
  return m_PlayerSpawnPoint;
}

void GameLevel::addLightSource(LightSource* light_source)
{
  m_LightManager.addLight(light_source);
}

int GameLevel::getLightLevel()
{
  return m_LightLevel;
}

void GameLevel::setLightLevel(int light_level)
{
  m_LightLevel = light_level;
  m_LightManager.setBaseColor(sf::Color(light_level, light_level, light_level, 255));
}

LightManager* GameLevel::getLightManager()
{
  return &m_LightManager;
}