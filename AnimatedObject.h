#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include "GameObject.h"
#include "GameEvent.h"

class Animation;

class AnimatedObject : public GameObject
{
protected:
	Animation* getCurrentAnimation();

	bool m_XFlipped;
	bool m_YFlipped;
	bool m_Blinking;
	bool m_Visible;

	sf::Clock m_BlinkTimer;	/* Total duration of the blinking. */
	sf::Int32 m_TimeToBlink;	/* Total duration of the blinking. */
	sf::Clock m_CurrentBlink;	/* Time to hide or show */
	sf::Int32 m_TimeBetweenBlinksInMs;	/* Time to hide or show */


private:
	std::map<std::string, Animation*> m_RegisteredAnimations;
	Animation* m_CurrentAnimation;

public:
	AnimatedObject();
	virtual ~AnimatedObject();

	bool registerAnimation(std::string local_name, std::string global_name);
	bool setCurrentAnimation(std::string local_name);

	void renderAt(const Camera& camera, int brightness);

	void blinkFor(sf::Int32 blinkingTimeInMs);

	virtual void update(float tick_ms, GameLevel* game_level) = 0;
	virtual void notify(GameEvent* e) = 0;
};

#endif