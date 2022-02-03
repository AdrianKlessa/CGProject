#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include <list>
#include <vector>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Physics.h"
#include "..\ParticleSystem.h"
#include "..\ParticleGroup.h"
#include "..\ShipMovementEnums.h"

float skyboxVertices[] = {
	// positions
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f };

std::vector<std::string> cubeFaces = { "textures/skybox/exp/right.png",
										"textures/skybox/exp/left.png",
										"textures/skybox/exp/top.png",
										"textures/skybox/exp/bottom.png",
										"textures/skybox/exp/front.png",
										"textures/skybox/exp/back.png" };

// vars for programs
GLuint programColor;
GLuint programTexture;
GLuint programWater;
GLuint programSkybox;
Core::Shader_Loader shaderLoader;

float appLoadingTime;

// vars for models
obj::Model planeModel;
obj::Model shipModel;
obj::Model sphereModel;
obj::Model hamSharkModel;
obj::Model megalodonModel;

obj::Model terrainModel;
obj::Model rockModel1;
obj::Model rockModel2;
obj::Model seaWeedModel1;
obj::Model seaWeedModel2;
obj::Model seaWeedModel3;
obj::Model boxModel;
obj::Model mineModel;

// camera stuff
float cameraAngle = glm::radians(-90.f);
glm::vec3 cameraPos = glm::vec3(0, 0, 5);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f));

// vars for textures
GLuint textureTest;
GLuint textureEarth;
GLuint textureAsteroid;
GLuint textureShip;
GLuint textureTerrain;
GLuint textureMine;
GLuint textureMegalodon;

// vars for normals
GLuint normalTest;
GLuint normalEarth;
GLuint normalAsteroid;
GLuint normalShip;
GLuint normalTerrain;
GLuint normalMine;
GLuint normalMegalodon;

GLuint normalVBO;
GLuint normalVAO;

float frustumScale = 1.f;
const int NUM_ROCKS = 120;
const int NUM_MINES = 5;
std::vector <glm::vec3> rockPositions(NUM_ROCKS);
std::vector <glm::vec3> seaWeedPositions(NUM_ROCKS);

const int NUM_CAMERA_POINTS = 10;
glm::vec3 cameraKeyPoints[NUM_ROCKS];

GLuint skyboxVAO;
GLuint skyboxVBO;
GLuint skyboxTexture;

GLuint defaultVAO;
GLuint defaultVBO;

//Physics stuff
GLuint textureBox, textureGround;
std::vector <glm::mat4> boxModelMatrices(NUM_MINES);
std::vector <std::tuple<PxRigidDynamic*, PxMaterial*, PxShape*>> boxes;
std::vector <double> timeCreated(NUM_MINES);
float current_time=0;

// Initalization of physical scene (PhysX)
Physics pxScene(2.8f /* gravity (m/s^2) */);

// fixed timestep for stable and deterministic simulation
const double physicsStepTime = 1.f / 60.f;
double physicsTimeToProcess = 0;

// physical objects
PxRigidStatic* planeBody = nullptr;
PxMaterial* planeMaterial = nullptr;

const double boxLifetime = 5.0f; // after this time a new box spawns (with some element of randomness)
const double explosionDistance = 10.0f; // how far away the mine is from the player when it triggers

glm::vec3 initSubmarinePos;
glm::vec3 prevSubmarinePos;
glm::vec3 newSubmarinePos;



//float particlePerSecond, float speed, float gravityStrength, float drag, float lifeLength, particleType type
ParticleGroup engineParticles = ParticleGroup(2, 2, 0.3, 0.0, 4, particleType::PARTICLE_BUBBLE);
ParticleGroup explosionParticles = ParticleGroup(0, 4, 0.2, 0.0, 0.75, particleType::PARTICLE_SMOKE);




//Ship movement physics
const double shipAcceleration = 8.0; //How fast the ship accelerates 
const double shipMaxSpeed = 20; //Max ship speed
const double shipDecceleration = 5.0; //How fast the ship deccelerates when no button is pushed
const double shipTurningAcceleration = 1.0; //How fast the ship increases its rotational speed
const double shipTurningMax=1.5; //Max rotational speed
const double shipTurningDecceleration = 3.0; //How fast it stops rotating when no button is pressed
const float keypressLatency = 0.45;

ShipRotation currentShipRotationState;
ShipAcceleration currentShipAccelerationState;
double shipRotationVelocity = 0;
glm::vec3 shipVelocity = glm::vec3(0, 0, 0);
glm::vec3 velFromBombs = glm::vec3(0, 0, 0); //Velocity that is added due to bombs exploding
double bombStrength = 10.0; //How powerful the explosions are
double bombSpeedDieout = 0.1; // How fast the speed added from bomb explosions disappears; 0.0-1.0
float lastKeypressTime=0;

