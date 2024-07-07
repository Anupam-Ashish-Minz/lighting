#version 330 core

layout (location = 1) in vec3 pos;

void main() {
	gl_Position.xyz = pos;
	gl_Position.w = 1.0;
}
