#version 330 core

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

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

	vec3 res = (ambientColor + diffuse) * objectColor;
	fragColor = vec4(res, 1.0);
}
