#include "PlayerObject.h"

#include "GameEvent.h"
#include "EventDispatcher.h"
#include "ResourceLocator.h"

#include "ProjectileObject.h"
#include "ProjectileFiredEvent.h"

#include "RenderTickEvent.h"
#include "UpdateTickEvent.h"
#include "SFEvent.h"


#include <iostream>

PlayerObject::PlayerObject()
{
  m_CanJump = true;
  m_JumpTimer.first = 500;
  m_LastDrawLoc.x = 0;
  m_LastDrawLoc.y = 0;
  m_RunSpeed = 200;
  m_CurrentProjectileType = ProjectileType::PROJECTILE_BASIC;

  sf::IntRect collision_rect;
  collision_rect.left = 18;
  collision_rect.top = 10;
  collision_rect.width = 38;
  collision_rect.height = 52;
  setCollisionRect(collision_rect);
}

PlayerObject::~PlayerObject()
{

}

void PlayerObject::renderAt(int x, int y, int brightness)
{
  if(getCurrentAnimation())
  {
    int texture_w = m_Sprite.getTextureRect().width;
    int texture_h = m_Sprite.getTextureRect().height;

    m_Sprite.setColor(sf::Color(brightness, brightness, brightness, 255));
    m_Sprite.setPosition((float)x,(float)y);

    if(m_XFlipped)
    {
      m_Sprite.scale(-1.f,1.f);
      m_Sprite.move((float)texture_w,0.f);
      getCurrentAnimation()->render();
      m_Sprite.scale(-1.f,1.f);
    }
    else
    {
      getCurrentAnimation()->render();
    }
  }
}

void PlayerObject::renderAtTrueLocation()
{
  m_Sprite.setPosition(m_Location.x,m_Location.y);
  ResourceLocator::getDrawSurface()->draw(m_Sprite);
}

void PlayerObject::updateAnimation(float tick_ms)
{
  if(getVelocity().x < 0)
  {
    m_XFlipped = true;
  }
  else if(getVelocity().x > 0)
  {
    m_XFlipped = false;
  }

  if(m_OnSlope && getVelocity().y >= 0 && getVelocity().x != 0)
  {
    setCurrentAnimation("walking");
  }
  else if(hasLanded() && getVelocity().x != 0)
  {
    setCurrentAnimation("walking");
  }
  else if(hasNotLanded())
  {
    setCurrentAnimation("jumping");
  }
  else
  {
    setCurrentAnimation("idling");
  }

  getCurrentAnimation()->update(tick_ms); 
}

void PlayerObject::update(float tick_ms, GameLevel* level)
{
  if((sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
     sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) ||
     (!sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
     !sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
  {
    setXVelocity(0);
  }

  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
  {
    setXVelocity((float) m_RunSpeed);
  }
  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
  {
    setXVelocity((float) (-1*m_RunSpeed));
  }

  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
  {
    m_IgnoringOneSidedPlatforms = true;
  }
  else
  {
    m_IgnoringOneSidedPlatforms = false;
  }
  

  if(m_CanJump && hasLanded())
  {
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
      setYVelocity(-350);
      m_JumpTimer.second.restart();
      m_CanJump = false;
    }
  }

  if(m_CanJump == false)
  {
    if(m_JumpTimer.second.getElapsedTime().asMilliseconds() < 300)
    {
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
      {
        addVelocity(0, -1650 * tick_ms);
      }
    }
  }

  if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) == false)
  {
    m_CanJump = true;
  }

  updateLocation(tick_ms, level);
  m_Sprite.setPosition(m_Location);
  updateAnimation(tick_ms);
}

void PlayerObject::handleEvents(sf::Event* event)
{
  if(event->type == sf::Event::KeyPressed)
  {
    if(event->key.code == sf::Keyboard::Space)
    {
      ProjectileObject* projectile = new ProjectileObject(m_CurrentProjectileType);
      projectile->init(getDispatcher(), m_GameLevel);
      projectile->setLocation(getLocation().x, getLocation().y);
      
      if(m_XFlipped)
      {
        projectile->setVelocity(-400,0);
      }
      else
      {
        projectile->setVelocity(400, 0);
      }

      ProjectileFiredEvent e(projectile);
      getDispatcher()->dispatchEvent(&e);
    }
  }
}

void PlayerObject::init(EventDispatcher* dispatcher, GameLevel* game_level)
{
  ((GameObject*)(this))->init(dispatcher, game_level);

  registerAnimation("jumping", "robo_jump");
  registerAnimation("walking", "robo_walk");
  registerAnimation("idling", "robo_idle");

  setCurrentAnimation("walking");
}

void PlayerObject::notify(GameEvent* event)
{
  switch(event->getEventType())
  {
    case Event::EVENT_RENDER:
    {
      break;
    }
    case Event::EVENT_UPDATE:
    {
      float delta = ((UpdateTickEvent*) event)->getDelta();
    
      break;
    }
    case Event::EVENT_SFEVENT:
    {
      sf::Event* sfevent = ((SFEvent*) event)->getEvent();
      handleEvents(sfevent);
      break;
    }
  }
}

