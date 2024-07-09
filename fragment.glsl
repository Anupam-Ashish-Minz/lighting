#version 330 core

out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
	float ambientStrength = 0.1;
	vec3 ambientColor = ambientStrength * lightColor;
	color = vec4(ambientColor * objectColor, 1.0);
}
