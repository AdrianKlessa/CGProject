#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelMatrix;
uniform vec3 cameraPos;	


const vec4 plane = vec4(0,-1,0,15);
const float density = 0.007;
const float gradient = 1.5;

out vec3 interpNormal;
out vec3 fragPos;
out float visibility;

void main()
{
	vec4 worldPosition = modelMatrix*vec4(vertexPosition,1.0);

	gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
	fragPos = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;

	interpNormal = (modelMatrix * vec4(vertexNormal, 0.0)).xyz;

	// fog setup
	float dist = distance(cameraPos.xyz, worldPosition.xyz);
	visibility = exp(-pow((dist * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
