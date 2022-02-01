#version 330 core

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;
uniform vec3 lightDir;
uniform vec3 cameraPos;

uniform float cutOff;
uniform float cutOffOut;
uniform vec3 cameraDir;

in vec3 interpNormal;
in vec2 interpTexCoord;
in vec3 fragPos;
in vec3 lightDirTS;
in vec3 viewDirTS;

in float visibility;

void main()
{

	
	vec3 L = -(normalize(lightDirTS));
	vec3 V =  normalize(viewDirTS);
	vec3 N = texture2D(normalSampler,interpTexCoord).rgb;
	N=(N*2)-1;
	N=normalize(N);
	vec3 R = reflect(-normalize(L), N);

	float diffuse = max(0, dot(N, L));
	
	float specular_pow = 10;
	float specular = pow(max(0, dot(R, V)), specular_pow);

	vec3 color = texture2D(textureSampler, interpTexCoord).rgb;

	vec3 lightColor = vec3(1);
	vec3 shadedColor = color * diffuse + lightColor * specular;
	
	float ambient = 0.125;
    
	vec3 light_direction = normalize(cameraPos - fragPos);
	vec3 spotDir = cameraDir;

	
	float theta = dot(light_direction, normalize(-cameraDir));
    float epsilon = (cutOff - cutOffOut);
    float intensity = max(clamp((theta - cutOffOut) / epsilon, 0.0, 1.0),ambient);

	//fog setup
	gl_FragColor = vec4(mix(vec3(0.3, 0.3, 0.3), color, visibility*intensity), 1.0);

}
