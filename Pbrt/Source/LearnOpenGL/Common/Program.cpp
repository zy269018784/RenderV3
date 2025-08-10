#include <Common/Program.h>
#include <Common/Shader.h>
#include <iostream>

Program::Program() 
 :  DELETE_STATUS(-1),
	VALIDATE_STATUS(-1),
	INFO_LOG_LENGTH(-1),
	ATTACHED_SHADERS(-1),
	ACTIVE_ATTRIBUTES(-1),
	ACTIVE_ATTRIBUTE_MAX_LENGTH(-1),
	ACTIVE_UNIFORMS(-1),
	ACTIVE_UNIFORM_MAX_LENGTH(-1),
	TRANSFORM_FEEDBACK_BUFFER_MODE(-1),
	TRANSFORM_FEEDBACK_VARYINGS(-1),
	TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH(-1),
	ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH(-1),
	GEOMETRY_VERTICES_OUT(-1),
	GEOMETRY_INPUT_TYPE(-1),
	GEOMETRY_OUTPUT_TYPE(-1),
	GEOMETRY_SHADER_INVOCATIONS(-1),
	TESS_CONTROL_OUTPUT_VERTICES(-1),
	TESS_GEN_MODE(-1),
	TESS_GEN_SPACING(-1),
	TESS_GEN_VERTEX_ORDER(-1),
	TESS_GEN_POINT_MODE(-1),
	COMPUTE_WORK_GROUP_SIZE(-1),
	PROGRAM_SEPARABLE(-1),
	PROGRAM_BINARY_RETRIEVABLE_HINT(-1),
	ACTIVE_ATOMIC_COUNTER_BUFFERS(-1),
	PROGRAM(-1) {

}
Program::Program(const std::string vShaderPath, const std::string fShaderPath) 
 : Program() {
	VertexShader = new Shader(vShaderPath, GL_VERTEX_SHADER);
	FragmentShader = new Shader(fShaderPath, GL_FRAGMENT_SHADER);
	if (VertexShader && FragmentShader)
	{
		PROGRAM = glCreateProgram();
		glAttachShader(PROGRAM, VertexShader->ShaderObject());
		glAttachShader(PROGRAM, FragmentShader->ShaderObject());
		glLinkProgram(PROGRAM);
	}
	delete VertexShader;
	delete FragmentShader;
}

Program::Program(const std::string vShaderSrc, const std::string fShaderSrc, int a) 
 : Program() {
	VertexShader = new Shader(GL_VERTEX_SHADER, vShaderSrc);
	FragmentShader = new Shader(GL_FRAGMENT_SHADER, fShaderSrc);
	if (VertexShader && FragmentShader)
	{
		PROGRAM = glCreateProgram();
		glAttachShader(PROGRAM, VertexShader->ShaderObject());
		glAttachShader(PROGRAM, FragmentShader->ShaderObject());
		glLinkProgram(PROGRAM);
	}
	delete VertexShader;
	delete FragmentShader;
}

Program::~Program()
{
	
}

