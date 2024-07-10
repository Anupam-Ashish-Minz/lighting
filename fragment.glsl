#version 330 core

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 fragNormal;
in vec3 fragPos;

out vec4 fragColor;

void main() {
	float ambientStrength = 0.1;
	vec3 ambientColor = ambientStrength * lightColor;

	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 res = (ambientColor + diffuse + specular) * objectColor;
	fragColor = vec4(res, 1.0);
}
