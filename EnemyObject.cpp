#include "EnemyObject.h"

#include "Animation.h"
#include "GameEvent.h"
#include "GameObject.h"
#include "ProjectileObject.h"
#include "ProjectileFiredEvent.h"
#include "EventDispatcher.h"
#include "GameLevel.h"

#include <iostream>

EnemyObject::EnemyObject(Tmx::Object* description)
{
	std::string name = description->GetType();
	std::cout << "" << name << std::endl;
	const Tmx::PropertySet& properties = description->GetProperties();

	if( properties.HasProperty("anim_idle") )
	{
		registerAnimation("anim_idle", properties.GetLiteralProperty("anim_idle"));
	}
	else
	{
		std::cerr << "\t\t\tEnemy " << name << " lacks anim_idle" << std::endl;
	}

	m_Location.x = description->GetX();
	m_Location.y = description->GetY();
	m_XFlipped = false;
	m_EnemyBehavior = [](float, GameLevel*){};
}

EnemyObject::~EnemyObject()
{}	

void EnemyObject::update(float tick_ms, GameLevel* game_level)
{
	getCurrentAnimation()->update(tick_ms);
	m_EnemyBehavior(tick_ms, game_level);

	// Just for demonstration, set up a behavior where the
	// enemy fires a projectile at the player.
	m_EnemyBehavior = [this](float tick_ms, GameLevel* level) {
		sf::Clock& clock = this->m_Clock;
		sf::Int32 delayBetweenBulletsInMs = 600;

		if( clock.getElapsedTime().asMilliseconds() > delayBetweenBulletsInMs )
		{
			clock.restart();
		}
		else
		{
			return;
		}

		float firingDistance = 1000;
		const sf::Vector2f& playerLocation = level->getPlayerLocation();
		
		float xdiff = (this->getLocation().x - playerLocation.x);
		float ydiff = (this->getLocation().y - playerLocation.y);
		float dist = sqrt(xdiff*xdiff+ydiff*ydiff);
		sf::Vector2f projectileHeading(xdiff/dist, ydiff/dist);

		if(dist <= firingDistance)
		{
			ProjectileObject* projectile = new ProjectileObject( ProjectileType::PROJECTILE_BASIC, CollidesWith::PLAYERS, this );
			projectile->init(getDispatcher(), m_GameLevel);
			projectile->setLocation(getLocation().x, getLocation().y);

			projectile->setVelocity( -projectileHeading.x * 300, -projectileHeading.y * 300);

			ProjectileFiredEvent e(projectile);
			this->getDispatcher()->dispatchEvent(&e);
		}
	};
}

void EnemyObject::init(EventDispatcher* dispatcher, GameLevel* game_level)
{
	((GameObject*)(this))->init(dispatcher, game_level);
}

void EnemyObject::notify(GameEvent* event)
{}