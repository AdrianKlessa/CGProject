#pragma once
#include "ParticleGroup.h"
#include <list>
#include "Particle.h"
#include "glm.hpp"

#include "glew.h"
#include "freeglut.h"
#include "src/Shader_Loader.h"
#include "src/Render_Utils.h"
#include "src/Texture.h"

class ParticleSystem {
private:
	static std::list<ParticleGroup*> particleGroups;
	static GLuint program;
	static Core::Shader_Loader shaderLoader;
	static GLuint bubbleTexture;
	static GLuint smokeTexture;
	static GLuint fireTexture;

public:
	static void ParticleSystem::init();
	static void ParticleSystem::renderParticles(glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix, glm::vec3 cameraDir, glm::vec3 cameraPos);

	//Adds a new particle group to the list
	//Used automatically when a new particle group is created
	static void ParticleSystem::addGroup(ParticleGroup &particleGroup);
	//Updates the state of all particles given a change in time
	static void ParticleSystem::update(double deltaTime);


};

