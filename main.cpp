#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define WIDTH 1280
#define HEIGHT 720

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

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	float data[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
	}

	return 0;
}
