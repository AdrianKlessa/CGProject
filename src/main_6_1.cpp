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


class waterTile
{
private:
	float height;
	float x, z;

public:
	float TILE_SIZE = 60;

	waterTile(float centerX, float centerZ, float h)
	{
		x = centerX;
		z = centerZ;
		height = h;
	}

	float getHeight()
	{
		return height;
	}

	float getX()
	{
		return x;
	}

	float getZ()
	{
		return z;
	}
};



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
	1.0f, -1.0f, 1.0f};

std::vector<std::string> cubeFaces = { "textures/skybox/exp/right.png",
										"textures/skybox/exp/left.png",
										"textures/skybox/exp/top.png",
										"textures/skybox/exp/bottom.png",
										"textures/skybox/exp/front.png",
										"textures/skybox/exp/back.png" };

GLuint programColor;
GLuint programTexture;
GLuint programWater;
GLuint programSkybox;
Core::Shader_Loader shaderLoader;

float appLoadingTime;

obj::Model planeModel;
obj::Model shipModel;
obj::Model sphereModel;
obj::Model hamSharkModel;
obj::Model terrainModel;
obj::Model rockModel1;
obj::Model rockModel2;
obj::Model seaWeedModel1;
obj::Model boxModel;


float cameraAngle = glm::radians(-90.f);
glm::vec3 cameraPos = glm::vec3(0, 0, 5);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f));

GLuint textureTest;
GLuint textureEarth;
GLuint textureAsteroid;
GLuint textureShip;
GLuint textureTerrain;


GLuint normalTest;
GLuint normalEarth;
GLuint normalAsteroid;
GLuint normalShip;
GLuint normalTerrain;
float waterVertices[] = {-100.0f, -100.0f, -100.0f, 1.0f, 1.0f, -1.0f, 100.0f, -1.0f, -1.0f, 1.0f, 1.0f, 100.0f};

GLuint waterVBO;
GLuint waterVAO;

GLuint normalVBO;
GLuint normalVAO;

float frustumScale = 1.f;
std::list<waterTile> waterTiles;
static const int NUM_ROCKS = 100;
static const int NUM_MINES = 20;
glm::vec3 rockPositions[NUM_ROCKS];
glm::vec3 seaWeedPositions[NUM_ROCKS];

static const int NUM_CAMERA_POINTS = 10;
glm::vec3 cameraKeyPoints[NUM_CAMERA_POINTS];

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
float current_time;
// Initalization of physical scene (PhysX)
Physics pxScene(2.8f /* gravity (m/s^2) */);

// fixed timestep for stable and deterministic simulation
const double physicsStepTime = 1.f / 60.f;
double physicsTimeToProcess = 0;

// physical objects
PxRigidStatic* planeBody = nullptr;
PxMaterial* planeMaterial = nullptr;

double box_lifetime = 5.0f; //after this time a new box spawns (with some element of randomness)
void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeedXZ = 0.1f * 30;
	float moveSpeedY = 0.1f * 30;


	const float xAndZBoundary = 10000.0f;
	const float yTopBoundary = 60.0f;
	const float yBottomBoundary = -30.0f;
	glm::vec3 nextStep;

	switch (key)
	{
	case 'z':
	case 'Z':
	case 'ÿ':
	case 'ß': // turn left
		cameraAngle -= angleSpeed;
		break;
	case 'x':
	case 'X':
	case '÷':
	case '×': // turn right
		cameraAngle += angleSpeed;
		break;

	case 'w': 
	case 'W': //forward
		nextStep = cameraPos + cameraDir * moveSpeedXZ;

		if (abs(abs(nextStep.z)) <= xAndZBoundary && abs(nextStep.x) <= xAndZBoundary) {
			cameraPos = nextStep;
			break;
		}
		else
		{
			break;
		}
	case 's':
	case 'S': //back
		nextStep = cameraPos - cameraDir * moveSpeedXZ;

		if (abs(nextStep.z) <= xAndZBoundary && abs(nextStep.x) <= xAndZBoundary) {
			cameraPos = nextStep;
			break;
		}
		else
		{
			break;
		}
	case 'd':
	case 'D': //right
		nextStep = cameraPos + glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeedXZ;

		if (abs(nextStep.z) <= xAndZBoundary && abs(nextStep.x) <= xAndZBoundary) {
			cameraPos = nextStep;
			break;
		}
		else
		{
			break;
		}
	case 'a':
	case 'A': //left
		nextStep = cameraPos - glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeedXZ;

		if (abs(nextStep.z) <= xAndZBoundary && abs(nextStep.x) <= xAndZBoundary) {
			cameraPos = nextStep;
			break;
		}
		else
		{
			break;
		}

	case 'e':
	case 'E': //up
		if (cameraPos.y <= yTopBoundary) {
			cameraPos += glm::vec3(0, 1, 0) * moveSpeedY;
			break;
		}
		else
		{
			break;
		}
	case 'q':
	case 'Q': //down
		if (cameraPos.y >= yBottomBoundary) {
			cameraPos -= glm::vec3(0, 1, 0) * moveSpeedY;
			break;
		}
		else
		{
			break;
		}
	}
}

