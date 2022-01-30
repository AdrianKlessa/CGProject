#pragma once

class ParticleSystem {
private:
	static std::list<ParticleGroup> particleGroups;

public:
	static void init();

	static void renderParticles(glm::mat4 cameraMatrix);

	//Adds a new particle group to the list
	static void addGroup(ParticleGroup particleGroup);
	//Updates the state of all particles given a change in time
	static void update(double deltaTime);


};

