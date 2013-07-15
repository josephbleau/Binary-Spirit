#ifndef ENEMYBEHAVIOR_H
#define ENEMYBEHAVIOR_H

class GameLevel;
class GameObject;

class EnemyBehavior
{
private:
	GameLevel* m_GameLevel;
	GameObject* m_ParentObject;

public:
	EnemyBehavior( GameObject* parentObject, GameLevel* gameLevel );

	void Update( float ticks_ms );
};

#endif