#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SFML/Graphics.hpp>

#include "Camera.h"
#include "EventListener.h"

class GameLevel;

namespace CollidesWith 
{
	enum Type{ NONE, ALL, OTHERS, PLAYERS };
}

class GameObject : public EventListener
{
protected:
	sf::Sprite m_Sprite;

	bool m_MovementEnabled;
	bool m_Landed;
	bool m_OnSlope;
	bool m_IgnoringOneSidedPlatforms;

	sf::Vector2f m_Location;
	sf::Vector2f m_LastLocation;
	sf::Vector2f m_Velocity;
	sf::Vector2f m_MaxVelocity;
	sf::Vector2f m_FiringDirection;
	sf::FloatRect m_CollisionRect;

	GameLevel *m_GameLevel;

	CollidesWith::Type m_CollidesWith;

	sf::Clock m_InvulnClock;
	sf::Int32 m_TimeBetweenPainInMs;
	

public:
	void nudge(float x, float y);

	void setLocation(float x, float y);
	sf::Vector2f getLocation() const;

	void setVelocity(float x, float y);
	sf::Vector2f getVelocity() const;

	void addVelocity(float x, float y);
	void deductVelocity(float x, float y);

	void setXVelocity(float x);
	void setYVelocity(float y);

	sf::FloatRect getCollisionRect();
	void setCollisionRect(sf::IntRect rect);
	void flipCollisionRect();

	void init(EventDispatcher* dispatcher, GameLevel *game_level);
	void updateLocation(float tick_ms, GameLevel* level, bool announce_collision = false);

	void revertLocation();
	void revertXLocation();
	void revertYLocation();

	void setLanded(bool landed);
	bool hasLanded();
	bool hasNotLanded();
	void setOnSlope(bool on_slope);
	bool isOnSlope();
	bool isIgnoringOneSiders();

	void setRotation(float a);

	void disableMovement();
	void enableMovement();

	CollidesWith::Type getCollisionType() const;

	virtual void update(float tick_ms, GameLevel* game_level) = 0;
	virtual void notify(GameEvent* event) = 0;
	virtual void renderAt(const Camera& camera, int brightness) = 0;

	GameObject();
	virtual ~GameObject();
};

#endif