#include "InWorldState.h"

#include <iostream>

#include "EventDispatcher.h"
#include "PlayerObject.h"
#include "GameLevel.h"

#include "GameEvent.h"
#include "UpdateTickEvent.h"
#include "SFEvent.h"
#include "ClickEvent.h"
#include "ProjectileFiredEvent.h"
#include "GameObjectTerrainCollision.h"
#include "ResourceLocator.h"

#include "MiscUtils.h"

InWorldState::InWorldState()
{
	m_GameLevel = new GameLevel();
	m_Player = new PlayerObject();
	m_Camera.top = 0;
	m_Camera.left = 0;
	m_Camera.width = 1024;
	m_Camera.height = 768;
}

InWorldState::~InWorldState()
{
}

void InWorldState::init(std::string map_name)
{
	registerListener(this);
	m_GameLevel->init((EventDispatcher*) this, map_name);

	m_Player->init((EventDispatcher*) this, m_GameLevel);

	const sf::Vector2f& spawn_point = m_GameLevel->getPlayerSpawnPoint();
	m_Player->setLocation(spawn_point.x, spawn_point.y);
}

void InWorldState::registerObject(GameObject* game_object)
{
	if(game_object == NULL)
		return;

	m_GameObjects.push_back(game_object);
	registerListener(game_object);
}

void InWorldState::unregisterObject(GameObject* game_object)
{
	auto it = std::find(m_GameObjects.begin(), m_GameObjects.end(), game_object);
	m_GameObjects.erase(it);

	if(game_object)
	{
		delete game_object;
		game_object = NULL;
	}
}

void InWorldState::handleEvents(sf::Event* event)
{
	if(event->type == sf::Event::KeyPressed)
	{
		if(event->key.code == sf::Keyboard::F1)
		{
			m_GameLevel->setDebugFlag(LevelDebug::kDFLAG_SHOWCOLLISION);
		}
	}
}

void InWorldState::update(float tick_ms)
{
	if(m_Player->hasNotLanded())
	{
		m_Player->addVelocity(0, 1800 * tick_ms);
	}  

	m_Player->update(tick_ms, m_GameLevel);

	m_Camera.left = (int)(m_Player->getLocation().x + m_Camera.width/2);
	m_Camera.top = (int) m_Player->getLocation().y;

	for(size_t i = 0; i < m_GameObjects.size(); i++)
	{
		m_GameObjects[i]->update(tick_ms, m_GameLevel);
	}
}

void InWorldState::render()
{
	sf::IntRect camera_offset;
	int magic_width = m_GameLevel->getMapWidth() - m_Camera.width;
	int magic_height = m_GameLevel->getMapHeight() - m_Camera.height;

	camera_offset.left = MiscUtils::max(0, m_Camera.left - m_Camera.width);
	camera_offset.left = MiscUtils::min(camera_offset.left, magic_width);
	camera_offset.top = MiscUtils::max(0, m_Camera.top - m_Camera.height/2-1);
	camera_offset.top = MiscUtils::min(camera_offset.top, magic_height);

	camera_offset.width = m_Camera.width;
	camera_offset.height = m_Camera.height;

	m_GameLevel->renderLayerByName("Background", camera_offset);

	sf::Vector2i player_offset;

	player_offset.x = (int) m_Player->getLocation().x - camera_offset.left;
	player_offset.y = (int) m_Player->getLocation().y - camera_offset.top;

	m_Player->renderAt(player_offset.x, player_offset.y, m_GameLevel->getLightLevel());

	for(size_t i = 0; i < m_GameObjects.size(); i++)
	{
		int object_offset_x = (int) (m_GameObjects[i]->getLocation().x - camera_offset.left);
		int object_offset_y = (int) (m_GameObjects[i]->getLocation().y - camera_offset.top);
		m_GameObjects[i]->renderAt(object_offset_x, object_offset_y, 255);
	}

	m_GameLevel->renderLayerByName("Foreground", camera_offset);
	m_GameLevel->renderLightMap(camera_offset);
}

void InWorldState::onLoad()
{

}

void InWorldState::onDestroy()
{

}

void InWorldState::notify(GameEvent* event)
{
	switch(event->getEventType())
	{
	case Event::EVENT_RENDER:
		{
			render();
			break;
		}
	case Event::EVENT_UPDATE:
		{
			float delta = ((UpdateTickEvent*) event)->getDelta();
			update(delta);
			break;
		}
	case Event::EVENT_SFEVENT:
		{
			sf::Event* sf_event = ((SFEvent*) event)->getEvent();

			if( sf_event->type == sf::Event::MouseButtonPressed )
			{
				sf::RenderWindow& renderWindow = *ResourceLocator::getDrawSurface();

				int xpos = m_Camera.left - (sf::Mouse::getPosition(renderWindow).x - 1024/2);
				int ypos = m_Camera.top - (sf::Mouse::getPosition(renderWindow).y - 768/2);

				ProjectileObject* projectile = new ProjectileObject( ProjectileType::PROJECTILE_BASIC );
				projectile->init(getDispatcher(), m_GameLevel);
				projectile->setLocation(xpos, ypos);

				projectile->setVelocity( 0, 0 );

				ProjectileFiredEvent e(projectile);
				getDispatcher()->dispatchEvent(&e);

				//ClickEvent clickEvent(xpos, ypos);
				//dispatchEvent(&clickEvent);
			}
			handleEvents(sf_event);
			break;
		}
	case Event::EVENT_PROJECTILE_FIRED:
		{
			GameObject* projectile =  ((ProjectileFiredEvent*) event)->getProjectile();
			registerObject(projectile);
			break;
		}
	case Event::EVENT_GAMEOBJECT_TERRAIN_COLLISION:
		{
			GameObject* delete_me =  ((GameObjectTerrainCollision*) event)->getGameObject();
			unregisterObject(delete_me);
			break;
		}
	}
}

sf::IntRect InWorldState::getCamera() const
{
	return m_Camera;
}