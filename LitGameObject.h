#ifndef LITGAMEOBJECT_H
#define LITGAMEOBJECT_H

#include "LightSource.h"

class LitGameObject 
{
protected:
	LightSource* m_LightSource;

public:
	LitGameObject();
	~LitGameObject();

	void updateLightLocation(int x, int y);
};

#endif