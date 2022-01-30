#include "Particle.h"
#include "glm.hpp"
class Particle {
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
	bool update(double deltaTime) {
		this->velocity += this->gravity * deltaTime;
		glm::vec3 posChange = velocity;
		posChange *= deltaTime;
		this->position += posChange;
		this->lifeLeft -= deltaTime;
		return (lifeLeft > 0);
	}

	Particle(glm::vec3 pos, glm::vec3 vel, float gravForce, float lifetime, float rotation, float scale) {
		this->position = pos;
		this->velocity = vel;
		this->gravity = gravForce;
		this->lifeLength = lifetime;
		this->rotation = rotation;
		this->scale = scale;
		this->lifeLeft = lifetime;
	}
};