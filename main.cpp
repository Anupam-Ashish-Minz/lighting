#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

using glm::mat4;
using glm::vec3;

#define WIDTH 1920
#define HEIGHT 1080

vec3 cameraPos = vec3(0.5f, 0.5f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
								GLenum severity, GLsizei length,
								const GLchar *message, const void *userParam) {
	fprintf(stderr,
			"GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
			severity, message);
}

std::string read_to_string(const char *path) {
	std::fstream f(path);
	std::stringstream ss;
	ss << f.rdbuf();
	return ss.str();
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

	vec3 direction;
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

	float vertices[] = {
		0.0f, 0.0f, 0.0f, ///
		0.0f, 1.0f, 0.0f, ///
		1.0f, 0.0f, 0.0f, ///
		1.0f, 1.0f, 0.0f, ///
		0.0f, 0.0f, 1.0f, ///
		0.0f, 1.0f, 1.0f, ///
		1.0f, 0.0f, 1.0f, ///
		1.0f, 1.0f, 1.0f, ///
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int indices[] = {
		0, 1, 2, 1, 2, 3, ///
		0, 1, 4, 1, 4, 5, ///
		0, 2, 4, 2, 4, 6, ///
		1, 3, 5, 3, 5, 7, ///
		2, 3, 6, 3, 6, 7, ///
		4, 5, 6, 5, 6, 7, ///
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
				 GL_STATIC_DRAW);

	std::string vertexShaderCode = read_to_string("vertex.glsl");
	const char *vertexShaderCodePointer = vertexShaderCode.c_str();

	std::string fragmentShaderCode = read_to_string("fragment.glsl");
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char *fragmentShaderCodePointer = fragmentShaderCode.c_str();

	glShaderSource(vertexShader, 1, &vertexShaderCodePointer, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderCodePointer, NULL);
	glCompileShader(vertexShader);
	int status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

#define S_LOG_BUF 100
	char log[S_LOG_BUF];
	int len;
	if (status == GL_FALSE) {
		glGetShaderInfoLog(vertexShader, S_LOG_BUF, &len, log);
		std::cerr << log << std::endl;
	}

	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderInfoLog(fragmentShader, S_LOG_BUF, &len, log);
		std::cerr << log << std::endl;
	}

	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		glGetProgramInfoLog(program, S_LOG_BUF, &len, log);
		std::cerr << log << std::endl;
	} else {
		glUseProgram(program);
	}

	unsigned int u_Model = glGetUniformLocation(program, "model");
	unsigned int u_View = glGetUniformLocation(program, "view");
	unsigned int u_Projection = glGetUniformLocation(program, "projection");

	mat4 model = mat4(1.0f);
	mat4 view = mat4(1.0f);
	mat4 projection = mat4(1.0f);

	projection = glm::perspective(glm::radians(-45.0f),
								  (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, process_mouse_input);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		process_key_input(window);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		glUniformMatrix4fv(u_Model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(u_View, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(u_Projection, 1, GL_FALSE,
						   glm::value_ptr(projection));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
	}

	return 0;
}
