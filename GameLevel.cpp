#include "GameLevel.h"

#include <iostream>
#include <fstream>

#include "ResourceLocator.h"
#include "MiscUtils.h"
#include "EventDispatcher.h"
#include "EventObjectsColliding.h"
#include "EnemyObject.h"
#include "GameObject.h"
#include "InWorldState.h"
#include "LightSource.h"

#include "GameEvent.h"
#include "CollidableTerrain.h"
#include "SpatialHash.h"
#include "SpecialTerrainObject.h"

GameLevel::GameLevel(InWorldState* worldState)
	: m_WorldState(worldState)
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
				parseMapObject(const_cast<Tmx::Object*>(object));
			}
		}

		std::cout << "\t" << m_LightManager.getNumLights() << " lights loaded" << std::endl;
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

				int onesided = 0; 
				int slope = 0;
				
				if( properties.HasProperty("onesided" ) )
					onesided = properties.GetNumericProperty("onesided");

				if( properties.HasProperty("slope" ) )
					slope = properties.GetNumericProperty("slope");

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

void GameLevel::parseMapObject(Tmx::Object* object)
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

			sf::Uint8 r = 0;
			sf::Uint8 g = 0;
			sf::Uint8 b = 0;
			sf::Uint8 a = 255;

			if( object->GetProperties().HasProperty("r") )
				r = object->GetProperties().GetNumericProperty("r");
			if( object->GetProperties().HasProperty("g") )
				g = object->GetProperties().GetNumericProperty("g");
			if( object->GetProperties().HasProperty("b") )
				b = object->GetProperties().GetNumericProperty("b");
			if( object->GetProperties().HasProperty("brightness") )
				a = object->GetProperties().GetNumericProperty("brightness");

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
		else if(object->GetName() == "enemy")
		{
			std::cout << "\tSpawning enemy... ";

			EnemyObject* enemy = new EnemyObject(object);
			enemy->init((EventDispatcher*) m_WorldState, this);
			m_WorldState->registerObject((GameObject*)enemy);
		}
		else
		{
			std::cout << "Unkown object named: " << object->GetName() << " of type: " << object->GetType() << std::endl;
		}
	}
}

int GameLevel::getMapHeight() const
{
	return m_Map->GetHeight() * m_Map->GetTileHeight();
}

int GameLevel::getMapWidth() const
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

bool GameLevel::resolveObjectCollision(GameObject* obj, Axis::Axis axis, float xdelta, float ydelta)
{
	sf::Vector2f newLocation( obj->getLocation() );

	bool colliding = false;
	GameObject* collidingWith = nullptr;

	if( axis == Axis::X )
		newLocation.x += xdelta;
	if( axis == Axis::Y )
		newLocation.y += ydelta;

	sf::FloatRect colRect( newLocation.x, newLocation.y, obj->getCollisionRect().width, obj->getCollisionRect().height );

	if( obj->getCollisionType() == CollidesWith::ALL ||
		obj->getCollisionType() == CollidesWith::OTHERS )
	{
		for( auto& object : m_WorldState->getObjects() )
		{
			if(object == obj)
				continue;

			if( colRect.intersects( object->getCollisionRect() ) )
			{
				colliding = true;
				collidingWith = object;
				break;
			}
		}
	}
	else if( obj->getCollisionType() == CollidesWith::PLAYERS )
	{
		GameObject* player = m_WorldState->getPlayer();
		if( colRect.intersects( player->getCollisionRect() ) )
		{
			colliding = true;
			collidingWith = player;
		}
	}

	if( colliding )
	{
		EventObjectsColliding e(obj, collidingWith);
		m_Dispatcher->dispatchEvent(&e);
	}

	return colliding;
}

// RENDER / UPDATE / NOTIFY
// ----------------------------------------------------------
void GameLevel::renderLayerByName(std::string layer_name, const Camera& camera)
{
	// Sprite used for rendering.
	sf::Sprite draw_me;
	draw_me.setColor(sf::Color(m_LightLevel,m_LightLevel,m_LightLevel,255));

	int tile_w = m_Map->GetTileWidth();
	int tile_h = m_Map->GetTileHeight();

	const sf::Vector2f& cameraPosition = camera.GetPosition();
	const sf::Vector2f& cameraSize = camera.GetSize();
	int cameraRightEdge = cameraPosition.x + cameraSize.x;
	int cameraBottomEdge = cameraPosition.y + cameraSize.y;

	// Tile boundary is used to determine the specific indices of tiles that should
	// be visible within our camera boundary. Width and height are used as if they
	// were actually 'right' and 'bottom.'
	sf::IntRect tileBoundary;
	tileBoundary.left = cameraPosition.x / tile_w - 1;
	tileBoundary.width = cameraRightEdge / tile_w + 1;
	tileBoundary.top = cameraPosition.y / tile_h - 1;
	tileBoundary.height = cameraBottomEdge / tile_h + 1;

	sf::Texture* tile_tex = NULL;
	int last_tiles_id = 0;

	// Iterate each layer in the map file looking for the layer
	// we're trying to render (as passed by layer_name)
	for(size_t i = 0; i < m_Map->GetNumLayers();  i++)
	{
		// Current layer
		const Tmx::Layer* layer = m_Map->GetLayer(i);

		// When "Background" is passed, draw every layer except for collision and foreground.
		// I know this is crap, but for now this is how I'm going to allow the multiple background
		// layers to be named anything the editor wants them to be.
		if(layer_name == "Background" && layer->GetName() != "Collision" && layer->GetName() != "Foreground")
		{}
		else if(layer->GetName() != layer_name)
			continue;
	
		// We must have found our layer, render each tile, but only
		// render the tiles that will be visible to the screen.
		for(int tx = tileBoundary.left; tx < tileBoundary.width; ++tx)
		{
			for(int ty = tileBoundary.top; ty < tileBoundary.height; ++ty)
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
					draw_me.setPosition((float)(-cameraPosition.x + tx * tile_w),(float)(-cameraPosition.y + ty * tile_h));

					ResourceLocator::getDrawSurface()->draw(draw_me);    
				}
			}
		}
	}
}

void GameLevel::renderLightMap(const Camera& camera)
{
	m_LightManager.render(-camera.GetPosition().x, -camera.GetPosition().y);
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

sf::Vector2f GameLevel::getPlayerLocation() const
{
	return m_WorldState->getPlayerLocation();
}

bool GameLevel::isLocationInLevel( const sf::Vector2f& location ) const
{
	return location.x < 0 ||
		   location.y < 0 ||
		   (location.x > getMapWidth()) ||
	       (location.y > getMapHeight());
}