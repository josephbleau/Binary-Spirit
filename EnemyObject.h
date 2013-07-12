#ifndef ENEMYOBJECT_H
#define ENEMYOBJECT_H

#include "AnimatedObject.h"

class GameLevel;
class GameEvent;

class EnemyObject : AnimatedObject
{
public:
	void renderAt(const Camera& camera, int brightness = 255);
	void renderAtTrueLocation();

	void update(float tick_ms, GameLevel* level);
	void init(EventDispatcher* dispatcher, GameLevel* game_level);
	void notify(GameEvent* event);
};

#endif