void keyboard(unsigned char key, int x, int y)
{
	const float angleSpeed = 0.1f;
	const float moveSpeedXZ = 0.1f * 30; // speed going forward, back, left n right
	const float moveSpeedY = 0.1f * 30; // speed going up n down
	lastKeypressTime = current_time;


	// var for pre-calculating of a next step
	glm::vec3 nextStep;

	switch (key)
	{
	case 'z':
	case 'Z':
	case 'ÿ':
	case 'ß': // turn left
		//cameraAngle -= angleSpeed;
		currentShipRotationState = ROTATION_LEFT;
		currentShipAccelerationState = MOVEMENT_NONE;
		break;
	case 'x':
	case 'X':
	case '÷':
	case '×': // turn right
		//cameraAngle += angleSpeed;
		currentShipRotationState = ROTATION_RIGHT;
		currentShipAccelerationState = MOVEMENT_NONE;
		break;

	case 'w':
	case 'W': //forward


		currentShipAccelerationState = MOVEMENT_FORWARD;
		currentShipRotationState = ROTATION_NONE;
		break;
		/*nextStep = cameraPos + cameraDir * moveSpeedXZ;

		// allow if next step not out of bounds
		if (abs(abs(nextStep.z)) <= xAndZBoundary && abs(nextStep.x) <= xAndZBoundary) {
			cameraPos = nextStep;
			break;
		}
		else
		{
			break;
		}
		*/
		
	case 's':
	case 'S': //back
		currentShipAccelerationState = MOVEMENT_BACK;
		currentShipRotationState = ROTATION_NONE;
		break;
		/*
		nextStep = cameraPos - cameraDir * moveSpeedXZ;

		// allow if next step not out of bounds
		if (abs(nextStep.z) <= xAndZBoundary && abs(nextStep.x) <= xAndZBoundary) {
			cameraPos = nextStep;
			break;
		}
		else
		{
			break;
		}
		*/
	case 'd':
	case 'D': //right
		currentShipAccelerationState = MOVEMENT_RIGHT;
		currentShipRotationState = ROTATION_NONE;
		break;
		/*
		nextStep = cameraPos + glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeedXZ;

		// allow if next step not out of bounds
		if (abs(nextStep.z) <= xAndZBoundary && abs(nextStep.x) <= xAndZBoundary) {
			cameraPos = nextStep;
			break;
		}
		else
		{
			break;
		}
		*/
	case 'a':
	case 'A': //left
		currentShipAccelerationState = MOVEMENT_LEFT;
		currentShipRotationState = ROTATION_NONE;
		break;
		/*
		nextStep = cameraPos - glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeedXZ;

		// allow if next step not out of bounds
		if (abs(nextStep.z) <= xAndZBoundary && abs(nextStep.x) <= xAndZBoundary) {
			cameraPos = nextStep;
			break;
		}
		else
		{
			break;
		}
		*/
	case 'e':
	case 'E': //up
		currentShipAccelerationState = MOVEMENT_UP;
		currentShipRotationState = ROTATION_NONE;
		break;
		/*
		// allow if next step not out of bounds
		if (cameraPos.y <= yTopBoundary) {
			cameraPos += glm::vec3(0, 1, 0) * moveSpeedY;
			break;
		}
		else
		{
			break;
		}
		*/
	case 'q':
	case 'Q': //down
		currentShipAccelerationState = MOVEMENT_DOWN;
		currentShipRotationState = ROTATION_NONE;
		break;
		/*
		// allow if next step not out of bounds
		if (cameraPos.y >= yBottomBoundary) {
			cameraPos -= glm::vec3(0, 1, 0) * moveSpeedY;
			break;
		}
		else
		{
			break;
		}
		*/

	default:
		currentShipAccelerationState = MOVEMENT_NONE;
		currentShipRotationState = ROTATION_NONE;
	}
}

int signOf(double number) {
	if (number >= 1) {
		return 1;
	}
	else {
		return -1;
	}
}

