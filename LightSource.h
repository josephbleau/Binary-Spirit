#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <SFML/Graphics.hpp>

namespace LightSourceType
{
	enum Type{ LIGHT_RADIAL, LIGHT_CONICAL };
};

class LightSource
{
protected:
	sf::Vector2i m_Offset;
	sf::IntRect m_Rect;
	sf::Color m_Color;
	sf::Texture* m_Texture;

	LightSourceType::Type m_Type;

	void setBrightness(sf::Uint8 brightness);

public:
	int getX();
	int getY();
	int getW();
	int getH();

	sf::Vector2i getOffset();

	void setOffset(int x, int y);
	void setLocation(int x, int y);

	sf::IntRect getRect();
	sf::Color getColor();
	sf::Texture* getTexture();

	static LightSource* createRadialLightSource(sf::IntRect rect, sf::Color);
	static LightSource* createConicalLightSource(sf::IntRect rect, sf::Color);

	LightSource(sf::IntRect rect, sf::Color color, LightSourceType::Type type);
	~LightSource();
};

#endif