#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

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

#endif