void updateShipMovement(double deltaTime) {

	/*
	const double shipAcceleration = 3.0; //How fast the ship accelerates 
	const double shipMaxSpeed = 5.0; //Max ship speed
	const double shipDecceleration = 1.0; //How fast the ship deccelerates when no button is pushed



	const double shipTurninAcceleration = 5.0; //How fast the ship increases its rotational speed
	const double shipTurningMax=15.0; //Max rotational speed
	const double shipTurningDecceleration = 3.0; //How fast it stops rotating when no button is pressed


	ShipRotation currentShipRotationState;
	ShipAcceleration currentShipAccelerationState;
	double shipRotationVelocity = 0;
	glm::vec3 shipVelocity = glm::vec3(0, 0, 0);

	
	*/


	// 'end of world' limits
	const float xAndZBoundary = 10000.0f;
	const float yTopBoundary = 60.0f;
	const float yBottomBoundary = -30.0f;

	glm::vec3 nextStep;
	glm::vec3 vectorRight = glm::cross(cameraDir, glm::vec3(0, 1, 0));
	double deltaRotationAcceleration = deltaTime * shipTurningAcceleration;
	double deltaAcceleration = deltaTime * shipAcceleration;

	velFromBombs -= velFromBombs * deltaTime * bombSpeedDieout;

	//Checking to make sure the last keypress wasn't too long ago since the keyboard function only triggers on a keypress
	if ((double)current_time - lastKeypressTime > keypressLatency) {
currentShipRotationState = ROTATION_NONE;
currentShipAccelerationState = MOVEMENT_NONE;
	}

	switch (currentShipAccelerationState) {
	case MOVEMENT_FORWARD:
		shipVelocity += cameraDir * deltaAcceleration;
		if (glm::length(shipVelocity) > shipMaxSpeed) {
			shipVelocity = glm::normalize(shipVelocity) * shipMaxSpeed;
		}
		break;
	case MOVEMENT_BACK:
		shipVelocity -= cameraDir * deltaAcceleration;
		if (glm::length(shipVelocity) > shipMaxSpeed) {
			shipVelocity = glm::normalize(shipVelocity) * shipMaxSpeed;
		}
		break;
	case MOVEMENT_LEFT:
		shipVelocity -= vectorRight * deltaAcceleration;
		if (glm::length(shipVelocity) > shipMaxSpeed) {
			shipVelocity = glm::normalize(shipVelocity) * shipMaxSpeed;
		}
		break;
	case MOVEMENT_RIGHT:
		shipVelocity += vectorRight * deltaAcceleration;
		if (glm::length(shipVelocity) > shipMaxSpeed) {
			shipVelocity = glm::normalize(shipVelocity) * shipMaxSpeed;
		}
		break;
	case MOVEMENT_UP:
		shipVelocity += glm::vec3(0, 1, 0) * deltaAcceleration;
		if (glm::length(shipVelocity) > shipMaxSpeed) {
			shipVelocity = glm::normalize(shipVelocity) * shipMaxSpeed;
		}
		break;
	case MOVEMENT_DOWN:
		shipVelocity += glm::vec3(0, -1, 0) * deltaAcceleration;
		if (glm::length(shipVelocity) > shipMaxSpeed) {
			shipVelocity = glm::normalize(shipVelocity) * shipMaxSpeed;
		}
		break;
	default:
		shipVelocity -= glm::normalize(shipVelocity) * shipDecceleration * deltaTime;
	}



	switch (currentShipRotationState) {
	case ROTATION_LEFT:
		shipRotationVelocity -= deltaRotationAcceleration;
		break;
	case ROTATION_RIGHT:
		shipRotationVelocity += deltaRotationAcceleration;
		break;
	default:
		if (shipRotationVelocity > 0) {
			shipRotationVelocity = std::max((shipRotationVelocity - shipTurningDecceleration * deltaTime), 0.0);
		}
		else {
			shipRotationVelocity = std::min((shipRotationVelocity + shipTurningDecceleration * deltaTime), 0.0);
		}

	}
	if (shipRotationVelocity < -shipTurningMax) {
		shipRotationVelocity = -shipTurningMax;
	}
	else if (shipRotationVelocity > shipTurningMax) {
		shipRotationVelocity = shipTurningMax;
	}


	cameraAngle += shipRotationVelocity * deltaTime;

	glm::vec3 shipMovement = (velFromBombs + shipVelocity) * deltaTime;


	//const float xAndZBoundary = 10000.0f;
	//const float yTopBoundary = 60.0f;
	//const float yBottomBoundary = -30.0f;
	/*
	if (((cameraPos + shipMovement).x > xAndZBoundary) | ((cameraPos + shipMovement).x < -1 * xAndZBoundary) | ((cameraPos + shipMovement).z > xAndZBoundary) | ((cameraPos + shipMovement).z < -1 * xAndZBoundary) | ((cameraPos + shipMovement).y > yTopBoundary) | ((cameraPos + shipMovement).y < yBottomBoundary)) {
		shipVelocity *= 0;
		velFromBombs *= 0;
	}
	*/


	//Stupid hack to prevent being stuck in terrain
	if ((cameraPos + shipMovement).x > xAndZBoundary) {
		shipVelocity.x = -1;
		velFromBombs.x= -1;
	}
	else if ((cameraPos + shipMovement).x < -1 * xAndZBoundary) {
		shipVelocity.x = 1;
		velFromBombs.x = 1;
	}
	else if (((cameraPos + shipMovement).z > xAndZBoundary)) {
		shipVelocity.z = -1;
		velFromBombs.z = -1;
	}
	else if ((cameraPos + shipMovement).z < -1 * xAndZBoundary) {
		shipVelocity.z = 1;
		velFromBombs.z = 1;
	}
	else if ((cameraPos + shipMovement).y > yTopBoundary) {
		shipVelocity.y = -1;
		velFromBombs.y = -1;
	}
	else if ((cameraPos + shipMovement).y < yBottomBoundary){
		shipVelocity.y = 1;
		velFromBombs.y = 1;
	}



	shipMovement = (velFromBombs + shipVelocity) * deltaTime;
	cameraPos += shipMovement;
	// cameraPos += velFromBombs;
}

