#version 330 core

out vec3 color;

uniform vec3 lightBoxColor;

void main() {
	color = lightBoxColor;
}
