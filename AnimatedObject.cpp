#include "AnimatedObject.h"

#include <iostream>
#include "ResourceLocator.h"

AnimatedObject::AnimatedObject()
{
  m_CurrentAnimation = NULL;
  m_XFlipped = false;
  m_YFlipped = false;
}

AnimatedObject::~AnimatedObject()
{
  if(m_CurrentAnimation)
  {
    delete m_CurrentAnimation;
  }
}

bool AnimatedObject::registerAnimation(std::string local_name, std::string global_name)
{
  Animation* anim = ResourceLocator::requestAnimation(global_name, &m_Sprite);
  m_CurrentAnimation = anim;

  if(anim)
  {
    if(m_RegisteredAnimations.count(local_name) == 0)
    {
      m_RegisteredAnimations[local_name] = anim;
    }
    else
    {
      std::cerr << "Attempted to register two animations by the same name in AnimatedObject. " << std::endl;
      return false;
    }
  }

  return false;
}

bool AnimatedObject::setCurrentAnimation(std::string local_name)
{
  if(m_RegisteredAnimations.count(local_name))
  {
    if(m_CurrentAnimation != m_RegisteredAnimations[local_name])
    {
      m_CurrentAnimation = m_RegisteredAnimations[local_name];
      m_CurrentAnimation->reset();
      m_Sprite.setTexture(*m_CurrentAnimation->getTexture());
    }
    return true;
  }

  return false;
}

Animation* AnimatedObject::getCurrentAnimation()
{
  return m_CurrentAnimation;
}