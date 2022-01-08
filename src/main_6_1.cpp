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

class waterTile {
private:
	float height;
	float x, z;
public:
	float TILE_SIZE = 60;

	waterTile(float centerX, float centerZ, float h) {
		x = centerX;
		z = centerZ;
		height = h;
	}

	float getHeight() {
		return height;
	}

	float getX() {
		return x;
	}

	float getZ() {
		return z;
	}

};


GLuint programColor;
GLuint programTexture;
GLuint programWater;

Core::Shader_Loader shaderLoader;

float appLoadingTime;

obj::Model planeModel;
obj::Model shipModel;
obj::Model sphereModel;

float cameraAngle = glm::radians(-90.f);
glm::vec3 cameraPos = glm::vec3(0, 0, 5);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f));

GLuint textureTest;
GLuint textureEarth;
GLuint textureAsteroid;
GLuint textureShip;

GLuint normalTest;
GLuint normalEarth;
GLuint normalAsteroid;
GLuint normalShip;
float waterVertices[] = { -100.0f, -100.0f, -100.0f, 1.0f, 1.0f, -1.0f, 100.0f, -1.0f, -1.0f, 1.0f, 1.0f, 100.0f };

GLuint waterVBO;
GLuint waterVAO;

GLuint normalVBO;
GLuint normalVAO;

float frustumScale = 1.f;
std::list<waterTile> waterTiles;
static const int NUM_ASTEROIDS = 8;
glm::vec3 asteroidPositions[NUM_ASTEROIDS];
static const int NUM_CAMERA_POINTS = 10;
glm::vec3 cameraKeyPoints[NUM_CAMERA_POINTS];



void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch(key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'e': cameraPos += glm::vec3(0, 1, 0) * moveSpeed; break;
	case 'q': cameraPos -= glm::vec3(0, 1, 0) * moveSpeed; break;
	}
}

glm::mat4 createCameraMatrix()
{	/*
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - appLoadingTime;
	if (time > NUM_CAMERA_POINTS) {
		time = std::fmod(time, NUM_CAMERA_POINTS);
	}
	int integerPartOfTime = floor(time);
	int v1index = integerPartOfTime;
	int v0index, v2index, v3index;

	if (v1index == 0) {
		v0index = NUM_CAMERA_POINTS - 1;
	}
	else {
		v0index = v1index - 1;
	}
	v2index = std::fmod(v1index + 1, NUM_CAMERA_POINTS);
	v3index = std::fmod(v1index + 2, NUM_CAMERA_POINTS);
	glm::vec3 v0 = cameraKeyPoints[v0index];
	glm::vec3 v1 = cameraKeyPoints[v1index];
	glm::vec3 v2 = cameraKeyPoints[v2index];
	glm::vec3 v3 = cameraKeyPoints[v3index];

	float s = time - integerPartOfTime;

	glm::vec3 interpolatedPos = glm::catmullRom(v0, v1, v2, v3,s);
	glm::vec3 tangent= glm::normalize(glm::catmullRom(v0, v1, v2, v3, s + 0.001) - glm::catmullRom(v0, v1, v2, v3, s - 0.001));
	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraAngle = atan2f(tangent.z, tangent.x);
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0,1,0);
	return Core::createViewMatrix(interpolatedPos, cameraDir, up);
	*/
	// return Core::createViewMatrix(cameraPos, cameraDir, up);

	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0, 1, 0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
	

}

void setUpUniforms(GLuint program, glm::mat4 modelMatrix)
{
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
}

void setUpUniformsWater(glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix) {
	GLuint program = programWater;
	
	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_FALSE, (float*)&cameraMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_FALSE, (float*)&perspectiveMatrix);

//Model matrix
//View matrix
//Projection matrix
}




