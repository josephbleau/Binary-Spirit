#ifndef INWORLDSTATE_H
#define INWORLDSTATE_H

#include "GameState.h"
#include <TmxParser/Tmx.h>

#include "Camera.h"
#include "GameLevel.h"
#include "PlayerObject.h"

class GameObject;

class InWorldState : public GameState
{
private:
	GameLevel m_GameLevel;
	PlayerObject m_Player;

	std::vector<GameObject*> m_GameObjects;
	std::vector<sf::IntRect> m_StaticCollidables;

	Camera m_Camera; 

public:
	void registerObject(GameObject* game_object);
	void unregisterObject(GameObject* game_object);

	void handleEvents(sf::Event* event);
	void update(float tick_ms);
	void render();

	void init(std::string map_name);

	void onLoad();
	void onDestroy();

	void notify(GameEvent* event);

	GameObject* getPlayer();
	sf::Vector2f getPlayerLocation() const;
	const std::vector<GameObject*>& getObjects();

	InWorldState();
	~InWorldState();
};

#endif