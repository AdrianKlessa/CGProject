#version 330 core


uniform sampler2D textureSampler;
uniform float opacity;
uniform vec3 cameraPos;
uniform float cutOff;
uniform float cutOffOut;
uniform vec3 cameraDir;

in vec3 fragPos;

in vec2 interpTexCoord;
in vec4 worldPosition;


in float visibility;

void main()
{
	
	vec4 color = texture2D(textureSampler, interpTexCoord).rgba;

	
	//fog setup
	//gl_FragColor = vec4(mix(vec3(0.3, 0.3, 0.3), color, visibility), opacity);
	float ambient = 0.125;
    
	vec3 light_direction = normalize(cameraPos - fragPos);
	vec3 spotDir = cameraDir;

	
	float theta = dot(light_direction, normalize(-cameraDir));
    float epsilon = (cutOff - cutOffOut);
    float intensity = max(clamp((theta - cutOffOut) / epsilon, 0.0, 1.0),ambient);

	gl_FragColor = vec4(mix(vec3(0.3, 0.3, 0.3), color.rgb, visibility*intensity),color.a);
}