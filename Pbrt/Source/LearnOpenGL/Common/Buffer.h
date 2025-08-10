#pragma once
#include <glad/glad.h>
class Buffer
{
public:
	Buffer();
	~Buffer();
	void Create();
	void Bind(GLuint BufferType);
	void Unbind(GLuint BufferType);
	void BufferData(GLsizeiptr size, void *data);
	void Delete();
	bool IsValidBufferType(GLint BufferType);
	void UpdateState();
	
private:
	GLuint BUFFER;
	GLint64		BUFFER_SIZE;
	GLint		BUFFER_USAGE;
	GLint		BUFFER_ACCESS;
	GLint		BUFFER_ACCESS_FLAGS;
	GLint		BUFFER_IMMUTABLE_STORAGE;
	GLint		BUFFER_STORAGE_FLAGS;
	GLint		BUFFER_MAPPED;
	void*		BUFFER_MAP_POINTER;
	GLint64		BUFFER_MAP_OFFSET;
	GLint64		BUFFER_MAP_LENGTH;
};