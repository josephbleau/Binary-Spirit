#include "SpecialTerrainObject.h"

SpecialTerrainObject::SpecialTerrainObject(SpecialTerrainType::Type type, sf::Texture* texture, 
										   sf::Vector2f location, sf::IntRect collision_rect)
{
	m_Type = type;
	m_Texture = texture;

	m_Location = location;

	m_CollisionRect.left = collision_rect.left;
	m_CollisionRect.top = collision_rect.top;
	m_CollisionRect.width = collision_rect.width;
	m_CollisionRect.height = collision_rect.height;
}

SpecialTerrainObject::~SpecialTerrainObject()
{
}

SpecialTerrainType::Type SpecialTerrainObject::getType()
{
	return m_Type;
}

void SpecialTerrainObject::update(float tick_ms, GameLevel* game_level)
{
}

void SpecialTerrainObject::renderAt(const Camera& camera, int brightness)
{
}

void SpecialTerrainObject::renderAtTrueLocation()
{
}

void SpecialTerrainObject::notify(GameEvent* event)
{
}
