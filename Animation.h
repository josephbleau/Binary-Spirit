#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics.hpp>

class Animation
{
	sf::Sprite* m_Sprite;
	sf::Texture* m_Texture;

	int m_FrameWidth;
	int m_FrameHeight;
	int m_CurrentFrame;
	int m_TotalFrames;
	int m_RepeatFromFrame;
	int m_MsBetweenFrames;
	sf::Clock m_Timer;

	bool m_Looping;
	bool m_Active;

public:
	Animation(sf::Texture* texture, int frame_width, int frame_height, int ms_between_frames, bool looping = true, int repeat_from_frame = 0);
	Animation(const Animation&);
	~Animation();

	void update(float delta_ms);
	void render();

	void setTexture(sf::Texture* texture);

	sf::Texture* getTexture() const;
	sf::Vector2i getFrameSize() const;
	int getMsBetweenFrames() const;
	int getRepeatFromFrame() const;
	bool isLooping() const;

	void reset();
	void setSprite(sf::Sprite* sprite);
};

#endif