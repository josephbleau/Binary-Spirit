#pragma once

#include <SFML/Graphics.hpp>

class GameObject;

class Camera
{
private:
	bool m_IsObjectLocked;
	bool m_IsEdgeLocked;
	bool m_FreeLookOn;
	bool m_IsSeekingObject;

	GameObject* m_TrackedObject;

	sf::Vector2f m_ScreenSize;
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

	void SetPosition(unsigned x, unsigned y);
	sf::Vector2f GetPosition() const;
	sf::Vector2f GetSize() const;
	
	void Update(float delta = 0.0f);
};