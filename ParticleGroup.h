#pragma once
#include "particleTypeEnum.h"
#include "glm.hpp"
#include <list>
#include "Particle.h"
#include "Random.h"
#include "glew.h"
#include "freeglut.h"
#include "src/Shader_Loader.h"
#include "src/Render_Utils.h"
#include "src/Texture.h"
#include <ext.hpp>
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
	void explode();
	void renderParticles(GLuint program, glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix);
};

