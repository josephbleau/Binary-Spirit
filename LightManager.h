#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <vector>
#include <SFML/Graphics.hpp>

class LightSource;

class LightManager
{
	std::vector<LightSource*> m_LightSources;
	sf::Color m_BaseColor;
	sf::RenderTexture m_LightMap;

	void updateLightMap(int x, int y);

public:
	LightManager();
	~LightManager();

	void addLight(LightSource* light);
	void destroyLight(LightSource* light);

	void setBaseColor(sf::Color);
	size_t getNumLights();

	void update(float tick_ms);
	void render(int x, int y);
};

#endif