#pragma once
#include "gamecomponent.h"
class PhysicsComponent :
	public GameComponent
{
public:
	void update(float deltaT);
};