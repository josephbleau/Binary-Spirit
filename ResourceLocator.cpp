
#include <iostream>
#include <fstream>
#include <sstream>

#include "ResourceLocator.h"
#include "MiscUtils.h"

ResourceLocator* ResourceLocator::m_Instance = NULL;
sf::RenderWindow* ResourceLocator::m_DrawSurface = NULL;
std::map<std::string, sf::Texture*> ResourceLocator::m_Textures;
std::map<std::string, Tmx::Map*> ResourceLocator::m_Maps;
std::map<Tmx::Map*, std::vector<sf::Texture*>> ResourceLocator::m_MapTextures;
std::map<std::string, Animation*> ResourceLocator::m_Animations;
std::vector<Animation*> ResourceLocator::m_UniqueAnimations; 

ResourceLocator::ResourceLocator()
{

}

ResourceLocator::~ResourceLocator()
{

}

void ResourceLocator::provideDrawSurface(sf::RenderWindow* draw_surface)
{
	if(ResourceLocator::m_DrawSurface != NULL)
	{
		std::cerr << "Warning in Service ResourceLocator, draw_surface overridden in provideDrawSurface" << std::endl;
	}

	ResourceLocator::m_DrawSurface = draw_surface;
}

sf::RenderWindow* ResourceLocator::getDrawSurface()
{
	return ResourceLocator::m_DrawSurface;
}

sf::Vector2u ResourceLocator::getDrawSurfaceSize()
{
	return ResourceLocator::m_DrawSurface->getSize();
}

bool ResourceLocator::loadMapResource(std::string identifier, std::string path)
{
	if(ResourceLocator::m_Textures.count(identifier) == 0)
	{
		Tmx::Map* map = new Tmx::Map();

		map->ParseFile(path);

		if( map->HasError() )
			return false;

		ResourceLocator::m_Maps[identifier] = map;

		std::cout << "\t" << identifier << " map loading..." << std::endl;

		int tile_w = map->GetTileWidth();
		int tile_h = map->GetTileHeight();

		std::vector<sf::Texture*> textures;

		for(int i = 0; i < map->GetNumTilesets(); i++)
		{
			std::cout << "\t\tSlicing tileset " << i << " " << map->GetTileset(i)->GetName() << std::endl;
			const Tmx::Tileset* tileset = map->GetTileset(i);
			const std::string src = map->GetFilepath() + tileset->GetImage()->GetSource();

			int tileset_w = tileset->GetImage()->GetWidth();
			int tileset_h =  tileset->GetImage()->GetHeight();
			int tiles_per_row = tileset_w / tile_w;
			int tiles_per_col = tileset_h / tile_h;

			for(int y = 0; y < tiles_per_col; y++)
			{
				for(int x = 0; x < tiles_per_row; x++)
				{
					sf::Texture* texture = new sf::Texture();
					Tmx::Tile* tile = (Tmx::Tile*) tileset->GetTile((tiles_per_col * y)+x);

					sf::IntRect cliprect(x * tile_w, y * tile_h, tile_w, tile_h);
					texture->loadFromFile(src, cliprect);
					textures.push_back(texture);
				}
			}
		}

		m_MapTextures[map] = textures;
		std::cout << "\t" << identifier << " map loaded" << std::endl;
	}
	else
	{
		std::cerr << "Error loading map from atlus file, map already loaded. Entry ignored." << std::endl;
		return false;
	}

	return true;
}

void ResourceLocator::loadTextureResource(std::string identifier, std::string path)
{
	if(ResourceLocator::m_Textures.count(identifier) == 0)
	{
		sf::Texture* texture = new sf::Texture();
		bool result = texture->loadFromFile(path);
		if(result)
		{
			std::cout << "\t" << identifier << " texture loaded" << std::endl;
			ResourceLocator::m_Textures[identifier] = texture;
		}
		else
		{
			std::cerr << "\t Error loading file " << path << std::endl;
			delete texture;
		}
	}
	else 
	{
		std::cerr << "Error loading texture from atlus file, texture already loaded. Entry ignored." << std::endl;
	}
}

void ResourceLocator::loadAnimationResource(std::string identifier, std::string path, 
											int frame_width, int frame_height, int ms_between_frames,
											bool looping, int repeat_from_frame)
{
	if(ResourceLocator::m_Animations.count(identifier) == 0)
	{
		std::cout << "\t\tanimation " << identifier << "... ";
		sf::Texture* texture = new sf::Texture();
		bool result = texture->loadFromFile(path);
		if(result == false)
		{
			std::cout << " FAILED " << std::endl;
			std::cerr << "Error loading animation " << identifier << " texture path invalid " << std::endl;
			delete texture;
			return;
		}

		Animation* anim = new Animation(texture, frame_width, frame_height, ms_between_frames, looping, repeat_from_frame);
		//anim->setTexture(texture);
		m_Animations[identifier] = anim;
	}
	else
	{
		std::cerr << "Error animation from atlus file, animation already loaded. Entry ignored." << std::endl;
	}
}

