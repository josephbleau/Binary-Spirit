#ifndef RESOURCELOCATOR_H
#define RESOURCELOCATOR_H

#include <SFML/Graphics.hpp>
#include <TmxParser/Tmx.h>
#include "Animation.h"

class ResourceLocator 
{
private:
	ResourceLocator();
	~ResourceLocator();

	static ResourceLocator* m_Instance;

	static sf::RenderWindow* m_DrawSurface;
	static std::map<std::string, sf::Texture*> m_Textures;

	static std::map<std::string, Tmx::Map*> m_Maps;
	static std::map<Tmx::Map*, std::vector<sf::Texture*>> m_MapTextures;

	static std::map<std::string, Animation*> m_Animations;
	static std::vector<Animation*> m_UniqueAnimations; 

	static std::map<std::string, sf::Texture*> m_AnimationTextures;

public:
	static void provideDrawSurface(sf::RenderWindow* draw_surface);
	static sf::RenderWindow* getDrawSurface();
	static sf::Vector2u getDrawSurfaceSize();

	static void loadMapResource(std::string identifier, std::string path);
	static void loadTextureResource(std::string identifier, std::string path);
	static void loadAnimationResource(std::string identifier, std::string path,
		int frame_width, int frame_height, int ms_between_frames, 
		bool looping = true, int repeat_from_frame = 0);

	static void loadResources(std::string resource_file);

	static sf::Texture* requestTexture(std::string identifier);
	static Tmx::Map* requestMap(std::string identifier);

	static Animation* requestAnimation(std::string identifier, sf::Sprite* sprite);

	static sf::Texture* requestMapTexture(std::string identifier, int tileid);
	static sf::Texture* requestMapTexture(Tmx::Map* map, int tileid);
};

#endif