glm::mat4 createCameraMatrix()
{
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0, 1, 0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

// uniforms used in shaders
void setUpUniforms(GLuint program, glm::mat4 modelMatrix)
{
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraDir"), cameraDir.x, cameraDir.y, cameraDir.z);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform1f(glGetUniformLocation(program, "cutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(program, "cutOffOut"), glm::cos(glm::radians(17.5f)));


	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glm::mat4 modelViewMatrix = cameraMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewMatrix"), 1, GL_FALSE, (float*)&transformation);

	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

}

// method to draw obj with color
void drawObjectColor(obj::Model* model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	setUpUniforms(program, modelMatrix);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);

	Core::DrawModel(model);

	glUseProgram(0);
}

// method to draw obj with textures only
void drawObjectTexture(obj::Model* model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	setUpUniforms(program, modelMatrix);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	Core::DrawModel(model);

	glUseProgram(0);
}

// method to draw obj with textures and normals
void drawObjectTextureNM(obj::Model* model, glm::mat4 modelMatrix, GLuint textureId, GLuint normalmapId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	setUpUniforms(program, modelMatrix);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);
	Core::SetActiveTexture(normalmapId, "normalSampler", program, 1);
	Core::DrawModel(model);

	glUseProgram(0);
}

// method to draw a cube map
void drawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	glDepthFunc(GL_LEQUAL);
	glDepthRange(1, 1);
	glEnable(GL_DEPTH_CLAMP);
	glUseProgram(programSkybox);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, skyboxVertices);
	glEnableVertexAttribArray(0);
	glm::mat4 view = glm::mat4(glm::mat3(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "projection"), 1, GL_FALSE, (float*)&projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "view"), 1, GL_FALSE, (float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "view"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform1i(glGetUniformLocation(programSkybox, "skybox"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glUseProgram(0);
	glDisable(GL_DEPTH_CLAMP);
	glDepthRange(0, 1);
	glDepthFunc(GL_LESS);
}

// method to load a sky box from the pictures
void loadSkybox()
{
	glGenTextures(1, &skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	int width, height, nrChannels;
	unsigned char* data;
	for (unsigned int i = 0; i < cubeFaces.size(); i++)
	{
		data = stbi_load(cubeFaces[i].c_str(), &width, &height, &nrChannels, 3);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << cubeFaces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

// method to update the matrices of actors (physics stuff)
void updateTransforms()
{
	// Here we retrieve the current transforms of the objects from the physical simulation.
	auto actorFlags = PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC;
	PxU32 nbActors = pxScene.scene->getNbActors(actorFlags);
	if (nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		pxScene.scene->getActors(actorFlags, (PxActor**)&actors[0], nbActors);
		for (auto actor : actors)
		{
			// We use the userData of the objects to set up the proper model matrices.
			if (!actor->userData)
			{
				continue;
			}
			else {

				glm::mat4* modelMatrix = (glm::mat4*)actor->userData;

				// get world matrix of the object (actor)
				PxMat44 transform = actor->getGlobalPose();
				auto& c0 = transform.column0;
				auto& c1 = transform.column1;
				auto& c2 = transform.column2;
				auto& c3 = transform.column3;

				// set up the model matrix used for the rendering
				*modelMatrix = glm::mat4(
					c0.x, c0.y, c0.z, c0.w,
					c1.x, c1.y, c1.z, c1.w,
					c2.x, c2.y, c2.z, c2.w,
					c3.x, c3.y, c3.z, c3.w);

				//std::cout << modelMatrix;
			}
		}
	}
}

// respawns a mine at a given index
void renewMine(int i, glm::vec3 shipPos) {

	PxMaterial* mat = pxScene.physics->createMaterial(0.2, 0.01, 0.7);

	int rand_value = rand() % 19;
	boxes[i] = (std::make_tuple(pxScene.physics->createRigidDynamic(PxTransform(shipPos.x + (rand() % 40 + -20),
		20,
		shipPos.z + (rand() % 40 + -20))),
		mat,
		pxScene.physics->createShape(PxSphereGeometry(1),
			*mat)));
	//std::cout << shipPos.x << "  " << shipPos.z << "\n";

	std::get<0>(boxes[i])->attachShape(*std::get<2>(boxes[i]));
	std::get<2>(boxes[i])->release();
	std::get<0>(boxes[i])->userData = &boxModelMatrices[i];
	timeCreated[i] = current_time;

	pxScene.scene->addActor(*std::get<0>(boxes[i]));

}

glm::vec3 positionFromModelMatrix(glm::mat4 modelMatrix) {
	return glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
}

// detects collisions and triggers mines' respawn
void updateMines(glm::mat4 shipModelMatrix) {
	
	int random_margin = 5; // how much randomness in the lifetime of the boxes we want
	//Added this so that not all boxes disappear at the same time

	glm::vec3 shipPos = positionFromModelMatrix(shipModelMatrix);
	glm::mat4 currentBoxMatrix; // model matrix of the current box
	glm::vec3 currentBoxPos; // position of the current box
	double distance; // distance from the submarine to the box
	for (int i = 0; i < NUM_MINES; i++)
	{
		currentBoxMatrix = boxModelMatrices[i];
		currentBoxPos = positionFromModelMatrix(currentBoxMatrix);
		distance = glm::distance(shipPos, currentBoxPos);

		if (distance <= explosionDistance) {
			std::cout << "The player exploded \n";
			explosionParticles.explode(currentBoxPos);
			glm::vec3 vectorFromBombToShip = shipPos - currentBoxPos;
			velFromBombs += glm::normalize(vectorFromBombToShip) * bombStrength;
			pxScene.scene->removeActor(*std::get<0>(boxes[i]));

			renewMine(i, shipPos);
			//TODO: add the actual explosion 
		}

		if ((current_time - timeCreated[i]) > (boxLifetime + rand() % random_margin)) {
			if (rand() % 3 == 0) {
				timeCreated[i] -= 1;
				continue;
			}
			pxScene.scene->removeActor(*std::get<0>(boxes[i]));

			renewMine(i, shipPos);

		}
	}
}

// generate positions for rocks and seaweed
void generateFaunaPos(glm::vec3 submarinepPos) {

	for (int i = 0; i < NUM_ROCKS; i++)
	{
		int spreadMeasure = 160;
		int lowestSeaWeedY = -40.f;
		int lowestRockY = -42.f;
		int alterZ = 0;
		int alterX = 0;

		switch (i % 4)
		{
		case 0: // pos x, pos z
			seaWeedPositions[i] = glm::vec3(rand() % spreadMeasure + submarinepPos.x + alterX,
												rand() % 10 + (lowestSeaWeedY),
												+rand() % spreadMeasure + submarinepPos.z + alterZ);
			rockPositions[i] = glm::vec3(rand() % spreadMeasure + submarinepPos.x + alterX,
											rand() % 10 + (lowestRockY),
											+rand() % spreadMeasure + submarinepPos.z + alterZ);
			break;
		case 1: //pos x, neg z
			seaWeedPositions[i] = glm::vec3(rand() % spreadMeasure + submarinepPos.x + alterX,
												rand() % 10 + (lowestSeaWeedY),
												+rand() % spreadMeasure - spreadMeasure + submarinepPos.z + alterZ);
			rockPositions[i] = glm::vec3(rand() % spreadMeasure + submarinepPos.x + alterX,
												rand() % 10 + (lowestRockY),
												+rand() % spreadMeasure - spreadMeasure + submarinepPos.z + alterZ);
			break;
		case 2: //neg x, neg z
			seaWeedPositions[i] = glm::vec3(rand() % spreadMeasure - spreadMeasure + submarinepPos.x + alterX,
												rand() % 10 + (lowestSeaWeedY),
												+rand() % spreadMeasure - spreadMeasure + submarinepPos.z + alterZ);
			rockPositions[i] = glm::vec3(rand() % spreadMeasure - spreadMeasure + submarinepPos.x + alterX,
												rand() % 10 + (lowestRockY),
												+rand() % spreadMeasure - spreadMeasure + submarinepPos.z + alterZ);
			break;
		case 3: //neg x, pos z
			seaWeedPositions[i] = glm::vec3(rand() % spreadMeasure - spreadMeasure + submarinepPos.x + alterX,
												rand() % 10 + (lowestSeaWeedY),
												+rand() % spreadMeasure + submarinepPos.z + alterZ);
			rockPositions[i] = glm::vec3(rand() % spreadMeasure - spreadMeasure + submarinepPos.x + alterX,
												rand() % 10 + (lowestRockY),
												+rand() % spreadMeasure + submarinepPos.z + alterZ);
			break;
		}
	}
}

// draw rocks and seaweed
void drawFauna() {

	// drawing rocks and seaweed
	for (int i = 0; i < NUM_ROCKS; i++)
	{
		switch (i % 2) {
		case 0:
			drawObjectColor(&rockModel1, glm::translate(rockPositions[i]) // put at places from the preinitialized arrray with rand coords
				* glm::scale(glm::vec3(3.0f)), // scale 3x
				glm::vec3(0.3, 0.5, 0.3)); // color
			break;
		case 1:
			drawObjectColor(&rockModel2, glm::translate(rockPositions[i]) // put at places from the preinitialized arrray with rand coords
				* glm::scale(glm::vec3(3.0f)), // scale 3x
				glm::vec3(0.3, 0.5, 0.3));	// color
			break;
		}

		switch (i % 3) {
		case 0:
			drawObjectColor(&seaWeedModel3, glm::translate(seaWeedPositions[i]) // put at places from the preinitialized arrray with rand coords
				* glm::scale(glm::vec3(3.0f)), // scale 3x
				glm::vec3(0.7, 0.1, 0.3)); //color
			break;
		case 1:
			drawObjectColor(&seaWeedModel2, glm::translate(seaWeedPositions[i]) // put at places from the preinitialized arrray with rand coords
				* glm::scale(glm::vec3(3.0f)), // scale 3x
				glm::vec3(0.2, 0.8, 0.7)); //color
			break;
		case 2:
			drawObjectColor(&seaWeedModel1, glm::translate(seaWeedPositions[i]) // put at places from the preinitialized arrray with rand coords
				* glm::scale(glm::vec3(3.0f)), // scale 3x
				glm::vec3(0.5, 0.8, 0.3)); //color
			break;
		}
	}
}

// update positions for rocks and seaweed
void updateFauna(glm::mat4 shipModelMatrix) {
	newSubmarinePos = positionFromModelMatrix(shipModelMatrix);
	int threshold = 110;

	if (abs(prevSubmarinePos.x - newSubmarinePos.x) >= threshold ||
		abs(prevSubmarinePos.z - newSubmarinePos.z) >= threshold)
	{
		//generateFaunaPos(prevSubmarinePos);
		//drawFauna();

		generateFaunaPos(newSubmarinePos);
		drawFauna();

		prevSubmarinePos = newSubmarinePos;
	}
}

// generating and rendering particles for the submarine and mines
void generateAndRenderParticles(glm::mat4 shipModelMatrix, double dtime) {

	// particles for mines
	for (size_t i = 0; i < NUM_MINES; i++)
	{
		engineParticles.setParticleSpeed(4);
		engineParticles.generateParticles(positionFromModelMatrix(boxModelMatrices[i]), dtime * 2);
		ParticleSystem::renderParticles(cameraMatrix, perspectiveMatrix, cameraDir, cameraPos);
	}

	for (size_t i = 0; i < NUM_ROCKS / 6; i++)
	{
		//engineParticles.setParticleSpeed(1);
		engineParticles.generateParticles(seaWeedPositions[i], dtime * 2);
		ParticleSystem::renderParticles(cameraMatrix, perspectiveMatrix, cameraDir, cameraPos);
	}

	// particles for the submarine
	engineParticles.setParticleSpeed(1);
	engineParticles.generateParticles(positionFromModelMatrix(shipModelMatrix), dtime / 2);
	ParticleSystem::renderParticles(cameraMatrix, perspectiveMatrix, cameraDir, cameraPos);
}

void renderScene()
{

	//data for hammer shark movement
	current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - appLoadingTime;
	float time = current_time;
	int time_int = floorf(time);
	int v1 = (time_int - 1) % NUM_CAMERA_POINTS;
	int v2 = time_int % NUM_CAMERA_POINTS;
	int v3 = (time_int + 1) % NUM_CAMERA_POINTS;
	int v4 = (time_int + 2) % NUM_CAMERA_POINTS;

	// opcjonalny ruch swiatla do testow
	bool animateLight = false;
	if (animateLight)
	{
		float lightAngle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * 3.14 / 8;
		lightDir = glm::normalize(glm::vec3(sin(lightAngle), -1.0f, cos(lightAngle)));
	}




	// physics settings
	static double prevTime = time;
	double dtime = time - prevTime;
	prevTime = time;

	if (dtime < 1.f) {
		physicsTimeToProcess += dtime;
		while (physicsTimeToProcess > 0) {
			// here we perform the physics simulation step
			pxScene.step(physicsStepTime);
			physicsTimeToProcess -= physicsStepTime;
		}
	}

	ParticleSystem::update(dtime);
	updateShipMovement(dtime);


	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix(0.01f, 5000.f, frustumScale);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);


	// submarine matrix
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 1.0f + glm::vec3(0, -0.25f, 0))
		* glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0, 1, 0))
		* glm::scale(glm::vec3(0.07f))
		* glm::rotate(glm::radians(90.0f), glm::vec3(1.f, 0.0f, 0.f))
		* glm::rotate(glm::radians(180.0f), glm::vec3(0.f, 1.0f, 0.f))
		* glm::rotate(glm::radians(180.0f), glm::vec3(0.f, 0.0f, 1.f));

	// drawing submarine model
	drawObjectTextureNM(&shipModel, shipModelMatrix, textureShip, normalShip);

	// drawing rocks and seaweed
	for (int i = 0; i < NUM_ROCKS; i++)
	{
		switch (i % 2) {
		case 0:
			drawObjectColor(&rockModel1, glm::translate(rockPositions[i]) // put at places from the preinitialized arrray with rand coords
				* glm::scale(glm::vec3(3.0f)), // scale 3x
				glm::vec3(0.3, 0.3, 0.3)); // color
			break;
		case 1:
			drawObjectColor(&rockModel2, glm::translate(rockPositions[i]) // put at places from the preinitialized arrray with rand coords
				* glm::scale(glm::vec3(3.0f)), // scale 3x
				glm::vec3(0.3, 0.3, 0.3));	// color
			break;
		}

		switch (i % 3) {
		case 0:
			drawObjectColor(&seaWeedModel3, glm::translate(seaWeedPositions[i]) // put at places from the preinitialized arrray with rand coords
				* glm::scale(glm::vec3(3.0f)), // scale 3x
				glm::vec3(0.7, 0.1, 0.3)); //color
			break;
		case 1:
			drawObjectColor(&seaWeedModel2, glm::translate(seaWeedPositions[i]) // put at places from the preinitialized arrray with rand coords
				* glm::scale(glm::vec3(3.0f)), // scale 3x
				glm::vec3(0.2, 0.8, 0.7)); //color
			break;
		case 2:
			drawObjectColor(&seaWeedModel1, glm::translate(seaWeedPositions[i]) // put at places from the preinitialized arrray with rand coords
				* glm::scale(glm::vec3(3.0f)), // scale 3x
				glm::vec3(0.5, 0.8, 0.3)); //color
			break;
		}
	}

	// update rocks and seaweed positions
	updateFauna(shipModelMatrix);

	// drawing hammer shark
	//drawObjectColor(&hamSharkModel, glm::translate(glm::catmullRom(rockPositions[v1], rockPositions[v2], rockPositions[v3], rockPositions[v4], (time - time_int) ))
	//								* glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0)) //init pos
	//								* glm::rotate(glm::radians(-90.0f), glm::vec3(0, 0, 1)) //init pos
	//								* glm::rotate(glm::radians(-36.0f * time), glm::vec3(0, 0, 1)) //follow forward
	//								* glm::scale(glm::vec3(0.1f)), //make small
	//								glm::vec3(0.5, 0.6, 0.3)); //color

	//drawObjectTextureNM(&megalodonModel, glm::translate(glm::vec3(0, 0, 0)), textureMegalodon, normalMegalodon);

	// draw terrain
	drawObjectTextureNM(&terrainModel, glm::translate(glm::vec3(0, -40, 0)) * glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(2.f)),
		textureTerrain,
		normalTerrain);
	
	updateTransforms();

	// draw mines
	for (size_t i = 0; i < NUM_MINES; i++)
	{
		drawObjectTextureNM(&mineModel, boxModelMatrices[i] * glm::scale(glm::vec3(3)), textureMine, normalMine); // boxModelMatrix was updated in updateTransforms()
	}

	// update mine positions
	updateMines(shipModelMatrix); // shipModelMatrix must be here for distance calculations

	// draw a sky box
	drawSkybox(cameraMatrix, perspectiveMatrix);

	//Drawing particles (has to be put after 3D stuff)
	generateAndRenderParticles(shipModelMatrix, dtime);

	glutSwapBuffers();

}

