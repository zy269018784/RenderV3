#ifdef USE_QT
#pragma once
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QQuickWidget>
class OtherParameterGroupBox : public QGroupBox
{
	Q_OBJECT
public:
	OtherParameterGroupBox(QWidget* parent = nullptr);
	void CreateTonemapingLayout();
	void CreateOffScreenRenderLayout();
	void CreateGammaCorrectionLayout();
signals:
	void SetTonemapingCheckBoxText(QString str);
	void SetOffScreenRenderCheckBoxText(QString str);
	void SetGammaCorrectionCheckBoxText(QString str);
	void SignalOffScreenRenderCilcked(bool str);
private:
	QVBoxLayout* Layout;

	QQuickWidget* TonemapingCheckBox;

	QQuickWidget* OffScreenRenderCheckBox;

	QHBoxLayout*  GammaCorrectionLayout;
	QQuickWidget* GammaCorrectionBox;
	QQuickWidget* GammaCorrectionLineEdit;
};
#endif