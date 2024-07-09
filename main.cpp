#include "shader.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define WIDTH 1920
#define HEIGHT 1080

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
								GLenum severity, GLsizei length,
								const GLchar *message, const void *userParam) {
	fprintf(stderr,
			"GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
			severity, message);
}

void process_key_input(GLFWwindow *window) {
	const float cameraSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos +=
			glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos -=
			glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraUp;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraUp;
	}
}

bool firstMouse = true;
float lastX, lastY;
float yaw = -90.0f;
float pitch = 0.0f;

void process_mouse_input(GLFWwindow *window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	const float sensitivity = 0.1f;
	float xoffset = lastX - xpos;
	float yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraFront = glm::normalize(direction);
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
		/// front
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, /// 0
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, /// 1
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, /// 2
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, /// 1
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, /// 2
		1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, /// 3
		/// back
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, /// 4
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, /// 5
		1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, /// 6
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, /// 5
		1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, /// 6
		1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, /// 7
		/// bottom
		0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, /// 0
		0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, /// 1
		0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, /// 4
		0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, /// 1
		0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, /// 4
		0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, /// 5
		/// left
		0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, /// 0
		1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, /// 2
		0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, /// 4
		1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, /// 2
		0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, /// 4
		1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, /// 6
		/// right
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, /// 1
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, /// 3
		0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, /// 5
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, /// 3
		0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, /// 5
		1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, /// 7
		/// top
		1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, /// 2
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, /// 3
		1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, /// 6
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, /// 3
		1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, /// 6
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, /// 7
		/// lighting cube
		/// front
		-6.0f, -6.0f, 0.0f, /// 0
		-6.0f, -7.0f, 0.0f, /// 1
		-7.0f, -6.0f, 0.0f, /// 2
		-6.0f, -7.0f, 0.0f, /// 1
		-7.0f, -6.0f, 0.0f, /// 2
		-7.0f, -7.0f, 0.0f, /// 3
		/// back
		-6.0f, -6.0f, 1.0f, /// 4
		-6.0f, -7.0f, 1.0f, /// 5
		-7.0f, -6.0f, 1.0f, /// 6
		-6.0f, -7.0f, 1.0f, /// 5
		-7.0f, -6.0f, 1.0f, /// 6
		-7.0f, -7.0f, 1.0f, /// 7
		/// bottom
		-6.0f, -6.0f, 0.0f, /// 0
		-6.0f, -7.0f, 0.0f, /// 1
		-6.0f, -6.0f, 1.0f, /// 4
		-6.0f, -7.0f, 0.0f, /// 1
		-6.0f, -6.0f, 1.0f, /// 4
		-6.0f, -7.0f, 1.0f, /// 5
		/// left
		-6.0f, -6.0f, 0.0f, /// 0
		-7.0f, -6.0f, 0.0f, /// 2
		-6.0f, -6.0f, 1.0f, /// 4
		-7.0f, -6.0f, 0.0f, /// 2
		-6.0f, -6.0f, 1.0f, /// 4
		-7.0f, -6.0f, 1.0f, /// 6
		/// right
		-6.0f, -7.0f, 0.0f, /// 1
		-7.0f, -7.0f, 0.0f, /// 3
		-6.0f, -7.0f, 1.0f, /// 5
		-7.0f, -7.0f, 0.0f, /// 3
		-6.0f, -7.0f, 1.0f, /// 5
		-7.0f, -7.0f, 1.0f, /// 7
		/// top
		-7.0f, -6.0f, 0.0f, /// 2
		-7.0f, -7.0f, 0.0f, /// 3
		-7.0f, -6.0f, 1.0f, /// 6
		-7.0f, -7.0f, 0.0f, /// 3
		-7.0f, -6.0f, 1.0f, /// 6
		-7.0f, -7.0f, 1.0f, /// 7
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, process_mouse_input);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();
		process_key_input(window);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		baseShader->use();
		baseShader->setUniformVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
		baseShader->setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		baseShader->setMVPMatrix(model, view, projection);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightingShader->use();
		lightingShader->setMVPMatrix(model, view, projection);
		glBindVertexArray(lightingVAO);
		glDrawArrays(GL_TRIANGLES, 72, 36);
		glfwSwapBuffers(window);
	}

	return 0;
}
