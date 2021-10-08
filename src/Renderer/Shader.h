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

	void bind() const;
	void unbind() const;

	void setInt(const std::string& name, int value);
	void setIntArray(const std::string& name, int* values, uint32_t count);
	void setFloat(const std::string& name, float value);
	void setFloat2(const std::string& name, const glm::vec2& value);
	void setFloat3(const std::string& name, const glm::vec3& value);
	void setFloat4(const std::string& name, const glm::vec4& value);
	void setMat4(const std::string& name, const glm::mat4& value);

	void uploadUniformInt(const std::string& name, int value);
	void uploadUniformIntArray(const std::string& name, int* values, uint32_t count);

	void uploadUniformFloat(const std::string& name, float value);
	void uploadUniformFloat2(const std::string& name, const glm::vec2& value);
	void uploadUniformFloat3(const std::string& name, const glm::vec3& value);
	void uploadUniformFloat4(const std::string& name, const glm::vec4& value);

	void uploadUniformMat3(const std::string& name, const glm::mat3& matrix);
	void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);
public:
	static std::string ReadFile(const std::string& filepath);

	static Ref<Shader> CreateFromFile(const std::string& vertexFilepath, const std::string& fragmentFilepath);
	static Ref<Shader> CreateFromSource(const std::string& vertexSource, const std::string& fragmentSource);
private:
	uint32_t mRendererID;
	std::string mFilePath;

	std::unordered_map<unsigned int, std::vector<uint32_t>> mSPIRV;

	std::unordered_map<unsigned int, std::string> mSourceCode;
};