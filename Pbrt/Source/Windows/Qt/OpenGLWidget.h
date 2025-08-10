#pragma once
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QVector>
#include <QTimer>
#include <Scene/SceneWrapper.h>
class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions_4_5_Core
{
	Q_OBJECT
public:
	OpenGLWidget(QWidget* parent = Q_NULLPTR);
	~OpenGLWidget();
	void SetSceneWrapper(Render::SceneWrapper *wrapper);
public slots:
	void ReRender();
	void RenderOnSample();
	void StartTimer();
	void StopTimer();
protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);
protected:
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
private:
	void copyImageToGPU(int w, int h, const void* data, GLsizei size);
private:
	GLuint texture = -1;
	GLuint vao = -1;
	GLuint vbo = -1;
	GLuint ebo = -1;
	GLuint pbo = -1;
	GLuint program = -1;
	GLuint vshader = -1;
	GLuint fshader = -1;
	bool pressed = false;
	float lastPosX;
	float lastPosY;
	QTimer timer;
	Render::SceneWrapper * SceneWrapper = nullptr;
};