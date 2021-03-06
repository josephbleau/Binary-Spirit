#ifndef SPECIALTERRAINOBJECT_H
#define SPECIALTERRAINOBJECT_H

#include <SFML/Graphics.hpp>

#include "GameObject.h"

namespace SpecialTerrainType
{
	enum Type 
	{
		STERRAIN_MOVING
	};
}

class SpecialTerrainObject : public GameObject
{
	sf::Texture* m_Texture;
	SpecialTerrainType::Type m_Type;

public:
	SpecialTerrainObject(SpecialTerrainType::Type type, sf::Texture* texture, 
		sf::Vector2f location, sf::IntRect collision_rect);

	~SpecialTerrainObject();

	SpecialTerrainType::Type getType();

	void update(float tick_ms, GameLevel* game_level);
	void notify(GameEvent* event);
	void renderAt(const Camera& camera, int brightness);
	void renderAtTrueLocation();
};

#endif