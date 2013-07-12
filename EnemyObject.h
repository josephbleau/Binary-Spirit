#ifndef ENEMYOBJECT_H
#define ENEMYOBJECT_H


#include "AnimatedObject.h"
#include "TmxParser\TmxObject.h"

class GameLevel;
class GameEvent;

class EnemyObject : AnimatedObject
{
private:
	std::string m_Name;

public:
	EnemyObject(Tmx::Object* description);
	~EnemyObject();

	void update(float tick_ms, GameLevel* level);
	void init(EventDispatcher* dispatcher, GameLevel* game_level);
	void notify(GameEvent* event);
};

#endif