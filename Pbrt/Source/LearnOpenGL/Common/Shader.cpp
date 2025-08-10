#include <Common/Shader.h>
#include <fstream>
#include <sstream>
#include <iostream>

void readFile(const std::string& file, std::string& out)
{
    std::ifstream ShaderFile;
    ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        ShaderFile.open(file);
        std::stringstream ShaderStream;
        // read file's buffer contents into streams
        ShaderStream << ShaderFile.rdbuf();
        // close file handlers
        ShaderFile.close();
        // convert stream into string
        out = ShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
}

Shader::Shader() : SHADER_TYPE(-1),
                   DELETE_STATUS(-1),
                   COMPILE_STATUS(-1),
                   INFO_LOG_LENGTH(-1),
                   SHADER_SOURCE_LENGTH(-1),
                   SHADER(-1) {
    
}

Shader::Shader(const std::string ShaderPath, GLint ShaderType) 
    : Shader() {

    readFile(ShaderPath, ShaderCode);
    //std::cout << ShaderCode << std::endl;

    const char* pShaderCode = ShaderCode.c_str();

    if (IsValidShaderType(ShaderType))
    {
        SHADER = glCreateShader(ShaderType);
        glShaderSource(SHADER, 1, &pShaderCode, NULL);
        glCompileShader(SHADER);
    }
}

Shader::Shader(GLint ShaderType, const std::string src) 
    : Shader() {
    ShaderCode = src;
    const char* pShaderCode = ShaderCode.c_str();

    if (IsValidShaderType(ShaderType))
    {
        SHADER = glCreateShader(ShaderType);
        glShaderSource(SHADER, 1, &pShaderCode, NULL);
        glCompileShader(SHADER);
    }
}

Shader::~Shader()
{
    glDeleteShader(SHADER);
}


void Shader::UpdateState()
{
    glGetShaderiv(SHADER, GL_SHADER_TYPE, &SHADER_TYPE);
    glGetShaderiv(SHADER, GL_DELETE_STATUS, &DELETE_STATUS);
    glGetShaderiv(SHADER, GL_COMPILE_STATUS, &COMPILE_STATUS);
    glGetShaderiv(SHADER, GL_INFO_LOG_LENGTH, &INFO_LOG_LENGTH);
    glGetShaderiv(SHADER, GL_SHADER_SOURCE_LENGTH, &SHADER_SOURCE_LENGTH);

    std::cout << "SHADER_TYPE   " << SHADER_TYPE << std::endl;
    std::cout << "DELETE_STATUS   " << DELETE_STATUS << std::endl;
    std::cout << "COMPILE_STATUS   " << COMPILE_STATUS << std::endl;
    std::cout << "SHADER_SOURCE_LENGTH   " << SHADER_SOURCE_LENGTH << std::endl;
    std::cout << "INFO_LOG_LENGTH   " << INFO_LOG_LENGTH << std::endl;
    if (INFO_LOG_LENGTH > 0)
    {
        char* Info = new char[INFO_LOG_LENGTH + 1];

        glGetShaderInfoLog(SHADER, INFO_LOG_LENGTH, NULL, Info);
        std::cout << Info << std::endl;
        delete[] Info;
    }
 }
    

bool Shader::IsValidShaderType(GLint ShaderType)
{
    switch (ShaderType)
    {
        case GL_VERTEX_SHADER:
        case GL_FRAGMENT_SHADER:
        case GL_COMPUTE_SHADER:
        case GL_TESS_CONTROL_SHADER:
        case GL_TESS_EVALUATION_SHADER:
        case GL_GEOMETRY_SHADER:
            return true;
        default:
            return false;
    }
}

GLuint Shader::ShaderObject()
{
    return SHADER;
}