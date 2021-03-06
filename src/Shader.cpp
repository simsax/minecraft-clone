#include <GL/glew.h>
#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string &folderPath)
{
	ShaderProgramSource source = ParseShader(folderPath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string &name, int v)
{
	glUniform1i(GetUniformLocation(name), v);
}

void Shader::SetUniform1iv(const std::string &name, int length, int *v)
{
	glUniform1iv(GetUniformLocation(name), length, v);
}

void Shader::SetUniform1f(const std::string &name, float v)
{
	glUniform1f(GetUniformLocation(name), v);
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 &matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::SetUniform2fv(const std::string &name, const glm::vec2 &value)
{
	glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetUniform3fv(const std::string &name, const glm::vec3 &value)
{
	glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

GLint Shader::GetUniformLocation(const std::string &name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLint location = glGetUniformLocation(m_RendererID, name.c_str()); // retrieve the location of the uniform variable present in the shader
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!\n";
	m_UniformLocationCache[name] = location;
	return location;
}

unsigned int Shader::CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
	// provide opengl with our shader source code, compile it and link those 2 together and return and id for that shader
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs); // attach shader to program object
	glAttachShader(program, fs);
	glLinkProgram(program);		// links the program
	glValidateProgram(program); // does validation

	glDeleteShader(vs); // frees the memory associated with shader object (deletes intermediates obj file)
	glDeleteShader(fs);

	return program;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string &source)
{
	unsigned int id = glCreateShader(type); // creates a shader object that contains the source code
	const char *src = source.c_str();		// pointer to beginning of the string
	glShaderSource(id, 1, &src, NULL);		// sets the source code in the shader
	glCompileShader(id);					// compile the shader

	// error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result); // query the shader object to get the value of some parameter
	if (result == GL_FALSE)
	{ // our shader hasn't compiled successfully
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		// char* message = (char*)_malloca(length * sizeof(char));
		char *message = (char *)malloc(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex\n" : "fragment\n");
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static std::string ReadFile(const std::string &filePath)
{
	std::string line;
	std::stringstream ss;
	std::ifstream stream(filePath);

	while (getline(stream, line))
	{
		ss << line << "\n";
	}

	return ss.str();
}

ShaderProgramSource Shader::ParseShader(const std::string &folderPath)
{
	std::string vertexShader = ReadFile(folderPath + "shader.vert");
	std::string fragmentShader = ReadFile(folderPath + "shader.frag");

	return {vertexShader, fragmentShader};
}
