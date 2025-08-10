#ifdef USE_QT
#include "HDRParameterGroupBox.h"
#include "GlfwRender.h"
#include <QObject>
#include <QFileDialog>
#include <Scene/ColorTemperatureTable.h>

HDRParameterGroupBox::HDRParameterGroupBox(QWidget* parent) : QGroupBox(parent)
{
	setTitle(QStringLiteral("HDR参数"));
	setMinimumSize(100, 100);

	HDRParameterGroupBoxLayout = new QVBoxLayout;
	setLayout(HDRParameterGroupBoxLayout);

	CreateHDRFileLayout();
	CreateHDRRotateLayout();
	CreateHDRRotateSlider();
	CreateColorTemperatureLayout();
	/*
		HDR Slider
	*/
	//HDRRotateSlider = new QQuickWidget(this);
	//HDRRotateSlider->setMinimumHeight(30);
	//HDRRotateSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
	//HDRRotateSlider->setSource(QUrl("qrc:Resource/QML/Slider.qml"));

	connect((QObject*)HDRFileButton->rootObject(), SIGNAL(signalClicked()), this, SLOT(OnHDRFileButtonClicked()));
	connect((QObject*)HDRRotateSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnHDRLightSliderMoved(QString)));
	connect(this, SIGNAL(SetHDRText(QString)), (QObject*)HDRRotateEdit->rootObject(), SIGNAL(signalSetText(QString)));

	//HDRParameterGroupBoxLayout->addWidget(HDRRotateSlider);

	HDRParameterGroupBoxLayout->addStretch();
}

void HDRParameterGroupBox::SetScene(GlfwRender* Scene1)
{
	Scene = Scene1;
}

void HDRParameterGroupBox::SetMaterialIndex(int Index1)
{
	MaterialIndex = Index1;
}

void HDRParameterGroupBox::OnHDRFileButtonClicked()
{
	auto CurrentTextureFileName = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Images (*.exr)"));
    if (Scene)
    {
        Scene->UpdateHDR(CurrentTextureFileName.toStdString());
		HDRFileEdit->setText(CurrentTextureFileName);
    }
}

void HDRParameterGroupBox::OnColorTemperatureCilcked(bool str)
{
#if 0
	QObject* po = (QObject *)(ColorTemperatureLineEdit->rootObject());
	QVariant ret;
	QMetaObject::invokeMethod(po, "getText", Q_RETURN_ARG(QVariant, ret));
	auto Color = ColorTemperatureTable[ret.toInt()];
	qDebug() << "OnColorTemperatureCilcked " << Color.r << Color.g << Color.b;

	QColor c(Color.r * 255, Color.g * 255, Color.b * 255);
	QObject* po1 = (QObject*)(ColorTemperatureBox->rootObject());
	QMetaObject::invokeMethod(po1, "setColor", Q_ARG(QVariant, c));
#endif
}

void HDRParameterGroupBox::OnHDRLightSliderMoved(QString p)
{
	Scene->UpdateHDRRotate(p.toFloat(nullptr) * 360);
	emit SetHDRText(QString("%1").arg(p));
}

void HDRParameterGroupBox::CreateHDRFileLayout()
{
	HDRFileEdit = new QLineEdit(QStringLiteral("右侧按钮选择exr文件"));

	QUrl ButtonSource("qrc:Resource/QML/Button.qml");
	HDRFileButton = new QQuickWidget(this);
    HDRFileButton->setFixedSize(40, 20);
    HDRFileButton->setResizeMode(QQuickWidget::SizeRootObjectToView);
    HDRFileButton->setSource(ButtonSource);

    HDRFileLayout = new QHBoxLayout;
    HDRFileLayout->addWidget(HDRFileEdit);
	HDRFileLayout->addStretch();
    HDRFileLayout->addWidget(HDRFileButton);

    HDRParameterGroupBoxLayout->addLayout(HDRFileLayout);
}

void HDRParameterGroupBox::CreateHDRRotateLayout()
{
	HDRLabel = new QLabel(QStringLiteral("角度"));

	QUrl Source("qrc:Resource/QML/LineEdit.qml");

	HDRRotateEdit = new QQuickWidget(this);
	HDRRotateEdit->setFixedWidth(40);
	HDRRotateEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
	HDRRotateEdit->setSource(Source);

	HDRRotateLayout = new QHBoxLayout;
	HDRRotateLayout->addWidget(HDRLabel);
	HDRRotateLayout->addStretch();
	HDRRotateLayout->addWidget(HDRRotateEdit);

	HDRParameterGroupBoxLayout->addLayout(HDRRotateLayout);
}

void HDRParameterGroupBox::CreateHDRRotateSlider()
{
	/*
		HDR Slider
	*/
	HDRRotateSlider = new QQuickWidget(this);
	HDRRotateSlider->setMinimumHeight(30);
	HDRRotateSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
	HDRRotateSlider->setSource(QUrl("qrc:Resource/QML/Slider.qml"));
	HDRParameterGroupBoxLayout->addWidget(HDRRotateSlider);
}

void HDRParameterGroupBox::CreateColorTemperatureLayout()
{
	 ColorTemperatureLineEdit = new QQuickWidget;
	 ColorTemperatureLineEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
	 ColorTemperatureLineEdit->setSource(QUrl(("qrc:Resource/QML/LineEdit.qml")));
	 ColorTemperatureLineEdit->setFixedWidth(80);

	 ColorTemperatureBox = new QQuickWidget;
	 ColorTemperatureBox->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
	 ColorTemperatureBox->setFixedHeight(40);
	 ColorTemperatureBox->setResizeMode(QQuickWidget::SizeRootObjectToView);
	 ColorTemperatureBox->setSource(QUrl(("qrc:Resource/QML/CheckBox.qml")));
	 ColorTemperatureBox->show();

	 ColorTemperatureLayout = new QHBoxLayout;
	 ColorTemperatureLayout->addWidget( ColorTemperatureBox);
	 ColorTemperatureLayout->addStretch(1);
	 ColorTemperatureLayout->addWidget( ColorTemperatureLineEdit);

	connect(this, SIGNAL(SetColorTemperatureCheckBoxText(QString)), (QObject*) ColorTemperatureBox->rootObject(), SIGNAL(signalSetText(QString)));
	connect((QObject*)ColorTemperatureBox->rootObject(), SIGNAL(signalChecked(bool)), this, SLOT(OnColorTemperatureCilcked(bool)));

	emit SetColorTemperatureCheckBoxText(QStringLiteral("Color Temperature"));

	HDRParameterGroupBoxLayout->addLayout( ColorTemperatureLayout);
}
#endif