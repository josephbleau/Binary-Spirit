#ifndef COLLISIONDIRECTION_H
#define COLLISIONDIRECTION_H

namespace CollisionSide
{
	enum Side{ COLLISION_NONE = 0, COLLIDE_LEFT, COLLIDE_RIGHT, COLLIDE_TOP, COLLIDE_BOTTOM };
};

namespace Axis
{
	enum Axis{ X, Y, Z };
};

#endif