#include "ParticleGroup.h"
#include "particleTypeEnum.h"
#include "glm.hpp"
#include <list>
#include "Particle.h"
#include "Random.h"
#include "ParticleSystem.h"
#include "glew.h"
#include "freeglut.h"
#include "src/Shader_Loader.h"
#include "src/Render_Utils.h"
#include "src/Texture.h"
#include <ext.hpp>



	ParticleGroup::ParticleGroup(int particlePerSecond, float speed, float gravityStrength, float drag, float lifeLength, particleType type) {
		this->particlePerSecond = particlePerSecond;
		this->speed = speed;
		this->gravityStrength = gravityStrength;
		this->lifeLength = lifeLength;
		this->type = type;
		this->drag = drag;
	};

	void ParticleGroup::setParticleSpeed(int particleSpeed) {
		this->particlePerSecond = particleSpeed;
	}

	//The center is where the particles will be created
	//To be called every frame
	void ParticleGroup::generateParticles(glm::vec3 center, double deltaTime) {
		float particlesToCreate = particlePerSecond * deltaTime;
		int count = std::floor(particlesToCreate);
		float partialParticles = particlesToCreate -count;
		//std::cout << "Generating...";
		for (int i = 0; i < count; i++) {
			emitParticle(center);
			//std::cout << "Generated particle\n";
		}
		
		if (Random::Float(0, 1) < partialParticles) {
			emitParticle(center);
			//std::cout << "Generated particle\n";
		}
	}

	void ParticleGroup::emitParticle(glm::vec3 center) {
		float dirX = Random::Float(-1, 1);
		float dirZ = Random::Float(-1, 1);
		glm::vec3 velocity = glm::vec3(dirX, 1, dirZ);
		velocity = glm::normalize(velocity);
		velocity *= speed;
		//glm::vec3 pos, glm::vec3 vel, float gravForce, float lifetime, float rotation, float scale
		Particle newParticle = Particle(center, velocity, gravityStrength, drag, lifeLength, 0.0, 0.2);
		particleList.push_back(newParticle);
		//std::cout << "\n Size of particle list on addition: " << particleList.size() <<"\n";
		//std::cout<<(this);
	}

	void ParticleGroup::emitParticle(glm::vec3 center, glm::vec3 velocity) {
		float dirX = Random::Float(-1, 1);
		float dirZ = Random::Float(-1, 1);
		glm::vec3 vel = velocity;
		vel = glm::normalize(vel);
		vel *= speed;
		//glm::vec3 pos, glm::vec3 vel, float gravForce, float lifetime, float rotation, float scale
		Particle newParticle = Particle(center, vel, gravityStrength, drag, lifeLength, 0.0, 0.2);
		particleList.push_back(newParticle);
		//std::cout << "\n Size of particle list on addition: " << particleList.size() << "\n";
		//std::cout << (this);
	}

	void ParticleGroup::update(double deltaTime) {

		std::list<Particle>::iterator i = particleList.begin();
		while (i != particleList.end()) {

			bool alive = i->update(deltaTime);
			if (!alive) {
				i = particleList.erase(i);
			}
			else {
				i++;
			}
		}


		/*
		for (auto& particle : particleList.) {
			bool alive=particle.update(deltaTime);

			if (!alive) {

			}
		}
		*/
	}

	void ParticleGroup::explode(glm::vec3 location) {
		for (int i = 0; i < 100; i++) {
			float dirX = Random::Float(-1, 1);
			float dirY = Random::Float(-1, 1);
			float dirZ = Random::Float(-1, 1);
			glm::vec3 velocity = glm::vec3(dirX, dirY, dirZ);
			velocity = glm::normalize(velocity);
			velocity *= speed;
			//glm::vec3 pos, glm::vec3 vel, float gravForce, float lifetime, float rotation, float scale
			Particle newParticle = Particle(location, velocity, gravityStrength, drag, lifeLength, 0.0, 0.2);
			particleList.push_back(newParticle);
		}

	}
	
	void ParticleGroup::renderParticles(GLuint program, glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix) {
		
		glm::mat4 modelMatrix; 
		glUniformMatrix4fv(glGetUniformLocation(program, "perspectiveMatrix"), 1, GL_FALSE, (float*)&perspectiveMatrix);
		glUniformMatrix4fv(glGetUniformLocation(program, "cameraMatrix"), 1, GL_FALSE, (float*)&cameraMatrix);
		
		//std::cout << "Render function\n";
		//std::cout << "\n Size of particle list on rendering: " << particleList.size() << "\n";
		for (auto& particle : this->particleList) {
			//std::cout << "Rendering particle\n";
			modelMatrix = glm::translate(particle.position);
			modelMatrix[0][0] = cameraMatrix[0][0];
			modelMatrix[0][1] = cameraMatrix[1][0];
			modelMatrix[0][2] = cameraMatrix[2][0];
			modelMatrix[1][0] = cameraMatrix[0][1];
			modelMatrix[1][1] = cameraMatrix[1][1];
			modelMatrix[1][2] = cameraMatrix[2][1];
			modelMatrix[2][0] = cameraMatrix[0][2];
			modelMatrix[2][1] = cameraMatrix[1][2];
			modelMatrix[2][2] = cameraMatrix[2][2];
			modelMatrix *= glm::rotate(glm::radians(particle.rotation), glm::vec3(0, 0, 1));
			modelMatrix *= glm::scale(glm::vec3(particle.scale));
			glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
			float opacity = particle.lifeLeft / particle.lifeLength;
			glUniform1f(glGetUniformLocation(program, "opacity"), opacity);
			glUniform1f(glGetUniformLocation(program, "particleZ"), particle.position.z);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}

