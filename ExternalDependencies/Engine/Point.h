#pragma once
#include <glm\glm.hpp>
using glm::vec4;
struct Point
{
	vec4 positionMass;
	vec4 velocity;
	vec4 force;
	vec4 unused;
	Point(vec4 pos, vec4 vel, vec4 forc, vec4 un){
		positionMass = pos;
		velocity = vel;
		force = forc;
		unused = un;
	}
};

