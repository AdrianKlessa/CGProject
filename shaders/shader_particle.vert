#version 430 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 texCoords;

uniform mat4 perspectiveMatrix;
uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;
uniform float opacity;

// uniform vec3 cameraPos;	
//No fog for now...


const float density = 0.007;
const float gradient = 1.5;


out vec3 fragPos;
out float visibility;
out vec2 interpTexCoord;

void main(void){
	visibility = 1.0;
	interpTexCoord = texCoords;
	gl_Position = perspectiveMatrix * cameraMatrix*modelMatrix*vec4(vertexPosition, 0.0, 1.0);

}