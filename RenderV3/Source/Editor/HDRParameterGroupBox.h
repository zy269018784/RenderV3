#pragma once
#ifdef USE_QT
#include <QGroupBox>
#include <QLabel>
#include <QQuickWidget>
#include <QLineEdit>
#include <QHBoxLayout>
class GlfwRender;
class HDRParameterGroupBox : public QGroupBox
{
	Q_OBJECT
public:
	HDRParameterGroupBox(QWidget* parent = nullptr);
	void SetScene(GlfwRender* Scene1);
	void SetMaterialIndex(int Index1);
signals:
	void SetHDRText(QString str);
	void SetColorTemperatureCheckBoxText(QString str);
public slots:
	void OnColorTemperatureCilcked(bool str);
public slots:
    void OnHDRLightSliderMoved(QString p);
	void OnHDRFileButtonClicked();
private:
	void CreateHDRFileLayout();
	void CreateHDRRotateLayout();
	void CreateHDRRotateSlider();
	void CreateColorTemperatureLayout();
private:
	QVBoxLayout* HDRParameterGroupBoxLayout;

	QHBoxLayout*  HDRFileLayout;
	QLineEdit* HDRFileEdit;
	QQuickWidget* HDRFileButton;

	QHBoxLayout* HDRRotateLayout;
	QLabel* HDRLabel;
	QQuickWidget* HDRRotateEdit;

	QQuickWidget* HDRRotateSlider;

	QHBoxLayout*  ColorTemperatureLayout;
	QQuickWidget* ColorTemperatureBox;
	QQuickWidget* ColorTemperatureLineEdit;
private:
	GlfwRender* Scene;
	int MaterialIndex;
};
#endif