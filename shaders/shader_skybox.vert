#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;
out float visibility;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 modelMatrix;
uniform vec3 cameraPos;	

const float density = 0.007;
const float gradient = 1.5;


void main()
{
	vec4 worldPosition = modelMatrix*vec4(TexCoords, 1.0);

    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = aPos.xyzz;
    
    
    // fog setup
	//float dist = distance(cameraPos.xyz, worldPosition.xyz);
    float dist = 1000; //The skybox is really far away
	visibility = exp(-pow((dist * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}  