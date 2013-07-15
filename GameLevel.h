#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <TmxParser/Tmx.h>
#include <SFML/Graphics.hpp>

#include "Camera.h"
#include "CollidableTerrain.h"
#include "CollisionDirection.h"
#include "EventListener.h"
#include "LightManager.h"
#include "SpatialHash.h"

class InWorldState;
class GameEvent;
class GameObject;

namespace LevelDebug
{
	enum Flag
	{
		kDFLAG_SHOWCOLLISION = 0x00001
	};
};

class GameLevel : public EventListener
{
private:
	sf::Int32 m_DebugFlags;

	Tmx::Map* m_Map;

	InWorldState* m_WorldState;
	LightManager m_LightManager;

	std::vector<sf::FloatRect> m_StaticCollidableRects;
	std::vector<sf::FloatRect> m_OneSidedStaticCollidableRects;
	std::vector<std::pair<sf::FloatRect, int>> m_SlopedStaticCollidableRects;

	SpatialHash<CollidableTerrain> *m_CollisionHash;
	std::vector<CollidableTerrain> m_CollidableTerrain;

	sf::Vector2f m_PlayerSpawnPoint;
	int m_LightLevel;

	void rebuildCollisionHash();
	void parseCollisionLayer(const Tmx::Layer* layer);
	void parseLightMap();
	void parseMapObject(Tmx::Object* object);
	void renderDebugCollisionMapAt(int x = 0, int y = 0);

	bool resolveCollisionWithNormal(GameObject* obj, CollidableTerrain* terrain, Axis::Axis axis, float xdelta, float ydelta);
	bool resolveCollisionWithOneSided(GameObject* obj, CollidableTerrain* terrain, float xdelta, float ydelta);
	bool resolveCollisionWithSloped(GameObject* obj, CollidableTerrain* terrain, Axis::Axis axis, float xdelta, float ydelta);

public:
	GameLevel(InWorldState* worldState);
	~GameLevel();

	void setDebugFlag(LevelDebug::Flag flag);
	void unsetDebugFlag(LevelDebug::Flag flag);
	bool checkDebugFlag(LevelDebug::Flag flag);

	void addLightSource(LightSource* light_source);
	void setLightLevel(int light_level);
	int getLightLevel();

	LightManager* getLightManager();

	int getMapHeight() const;
	int getMapWidth() const;

	std::vector<CollidableTerrain*> getNearbyCollidables(sf::FloatRect rect);
	bool resolveCollision(GameObject* obj, CollidableTerrain* collidable, Axis::Axis axis, float xdelta, float ydelta);

	void init(EventDispatcher* dispatcher, std::string map_name);

	void renderAt(int x = 0, int y = 0);
	void renderLayerByName(std::string layer_name, const Camera& camera);
	void renderLightMap(const Camera& camera);

	void notify(GameEvent* event);

	const std::vector<sf::IntRect>& getStaticCollidables();
	const sf::Vector2f& getPlayerSpawnPoint();
	bool isLocationInLevel( const sf::Vector2f& location ) const;
};

#endif