void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	setUpUniforms(program, modelMatrix);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId, GLuint normalmapId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	setUpUniforms(program, modelMatrix);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);
	Core::SetActiveTexture(normalmapId, "normalSampler", program, 1);
	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTextureNM(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	setUpUniforms(program, modelMatrix);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawWater(std::list<waterTile> water, glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix) {
	GLuint program = programWater;
	glUseProgram(program);
	setUpUniformsWater(cameraMatrix, perspectiveMatrix);

	float vertices[] = { -1, -1, -1, 1, 1, -1, 1, -1, -1, 1, 1, 1 };
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(0);

	for (auto& tile : water) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(), glm::vec3(tile.getX(), tile.getHeight(), tile.getZ()));	
		modelMatrix = modelMatrix * glm::scale(glm::mat4(), glm::vec3(tile.TILE_SIZE,tile.TILE_SIZE, tile.TILE_SIZE));
		glUniformMatrix4fv(glGetUniformLocation(programWater, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	


	/*
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, waterVertices);
	glEnableVertexAttribArray(0);
	

	glDrawArrays(GL_TRIANGLES, 0, 4);
	
	

	
	GLfloat vertices[] = { -1, -1, -0.25, // bottom left corner
					  -1,  1, -0.52, // top left corner
					   1,  1, -0.25, // top right corner
					   1, -1, -0.25 }; // bottom right corner

	GLubyte indices[] = { 0,1,2, // first triangle (bottom left - top left - top right)
						 0,2,3 }; // second triangle (bottom left - top right - bottom right)


	glDrawArrays(GL_TRIANGLES, 0,6);
	*/
	glUseProgram(0);
}

void renderScene()
{
	// opcjonalny ruch swiatla do testow
	bool animateLight = false;
	if (animateLight) {
		float lightAngle = (glutGet(GLUT_ELAPSED_TIME) / 1000.0f) * 3.14 / 8;
		lightDir = glm::normalize(glm::vec3(sin(lightAngle), -1.0f, cos(lightAngle)));
	}

	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix(0.1f, 100.f, frustumScale);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	
	// Macierz statku "przyczepia" go do kamery. Warto przeanalizowac te linijke i zrozumiec jak to dziala.
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.6f + glm::vec3(0,-0.25f,0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.07f))*glm::rotate(glm::radians(90.0f), glm::vec3(1.f, 0.0f, 0.f)) * glm::rotate(glm::radians(180.0f), glm::vec3(0.f, 1.0f, 0.f)) * glm::rotate(glm::radians(180.0f), glm::vec3(0.f, 0.0f, 1.f));
	drawObjectTexture(&shipModel, shipModelMatrix, textureShip,normalShip);

	



	
	for (int i = 0; i < NUM_ASTEROIDS; i++)
	{
		drawObjectTexture(&sphereModel, glm::translate(asteroidPositions[i]) * glm::scale(glm::vec3(0.2f)), textureAsteroid,normalAsteroid);
	}
	drawObjectTexture(&sphereModel, glm::translate(glm::vec3(0, 0, 0)), textureEarth,normalEarth);

	drawObjectTexture(&planeModel, glm::translate(glm::vec3(-3, 0, 0))*glm::scale(glm::vec3(1, 0.5, 0.5)), textureTest,normalTest);
	
	//glBindVertexArray(waterVAO);
	drawWater(waterTiles, cameraMatrix, perspectiveMatrix);
	

	glutSwapBuffers();
}



void init()
{
	glEnable(GL_DEPTH_TEST);

	waterTile myWater = waterTile(0, 0, 1);
	waterTiles.push_back(myWater);
	/*
	glGenVertexArrays(1, &normalVAO);
	glGenBuffers(1, &normalVBO);
	glBindVertexArray(normalVAO);
	glBindBuffer(GL_ARRAY_BUFFER,normalVBO);
	*/

	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programWater = shaderLoader.CreateProgram("shaders/shader_water.vert", "shaders/shader_water.frag");

	shipModel = obj::loadModelFromFile("models/submarine.obj");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	planeModel = obj::loadModelFromFile("models/plane.obj");

	textureShip = Core::LoadTexture("textures/submarine.png");
	textureEarth = Core::LoadTexture("textures/earth2.png");
	textureAsteroid = Core::LoadTexture("textures/asteroid.png");
	textureTest = Core::LoadTexture("textures/test.png");

	normalShip = Core::LoadTexture("textures/Submarine_normals.png");
	normalEarth = Core::LoadTexture("textures/earth2_normals.png");
	normalAsteroid = Core::LoadTexture("textures/asteroid_normals.png");
	normalTest = Core::LoadTexture("textures/test_normals.png");

	/*
	
	glUseProgram(programWater);

	glGenVertexArrays(1, &waterVAO);
	glGenBuffers(1, &waterVBO);
	
	glBindVertexArray(waterVAO);
	glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindVertexArray(normalVAO);
	glUseProgram(0);
	*/

	static const float astRadius = 6.0;
	for (int i = 0; i < NUM_ASTEROIDS; i++)
	{
		float angle = (float(i))*(2 * glm::pi<float>() / NUM_ASTEROIDS);
		asteroidPositions[i] = glm::vec3(cosf(angle), 0.0f, sinf(angle)) * astRadius + glm::sphericalRand(0.5f);
	}

	static const float camRadius = 3.55;
	static const float camOffset = 0.6;
	for (int i = 0; i < NUM_CAMERA_POINTS; i++)
	{
		float angle = (float(i))*(2 * glm::pi<float>() / NUM_CAMERA_POINTS);
		float radius = camRadius *(0.95 + glm::linearRand(0.0f, 0.1f));
		cameraKeyPoints[i] = glm::vec3(cosf(angle) + camOffset, 0.0f, sinf(angle)) * radius;
	}

	appLoadingTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
	shaderLoader.DeleteProgram(programWater);
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

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL Pierwszy Program");
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

