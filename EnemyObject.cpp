#include "EnemyObject.h"

#include "Animation.h"
#include "GameEvent.h"
#include "GameObject.h"

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
}

EnemyObject::~EnemyObject()
{}	

void EnemyObject::update(float tick_ms, GameLevel* game_level)
{
	getCurrentAnimation()->update(tick_ms);
}

void EnemyObject::init(EventDispatcher* dispatcher, GameLevel* game_level)
{
	((GameObject*)(this))->init(dispatcher, game_level);
}

void EnemyObject::notify(GameEvent* event)
{}