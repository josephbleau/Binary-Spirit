#pragma once

#include "GameEvent.h"
#include "GameObject.h"

class ProjectileFiredEvent : public GameEvent
{
private:
	GameObject* m_Projectile;

public:

	GameObject* getProjectile(){ return m_Projectile; }

	ProjectileFiredEvent(GameObject* projectile) : GameEvent(Event::EVENT_PROJECTILE_FIRED)
	{
		m_Projectile = projectile;
	}

	~ProjectileFiredEvent(){}
};