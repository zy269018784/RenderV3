#pragma once
#include <string>
#include <glad/glad.h>
class Shader
{
public:
	Shader();
	Shader(const std::string ShaderPath, GLint ShaderType);
	Shader(GLint ShaderType, const std::string src);
	~Shader();
	void UpdateState();
	bool IsValidShaderType(GLint ShaderType);
	GLuint ShaderObject();
private:
	std::string ShaderCode;
private: 
	GLuint SHADER;
	GLint SHADER_TYPE;
	GLint DELETE_STATUS;
	GLint COMPILE_STATUS;
	GLint INFO_LOG_LENGTH;
	GLint SHADER_SOURCE_LENGTH;
};