
#include <OpenGLWidget.h>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QDebug>
#include <QThread>
#include <string>

const std::string vShaderSrc =
"#version 450 core								\n"
"layout(location = 0) in vec3 aPos;				\n"
"layout(location = 1) in vec3 aColor;			\n"
"layout(location = 2) in vec2 aTexCoord;		\n"
"out vec3 ourColor;								\n"
"out vec2 TexCoord;								\n"
"void main()									\n"
"{												\n"
"    gl_Position = vec4(aPos, 1.0);				\n"
"    ourColor = aColor;							\n"
"    TexCoord = vec2(aTexCoord.x, aTexCoord.y);	\n"
"}												\n";

const std::string fShaderSrc =
"#version 450 core                               \n"
"out vec4 FragColor;                             \n"
"in vec3 ourColor;                               \n"
"in vec2 TexCoord;                               \n"
"// texture sampler                              \n"
"uniform sampler2D texture1;                     \n"
"                                                \n"
"void main()                                     \n"
"{                                               \n"
"    FragColor =   texture(texture1, TexCoord);  \n"
"    //FragColor = vec4(ourColor, 1);			 \n"
"}                                               \n";

static float vertices[] = {
	// positions        // colors           // texture coords
	 1.f,  1.f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // top right
	 1.f, -1.f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // bottom right
	-1.f, -1.f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, // bottom left
	-1.f,  1.f, 0.0f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f  // top left 
};

static unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};

OpenGLWidget::OpenGLWidget(QWidget* parent)
	: QOpenGLWidget(parent)
{
	grabKeyboard();

	// 计时器刷新
	connect(&timer, &QTimer::timeout, this, &OpenGLWidget::RenderOnSample);
	//setMouseTracking(true);

	//ClearSamples();
}

OpenGLWidget::~OpenGLWidget()
{

}

void OpenGLWidget::SetSceneWrapper(Render::SceneWrapper* wrapper) {
	SceneWrapper = wrapper;
	SceneWrapper->ClearSamples();
}

void OpenGLWidget::ReRender()
{
	int w = width();
	int h = height();

	SceneWrapper->ReRender();

	copyImageToGPU(w, h, SceneWrapper->GetColorBufferPointer(), SceneWrapper->GetColorBufferSize());

	update();
}

void OpenGLWidget::RenderOnSample()
{

	SceneWrapper->RenderOneSample();

	copyImageToGPU(width(), height(), SceneWrapper->GetColorBufferPointer(), SceneWrapper->GetColorBufferSize());

	update();
}

void OpenGLWidget::StartTimer() {
	timer.start(100);
}

void OpenGLWidget::StopTimer() {
	timer.stop();
}

void OpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glGenBuffers(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	//  绑定vao
	glBindVertexArray(vao);

	// 绑定vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// 顶点传入vbo
	glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 绑定ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	// 顶点传入ebo
	glNamedBufferData(ebo, sizeof(indices), indices, GL_STATIC_DRAW);

	// vao
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 创建pbo
	glGenBuffers(1, &pbo);

	const char* pFragShaderCode = fShaderSrc.c_str();
	const char* pVertexShaderCode = vShaderSrc.c_str();

	// 顶点着色器
	vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &pVertexShaderCode, nullptr);
	//glShaderSource(vshader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vshader);

	// 片元着色器
	fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &pFragShaderCode, nullptr);
	//glShaderSource(fshader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fshader);

	// program
	program = glCreateProgram();
	glAttachShader(program, fshader);
	glAttachShader(program, vshader);
	glLinkProgram(program);

	char infoLog[512];
	int success;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vshader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
	}
	qDebug() << "program " << program
		<< "pbo " << pbo
		<< "vbo " << vbo
		<< "ebo " << ebo
		<< "vshader " << vshader
		<< "fshader " << fshader;
}

void OpenGLWidget::paintGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (program >= 0) {
		
		glUseProgram(program);

		glBindVertexArray(vao);

		glBindTexture(GL_TEXTURE_2D, texture);

		// 绘制
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void OpenGLWidget::resizeGL(int w, int h)
{
	// 停止定时器刷新
	StopTimer();
	// Resize缓冲区, 重新渲染
	SceneWrapper->ResizeThenRender(w, h);
	// 拷贝到PBO
	copyImageToGPU(w, h, SceneWrapper->GetColorBufferPointer(), SceneWrapper->GetColorBufferSize());
	// 改变OpenGL视口
	glViewport(0, 0, w, h);
	// 刷新UI
	update();
	// 开始定时器刷新
	StartTimer();
}

void OpenGLWidget::copyImageToGPU(int w, int h, const void *data, GLsizei size)
{
	// 绑定pbo
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
	// 图像 -> pbo
	glNamedBufferData(pbo, size, data, GL_STREAM_DRAW);
	// 绑定texture
	glBindTexture(GL_TEXTURE_2D, texture);
	// pbo -> texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	// 解绑texture
	glBindTexture(GL_TEXTURE_2D, 0);
	// 解绑pbo
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void OpenGLWidget::keyPressEvent(QKeyEvent* event)
{
	StopTimer();

	switch (event->key()) {
	case Qt::Key_A:
		SceneWrapper->CameraMoveLeft();
		break;	
	case Qt::Key_D:
		SceneWrapper->CameraMoveRight();
		break;
	case Qt::Key_W:
		SceneWrapper->CameraMoveForward();
		break;
	case Qt::Key_S:
		SceneWrapper->CameraMoveBackward();
		break;
	case Qt::Key_Q:
		SceneWrapper->CameraMoveDown();
		break;
	case Qt::Key_E:
		SceneWrapper->CameraMoveUp();
		break;
	}
	SceneWrapper->ShowCamera();

	// 渲染
	SceneWrapper->UpdateCameraThenRender();

	copyImageToGPU(width(), height(), SceneWrapper->GetColorBufferPointer(), SceneWrapper->GetColorBufferSize());

	update();
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent* event) {
	StartTimer();
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event) {
	pressed = true;
	lastPosX = event->x();
	lastPosY = event->y();
	timer.stop();
	qDebug() << pressed;
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
	pressed = false;
	SceneWrapper->ClearSamples();
	timer.start(100);
	qDebug() << pressed;
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	qDebug() << "mouseMoveEvent " << width() << height();
	//MousePosToDir(event->localPos().x(), event->localPos().y());
	float x = event->localPos().x();
	float y = event->localPos().y();
	auto dx = lastPosX - x;
	auto dy = lastPosY - y;
	SceneWrapper->RasterToCamera(dx, dy);

	int w = width();
	int h = height();

	SceneWrapper->UpdateCameraThenRender();

	void* ColorBufferPointer = SceneWrapper->GetColorBufferPointer();
	size_t ColorBufferSize = SceneWrapper->GetColorBufferSize();

	copyImageToGPU(w, h, ColorBufferPointer, ColorBufferSize);

	update();
}
