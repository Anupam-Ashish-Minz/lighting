#version 330 core

out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	color = vec3(1.0, 1.0, 1.0);
}
