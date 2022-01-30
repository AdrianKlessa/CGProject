#include "ParticleSystem.h"
#include <list>
#include "ParticleGroup.h"
#include "Particle.h"
#include "glm.hpp"
class ParticleSystem {
private:
	static std::list<ParticleGroup> particleGroups;

public:
	static void init() {

	}

	static void renderParticles(glm::mat4 cameraMatrix){

	}

	//Adds a new particle group to the list
	static void addGroup(ParticleGroup particleGroup) {
		particleGroups.push_back(particleGroup);
	}
	//Updates the state of all particles given a change in time
	static void update(double deltaTime) {

		for (auto& ParticleGroup : particleGroups) {
			ParticleGroup.update(deltaTime);
		}
	}


};