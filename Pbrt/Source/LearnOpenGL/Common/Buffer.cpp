#include <Common/Buffer.h>
#include <iostream>
Buffer::Buffer() :
	BUFFER(-1),
    BUFFER_SIZE(-1),
    BUFFER_USAGE(-1),
    BUFFER_ACCESS(-1),
    BUFFER_ACCESS_FLAGS(-1),
    BUFFER_IMMUTABLE_STORAGE(-1),
    BUFFER_MAPPED(-1),
    BUFFER_MAP_POINTER(nullptr),
    BUFFER_MAP_OFFSET(-1),
    BUFFER_MAP_LENGTH(-1),
    BUFFER_STORAGE_FLAGS(-1)
{
	
}

Buffer::~Buffer()
{
	
}

void Buffer::Create()
{
    glGenBuffers(1, &BUFFER);
}

void Buffer::Bind(GLuint BufferType)
{
    if (IsValidBufferType(BufferType))
        glBindBuffer(BufferType, BUFFER);
}

void Buffer::Unbind(GLuint BufferType)
{
    if (IsValidBufferType(BufferType))
        glBindBuffer(BufferType, 0);
}

void Buffer::BufferData(GLsizeiptr size, void* data)
{
    glNamedBufferData(BUFFER, size, data, GL_STREAM_DRAW);
}

void Buffer::Delete()
{
    glDeleteBuffers(1, &BUFFER);
}

bool Buffer::IsValidBufferType(GLint BufferType)
{
    switch (BufferType)
    {
    case GL_ARRAY_BUFFER:
    case GL_ATOMIC_COUNTER_BUFFER:
    case GL_COPY_READ_BUFFER:
    case GL_COPY_WRITE_BUFFER:
    case GL_DISPATCH_INDIRECT_BUFFER:
    case GL_DRAW_INDIRECT_BUFFER:
    case GL_ELEMENT_ARRAY_BUFFER:
   // case GL_PARAMETER_BUFFER:
    case GL_PIXEL_PACK_BUFFER:
    case GL_PIXEL_UNPACK_BUFFER:
    case GL_QUERY_BUFFER:
    case GL_SHADER_STORAGE_BUFFER:
    case GL_TEXTURE_BUFFER:
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return true;
    default:
        return false;
    }
}

void Buffer::UpdateState()
{
    glGetNamedBufferParameteri64v(BUFFER, GL_BUFFER_SIZE, &BUFFER_SIZE);
    glGetNamedBufferParameteriv(BUFFER, GL_BUFFER_USAGE, &BUFFER_USAGE);
    glGetNamedBufferParameteriv(BUFFER, GL_BUFFER_ACCESS, &BUFFER_ACCESS);
    glGetNamedBufferParameteriv(BUFFER, GL_BUFFER_ACCESS_FLAGS, &BUFFER_ACCESS_FLAGS);
    glGetNamedBufferParameteriv(BUFFER, GL_BUFFER_IMMUTABLE_STORAGE, &BUFFER_IMMUTABLE_STORAGE);
    glGetNamedBufferParameteriv(BUFFER, GL_BUFFER_STORAGE_FLAGS, &BUFFER_STORAGE_FLAGS);
    glGetNamedBufferParameteriv(BUFFER, GL_BUFFER_MAPPED, &BUFFER_MAPPED);
    glGetNamedBufferPointerv(BUFFER, GL_BUFFER_MAP_POINTER, &BUFFER_MAP_POINTER);
    glGetNamedBufferParameteri64v(BUFFER, GL_BUFFER_MAP_OFFSET, &BUFFER_MAP_OFFSET);
    glGetNamedBufferParameteri64v(BUFFER, GL_BUFFER_MAP_LENGTH, &BUFFER_MAP_LENGTH);
    std::cout << "BUFFER                   " << BUFFER                          << std::endl;
    std::cout << "BUFFER_SIZE              " << BUFFER_SIZE                     << std::endl;
    std::cout << "BUFFER_USAGE             " << BUFFER_USAGE                    << std::endl;
    std::cout << "BUFFER_ACCESS            " << BUFFER_ACCESS                   << std::endl;
    std::cout << "BUFFER_ACCESS_FLAGS      " << BUFFER_ACCESS_FLAGS             << std::endl;
    std::cout << "BUFFER_IMMUTABLE_STORAGE " << BUFFER_IMMUTABLE_STORAGE        << std::endl;
    std::cout << "BUFFER_STORAGE_FLAGS     " << BUFFER_STORAGE_FLAGS            << std::endl;
    std::cout << "BUFFER_MAPPED            " << BUFFER_MAPPED                   << std::endl;
    std::cout << "BUFFER_MAP_POINTER       " << BUFFER_MAP_POINTER              << std::endl;
    std::cout << "BUFFER_MAP_OFFSET        " << BUFFER_MAP_OFFSET               << std::endl;
    std::cout << "BUFFER_MAP_LENGTH        " << BUFFER_MAP_LENGTH               << std::endl;
}

