#include "Renderer/Shader.h"

#include <iostream>
#include <fstream>
#include <cassert>

#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
{
	const char* vertexSrcCStr = vertexSrc.c_str();
	const char* fragmentSrcCStr = fragmentSrc.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSrcCStr, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << '\n';
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSrcCStr, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << '\n';
	}

	mRendererID = glCreateProgram();
	glAttachShader(mRendererID, vertexShader);
	glAttachShader(mRendererID, fragmentShader);
	glLinkProgram(mRendererID);

	glGetProgramiv(mRendererID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(mRendererID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << '\n';
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
	glDeleteProgram(mRendererID);
}

std::string Shader::ReadFile(const std::string& filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
	if (in)
	{
		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		if (size != -1)
		{
			result.resize(size);
			in.seekg(0, std::ios::beg);
			in.read(&result[0], size);
		}
		else
		{
			std::cout << "Could not read from file " << filepath << '\n';
		}
	}
	else
	{
		std::cout << "Could not open file " << filepath;
	}

	return result;
}

void Shader::Bind() const
{
	glUseProgram(mRendererID);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::SetInt(const std::string& name, int value)
{
	UploadUniformInt(name, value);
}

void Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
	UploadUniformIntArray(name, values, count);
}

void Shader::SetFloat(const std::string& name, float value)
{
	UploadUniformFloat(name, value);
}

void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
{
	UploadUniformFloat2(name, value);
}

void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
{
	UploadUniformFloat3(name, value);
}

void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
{
	UploadUniformFloat4(name, value);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value)
{
	UploadUniformMat4(name, value);
}

void Shader::UploadUniformInt(const std::string& name, int value)
{
	GLint location = glGetUniformLocation(mRendererID, name.c_str());
	glUniform1i(location, value);
}

void Shader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
{
	GLint location = glGetUniformLocation(mRendererID, name.c_str());
	glUniform1iv(location, count, values);
}

void Shader::UploadUniformFloat(const std::string& name, float value)
{
	GLint location = glGetUniformLocation(mRendererID, name.c_str());
	glUniform1f(location, value);
}

void Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
{
	GLint location = glGetUniformLocation(mRendererID, name.c_str());
	glUniform2f(location, value.x, value.y);
}

void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
{
	GLint location = glGetUniformLocation(mRendererID, name.c_str());
	glUniform3f(location, value.x, value.y, value.z);
}

void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
{
	GLint location = glGetUniformLocation(mRendererID, name.c_str());
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
	GLint location = glGetUniformLocation(mRendererID, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
	GLint location = glGetUniformLocation(mRendererID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

Ref<Shader> Shader::CreateFromFile(const std::string& vertexFilepath, const std::string& fragmentFilepath)
{
	std::string vertexSource = ReadFile(vertexFilepath);
	std::string fragmentSource = ReadFile(fragmentFilepath);

	return CreateRef<Shader>(vertexSource, fragmentSource);
}

Ref<Shader> Shader::CreateFromSource(const std::string& vertexSource, const std::string& fragmentSource)
{
	return CreateRef<Shader>(vertexSource, fragmentSource);
}
