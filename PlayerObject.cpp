#include "PlayerObject.h"

#include "Camera.h"
#include "GameEvent.h"
#include "GameLevel.h"
#include "EventDispatcher.h"
#include "ResourceLocator.h"

#include "ProjectileObject.h"
#include "ProjectileFiredEvent.h"

#include "RenderTickEvent.h"
#include "UpdateTickEvent.h"
#include "SFEvent.h"
#include "ClickEvent.h"
#include "EventObjectsColliding.h"


#include <iostream>

PlayerObject::PlayerObject()
{
	m_CanJump = true;
	m_JumpTimer.first = 500;
	m_LastDrawLoc.x = 0;
	m_LastDrawLoc.y = 0;
	m_RunSpeed = 200;
	m_MouseLookLeft = false;
	m_CurrentProjectileType = ProjectileType::PROJECTILE_BASIC;
	m_OutOfControlTimeInMs = 200;

	sf::IntRect collision_rect;
	collision_rect.left = 18;
	collision_rect.top = 10;
	collision_rect.width = 38;
	collision_rect.height = 52;
	setCollisionRect(collision_rect);
}

PlayerObject::~PlayerObject()
{

}

void PlayerObject::updateAnimation(float tick_ms)
{
	if(m_MouseLookLeft)
	{
		m_XFlipped = true;
	}
	else if(!m_MouseLookLeft)
	{
		m_XFlipped = false;
	}

	if(m_OnSlope && getVelocity().y >= 0 && getVelocity().x != 0)
	{
		setCurrentAnimation("walking");
	}
	else if(hasLanded() && getVelocity().x != 0)
	{
		setCurrentAnimation("walking");
	}
	else if(hasNotLanded())
	{
		setCurrentAnimation("jumping");
	}
	else
	{
		setCurrentAnimation("idling");
	}

	getCurrentAnimation()->update(tick_ms); 
}

void PlayerObject::update(float tick_ms, GameLevel* level)
{
	if( m_NoControlTimer.getElapsedTime().asMilliseconds()  > m_OutOfControlTimeInMs )
	{
		if((sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
			sf::Keyboard::isKeyPressed(sf::Keyboard::A)) ||
			(!sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
			!sf::Keyboard::isKeyPressed(sf::Keyboard::A)))
		{
				setXVelocity(0);
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			setXVelocity((float) m_RunSpeed);
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			setXVelocity((float) (-1*m_RunSpeed));
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			m_IgnoringOneSidedPlatforms = true;
		}
		else
		{
			m_IgnoringOneSidedPlatforms = false;
		}  

		if(m_CanJump && hasLanded())
		{
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				setYVelocity(-350);
				m_JumpTimer.second.restart();
				m_CanJump = false;
				m_Landed = false;
			}
		}

		if(m_CanJump == false)
		{
			if(m_JumpTimer.second.getElapsedTime().asMilliseconds() < 300)
			{
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				{
					addVelocity(0, -1650 * tick_ms);
				}
			}
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) == false && sf::Keyboard::isKeyPressed(sf::Keyboard::W) == false)
		{
			m_CanJump = true;
		}
	}

	updateLocation(tick_ms, level);
	m_Sprite.setPosition(m_Location);
	updateAnimation(tick_ms);

	if( m_GameLevel->isLocationInLevel( m_Location ) )
		m_Location = m_GameLevel->getPlayerSpawnPoint();
}

void PlayerObject::handleEvents(sf::Event* event)
{
	if(event->type == sf::Event::MouseButtonPressed)
	{
		if(event->mouseButton.button == 0)
		{
			ProjectileObject* projectile = new ProjectileObject(m_CurrentProjectileType, CollidesWith::OTHERS, this);
			projectile->init(getDispatcher(), m_GameLevel);
			projectile->setLocation(getLocation().x, getLocation().y);

			projectile->setVelocity( m_FiringDirection.x * 400, m_FiringDirection.y * 400);

			ProjectileFiredEvent e(projectile);
			getDispatcher()->dispatchEvent(&e);
		}
	}
}

void PlayerObject::init(EventDispatcher* dispatcher, GameLevel* game_level)
{
	((GameObject*)(this))->init(dispatcher, game_level);

	registerAnimation("jumping", "robo_jump");
	registerAnimation("walking", "robo_walk");
	registerAnimation("idling", "robo_idle");

	setCurrentAnimation("walking");
}

void PlayerObject::notify(GameEvent* event)
{
	switch(event->getEventType())
	{
	case Event::EVENT_RENDER:
		{
			break;
		}
	case Event::EVENT_UPDATE:
		{
			float delta = ((UpdateTickEvent*) event)->getDelta();

			break;
		}
	case Event::EVENT_CLICK:
		{
			sf::Vector2i position = ((ClickEvent*) event)->getPosition();

			position.x = m_Location.x - position.x;
			position.y = m_Location.y - position.y;

			float mag = sqrt(position.x*position.x+position.y*position.y);
			float xnorm = position.x / mag;
			float ynorm = position.y / mag;

			m_FiringDirection.x = -xnorm;
			m_FiringDirection.y = -ynorm;

			break;
		}
	case Event::EVENT_OBJECTS_COLLIDING:
		{
			GameObject* first = ((EventObjectsColliding*)event)->first;
			GameObject* second = ((EventObjectsColliding*)event)->second;

			if( first == this &&
				second->getCollisionType() == CollidesWith::PLAYERS )
			{
				if( m_InvulnClock.getElapsedTime().asMilliseconds() > m_TimeBetweenPainInMs )
				{
					m_NoControlTimer.restart();
					m_InvulnClock.restart();

					if( m_MouseLookLeft )
					{
						m_Velocity.x = 100;
						m_Velocity.y = -200;
					}
					else
					{
						m_Velocity.x = -100;
						m_Velocity.y = -200;
					}
				}
			}
		}
	case Event::EVENT_SFEVENT:
		{
			sf::Event* sfevent = ((SFEvent*) event)->getEvent();

			int halfScreenWidth = ResourceLocator::getDrawSurfaceSize().x/2;
			int halfScreenHeight = ResourceLocator::getDrawSurfaceSize().y/2;

			if(sfevent->type == sf::Event::MouseMoved)
			{
				int xpos = sf::Mouse::getPosition(*ResourceLocator::getDrawSurface()).x - halfScreenWidth;
				int ypos = sf::Mouse::getPosition(*ResourceLocator::getDrawSurface()).y - halfScreenHeight;

				if(xpos < 0){
					m_MouseLookLeft = true;
				}
				else if(xpos > 0){
					m_MouseLookLeft = false;
				}
			}
			else
			{
				handleEvents(sfevent);
			}
			break;
		}
	}
}

sf::Vector2f PlayerObject::getReticuleVector() const
{
	sf::Vector2f reticule( m_FiringDirection );
	reticule.x *= 30;
	reticule.y *= 30;
	reticule.x += 1024/2;
	reticule.y += 768/2;
	return reticule;
}