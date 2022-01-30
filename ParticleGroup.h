#pragma once

#include "ParticleTypeEnum.h"
class ParticleGroup
{
private:
	float particlePerSecond; //How many are created per second
	float speed; //The initial speed of the particles
	float gravityStrength; //How much gravity affects the particles
	float lifeLength; //Lifetime of the particles


public:

	std::list<Particle> particleList;
	particleType type; //The type of the particle (used for choosing the texture)
	ParticleGroup(float particlePerSecond, float speed, float gravityStrength, float lifeLength, particleType type);

	//The center is where the particles will be created
	//To be called every frame
	void generateParticles(glm::vec3 center, double deltaTime);

	void emitParticle(glm::vec3 center);
	void update(double deltaTime);
};

