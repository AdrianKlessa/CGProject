#version 330 core


uniform sampler2D textureSampler;
uniform float opacity;
in vec3 fragPos;

in vec2 interpTexCoord;



in float visibility;

void main()
{
	
	vec3 color = texture2D(textureSampler, interpTexCoord).rgb;

	
	//fog setup
	gl_FragColor = vec4(mix(vec3(0.3, 0.3, 0.3), color, visibility), opacity);
}