void initPhysics() {
	
	// create initial plane for physics objects
	planeBody = pxScene.physics->createRigidStatic(PxTransformFromPlaneEquation(PxPlane(0, 0.07, 0, 2)));
	planeMaterial = pxScene.physics->createMaterial(0.5, 0.5, 0.6);
	PxShape* planeShape = pxScene.physics->createShape(PxPlaneGeometry(), *planeMaterial);
	planeBody->attachShape(*planeShape);
	planeShape->release();
	planeBody->userData = NULL;
	pxScene.scene->addActor(*planeBody);

	// create the mines
	PxMaterial* mat = pxScene.physics->createMaterial(0.2, 0.01, 0.7);
	for (int i = 0; i < NUM_MINES; i++)
	{
		boxes.push_back(std::make_tuple(pxScene.physics->createRigidDynamic(PxTransform((2 * (i + 1)) * (rand() % 20 + -10),
			20,
			(2 * (i + 1)) * (rand() % 20 + -10))),
			mat,
			pxScene.physics->createShape(PxSphereGeometry(1),
				*mat)));

		std::get<0>(boxes[i])->attachShape(*std::get<2>(boxes[i]));
		std::get<2>(boxes[i])->release();
		std::get<0>(boxes[i])->userData = &boxModelMatrices[i];


		pxScene.scene->addActor(*std::get<0>(boxes[i]));
	}
}

