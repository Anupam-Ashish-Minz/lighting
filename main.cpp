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

#define WIDTH 1920
#define HEIGHT 1080

glm::vec3 cameraPos = glm::vec3(0.5f, 0.5f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

class Shader {
  private:
	unsigned int ID;
	unsigned int u_Model;
	unsigned int u_View;
	unsigned int u_Projection;

	std::string read_file(const char *path) {
		std::ifstream f(path);
		std::stringstream ss;
		ss << f.rdbuf();
		return ss.str();
	}

  public:
	Shader(const char *vertexShaderPath, const char *fragmentShaderPath) {
		std::string vertexShaderCode = this->read_file(vertexShaderPath);
		const char *vertexShaderCodePointer = vertexShaderCode.c_str();
		std::string fragmentShaderCode = this->read_file(fragmentShaderPath);
		const char *fragmentShaderCodePointer = fragmentShaderCode.c_str();

		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertexShader, 1, &vertexShaderCodePointer, NULL);
		glShaderSource(fragmentShader, 1, &fragmentShaderCodePointer, NULL);

		int status;
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
#define S_LOG_BUF 100
		char log[S_LOG_BUF];
		int len;
		if (status == GL_FALSE) {
			std::cerr << "**GL error**" << "failed to complie vertex shader"
					  << std::endl;
			glGetShaderInfoLog(vertexShader, S_LOG_BUF, &len, log);
			std::cerr << log << std::endl;
		}

		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			std::cerr << "**GL error**" << "failed to complie fragment shader"
					  << std::endl;
			glGetShaderInfoLog(fragmentShader, S_LOG_BUF, &len, log);
			std::cerr << log << std::endl;
		}
		this->ID = glCreateProgram();
		glAttachShader(this->ID, vertexShader);
		glAttachShader(this->ID, fragmentShader);
		glLinkProgram(this->ID);
		glGetProgramiv(this->ID, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			std::cerr << "**GL error**" << "failed to link the program"
					  << std::endl;
			glGetProgramInfoLog(this->ID, S_LOG_BUF, &len, log);
			std::cerr << log << std::endl;
		}
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		this->u_Model = glGetUniformLocation(this->ID, "model");
		this->u_View = glGetUniformLocation(this->ID, "view");
		this->u_Projection = glGetUniformLocation(this->ID, "projection");
	}

	void use() { glUseProgram(this->ID); }

	unsigned int createUniform(const char *name) {
		return glGetUniformLocation(this->ID, name);
	}

	void setUniformVec3(const char *name, glm::vec3 value) {
		glUniform3fv(glGetUniformLocation(this->ID, name), 1,
					 glm::value_ptr(value));
	}

	void setMVPMatrix(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
		glUniformMatrix4fv(this->u_Model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(this->u_View, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(this->u_Projection, 1, GL_FALSE,
						   glm::value_ptr(projection));
	}
};

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

	float vertices[] = {
		0.0f, 0.0f, 0.0f, /// 0
		0.0f, 1.0f, 0.0f, /// 1
		1.0f, 0.0f, 0.0f, /// 2
		1.0f, 1.0f, 0.0f, /// 3
		0.0f, 0.0f, 1.0f, /// 4
		0.0f, 1.0f, 1.0f, /// 5
		1.0f, 0.0f, 1.0f, /// 6
		1.0f, 1.0f, 1.0f, /// 7
		///
		-6.0f, -6.0f, 6.0f, ///
		-6.0f, -7.0f, 6.0f, ///
		-7.0f, -6.0f, 6.0f, ///
		-7.0f, -7.0f, 6.0f, ///
		-6.0f, -6.0f, 7.0f, ///
		-6.0f, -7.0f, 7.0f, ///
		-7.0f, -6.0f, 7.0f, ///
		-7.0f, -7.0f, 7.0f, ///
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int indices[] = {
		0, 1, 2, 1, 2, 3, ///
		0, 1, 4, 1, 4, 5, ///
		0, 2, 4, 2, 4, 6, ///
		1, 3, 5, 3, 5, 7, ///
		2, 3, 6, 3, 6, 7, ///
		4, 5, 6, 5, 6, 7, ///
		///
		8, 9, 10, 9, 10, 11,	///
		8, 9, 12, 9, 12, 13,	///
		8, 10, 12, 10, 12, 14,	///
		9, 11, 13, 11, 13, 15,	///
		10, 11, 14, 11, 14, 15, ///
		12, 13, 14, 13, 14, 15, ///
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
				 GL_STATIC_DRAW);

	Shader *baseShader = new Shader("vertex.glsl", "fragment.glsl");

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(
		glm::radians(-45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

	unsigned int lightingVAO;
	glGenVertexArrays(1, &lightingVAO);
	glBindVertexArray(lightingVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	Shader *lightingShader = new Shader("vertex.glsl", "lighting.glsl");

	glBindVertexArray(VAO);

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

		baseShader->use();
		baseShader->setUniformVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
		baseShader->setUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		baseShader->setMVPMatrix(model, view, projection);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		lightingShader->use();
		lightingShader->setMVPMatrix(model, view, projection);
		glBindVertexArray(lightingVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)36);

		glfwSwapBuffers(window);
	}

	return 0;
}
