#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define WIDTH 1280
#define HEIGHT 720

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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Set the clear color to blue
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
	}

	return 0;
}
