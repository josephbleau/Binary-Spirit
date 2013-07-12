#pragma once

#include "GameState.h"
#include <TmxParser/Tmx.h>

#include "Camera.h"

class GameLevel;
class GameObject;
class PlayerObject;

class InWorldState : public GameState
{
private:
	GameLevel* m_GameLevel;
	PlayerObject* m_Player;
	std::vector<GameObject*> m_GameObjects;
	std::vector<sf::IntRect> m_StaticCollidables;

	sf::IntRect m_Camera;
	Camera m_RealCamera; // rename when old camera is replaced

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

	sf::IntRect getCamera() const;

	InWorldState();
	~InWorldState();
};