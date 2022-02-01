#version 330 core

uniform vec3 objectColor;
uniform vec3 lightDir;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 fragPos;
in float visibility;

uniform float cutOff;
uniform float cutOffOut;
uniform vec3 cameraDir;

void main()
{
	vec3 L = -lightDir;
	vec3 V = normalize(cameraPos - fragPos);
	vec3 N = normalize(interpNormal);
	vec3 R = reflect(-normalize(L), N);

	float diffuse = max(0, dot(N, L));
	
	float specular_pow = 10;
	float specular = pow(max(0, dot(R, V)), specular_pow);

	vec3 lightColor = vec3(1);
	vec3 shadedColor = objectColor * diffuse + lightColor * specular;
	
	float ambient = 0.125;

    
	float theta = dot(V, normalize(-cameraDir));
    float epsilon = (cutOff - cutOffOut);
    float intensity = max(ambient,clamp((theta - cutOffOut) / epsilon, 0.0, 1.0));

	gl_FragColor = vec4(mix(vec3(0.3, 0.3, 0.3), objectColor, visibility * intensity), 1.0);
}