#include <SFML/Graphics.hpp>

#include "ResourceLocator.h"
#include "GameStateManager.h"
#include "InWorldState.h"

#include <iostream>

int main(int argc, char* argv[]) 
{
	sf::RenderWindow app(sf::VideoMode(1024, 768), "Binary Spirit"); 
	//app.setVerticalSyncEnabled(true);

	std::string initialLevel = "test";
	std::string currentPath = argv[0];
	currentPath = currentPath.substr(0, currentPath.find_last_of("\\")) + "\\"; 

	ResourceLocator::provideDrawSurface(&app);
	ResourceLocator::loadResources(currentPath);

	if( argc > 1 )
	{
		std::string mapFileName;
		for( int i = 1; i < argc; ++i )
		{
			mapFileName.append(argv[i]);
		}

		if( ResourceLocator::loadMapResource( "__argmap", currentPath + mapFileName ) )
			initialLevel = "__argmap";
	}

	sf::Event e;
	sf::Clock timer;

	sf::Int64 since_last_frame_ms = 0;
	sf::Int64 frame_start_ms = 0;
	float delta_ms = 0;

	InWorldState* state = new InWorldState();
	state->init( initialLevel );

	GameStateManager gsm(state);

	while(app.isOpen())
	{
		since_last_frame_ms = timer.getElapsedTime().asMilliseconds() - frame_start_ms;
		frame_start_ms = timer.getElapsedTime().asMilliseconds();
		delta_ms = (float) since_last_frame_ms / 1000.f;

		if(delta_ms > 0.005)
			delta_ms = 0.005f;

		while(app.pollEvent(e))
		{
			if(e.type == sf::Event::Closed)
			{
				app.close();
			}

			gsm.getCurrentState()->handleEvents(&e);
		}

		gsm.getCurrentState()->update(delta_ms);
		app.clear( sf::Color::Black );
		gsm.getCurrentState()->render();
		app.display();
	}

	return 0;
}