#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

struct ShaderProgramSource {
	std::string VertexSource;
	std::string GeometrySource;
	std::string FragmentSource;
};

class Shader {
public:
	Shader();
	~Shader();
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	void Init(const std::string& vertexPath, const std::string& fragPath);
	void Init(const std::string& vertexPath, const std::string& geoPath, const std::string& fragPath);
	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform1i(const std::string& name, int v);
	void SetUniform1iv(const std::string& name, int length, int* v);
	void SetUniform1f(const std::string& name, float v);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
	void SetUniform2fv(const std::string& name, const glm::vec2& value);
	void SetUniform3fv(const std::string& name, const glm::vec3& value);
	void SetUniform4fv(const std::string& name, const glm::vec4& value);
	void SetUniform4fv(const std::string& name, int length, float* v);

private:
	GLint GetUniformLocation(const std::string& name);
	uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	uint32_t CreateShader(const std::string& vertexShader, const std::string& geometryShader,
		const std::string& fragmentShader);
	uint32_t CompileShader(uint32_t type, const std::string& source);
	ShaderProgramSource ParseShader(const std::string& vertexPath, const std::string& fragPath);
	ShaderProgramSource ParseShader(const std::string& vertexPath, const std::string& geoPath,
		const std::string& fragPath);

	static uint32_t s_CurrentlyBound;
	uint32_t m_ShaderId;
	std::unordered_map<std::string, GLint> m_UniformLocationCache;
};
