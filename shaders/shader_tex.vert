#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;

uniform vec3 lightDir;
uniform vec3 cameraPos;	
uniform vec4 plane;


out vec2 interpTexCoord;
out vec3 lightDirTS;
out vec3 viewDirTS;
out float visibility;

const float density = 0.007;
const float gradient = 1.5;


void main()
{	

	gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);
	vec3 vertPos = (modelMatrix * vec4(vertexPosition, 1.0)).xyz; //poss with the submarine

	vec4 worldPosition = modelMatrix*vec4(vertexPosition,1.0);
	gl_ClipDistance[0]=dot(worldPosition,plane);

	vec3 normal=(modelMatrix*vec4(vertexNormal,0.0f)).xyz; //Not sure if correct order of multiplication, check to make sure
	vec3 tangent=(modelMatrix*vec4(vertexTangent,0.0f)).xyz;
	vec3 bitangent=(modelMatrix*vec4(vertexBitangent,0.0f)).xyz;
	mat3 TBN = transpose(mat3(tangent,bitangent,normal));
	vec3 viewDir = normalize(cameraPos-vertPos);
	lightDirTS = TBN*lightDir;
	viewDirTS = TBN*viewDir;

	//interpNormal = (modelMatrix * vec4(vertexNormal, 0.0)).xyz;
	interpTexCoord = vertexTexCoord;

    // fog setup
	float dist = distance(cameraPos.xyz, worldPosition.xyz);
	visibility = exp(-pow((dist * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
