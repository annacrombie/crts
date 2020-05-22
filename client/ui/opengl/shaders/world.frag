#version 330 core

out vec4 clr;

flat in vec4 inclr;
flat in vec3 normal;
in vec3 frag_pos;

vec3 lightColor = vec3(1, 1, 1);
vec3 selColor = vec3(0.0, 0.0, 1.0);
vec3 lightDir = normalize(vec3(0.1, 3.2, 0.0));
float ambientStrength = 0.1;
float specularStrength = 0.04;

uniform vec3 view_pos;

void main()
{
	vec3 norm = normalize(normal);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 viewDir = normalize(view_pos - frag_pos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 ambient = ambientStrength * lightColor;

	clr = vec4(ambient + diffuse + specular, 1.0) * inclr;
	//clr = vec4(normal, 1.0);
}