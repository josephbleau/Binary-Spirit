#ifndef GAMEOBJECTTERRAINCOLLISION_H
#define GAMEOBJECTTERRAINCOLLISION_H

#include "GameEvent.h"
#include "GameObject.h"

class GameObjectTerrainCollision : public GameEvent
{
private:
	GameObject* m_GameObject;

public:

	GameObject* getGameObject(){ return m_GameObject; }

	GameObjectTerrainCollision(GameObject* game_object) : GameEvent(Event::EVENT_GAMEOBJECT_TERRAIN_COLLISION)
	{
		m_GameObject = game_object;
	}

	~GameObjectTerrainCollision(){}
};

#endif