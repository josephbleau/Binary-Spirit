#ifndef PROJECTILEOBJECT_H
#define PROJECTILEOBJECT_H

#include "AnimatedObject.h"
#include "LitGameObject.h"

class GameLevel;

namespace ProjectileType
{
	enum Type{ PROJECTILE_BASIC };
};

class ProjectileObject : public AnimatedObject, public LitGameObject
{
private:
	ProjectileType::Type m_Type;

	GameObject* m_Owner;
	bool m_DestroyedOnCollision;
	int m_Damage;

public:
	ProjectileObject(ProjectileType::Type type);
	~ProjectileObject();

	void update(float tick_ms, GameLevel* level);
	void init(EventDispatcher* dispatcher, GameLevel* game_level);
	void notify(GameEvent* event);
};

#endif