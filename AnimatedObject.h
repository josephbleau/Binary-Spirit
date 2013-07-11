#pragma once

#include "GameObject.h"
#include "GameEvent.h"

class Animation;

class AnimatedObject : public GameObject
{
protected:
	Animation* getCurrentAnimation();

	bool m_XFlipped;
	bool m_YFlipped;

private:
	std::map<std::string, Animation*> m_RegisteredAnimations;
	Animation* m_CurrentAnimation;

public:
	AnimatedObject();
	virtual ~AnimatedObject();

	bool registerAnimation(std::string local_name, std::string global_name);
	bool setCurrentAnimation(std::string local_name);

	virtual void update(float tick_ms, GameLevel* game_level) = 0;
	virtual void notify(GameEvent* e) = 0;
};