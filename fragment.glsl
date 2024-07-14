#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 fragNormal;
in vec3 fragPos;

out vec4 fragColor;

vec3 diffuse_light(vec3 lightPos, vec3 lightColor) {
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = lightColor * (diff * material.diffuse);
	return diffuse;
}

vec3 specular_light(vec3 lightPos, vec3 lightColor) {
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = lightColor * (spec * material.specular);
	return specular;
}

void main() {
	float ambientStrength = 0.1;
	vec3 ambient = lightColor * material.ambient;
	vec3 diffuse = diffuse_light(lightPos, lightColor);
	vec3 specular = specular_light(lightPos, lightColor);

	vec3 res = (ambient + diffuse + specular) * objectColor;
	fragColor = vec4(res, 1.0);
}