glm::mat4 createCameraMatrix()
{
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0, 1, 0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

void setUpUniforms(GLuint program, glm::mat4 modelMatrix)
{
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glm::mat4 modelViewMatrix = cameraMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewMatrix"), 1, GL_FALSE, (float*)&transformation);

	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
}

void setUpUniformsWater(glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix)
{
	GLuint program = programWater;

	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_FALSE, (float *)&cameraMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_FALSE, (float *)&perspectiveMatrix);

	//Model matrix
	//View matrix
	//Projection matrix
}

// default methods for drawing objects
void drawObjectColor(obj::Model *model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	setUpUniforms(program, modelMatrix);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTextureNM(obj::Model *model, glm::mat4 modelMatrix, GLuint textureId, GLuint normalmapId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	setUpUniforms(program, modelMatrix);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);
	Core::SetActiveTexture(normalmapId, "normalSampler", program, 1);
	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model *model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	setUpUniforms(program, modelMatrix);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	Core::DrawModel(model);

	glUseProgram(0);
}

// additional methods for drawing stuff
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
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "projection"), 1, GL_FALSE, (float *)&projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "view"), 1, GL_FALSE, (float *)&view);
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

void drawWater(std::list<waterTile> water, glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix)
{
	GLuint program = programWater;
	glUseProgram(program);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	setUpUniformsWater(cameraMatrix, perspectiveMatrix);

	float vertices[] = { -1, -1, -1, 1, 1, -1, 1, -1, -1, 1, 1, 1 };
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(2);

	for (auto &tile : water)
	{
		glm::mat4 modelMatrix = glm::translate(glm::mat4(), glm::vec3(tile.getX(), tile.getHeight(), tile.getZ()));
		modelMatrix = modelMatrix * glm::scale(glm::mat4(), glm::vec3(tile.TILE_SIZE * 15.0f, tile.TILE_SIZE, tile.TILE_SIZE * 15.0f));
		glUniformMatrix4fv(glGetUniformLocation(programWater, "modelMatrix"), 1, GL_FALSE, (float *)&modelMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glDisable(GL_BLEND);

	glUseProgram(0);
}

void loadSkybox()
{
	glGenTextures(1, &skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	int width, height, nrChannels;
	unsigned char *data;
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

//Respawns a mine at a given index
void renewMine(int i) {

	PxMaterial* mat = pxScene.physics->createMaterial(0.2, 0.01, 0.7);

		int rand_value = rand() % 19;
		boxes[i]=(std::make_tuple(pxScene.physics->createRigidDynamic(PxTransform((2 * (rand_value + 1)) * (rand() % 20 + -10),
			20,
			(2 * (rand_value + 1)) * (rand() % 20 + -10))),
			mat,
			pxScene.physics->createShape(PxBoxGeometry(1 + 1 * rand_value, 1 + 1 * rand_value, 1 + 1 * rand_value),
				*mat)));

		std::get<0>(boxes[i])->attachShape(*std::get<2>(boxes[i]));
		std::get<2>(boxes[i])->release();
		std::get<0>(boxes[i])->userData = &boxModelMatrices[i];
		timeCreated[i]=current_time;

		pxScene.scene->addActor(*std::get<0>(boxes[i]));
	
}

void updateMines() {
	/*
	if ((time - last_spawn_time) > box_spawn_interval) {
		last_spawn_time = time;
		spawnRandomMine();
	}
	*/
	int random_margin = 5; //How much randomness in the lifetime of the boxes we want
	//Added this so that not all boxes disappear at the same time

	for (int i = 0; i < NUM_MINES; i++)
	{
		if ((current_time - timeCreated[i]) > (box_lifetime+rand()%random_margin)) {
			if (rand() % 3 == 0) {
				timeCreated[i] -= 1;
				continue;
			}
			pxScene.scene->removeActor(*std::get<0>(boxes[i]));
			
			//boxModelMatrices.erase(boxModelMatrices.begin() + i);
			//boxes.erase(boxes.begin() + i);
			//timeCreated.erase(timeCreated.begin() + i);
			renewMine(i);
		
		}

	}

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


	//Spawning new boxes and removing old ones
	updateMines();


	//Draw physics objects
	for (size_t i = 0; i < NUM_MINES; i++)
	{
		drawObjectTexture(&boxModel, boxModelMatrices[i], textureBox); // boxModelMatrix was updated in updateTransforms()

	}

	//Physics
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
	


	updateTransforms();


	// opcjonalny ruch swiatla do testow
	bool animateLight = false;
	if (animateLight)
	{
		float lightAngle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * 3.14 / 8;
		lightDir = glm::normalize(glm::vec3(sin(lightAngle), -1.0f, cos(lightAngle)));
	}

	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix(0.01f, 5000.f, frustumScale);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	// drawing ship model
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 1.0f + glm::vec3(0, -0.25f, 0))
								* glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0, 1, 0))
								* glm::scale(glm::vec3(0.07f))
								* glm::rotate(glm::radians(90.0f), glm::vec3(1.f, 0.0f, 0.f)) 
								* glm::rotate(glm::radians(180.0f), glm::vec3(0.f, 1.0f, 0.f)) 
								* glm::rotate(glm::radians(180.0f), glm::vec3(0.f, 0.0f, 1.f));
	drawObjectTextureNM(&shipModel, shipModelMatrix, textureShip, normalShip);

	// drawing EARTH
	//drawObjectTextureNM(&sphereModel, glm::translate(glm::vec3(0, 0, 0)), textureEarth, normalEarth);

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
					glm::vec3(0.3, 0.3, 0.3)); // color
				break;
		}
		

		drawObjectColor(&seaWeedModel1, glm::translate(seaWeedPositions[i]) // put at places from the preinitialized arrray with rand coords
											* glm::scale(glm::vec3(3.0f)),	// scale 3x
						glm::vec3(0.5, 0.8, 0.3));							//color
	}

	// drawing hammer shark
	drawObjectColor(&hamSharkModel, glm::translate(glm::catmullRom(cameraKeyPoints[v1], cameraKeyPoints[v2], cameraKeyPoints[v3], cameraKeyPoints[v4], time - time_int)) * glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0)) //init pos
										* glm::rotate(glm::radians(-90.0f), glm::vec3(0, 0, 1))																																	 //init pos
										* glm::rotate(glm::radians(-36.0f * time), glm::vec3(0, 0, 1))																															 //follow forward
										* glm::scale(glm::vec3(0.01f)),																																							 //make small
					glm::vec3(0.5, 0.6, 0.3));																																													 //color

	// draw terrain
	drawObjectTextureNM(&terrainModel, glm::translate(glm::vec3(0, -40, 0)) * glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(2.f)),
		textureTerrain,
		normalTerrain);

	//Draw physics objects
	for (size_t i = 0; i < NUM_MINES; i++)
	{
		drawObjectTexture(&boxModel, boxModelMatrices[i], textureBox); // boxModelMatrix was updated in updateTransforms()

	}

	drawSkybox(cameraMatrix, perspectiveMatrix);

	glutSwapBuffers();
}

