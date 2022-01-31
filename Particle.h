#pragma once
#include "glm.hpp"
class Particle
{
private:
	glm::vec3 velocity;
	float gravity;

public:
	glm::vec3 position;
	float rotation;
	float lifeLength;
	float lifeLeft;
	float scale;
	//Returns whether the particle is still alive after the update
	bool update(double deltaTime);

	Particle(glm::vec3 pos, glm::vec3 vel, float gravForce, float lifetime, float rotation, float scale);
	
};

