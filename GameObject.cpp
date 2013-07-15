#include <iostream>

#include "GameObject.h"
#include "GameEvent.h"
#include "EventDispatcher.h"
#include "GameLevel.h"

#include "GameObjectTerrainCollision.h"

GameObject::GameObject() 
	: m_Sprite()
	, m_MovementEnabled(true)
	, m_Landed(false)
	, m_OnSlope(false)
	, m_IgnoringOneSidedPlatforms(false)
	, m_Location()
	, m_Velocity(0, 0)
	, m_MaxVelocity(300, 400)
	, m_FiringDirection(1,0)
	, m_CollisionRect()
	, m_GameLevel(nullptr)
{}

GameObject::~GameObject()
{
	m_Dispatcher->unregisterListener(this);
}

void GameObject::init(EventDispatcher* dispatcher, GameLevel* game_level)
{
	m_GameLevel = game_level;
	setDispatcher(dispatcher);
}

void GameObject::updateLocation(float tick_ms, GameLevel* level, bool announce_collision)
{
	if(m_MovementEnabled)
	{
		setOnSlope(false);

		std::vector<CollidableTerrain*> collidables = level->getNearbyCollidables(getCollisionRect());

		float delta_x = (m_Velocity.x * tick_ms);
		float delta_y = (m_Velocity.y * tick_ms);

		bool x_collided = false;
		bool y_collided = false;
		bool landed = false;

		for(size_t i = 0; i < collidables.size(); i++)
		{
			if(level->resolveCollision(this, collidables[i], Axis::X, delta_x, 0))
			{
				x_collided = true;
			}
			if((delta_y == 0 && m_Landed) || level->resolveCollision(this, collidables[i], Axis::Y, 0, delta_y))
			{
				y_collided = true;

				if(getVelocity().y >= 0) // landed on ground
					landed = true;
				else
					setYVelocity(1);     // hit head 
			}
		}

		if(x_collided == false)
		{
			m_LastLocation.x = m_Location.x;
			m_Location.x += delta_x;
		}

		if(landed == false && y_collided == false)
		{
			m_LastLocation.y = m_Location.y;
			m_Location.y += delta_y;
		}

		if(announce_collision && (y_collided || x_collided || m_OnSlope))
		{
			GameObjectTerrainCollision e(this);
			getDispatcher()->dispatchEvent(&e); 
		}

		setLanded(landed);

		// Default action for an item if it goes out of the level
		// is to set its velocity to nil
		if( m_GameLevel->isLocationInLevel( m_Location ) )
		{
			m_Velocity.x = 0;
			m_Velocity.y = 0;
		}
	}
}

void GameObject::revertLocation()
{
	m_Location.x = m_LastLocation.x;
	m_Location.y = m_LastLocation.y;
	m_Velocity.x = 0;
	m_Velocity.y = 0;
}

void GameObject::revertXLocation()
{
	m_Velocity.x = 0;
	m_Location.x = m_LastLocation.x;
}

void GameObject::revertYLocation()
{
	m_Velocity.y = 0;
	m_Location.y = m_LastLocation.y;
}


void GameObject::nudge(float x, float y)
{
	m_Location.x += x;
	m_Location.y += y;
	m_LastLocation.x += x;
	m_LastLocation.y += y;
}

void GameObject::setLocation(float x, float y)
{
	m_Location.x = x;
	m_Location.y = y;
}

sf::Vector2f GameObject::getLocation() const
{
	return m_Location;
}

void GameObject::setVelocity(float x, float y)
{
	m_Velocity.x = x;
	m_Velocity.y = y;
}

sf::Vector2f GameObject::getVelocity() const
{
	return m_Velocity;
}

void GameObject::addVelocity(float x, float y)
{
	if(abs(m_Velocity.x + x) <= m_MaxVelocity.x &&
		abs(m_Velocity.y + y) <= m_MaxVelocity.y)
	{
		m_Velocity.x += x;
		m_Velocity.y += y;
	}
}

void GameObject::deductVelocity(float x, float y)
{
	if(abs(m_Velocity.x - x) <= m_MaxVelocity.x &&
		abs(m_Velocity.y - y) <= m_MaxVelocity.y)
	{
		m_Velocity.x -= x;
		m_Velocity.y -= y;
	}
}

void GameObject::setXVelocity(float x)
{
	if(abs(x) < m_MaxVelocity.x)
	{
		m_Velocity.x = x;
	}
}


void GameObject::setYVelocity(float y)
{
	if(abs(y) < m_MaxVelocity.y)
	{
		m_Velocity.y = y;
	}
}

sf::FloatRect GameObject::getCollisionRect()
{
	sf::FloatRect rect;

	rect.left = m_Location.x + m_CollisionRect.left;
	rect.width = m_CollisionRect.width;


	rect.top = m_Location.y + m_CollisionRect.top;
	rect.height = m_CollisionRect.height;
	return rect;
}

void GameObject::setCollisionRect(sf::IntRect rect)
{
	m_CollisionRect.left = (float) rect.left;
	m_CollisionRect.top = (float) rect.top;
	m_CollisionRect.width =  (float) rect.width;
	m_CollisionRect.height = (float) rect.height;
}

void GameObject::flipCollisionRect()
{
	float t = m_CollisionRect.width;
	m_CollisionRect.left += t;
	m_CollisionRect.width = -1 * t;
}

void GameObject::setRotation(float a)
{
	m_Sprite.setRotation(a);
}

void GameObject::setLanded(bool landed)
{
	m_Landed = landed;
}

bool GameObject::hasNotLanded()
{
	return !m_Landed;
}

bool GameObject::hasLanded()
{
	return m_Landed;
}

void GameObject::setOnSlope(bool on_slope)
{
	m_OnSlope = on_slope;
}

bool GameObject::isOnSlope()
{
	return m_OnSlope;
}

bool GameObject::isIgnoringOneSiders()
{
	return m_IgnoringOneSidedPlatforms;
}

void GameObject::enableMovement()
{
	m_MovementEnabled = true;
}

void GameObject::disableMovement()
{
	m_MovementEnabled = false;
}