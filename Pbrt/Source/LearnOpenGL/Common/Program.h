#pragma once
#include <string>
#include <glad/glad.h>

class Shader;
class Program
{
public:
	Program();
	Program(const std::string vShaderPath, const std::string fShaderPath);
	Program(const std::string vShaderSrc, const std::string fShaderSrc, int a);
	~Program();
	void UpdateState();
	void Use();
	void Delete();
private:
	Shader* VertexShader;
	Shader* FragmentShader;
public:
	GLuint PROGRAM;
	GLint DELETE_STATUS;
	GLint VALIDATE_STATUS;
	GLint INFO_LOG_LENGTH;
	GLint ATTACHED_SHADERS;
	GLint ACTIVE_ATTRIBUTES;
	GLint ACTIVE_ATTRIBUTE_MAX_LENGTH;
	GLint ACTIVE_UNIFORMS;
	GLint ACTIVE_UNIFORM_MAX_LENGTH;
	GLint TRANSFORM_FEEDBACK_BUFFER_MODE;
	GLint TRANSFORM_FEEDBACK_VARYINGS;
	GLint TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH;
	GLint ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH;
	GLint GEOMETRY_VERTICES_OUT;
	GLint GEOMETRY_INPUT_TYPE;
	GLint GEOMETRY_OUTPUT_TYPE;
	GLint GEOMETRY_SHADER_INVOCATIONS;
	GLint TESS_CONTROL_OUTPUT_VERTICES;
	GLint TESS_GEN_MODE;
	GLint TESS_GEN_SPACING;
	GLint TESS_GEN_VERTEX_ORDER;
	GLint TESS_GEN_POINT_MODE;
	GLint COMPUTE_WORK_GROUP_SIZE;
	GLint PROGRAM_SEPARABLE;
	GLint PROGRAM_BINARY_RETRIEVABLE_HINT;
	GLint ACTIVE_ATOMIC_COUNTER_BUFFERS;
};