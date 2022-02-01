#include "ParticleSystem.h"
#include <list>
#include "ParticleGroup.h"
#include "Particle.h"
#include "glm.hpp"

#include "glew.h"
#include "freeglut.h"
#include "src/Shader_Loader.h"
#include "src/Render_Utils.h"
#include "src/Texture.h"

 std::list<ParticleGroup*> ParticleSystem::particleGroups;
 GLuint ParticleSystem::program;
 Core::Shader_Loader ParticleSystem::shaderLoader;
 GLuint ParticleSystem::bubbleTexture;
 GLuint ParticleSystem::fireTexture;
 GLuint ParticleSystem::smokeTexture;

	void ParticleSystem::init() {
		program = shaderLoader.CreateProgram("shaders/shader_particle.vert", "shaders/shader_particle.frag");
		bubbleTexture = Core::LoadTexture("textures/bubble_texture.png");
		fireTexture = Core::LoadTexture("textures/fire_texture.png");
		smokeTexture = Core::LoadTexture("textures/smoke_texture.png");

	}

	void ParticleSystem::renderParticles(glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix){
		float vertices[] = { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f,};
		float texCoords[] = {
	0.0f, 1.0f,    //top left
	0.0f, 0.0f,  //bottom left
	1.0f, 1.0f, //top right
	1.0f, 0.0f //bottom right
		};
		glUseProgram(program);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glDepthMask(false);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, vertices);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(0);
		for (auto ParticleGroup : particleGroups) {
			//Sending the correct texture to the shader based on particle type
			switch ((*ParticleGroup).type) {
			case PARTICLE_BUBBLE:
				Core::SetActiveTexture(bubbleTexture, "textureSampler", program, 0);
				break;

			case PARTICLE_FIRE:
				Core::SetActiveTexture(fireTexture, "textureSampler", program, 0);
				break;
			case PARTICLE_SMOKE:
				Core::SetActiveTexture(smokeTexture, "textureSampler", program, 0);
				break;
			}

			(*ParticleGroup).renderParticles(program, cameraMatrix, perspectiveMatrix);
		}

		glDepthMask(true);
		glDisable(GL_BLEND);
		glUseProgram(0);		
	}

	//Adds a new particle group to the list
	//Used automatically when a new particle group is created
	void ParticleSystem::addGroup(ParticleGroup &particleGroup) {
		particleGroups.push_back(&particleGroup);
	}
	//Updates the state of all particles given a change in time
	void ParticleSystem::update(double deltaTime) {

		for (auto& ParticleGroup : particleGroups) {
			(*ParticleGroup).update(deltaTime);
		}
	}
