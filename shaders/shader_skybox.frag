#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
in float visibility;

uniform samplerCube skybox;
uniform float cutOff;
uniform float cutOffOut;
uniform vec3 cameraDir;

void main()
{    
    FragColor = vec4(mix(vec3(0.3, 0.3, 0.3),  texture(skybox, TexCoords).xyz, visibility), 1.0);
}