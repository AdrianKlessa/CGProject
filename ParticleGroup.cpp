#include "ParticleGroup.h"
#include "particleTypeEnum.h"
#include "glm.hpp"
#include <list>
#include "Particle.h"
#include "Random.h"


class ParticleGroup {
private:
	float particlePerSecond; //How many are created per second
	float speed; //The initial speed of the particles
	float gravityStrength; //How much gravity affects the particles
	float lifeLength; //Lifetime of the particles
	 

public:

	std::list<Particle> particleList;
	particleType type; //The type of the particle (used for choosing the texture)
	ParticleGroup(float particlePerSecond, float speed, float gravityStrength, float lifeLength, particleType type) {
		this->particlePerSecond = particlePerSecond;
		this->speed = speed;
		this->gravityStrength = gravityStrength;
		this->lifeLength = lifeLength;
		this->type = type;
	};

	//The center is where the particles will be created
	//To be called every frame
	void generateParticles(glm::vec3 center, double deltaTime) {
		float particlesToCreate = particlePerSecond * deltaTime;
		int count = std::floor(particlesToCreate);
		float partialParticles = particlesToCreate -count;
		for (int i = 0; i < count; i++) {
			emitParticle(center);
		}
		
		if (Random::Float(0, 1) < partialParticles) {
			emitParticle(center);
		}
	}

	void emitParticle(glm::vec3 center) {
		float dirX = Random::Float(-1, 1);
		float dirZ = Random::Float(-1, 1);
		glm::vec3 velocity = glm::vec3(dirX, 1, dirZ);
		velocity = glm::normalize(velocity);
		velocity *= speed;
		//glm::vec3 pos, glm::vec3 vel, float gravForce, float lifetime, float rotation, float scale
		Particle newParticle = Particle(center, velocity, gravityStrength, lifeLength, 0.0, 1.0);
		particleList.push_back(newParticle);
	}

	void update(double deltaTime) {
		for (auto& particle : particleList) {
			particle.update(deltaTime);
		}
	}
	
};