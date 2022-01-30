#pragma once
class Particle
{
private:
	glm::vec3 position;
	glm::vec3 velocity;
	float gravity;
	float lifeLength;
	float lifeLeft;
	float rotation;
	float scale;

public:
	//Returns whether the particle is still alive after the update
	bool update(double deltaTime);

	Particle(glm::vec3 pos, glm::vec3 vel, float gravForce, float lifetime, float rotation, float scale);
	
};

