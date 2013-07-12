#include "Camera.h"

#include "GameObject.h"

#include <iostream>

Camera::Camera( sf::Vector2u screenSize, GameObject* trackedObject)
	: m_IsObjectLocked(false)
	, m_IsEdgeLocked(true)
	, m_IsSeekingObject(false)
	, m_FreeLookOn(false)
	, m_TrackedObject(trackedObject)
	, m_ScreenSize(screenSize)
	, m_PanSpeed(600)
	, m_TargetSnapDistance(5)
{}

Camera::~Camera()
{}

void Camera::SetEdgeLock(bool edgeLock)
{
	m_IsEdgeLocked = edgeLock;
}

void Camera::SetObjectLock(bool objectLock)
{
	m_IsObjectLocked = objectLock;
}

void Camera::SetTrackedObject(GameObject* trackedObject)
{
	if(trackedObject == nullptr)
		return;

	m_IsObjectLocked = true;
	m_TrackedObject = trackedObject;
}

void Camera::SetSeekObject(GameObject* seekedObject)
{
	if(seekedObject == nullptr)
		return;

	m_IsObjectLocked = true;
	m_TrackedObject = seekedObject;
	m_IsSeekingObject = true;
	m_TargetPosition = sf::Vector2f((int)seekedObject->getLocation().x, (int)seekedObject->getLocation().y);
}

bool Camera::IsFreeLookOn() const
{
	return m_FreeLookOn;
}

void Camera::EnableFreeLook()
{
	m_FreeLookOn = true;
}

void Camera::DisableFreeLook()
{
	m_FreeLookOn = false;
	if( m_TrackedObject )
	{
		SetSeekObject( m_TrackedObject );
	}
}

bool Camera::IsEdgeLocked() const
{
	return m_IsEdgeLocked;
}

bool Camera::IsObjectLocked() const
{
	return m_IsObjectLocked;
}

void Camera::Update(float delta)
{
	// Camera's x-position will be one-half screen distance left of the object,
	// and the camera's y-position will be one-half screen distance above the object.
	int halfScreenWidth = m_ScreenSize.x/2;
	int halfScreenHeight = m_ScreenSize.y/2;

	if(m_FreeLookOn)
	{
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Right ) )
		{
			m_Position.x += m_PanSpeed * delta;
		}
		else if( sf::Keyboard::isKeyPressed( sf::Keyboard::Left ) )
		{
			m_Position.x -= m_PanSpeed * delta;
		}
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Up ) )
		{
			m_Position.y -= m_PanSpeed * delta;
		}
		else if( sf::Keyboard::isKeyPressed( sf::Keyboard::Down ) )
		{
			m_Position.y += m_PanSpeed * delta;
		}

		return;
	}
	else if( m_IsSeekingObject && m_TrackedObject != nullptr )
	{
		m_TargetPosition = sf::Vector2f((int)m_TrackedObject->getLocation().x, 
			                            (int)m_TrackedObject->getLocation().y);

		float xdist = (halfScreenWidth + m_Position.x) - m_TargetPosition.x;
		float ydist = (halfScreenHeight + m_Position.y) - m_TargetPosition.y;
		float mag = sqrt(xdist*xdist+ydist+ydist);
		if(mag==0)mag = 1;
		float xnorm = xdist/mag;
		float ynorm = ydist/mag;

		if( abs(xdist) <= m_TargetSnapDistance && abs(ydist) <= m_TargetSnapDistance )
		{
			m_Position = m_TargetPosition;
			m_IsSeekingObject = false;
		}
		else
		{
			m_Position.x -= xnorm * m_PanSpeed * delta;
			m_Position.y -= ynorm * m_PanSpeed * delta;
		}

		return;
	}
	else if(m_IsObjectLocked && m_TrackedObject != nullptr)
	{
		// We're object locked to a valid object, attempt to put this object
		// at the center of the screen.
		m_Position.x = m_TrackedObject->getLocation().x - halfScreenWidth;
		m_Position.y = m_TrackedObject->getLocation().y - halfScreenHeight;
	}
}

void Camera::SetPosition(unsigned x, unsigned y)
{
	m_Position.x = x;
	m_Position.y = y;
}

sf::Vector2f Camera::GetPosition() const
{
	return m_Position;
}

sf::Vector2f Camera::GetSize() const
{
	return m_ScreenSize;
}