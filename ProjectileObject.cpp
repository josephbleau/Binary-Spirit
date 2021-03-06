#include "ProjectileObject.h"

#include <iostream>

#include "Animation.h"
#include "GameLevel.h"
#include "GameObject.h"
#include "EventDispatcher.h"
#include "GameObjectTerrainCollision.h"

#include "GameObjectTerrainCollision.h"

ProjectileObject::ProjectileObject(ProjectileType::Type type,  CollidesWith::Type collisionType, GameObject* owner)
	: m_Type( type )
	, m_Owner( owner )
{
	m_MovementEnabled = true;
	setCollisionRect(sf::IntRect(0,0,10,10));
	m_CollidesWith =  collisionType;
}

ProjectileObject::~ProjectileObject()
{
	if(m_LightSource)
	{
		m_GameLevel->getLightManager()->destroyLight(m_LightSource);
	}
}

void ProjectileObject::update(float tick_ms, GameLevel* level)
{
	m_Sprite.setPosition(m_Location);
	getCurrentAnimation()->update(tick_ms);
	updateLightLocation((int)getLocation().x,(int) getLocation().y);

	if( m_GameLevel->isLocationInLevel( m_Location ) )
	{
		GameObjectTerrainCollision gotc(this);
		m_Dispatcher->dispatchEvent(&gotc);
	}
	else
	{
		updateLocation(tick_ms, level, true); // has to be last, can cause ourselves to be deleted (oh my :o)
	}
}

void ProjectileObject::init(EventDispatcher* dispatcher, GameLevel* game_level)
{
	((GameObject*)(this))->init(dispatcher, game_level);

	switch(m_Type)
	{
	case ProjectileType::PROJECTILE_BASIC:
		{
			registerAnimation("shot", "proj_basic");
			setCurrentAnimation("shot");
			break;
		}
	}

	m_LightSource = LightSource::createRadialLightSource(sf::IntRect((int) getLocation().x, (int) getLocation().y, 50, 50), sf::Color::White);
	m_LightSource->setOffset(-128, -84);
	m_GameLevel->addLightSource(m_LightSource);
}

void ProjectileObject::notify(GameEvent* event)
{
	switch(event->getEventType())
	{
	case Event::EVENT_GAMEOBJECT_TERRAIN_COLLISION:
		{

		}
	}
}