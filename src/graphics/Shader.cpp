#include "GL/glew.h"
#include "Shader.h"
#include "../utils/Logger.h"
#include <fstream>
#include <string>
#include <sstream>

uint32_t Shader::s_CurrentlyBound = 0;

Shader::Shader() :
	m_ShaderId(0)
{
}

Shader::~Shader() { glDeleteProgram(m_ShaderId); }

void Shader::Init(const std::string& vertexPath, const std::string& fragPath) {
	ShaderProgramSource source = ParseShader(vertexPath, fragPath);
	m_ShaderId = CreateShader(source.VertexSource, source.FragmentSource);
}

void Shader::Init(const std::string& vertexPath, const std::string& geoPath, const std::string& fragPath) {
	ShaderProgramSource source = ParseShader(vertexPath, geoPath, fragPath);
	m_ShaderId = CreateShader(source.VertexSource, source.GeometrySource, source.FragmentSource);
}

void Shader::Bind() const {
	if (s_CurrentlyBound != m_ShaderId) {
		s_CurrentlyBound = m_ShaderId;
		glUseProgram(m_ShaderId);
	}
}

void Shader::Unbind() const { glUseProgram(0); }

void Shader::SetUniform1i(const std::string& name, int v)
{
	glUniform1i(GetUniformLocation(name), v);
}

void Shader::SetUniform1iv(const std::string& name, int length, int* v)
{
	glUniform1iv(GetUniformLocation(name), length, v);
}

void Shader::SetUniform1f(const std::string& name, float v)
{
	glUniform1f(GetUniformLocation(name), v);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::SetUniform2fv(const std::string& name, const glm::vec2& value)
{
	glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetUniform3fv(const std::string& name, const glm::vec3& value)
{
	glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetUniform4fv(const std::string& name, const glm::vec4& value)
{
	glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetUniform4fv(const std::string& name, int length, float* v)
{
	glUniform4fv(GetUniformLocation(name), length, v);
}

GLint Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLint location = glGetUniformLocation(m_ShaderId, name.c_str());
	if (location == -1)
		LOG_WARN("Uniform {} doesn't exist!", name);
	m_UniformLocationCache[name] = location;
	return location;
}

uint32_t Shader::CreateShader(
	const std::string& vertexShader, const std::string& fragmentShader)
{
	uint32_t program = glCreateProgram();
	uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

uint32_t Shader::CreateShader(const std::string& vertexShader, const std::string& geometryShader,
	const std::string& fragmentShader)
{
	uint32_t program = glCreateProgram();
	uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	uint32_t gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
	uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, gs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);

	return program;
}

uint32_t Shader::CompileShader(uint32_t type, const std::string& source)
{
	uint32_t id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);

	// error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)malloc(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		LOG_ERROR("Failed to compile shader of type: {}.", type);
		LOG_ERROR("{}", message);
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static std::string ReadFile(const std::string& filePath)
{
	std::string line;
	std::stringstream ss;
	std::ifstream stream(filePath);

	while (getline(stream, line)) {
		ss << line << "\n";
	}

	return ss.str();
}

ShaderProgramSource Shader::ParseShader(const std::string& vertexPath, const std::string& fragPath)
{
	std::string vertexShader = ReadFile(vertexPath);
	std::string fragmentShader = ReadFile(fragPath);

	return { vertexShader, "", fragmentShader };
}

ShaderProgramSource Shader::ParseShader(const std::string& vertexPath, const std::string& geoPath,
	const std::string& fragPath)
{
	std::string vertexShader = ReadFile(vertexPath);
	std::string geometryShader = ReadFile(geoPath);
	std::string fragmentShader = ReadFile(fragPath);

	return { vertexShader, geometryShader, fragmentShader };
}