void initPhysics() {
	planeBody = pxScene.physics->createRigidStatic(PxTransformFromPlaneEquation(PxPlane(0, 0.07, 0, 2)));
	planeMaterial = pxScene.physics->createMaterial(0.5, 0.5, 0.6);
	PxShape* planeShape = pxScene.physics->createShape(PxPlaneGeometry(), *planeMaterial);
	planeBody->attachShape(*planeShape);
	planeShape->release();
	planeBody->userData = NULL;
	pxScene.scene->addActor(*planeBody);


	PxMaterial* mat = pxScene.physics->createMaterial(0.2, 0.01, 0.7);
	for (int i = 0; i < NUM_MINES; i++)
	{
		boxes.push_back(std::make_tuple(pxScene.physics->createRigidDynamic(PxTransform((2 * (i + 1)) * (rand() % 20 + -10),
																						20,
																						(2 * (i + 1)) * (rand() % 20 + -10))),
										mat,
										pxScene.physics->createShape(PxBoxGeometry(1 + 1 * i, 1 + 1 * i, 1 + 1 * i),
																				   *mat)));

		std::get<0>(boxes[i])->attachShape(*std::get<2>(boxes[i]));
		std::get<2>(boxes[i])->release();
		std::get<0>(boxes[i])->userData = &boxModelMatrices[i];


		pxScene.scene->addActor(*std::get<0>(boxes[i]));
	}
}

