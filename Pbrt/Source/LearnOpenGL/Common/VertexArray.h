#pragma once
#include <glad/glad.h>
class VertexArray
{
public:
	VertexArray();
	~VertexArray();
	void Create();
	void Bind();
	void Unbind();
	void EnableVertexAttribArray(GLuint index);
	void DisableVertexAttribArray(GLuint index);
	void VertexAttribPointer(GLint index, GLint size, GLuint type,
		GLboolean normalized, GLsizei stride, const
		void* pointer);
	void Delete();
	void UpdateState();

private:
	GLuint VAO;
};