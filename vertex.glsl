#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 aPos;
in vec3 aNormal;

out vec3 fragNormal;
out vec3 fragPos;

void main() {
	vec4 mPos =  model * vec4(aPos, 1.0);
	vec4 mNormal = model * vec4(aNormal, 1.0);
	gl_Position = projection * view * mPos;
	fragNormal = vec3(mNormal);
	fragPos = vec3(mPos);
}
