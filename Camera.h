#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>

#include "EventListener.h"

class GameObject;
class GameEvent;

class Camera : public EventListener
{
private:
	bool m_IsObjectLocked;
	bool m_IsEdgeLocked;
	bool m_FreeLookOn;
	bool m_IsSeekingObject;

	GameObject* m_TrackedObject;

	sf::Vector2f m_ScreenSize;
	sf::Vector2f m_WorldSize;
	sf::Vector2f m_Position;
	sf::Vector2f m_TargetPosition;

	int m_PanSpeed;
	int m_TargetSnapDistance;

public:
	Camera(sf::Vector2u screenSize, GameObject* trackObject = nullptr);
	~Camera();

	void SetEdgeLock(bool edgeLock);
	void SetObjectLock(bool objectLock);

	bool IsEdgeLocked() const;
	bool IsObjectLocked() const;

	bool IsFreeLookOn() const;
	void EnableFreeLook();
	void DisableFreeLook();

	void SetTrackedObject(GameObject* object);
	void SetSeekObject(GameObject* object);

	void PanPosition(float x, float y);
	void SetPosition(float x, float y);
	sf::Vector2f GetPosition() const;
	sf::Vector2f GetSize() const;

	void SetWorldSize(float x, float y);
	
	void Update(float delta = 0.0f);

	void notify(GameEvent* event);
};

#endif