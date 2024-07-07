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

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	return 0;
}
