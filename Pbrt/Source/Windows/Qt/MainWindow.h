#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <Scene/SceneData.h>
#include <TextureEdit.h>
#include <MaterialEdit.h>
#include <Scene/SceneWrapper.h>
class OpenGLWidget;
namespace Ui {
class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void SetSceneData(Render::SceneData *sd);
    void SetSceneWrapper(Render::SceneWrapper *wrapper);
public slots:
    void SetSkyColor();
protected:
    void resizeEvent(QResizeEvent* ev);
private:
    Ui::MainWindow *ui;
    OpenGLWidget* w;
    Render::SceneData* SceneData;
    MaterialEdit me;
    TextureEdit te;
    Render::SceneWrapper* SceneWrapper = nullptr;
};

#endif // MAINWINDOW_H
