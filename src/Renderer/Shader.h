#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Base.h"

class Shader
{
public:
	Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
	~Shader();

	void Bind() const;
	static void Unbind();

	void SetInt(const std::string& name, int value);
	void SetIntArray(const std::string& name, int* values, uint32_t count);
	void SetFloat(const std::string& name, float value);
	void SetFloat2(const std::string& name, const glm::vec2& value);
	void SetFloat3(const std::string& name, const glm::vec3& value);
	void SetFloat4(const std::string& name, const glm::vec4& value);
	void SetMat4(const std::string& name, const glm::mat4& value);

	void UploadUniformInt(const std::string& name, int value);
	void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

	void UploadUniformFloat(const std::string& name, float value);
	void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
public:
	static std::string ReadFile(const std::string& filepath);

	static Ref<Shader> CreateFromFile(const std::string& vertexFilepath, const std::string& fragmentFilepath);
	static Ref<Shader> CreateFromSource(const std::string& vertexSource, const std::string& fragmentSource);
private:
	uint32_t mRendererID;
	std::string mFilePath;
};