#include "camera.hpp"
#include "shader.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1920
#define HEIGHT 1080

Camera *camera;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
								GLenum severity, GLsizei length,
								const GLchar *message, const void *userParam) {
	fprintf(stderr,
			"GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
			severity, message);
}

void MouseInputCallback(GLFWwindow *window, double xpos, double ypos) {
	camera->process_mouse_input(window, xpos, ypos);
}

int main() {
	if (!glfwInit()) {
		std::cerr << "failed to init glfw" << std::endl;
		return -1;
	}

	GLFWwindow *window =
		glfwCreateWindow(WIDTH, HEIGHT, "triangles", NULL, NULL);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cerr << "failed to init glfw" << std::endl;
		return -1;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
	glEnable(GL_DEPTH_TEST);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	unsigned int lightingVAO;
	glGenVertexArrays(1, &lightingVAO);
	glBindVertexArray(lightingVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	float vertices[] = {
		-0.5f, -0.5f, -0.5f, 0.0f,	0.0f,  -1.0f, 0.5f,	 -0.5f, -0.5f,
		0.0f,  0.0f,  -1.0f, 0.5f,	0.5f,  -0.5f, 0.0f,	 0.0f,	-1.0f,
		0.5f,  0.5f,  -0.5f, 0.0f,	0.0f,  -1.0f, -0.5f, 0.5f,	-0.5f,
		0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,	 0.0f,	-1.0f,

		-0.5f, -0.5f, 0.5f,	 0.0f,	0.0f,  1.0f,  0.5f,	 -0.5f, 0.5f,
		0.0f,  0.0f,  1.0f,	 0.5f,	0.5f,  0.5f,  0.0f,	 0.0f,	1.0f,
		0.5f,  0.5f,  0.5f,	 0.0f,	0.0f,  1.0f,  -0.5f, 0.5f,	0.5f,
		0.0f,  0.0f,  1.0f,	 -0.5f, -0.5f, 0.5f,  0.0f,	 0.0f,	1.0f,

		-0.5f, 0.5f,  0.5f,	 -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,	-0.5f,
		-1.0f, 0.0f,  0.0f,	 -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,	0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
		-1.0f, 0.0f,  0.0f,	 -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,	0.0f,

		0.5f,  0.5f,  0.5f,	 1.0f,	0.0f,  0.0f,  0.5f,	 0.5f,	-0.5f,
		1.0f,  0.0f,  0.0f,	 0.5f,	-0.5f, -0.5f, 1.0f,	 0.0f,	0.0f,
		0.5f,  -0.5f, -0.5f, 1.0f,	0.0f,  0.0f,  0.5f,	 -0.5f, 0.5f,
		1.0f,  0.0f,  0.0f,	 0.5f,	0.5f,  0.5f,  1.0f,	 0.0f,	0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f,	-1.0f, 0.0f,  0.5f,	 -0.5f, -0.5f,
		0.0f,  -1.0f, 0.0f,	 0.5f,	-0.5f, 0.5f,  0.0f,	 -1.0f, 0.0f,
		0.5f,  -0.5f, 0.5f,	 0.0f,	-1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
		0.0f,  -1.0f, 0.0f,	 -0.5f, -0.5f, -0.5f, 0.0f,	 -1.0f, 0.0f,

		-0.5f, 0.5f,  -0.5f, 0.0f,	1.0f,  0.0f,  0.5f,	 0.5f,	-0.5f,
		0.0f,  1.0f,  0.0f,	 0.5f,	0.5f,  0.5f,  0.0f,	 1.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	 0.0f,	1.0f,  0.0f,  -0.5f, 0.5f,	0.5f,
		0.0f,  1.0f,  0.0f,	 -0.5f, 0.5f,  -0.5f, 0.0f,	 1.0f,	0.0f,

		-0.5f, -0.5f, -0.5f, 0.5f,	-0.5f, -0.5f, 0.5f,	 0.5f,	-0.5f,
		0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f, 0.5f,	 0.5f,	-0.5f, 0.5f,  0.5f,	 0.5f,	0.5f,
		0.5f,  0.5f,  0.5f,	 -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,

		-0.5f, 0.5f,  0.5f,	 -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,	0.5f,

		0.5f,  0.5f,  0.5f,	 0.5f,	0.5f,  -0.5f, 0.5f,	 -0.5f, -0.5f,
		0.5f,  -0.5f, -0.5f, 0.5f,	-0.5f, 0.5f,  0.5f,	 0.5f,	0.5f,

		-0.5f, -0.5f, -0.5f, 0.5f,	-0.5f, -0.5f, 0.5f,	 -0.5f, 0.5f,
		0.5f,  -0.5f, 0.5f,	 -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f,

		-0.5f, 0.5f,  -0.5f, 0.5f,	0.5f,  -0.5f, 0.5f,	 0.5f,	0.5f,
		0.5f,  0.5f,  0.5f,	 -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,	-0.5f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	Shader *baseShader = new Shader("vertex.glsl", "fragment.glsl");

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(
		glm::radians(-45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	Shader *lightingShader = new Shader("vertex.glsl", "lighting.glsl");

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
						  (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(lightingVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	camera = new Camera();

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MouseInputCallback);

	glm::vec3 lightPos = glm::vec3(1.5f, -1.5f, 3.0f);
	glm::mat4 rotationMatrix =
		glm::rotate(glm::mat4(1.0f), 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));

	FILE *teapot_file = fopen("teapot_bezier0.norm.txt", "r");
	if (teapot_file == NULL) {
		perror("error opening file");
		return -1;
	}

	unsigned int triangle_count;
	if (fscanf(teapot_file, "%d", &triangle_count) != 1) {
		printf("failed to read the triangle count from file\n");
		return -1;
	}
	float a, b, c;
	int count = 0;
	int teapot_size = sizeof(float) * triangle_count * 18;
	float *teapot_data = (float *)malloc(teapot_size);
	while (true) {
		int res = fscanf(teapot_file, "%f %f %f", &a, &b, &c);
		if (res == EOF) {
			break;
		}
		if (res == 3) {
			teapot_data[count] = a;
			teapot_data[count + 1] = b;
			teapot_data[count + 2] = c;
			count += 3;
		} else {
			printf("some unexpected line encountered in teapot model file\n");
		}
	}

	unsigned int teapotVAO, teapotVBO;
	glGenVertexArrays(1, &teapotVAO);
	glGenBuffers(1, &teapotVBO);
	glBindVertexArray(teapotVAO);
	glBindBuffer(GL_ARRAY_BUFFER, teapotVBO);
	glBufferData(GL_ARRAY_BUFFER, teapot_size, teapot_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
						  (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glm::mat4 teapotModel = glm::mat4(1.0f);
	teapotModel = glm::rotate(teapotModel, glm::radians(180.0f),
							  glm::vec3(1.0f, 0.0f, 0.0f));

	float lastFrame = 0.0f;
	glm::vec3 lightColor1 = glm::vec3(1.0f, 1.0f, 1.0f);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();
		camera->process_key_input(window);

		view = camera->view();
		model = glm::mat4(1.0f);
		// lightPos = glm::vec3(glm::vec4(lightPos, 1.0f) * rotationMatrix);

		float currentFrame = (float)glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		baseShader->use();
		teapotModel = glm::rotate(teapotModel, glm::radians(100.0f * deltaTime),
								  glm::vec3(0.0f, 1.0f, 0.0f));
		baseShader->setMVPMatrix(teapotModel, view, projection);

		baseShader->setUniformVec3("light.position", lightPos);
		baseShader->setUniformVec3("light.ambient",
								   glm::vec3(0.2f, 0.2f, 0.2f));
		baseShader->setUniformVec3("light.diffuse",
								   glm::vec3(0.5f, 0.5f, 0.5f));
		baseShader->setUniformVec3("light.specular",
								   glm::vec3(1.0f, 1.0f, 1.0f));

		baseShader->setUniformVec3("viewPos", camera->getPos());

		baseShader->setUniformVec3("material.ambient",
								   glm::vec3(1.0f, 0.5f, 0.31f));
		baseShader->setUniformVec3("material.diffuse",
								   glm::vec3(1.0f, 0.5f, 0.31f));
		baseShader->setUniformVec3("material.specular",
								   glm::vec3(0.5f, 0.5f, 0.5f));
		baseShader->setUniformFloat("material.shininess", 32.0f);

		glBindVertexArray(teapotVAO);
		glBindBuffer(GL_ARRAY_BUFFER, teapotVBO);
		glDrawArrays(GL_TRIANGLES, 0, triangle_count * 9);

		// glBindVertexArray(VAO);
		// glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// glDrawArrays(GL_TRIANGLES, 0, 36);

		lightingShader->use();
		model = glm::translate(glm::mat4(1.0f), lightPos);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		lightingShader->setMVPMatrix(model, view, projection);
		lightingShader->setUniformVec3("lightBoxColor", lightColor1);
		glBindVertexArray(lightingVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDrawArrays(GL_TRIANGLES, 72, 36);

		// glfwSwapInterval(0);

		glfwSwapBuffers(window);
	}

	return 0;
}
