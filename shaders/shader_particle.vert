#version 330 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 texCoords;

uniform mat4 perspectiveMatrix;
uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;
uniform float opacity;
uniform vec3 cameraPos;	
uniform float particleZ;


const float density = 0.007;
const float gradient = 1.5;


out vec3 fragPos;
out vec2 interpTexCoord;
out float visibility;
out vec4 worldPosition;

void main(void){
	
	interpTexCoord = texCoords;
	fragPos = (modelMatrix * vec4(vertexPosition,0.0, 1.0)).xyz;
	float dist = distance(cameraPos.xyz, worldPosition.xyz);
	visibility = exp(-pow((dist * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);


	gl_Position = perspectiveMatrix * cameraMatrix*modelMatrix*vec4(vertexPosition, 0.0, 1.0);

}