void init()
{
	glEnable(GL_DEPTH_TEST);

	waterTile myWater = waterTile(0, 0, 1);
	waterTiles.push_back(myWater);

	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programWater = shaderLoader.CreateProgram("shaders/shader_water.vert", "shaders/shader_water.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");

	shipModel = obj::loadModelFromFile("models/submarine.obj");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");

	hamSharkModel = obj::loadModelFromFile("models/hamShark.obj");
	terrainModel = obj::loadModelFromFile("models/terrain.obj");
	rockModel1 = obj::loadModelFromFile("models/bunch/SM_Big_Rock_02.obj");
	rockModel2 = obj::loadModelFromFile("models/bunch/SM_Rock_04.obj");
	seaWeedModel1 = obj::loadModelFromFile("models/seaweed1.obj");
	boxModel = obj::loadModelFromFile("models/box.obj");
	planeModel= obj::loadModelFromFile("models/plane.obj");


	textureShip = Core::LoadTexture("textures/submarine.png");
	textureEarth = Core::LoadTexture("textures/earth2.png");
	textureAsteroid = Core::LoadTexture("textures/asteroid.png");
	textureTest = Core::LoadTexture("textures/test.png");
	textureTerrain = Core::LoadTexture("textures/terrain/diffuse.png");
	textureBox = Core::LoadTexture("textures/a.png");
	textureGround = Core::LoadTexture("textures/a.png");

	normalShip = Core::LoadTexture("textures/Submarine_normals.png");
	normalEarth = Core::LoadTexture("textures/earth2_normals.png");
	normalAsteroid = Core::LoadTexture("textures/asteroid_normals.png");
	normalTest = Core::LoadTexture("textures/test_normals.png");
	normalTerrain = Core::LoadTexture("textures/terrain/hight.png");

	loadSkybox();
	initPhysics();
	// generating rand rock positons and putting them to an array
	for (int i = 0; i < NUM_ROCKS; i++)
	{

		const int spreadMeasure = 1000;

		switch (i % 4)
		{
		case 0: // pos x, pos y
			rockPositions[i] = glm::vec3(rand() % spreadMeasure + 0, rand() % 10 + (-45.0f), rand() % spreadMeasure + 0);
			seaWeedPositions[i] = glm::vec3(rand() % spreadMeasure + 0, rand() % 10 + (-42.0f), rand() % spreadMeasure + 0);

			break;
		case 1: // neg x, pos y
			rockPositions[i] = glm::vec3(rand() % spreadMeasure + -spreadMeasure, rand() % 10 + (-45.0f), rand() % spreadMeasure + 0);
			seaWeedPositions[i] = glm::vec3(rand() % spreadMeasure + -spreadMeasure, rand() % 10 + (-42.0f), rand() % spreadMeasure + 0);

			break;
		case 2: //pos x, neg y
			rockPositions[i] = glm::vec3(rand() % spreadMeasure + 0, rand() % 10 + (-45.0f), rand() % spreadMeasure - spreadMeasure);
			seaWeedPositions[i] = glm::vec3(rand() % spreadMeasure + 0, rand() % 10 + (-42.0f), rand() % spreadMeasure - spreadMeasure);

			break;
		case 3: // neg x, neg y
			rockPositions[i] = glm::vec3(rand() % spreadMeasure + -spreadMeasure, rand() % 10 + (-45.0f), rand() % spreadMeasure + -spreadMeasure);
			seaWeedPositions[i] = glm::vec3(rand() % spreadMeasure + -spreadMeasure, rand() % 10 + (-42.0f), rand() % spreadMeasure + -spreadMeasure);

			break;
		}
	}

	static const float camRadius = 3.55;
	static const float camOffset = 0.6;
	for (int i = 0; i < NUM_CAMERA_POINTS; i++)
	{
		float angle = (float(i)) * (2 * glm::pi<float>() / NUM_CAMERA_POINTS);
		float radius = camRadius * (0.95 + glm::linearRand(0.0f, 0.1f));
		cameraKeyPoints[i] = glm::vec3(cosf(angle) + camOffset, 0.0f, sinf(angle)) * radius;
	}
	appLoadingTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
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

int main(int argc, char **argv)
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
