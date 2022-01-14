#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
in float visibility;

uniform samplerCube skybox;

void main()
{    
    FragColor = vec4(mix(vec3(0.3, 0.3, 0.3),  texture(skybox, TexCoords).xyz, visibility), 1.0);
   

    //fog setup
	// gl_FragColor = vec4(mix(vec3(0.3, 0.3, 0.3), FragColor, visibility), 1.0);

}