#include "Animation.h"

#include <iostream>

#include "ResourceLocator.h"

Animation::Animation(sf::Texture* texture, int frame_width, int frame_height, int ms_between_frames, bool looping, int repeat_from_frame)
{
  if(texture == NULL)
  {
    std::cerr << "Warning, null texture passed to Animation" << std::endl;
  }

  m_Texture = texture;
  m_FrameWidth = frame_width;
  m_FrameHeight = frame_height; 
  std::cout << m_FrameWidth << "x" << m_FrameHeight;
  m_Looping = looping;
  m_CurrentFrame = 0;
  m_MsBetweenFrames = ms_between_frames;
  m_Active = true;
  m_TotalFrames = texture->getSize().x / m_FrameWidth;
  std::cout << " frames: " <<  m_TotalFrames <<std::endl;
  m_RepeatFromFrame = repeat_from_frame;
}

// copy ctor! :D
Animation::Animation(const Animation& animation)
{
  m_Sprite = NULL;
  m_Active = true;
  m_CurrentFrame = 0;

  m_Texture = animation.getTexture();
  m_MsBetweenFrames = animation.getMsBetweenFrames();
  m_FrameHeight = animation.getFrameSize().y;
  m_FrameWidth = animation.getFrameSize().x;
  m_Looping = animation.isLooping();
  m_RepeatFromFrame = animation.getRepeatFromFrame();

  m_TotalFrames = m_Texture->getSize().x / m_FrameWidth;
}

Animation::~Animation()
{
}

void Animation::update(float delta_ms)
{
  if(m_Active)
  {
    if(m_Timer.getElapsedTime().asMilliseconds() >= m_MsBetweenFrames)
    {
      m_Timer.restart();
      m_CurrentFrame++;
      if(m_CurrentFrame == m_TotalFrames)
      {
        if(m_Looping == false)
        {
          m_Active = false;
        }

        m_CurrentFrame = m_RepeatFromFrame;
      }
    }
  }
}

void Animation::render()
{
  sf::IntRect texture_rect;
  texture_rect.left = m_CurrentFrame * m_FrameWidth;
  texture_rect.top = 0;
  texture_rect.width = m_FrameWidth;
  texture_rect.height = m_FrameHeight;

  m_Sprite->setTextureRect(texture_rect);
  ResourceLocator::getDrawSurface()->draw(*m_Sprite);
}

void Animation::setTexture(sf::Texture* texture)
{
  m_Texture = texture;
}

sf::Texture* Animation::getTexture() const
{
  return m_Texture;
}

sf::Vector2i Animation::getFrameSize() const
{
  return sf::Vector2i(m_FrameWidth, m_FrameHeight);
}

int Animation::getMsBetweenFrames() const
{
  return m_MsBetweenFrames;
}

int Animation::getRepeatFromFrame() const
{
  return m_RepeatFromFrame;
}

bool Animation::isLooping() const
{
  return m_Looping;
}

void Animation::reset()
{
  m_Timer.restart();
  m_CurrentFrame = 0;
  m_Active = true;
}


void Animation::setSprite(sf::Sprite* sprite)
{
  m_Sprite = sprite;
  sprite->setTexture(*m_Texture);
}