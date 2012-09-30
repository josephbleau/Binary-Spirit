#pragma once

#include "AnimatedObject.h"
#include "ProjectileObject.h"

class GameLevel;
class GameEvent;
class EventDispatcher;

class PlayerObject : public AnimatedObject
{
private:
  sf::Vector2i m_LastDrawLoc;
  int m_RunSpeed;
  bool m_CanJump;

  void handleEvents(sf::Event* event);
  std::pair<sf::Int64, sf::Clock> m_JumpTimer;

  void updateAnimation(float tick_ms);

  ProjectileType::Type m_CurrentProjectileType;

public:
  PlayerObject();
  ~PlayerObject();

  void renderAt(int x, int y, int brightness = 255);
  void renderAtTrueLocation();

  void update(float tick_ms, GameLevel* level);
  void init(EventDispatcher* dispatcher, GameLevel* game_level);
  void notify(GameEvent* event);
};