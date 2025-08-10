#ifdef USE_QT
#pragma once
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QQuickWidget>
#include <QComboBox>

class GlfwRender;
class LightParameterGroupBox : public QGroupBox
{
    Q_OBJECT
public:
	LightParameterGroupBox(QWidget* parent = nullptr);
    void SetScene(GlfwRender* Scene1);
    void SetLightIndex(int Index1);

private:
    void CreateLightNameLayout();
    void CreateLightTypeLayout();
    void CreateLightAlbedoLayout();
     
private:
    void BindUISignal();

signals: 
    void SetRedText(QString str);
    void SetGreenText(QString str);
    void SetBlueText(QString str);
    void SetColorButton(QColor c);
public slots:
    void OnColorButtonClicked();

private:
    QVBoxLayout* Layout;

    /*
        光源名称
    */
    QHBoxLayout* LightNameLayout;
    QLabel* LightNameLabel;
    QLabel* LightName2Label;
    /*
        材质类型
    */
    QHBoxLayout* LightTypeLayout;
    QLabel* LightTypeLabel;
    QComboBox* LightTypeComboBox;
    /*
        Albedo
    */
    QHBoxLayout* LightAlbedoLayout;
    QLabel* LightAlbedoLabel;
    QLabel* LightAlbedoRedLabel;
    QLabel* LightAlbedoGreenLabel;
    QLabel* LightAlbedoBlueLabel;
    QQuickWidget* RedEdit;
    QQuickWidget* GreenEdit;
    QQuickWidget* BlueEdit;
    QQuickWidget* ColorButton;

private:
    GlfwRender* Scene;
    int LightIndex;
};
#endif