void ResourceLocator::loadResources(std::string resource_file)
{
	std::ifstream atlus(resource_file);
	if(!atlus.good())
	{
		std::cerr << "Error loading atlus " << resource_file << " in loadResources" << std::endl;
	}

	std::cout << "Loading resources..." << std::endl;

	// Parse atlus file.
	std::string line;
	while(!atlus.eof())
	{
		std::getline(atlus, line);
		std::vector<std::string> tokens = MiscUtils::explode(line, ":");

		if(tokens.size() < 3)
		{
			std::cerr << "Error loading atlus, malformed line: " << line << std::endl;
			continue;
		}
		else
		{
			std::string type = tokens[0];
			std::string identifier = tokens[1];
			std::string path = tokens[2];

			if(type == "tex")
			{
				ResourceLocator::loadTextureResource(identifier, path);
			}
			else if(type == "map")
			{
				ResourceLocator::loadMapResource(identifier, path);
			}
			else if(type == "anim")
			{
				if(tokens.size() > 7)
				{
					std::stringstream ss;
					ss << tokens[3];

					int frame_width = 0;
					ss >> frame_width;
					ss.clear();

					int frame_height = 0;
					ss << tokens[4];
					ss >> frame_height;
					ss.clear();

					int ms_between_frames;
					ss << tokens[5];
					ss >> ms_between_frames;
					ss.clear();

					bool looping = false;
					ss << tokens[6];
					ss >> looping;
					ss.clear();

					int repeat_from_frame = 0;
					ss << tokens[7];
					ss >> repeat_from_frame;
					ss.clear();

					ResourceLocator::loadAnimationResource(identifier, path, frame_width, frame_height,
						ms_between_frames, looping, repeat_from_frame);
				}
				else
				{
					std::cerr << "Error, malformed animation line in atlus " << line << std::endl;
				}
			}
		}
	}
}

sf::Texture* ResourceLocator::requestTexture(std::string identifier)
{
	if(ResourceLocator::m_Textures.find(identifier) != ResourceLocator::m_Textures.end())
	{
		return ResourceLocator::m_Textures[identifier];
	}

	std::cerr << "Error requesting texture " << identifier << " does not exist" << std::endl;
	return NULL;
}

Tmx::Map* ResourceLocator::requestMap(std::string identifier)
{
	if(ResourceLocator::m_Maps.find(identifier) != ResourceLocator::m_Maps.end())
	{
		return ResourceLocator::m_Maps[identifier];
	}

	std::cerr << "Error requesting map " << identifier << " does not exist" << std::endl;
	return NULL;
}

sf::Texture* ResourceLocator::requestMapTexture(std::string identifier, int tileid)
{
	Tmx::Map* map = ResourceLocator::requestMap(identifier);
	if(map == NULL)
	{
		std::cerr << "Could not retrieve requested texture, map by identifier " << identifier << " not found" << std::endl;
		return NULL;
	}

	return ResourceLocator::requestMapTexture(map, tileid);
}

sf::Texture* ResourceLocator::requestMapTexture(Tmx::Map* map, int tileid)
{ 
	if(ResourceLocator::m_MapTextures.find(map) != ResourceLocator::m_MapTextures.end())
	{
		if(tileid >= 0 && (size_t) tileid < ResourceLocator::m_MapTextures[map].size())
		{
			return ResourceLocator::m_MapTextures[map][tileid];
		}
		else
		{
			//std::cerr << "Could not retrieve requested map texture, valid map, but invalid tileid " << tileid << std::endl;
			return NULL;
		}
	}

	return NULL;
}

Animation* ResourceLocator::requestAnimation(std::string identifier, sf::Sprite* sprite)
{
	if(ResourceLocator::m_Animations.find(identifier) != ResourceLocator::m_Animations.end())
	{
		// Unlike textures, animations are unique, in that for any given object
		// controlling an animation, it might be in a different state (frame, speed, etc)
		// and so in order to deal with this, we must create a unique instance of each animation
		// we serve up.
		Animation *new_animation = new Animation(*m_Animations[identifier]);
		new_animation->setSprite(sprite);
		return new_animation;
	}
	else
	{
		std::cerr << "Error, could not retrieve requested animation. " << std::endl;
		// return NULL;
	}

	return NULL;
}
