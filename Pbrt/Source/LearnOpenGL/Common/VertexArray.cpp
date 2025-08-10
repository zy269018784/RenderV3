#include <Common/VertexArray.h>


VertexArray::VertexArray() : VAO(-1)
{
	
}

VertexArray::~VertexArray()
{
	
}

void VertexArray::Create()
{
	glGenVertexArrays(1, &VAO);
}

void VertexArray::Bind()
{
	glBindVertexArray(VAO);
}

void  VertexArray::Unbind()
{
	glBindVertexArray(0);
}

void VertexArray::EnableVertexAttribArray(GLuint index)
{
	glEnableVertexArrayAttrib(VAO, index);
}

void VertexArray::DisableVertexAttribArray(GLuint index)
{
	glDisableVertexArrayAttrib(VAO, index);
}

void VertexArray::VertexAttribPointer(GLint index, GLint size, GLuint type, GLboolean normalized, GLsizei stride, const void* pointer)
{
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);

}
void VertexArray::Delete()
{
	glDeleteVertexArrays(1, &VAO);
}

void VertexArray::UpdateState()
{

}