void Program::UpdateState()
{
	glGetProgramiv(PROGRAM, GL_DELETE_STATUS, &DELETE_STATUS);
	glGetProgramiv(PROGRAM, GL_VALIDATE_STATUS, &VALIDATE_STATUS);
	
	glGetProgramiv(PROGRAM, GL_ATTACHED_SHADERS, &ATTACHED_SHADERS);
	glGetProgramiv(PROGRAM, GL_ACTIVE_ATTRIBUTES, &ACTIVE_ATTRIBUTES);
	glGetProgramiv(PROGRAM, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &ACTIVE_ATTRIBUTE_MAX_LENGTH);
	glGetProgramiv(PROGRAM, GL_ACTIVE_UNIFORMS, &ACTIVE_UNIFORMS);
	glGetProgramiv(PROGRAM, GL_ACTIVE_UNIFORM_MAX_LENGTH, &ACTIVE_UNIFORM_MAX_LENGTH);
	glGetProgramiv(PROGRAM, GL_TRANSFORM_FEEDBACK_BUFFER_MODE, &TRANSFORM_FEEDBACK_BUFFER_MODE);
	glGetProgramiv(PROGRAM, GL_TRANSFORM_FEEDBACK_VARYINGS, &TRANSFORM_FEEDBACK_VARYINGS);
	glGetProgramiv(PROGRAM, GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH, &TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH);
	glGetProgramiv(PROGRAM, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH);
	glGetProgramiv(PROGRAM, GL_GEOMETRY_VERTICES_OUT, &GEOMETRY_VERTICES_OUT);
	glGetProgramiv(PROGRAM, GL_GEOMETRY_INPUT_TYPE, &GEOMETRY_INPUT_TYPE);
	glGetProgramiv(PROGRAM, GL_GEOMETRY_OUTPUT_TYPE, &GEOMETRY_OUTPUT_TYPE);
	glGetProgramiv(PROGRAM, GL_GEOMETRY_SHADER_INVOCATIONS, &GEOMETRY_SHADER_INVOCATIONS);
	glGetProgramiv(PROGRAM, GL_TESS_CONTROL_OUTPUT_VERTICES, &TESS_CONTROL_OUTPUT_VERTICES);
	glGetProgramiv(PROGRAM, GL_TESS_GEN_MODE, &TESS_GEN_MODE);
	glGetProgramiv(PROGRAM, GL_TESS_GEN_SPACING, &TESS_GEN_SPACING);
	glGetProgramiv(PROGRAM, GL_TESS_GEN_VERTEX_ORDER, &TESS_GEN_VERTEX_ORDER);
	glGetProgramiv(PROGRAM, GL_TESS_GEN_POINT_MODE, &TESS_GEN_POINT_MODE);
	glGetProgramiv(PROGRAM, GL_COMPUTE_WORK_GROUP_SIZE, &COMPUTE_WORK_GROUP_SIZE);
	glGetProgramiv(PROGRAM, GL_PROGRAM_SEPARABLE, &PROGRAM_SEPARABLE);
	glGetProgramiv(PROGRAM, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, &PROGRAM_BINARY_RETRIEVABLE_HINT);
	glGetProgramiv(PROGRAM, GL_ACTIVE_ATOMIC_COUNTER_BUFFERS, &ACTIVE_ATOMIC_COUNTER_BUFFERS);

	glGetProgramiv(PROGRAM, GL_INFO_LOG_LENGTH, &INFO_LOG_LENGTH);
	std::cout << "DELETE_STATUS                         " << DELETE_STATUS << std::endl;
	std::cout << "VALIDATE_STATUS                       " << VALIDATE_STATUS << std::endl;
	std::cout << "INFO_LOG_LENGTH                       " << INFO_LOG_LENGTH << std::endl;
	std::cout << "ATTACHED_SHADERS                      " << ATTACHED_SHADERS << std::endl;
	std::cout << "ACTIVE_ATTRIBUTES                     " << ACTIVE_ATTRIBUTES << std::endl;
	std::cout << "ACTIVE_ATTRIBUTE_MAX_LENGTH           " << ACTIVE_ATTRIBUTE_MAX_LENGTH << std::endl;
	std::cout << "ACTIVE_UNIFORMS                       " << ACTIVE_UNIFORMS << std::endl;
	std::cout << "ACTIVE_UNIFORM_MAX_LENGTH             " << ACTIVE_UNIFORM_MAX_LENGTH << std::endl;
	std::cout << "TRANSFORM_FEEDBACK_BUFFER_MODE        " << TRANSFORM_FEEDBACK_BUFFER_MODE << std::endl;
	std::cout << "TRANSFORM_FEEDBACK_VARYINGS           " << TRANSFORM_FEEDBACK_VARYINGS << std::endl;
	std::cout << "TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH " << TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH << std::endl;
	std::cout << "ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH  " << ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH << std::endl;
	std::cout << "GEOMETRY_VERTICES_OUT                 " << GEOMETRY_VERTICES_OUT << std::endl;
	std::cout << "GEOMETRY_INPUT_TYPE                   " << GEOMETRY_INPUT_TYPE << std::endl;
	std::cout << "GEOMETRY_OUTPUT_TYPE                  " << GEOMETRY_OUTPUT_TYPE << std::endl;
	std::cout << "GEOMETRY_SHADER_INVOCATIONS           " << GEOMETRY_SHADER_INVOCATIONS << std::endl;
	std::cout << "TESS_CONTROL_OUTPUT_VERTICES          " << TESS_CONTROL_OUTPUT_VERTICES << std::endl;
	std::cout << "TESS_GEN_MODE                         " << TESS_GEN_MODE << std::endl;
	std::cout << "TESS_GEN_SPACING                      " << TESS_GEN_SPACING << std::endl;
	std::cout << "TESS_GEN_VERTEX_ORDER                 " << TESS_GEN_VERTEX_ORDER << std::endl;
	std::cout << "TESS_GEN_POINT_MODE                   " << TESS_GEN_POINT_MODE << std::endl;
	std::cout << "COMPUTE_WORK_GROUP_SIZE               " << COMPUTE_WORK_GROUP_SIZE << std::endl;
	std::cout << "PROGRAM_SEPARABLE                     " << PROGRAM_SEPARABLE << std::endl;
	std::cout << "PROGRAM_BINARY_RETRIEVABLE_HINT       " << PROGRAM_BINARY_RETRIEVABLE_HINT << std::endl;
	std::cout << "ACTIVE_ATOMIC_COUNTER_BUFFERS         " << ACTIVE_ATOMIC_COUNTER_BUFFERS << std::endl;
	if (INFO_LOG_LENGTH > 0)
	{
		char* Info = new char[INFO_LOG_LENGTH + 1];

		glGetProgramInfoLog(PROGRAM, INFO_LOG_LENGTH, NULL, Info);
		std::cout << Info << std::endl;
		delete[] Info;
	}
} 

void Program::Use()
{
	glUseProgram(PROGRAM);
}

void Program::Delete()
{
	glDeleteProgram(PROGRAM);
}