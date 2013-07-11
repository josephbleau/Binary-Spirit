#pragma once

#include <vector>

class GameState;

namespace GameStateType
{
	enum Type{ };
}

class GameStateManager
{
private:
	std::vector<GameState*> m_States;
	GameState* m_CurrentState;

	int m_NextUID;

public:
	GameStateManager(GameState* state);
	~GameStateManager();

	GameState* getCurrentState();

	bool switchCurrentState(GameState* state);
	bool loadState(GameState* state);
	bool destroyState(GameState* state);

	GameState* createState(GameStateType::Type type);
};
