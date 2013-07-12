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
	: m_GameLevel(new GameLevel())
	, m_Player(new PlayerObject())
	, m_GameObjects()
	, m_StaticCollidables()
	, m_Camera(0, 0, ResourceLocator::getDrawSurfaceSize().x, ResourceLocator::getDrawSurfaceSize().y)
	, m_RealCamera(ResourceLocator::getDrawSurfaceSize())
{
	m_RealCamera.SetTrackedObject(m_Player);
}

InWorldState::~InWorldState()
{}

void InWorldState::init(std::string map_name)
{
	registerListener(this);
	m_GameLevel->init((EventDispatcher*) this, map_name);
	m_RealCamera.SetWorldSize(m_GameLevel->getMapWidth(), m_GameLevel->getMapHeight());
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
		else if( event->key.code == sf::Keyboard::F2 )
		{
			if( m_RealCamera.IsFreeLookOn() )
				m_RealCamera.DisableFreeLook();
			else
				m_RealCamera.EnableFreeLook();
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
	for(size_t i = 0; i < m_GameObjects.size(); i++)
	{
		m_GameObjects[i]->update(tick_ms, m_GameLevel);
	}

	m_RealCamera.Update(tick_ms);
}

void InWorldState::render()
{
	// Render background layer, player will pass in front of
	// anything rendered in this layer.
	m_GameLevel->renderLayerByName("Background", m_RealCamera);

	// Render player and all game objects.
	m_Player->renderAt(m_RealCamera, m_GameLevel->getLightLevel());

	for(size_t i = 0; i < m_GameObjects.size(); i++)
	{
		m_GameObjects[i]->renderAt(m_RealCamera, 255);
	}

	// Render foreground layer, player will pass behind
	// anything rendered in this layer.
	m_GameLevel->renderLayerByName("Foreground", m_RealCamera);

	// Lights will render last and be on top of all layers.
	m_GameLevel->renderLightMap(m_RealCamera);
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

				//ProjectileObject* projectile = new ProjectileObject( ProjectileType::PROJECTILE_BASIC );
				//projectile->init(getDispatcher(), m_GameLevel);
				//projectile->setLocation(xpos, ypos);

				//projectile->setVelocity( 0, 0 );

				//ProjectileFiredEvent e(projectile);
				//getDispatcher()->dispatchEvent(&e);

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