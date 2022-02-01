#include "Particle.h"
#include "glm.hpp"

	//Returns whether the particle is still alive after the update
	bool Particle::update(double deltaTime) {
		this->velocity.y -= this->gravity * deltaTime;
		this->velocity = velocity - velocity * drag;
		glm::vec3 posChange = velocity;
		posChange *= deltaTime;
		this->position += posChange;
		this->lifeLeft -= deltaTime;
		return (lifeLeft > 0);
	}

	Particle::Particle(glm::vec3 pos, glm::vec3 vel, float gravForce, float drag, float lifetime, float rotation, float scale) {
		this->position = pos;
		this->drag = drag;
		this->velocity = vel;
		this->gravity = gravForce;
		this->lifeLength = lifetime;
		this->rotation = rotation;
		this->scale = scale;
		this->lifeLeft = lifetime;
	}
