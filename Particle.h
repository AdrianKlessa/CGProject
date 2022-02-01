#pragma once
#include "glm.hpp"
class Particle
{
private:
	glm::vec3 velocity;
	float gravity;
	float drag;
public:
	glm::vec3 position;
	float rotation;
	float lifeLength;
	float lifeLeft;
	float scale;
	//Returns whether the particle is still alive after the update
	bool update(double deltaTime);
	//Drag slows down the particle over time; 1 makes it stop immediately, 0 makes it not slow down at all
	Particle(glm::vec3 pos, glm::vec3 vel, float gravForce, float drag, float lifetime, float rotation, float scale);
	
};