void init()
{
	ParticleSystem::init();
	ParticleSystem::addGroup(engineParticles);
	ParticleSystem::addGroup(explosionParticles);
	srand(static_cast <unsigned> (time(0))); //Initializing random number generation
	glEnable(GL_DEPTH_TEST);

	// programs
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programWater = shaderLoader.CreateProgram("shaders/shader_water.vert", "shaders/shader_water.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");

	// models
	shipModel = obj::loadModelFromFile("models/submarine.obj");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	hamSharkModel = obj::loadModelFromFile("models/hamShark.obj");
	//megalodonModel = obj::loadModelFromFile("models/vurdalak_low.obj"); //
	terrainModel = obj::loadModelFromFile("models/terrain.obj");
	rockModel1 = obj::loadModelFromFile("models/bunch/SM_Big_Rock_02.obj");
	rockModel2 = obj::loadModelFromFile("models/bunch/SM_Rock_04.obj");
	seaWeedModel1 = obj::loadModelFromFile("models/seaweed1.obj");
	seaWeedModel2 = obj::loadModelFromFile("models/bunch/SM_DeadBush_01.obj"); 
	seaWeedModel3 = obj::loadModelFromFile("models/bunch/SM_Fern_02.obj"); 
	boxModel = obj::loadModelFromFile("models/box.obj");
	planeModel = obj::loadModelFromFile("models/plane.obj");
	mineModel = obj::loadModelFromFile("models/mine.obj");

	// textures
	textureShip = Core::LoadTexture("textures/submarine.png");
	textureEarth = Core::LoadTexture("textures/earth2.png");
	textureAsteroid = Core::LoadTexture("textures/asteroid.png");
	textureTest = Core::LoadTexture("textures/test.png");
	textureTerrain = Core::LoadTexture("textures/terrain/diffuse.png");
	textureBox = Core::LoadTexture("textures/a.png");
	textureGround = Core::LoadTexture("textures/a.png");
	textureMine = Core::LoadTexture("textures/mine_texture.png");
	//textureMegalodon = Core::LoadTexture("models/vurdalak_Roughness.png"); //

	// normals
	normalShip = Core::LoadTexture("textures/Submarine_normals.png");
	normalEarth = Core::LoadTexture("textures/earth2_normals.png");
	normalAsteroid = Core::LoadTexture("textures/asteroid_normals.png");
	normalTest = Core::LoadTexture("textures/test_normals.png");
	normalTerrain = Core::LoadTexture("textures/terrain/hight.png");
	normalMine = Core::LoadTexture("textures/mine_normals.png");
	//normalMegalodon = Core::LoadTexture("models/vurdalak_Normal_OpenGL.png"); //

	loadSkybox();
	initPhysics();


	// sth weird idk yet
	const float camRadius = 3.55;
	const float camOffset = 0.6;
	for (int i = 0; i < NUM_CAMERA_POINTS; i++)
	{
		float angle = (float(i)) * (2 * glm::pi<float>() / NUM_CAMERA_POINTS);
		float radius = camRadius * (0.95 + glm::linearRand(0.0f, 0.1f));
		cameraKeyPoints[i] = glm::vec3(cosf(angle) + camOffset, 0.0f, sinf(angle)) * radius;
	}
	appLoadingTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	// initial generating of rand rock and seaweed positons and putting them into the arrays
	initSubmarinePos = (cameraPos + cameraDir * 1.0f + glm::vec3(0, -0.25f, 0));
	prevSubmarinePos = (cameraPos + cameraDir * 1.0f + glm::vec3(0, -0.25f, 0));
	generateFaunaPos(prevSubmarinePos);

}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
	shaderLoader.DeleteProgram(programWater);
	shaderLoader.DeleteProgram(programSkybox);
}

void idle()
{
	glutPostRedisplay();
}

void onReshape(int width, int height)
{
	frustumScale = (float)width / height;

	glViewport(0, 0, width, height);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("CG PROJ X");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);
	glutReshapeFunc(onReshape);

	glutMainLoop();

	shutdown();

	return 0;
}
