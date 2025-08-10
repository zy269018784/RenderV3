#ifdef USE_QT
#include "OtherParameterGroupBox.h"

OtherParameterGroupBox::OtherParameterGroupBox(QWidget* parent) : QGroupBox(parent)
{
	setTitle(QStringLiteral("其他参数"));
	Layout = new QVBoxLayout(this);
	setLayout(Layout);

	//QUrl Source("qrc:Resource/QML/CheckBox.qml");
	//TonemapingCheckBox = new QQuickWidget;
	//TonemapingCheckBox->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
	//TonemapingCheckBox->setFixedHeight(40);
	//TonemapingCheckBox->setResizeMode(QQuickWidget::SizeRootObjectToView);
	//TonemapingCheckBox->setSource(Source);
	//TonemapingCheckBox->show();

	//Layout->addWidget(TonemapingCheckBox);
	CreateTonemapingLayout();
	CreateOffScreenRenderLayout();
	CreateGammaCorrectionLayout();
	Layout->addStretch(1);
}

void OtherParameterGroupBox::CreateTonemapingLayout()
{
	QUrl Source("qrc:Resource/QML/CheckBox.qml");
	TonemapingCheckBox = new QQuickWidget;
	TonemapingCheckBox->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
	TonemapingCheckBox->setFixedHeight(40);
	TonemapingCheckBox->setResizeMode(QQuickWidget::SizeRootObjectToView);
	TonemapingCheckBox->setSource(Source);
	TonemapingCheckBox->show();
	Layout->addWidget(TonemapingCheckBox);

	connect(this, SIGNAL(SetTonemapingCheckBoxText(QString)), (QObject*)TonemapingCheckBox->rootObject(), SIGNAL(signalSetText(QString)));

	emit SetTonemapingCheckBoxText(QStringLiteral("ACES Tone Mapping"));
}

void OtherParameterGroupBox::CreateOffScreenRenderLayout()
{
	QUrl Source("qrc:Resource/QML/CheckBox.qml");
	OffScreenRenderCheckBox = new QQuickWidget;
	OffScreenRenderCheckBox->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
	OffScreenRenderCheckBox->setFixedHeight(40);
	OffScreenRenderCheckBox->setResizeMode(QQuickWidget::SizeRootObjectToView);
	OffScreenRenderCheckBox->setSource(Source);
	OffScreenRenderCheckBox->show();
	Layout->addWidget(OffScreenRenderCheckBox);
	
	connect(this, SIGNAL(SetOffScreenRenderCheckBoxText(QString)), (QObject*)OffScreenRenderCheckBox->rootObject(), SIGNAL(signalSetText(QString)));

	emit SetOffScreenRenderCheckBoxText(QStringLiteral("离屏幕渲染"));

	connect((QObject*)OffScreenRenderCheckBox->rootObject(), SIGNAL(signalChecked(bool)), this, SIGNAL(SignalOffScreenRenderCilcked(bool)));
}

void OtherParameterGroupBox::CreateGammaCorrectionLayout()
{
	GammaCorrectionLineEdit = new QQuickWidget;
	GammaCorrectionLineEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
	GammaCorrectionLineEdit->setSource(QUrl(("qrc:Resource/QML/LineEdit.qml")));
	GammaCorrectionLineEdit->setFixedWidth(80);

	GammaCorrectionBox = new QQuickWidget;
	GammaCorrectionBox->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
	GammaCorrectionBox->setFixedHeight(40);
	GammaCorrectionBox->setResizeMode(QQuickWidget::SizeRootObjectToView);
	GammaCorrectionBox->setSource(QUrl(("qrc:Resource/QML/CheckBox.qml")));
	GammaCorrectionBox->show();

	GammaCorrectionLayout = new QHBoxLayout;
	GammaCorrectionLayout->addWidget(GammaCorrectionBox);
	GammaCorrectionLayout->addStretch(1);
	GammaCorrectionLayout->addWidget(GammaCorrectionLineEdit);

	connect(this, SIGNAL(SetGammaCorrectionCheckBoxText(QString)), (QObject*)GammaCorrectionBox->rootObject(), SIGNAL(signalSetText(QString)));

	emit SetGammaCorrectionCheckBoxText(QStringLiteral("Gamma Correction"));

	Layout->addLayout(GammaCorrectionLayout);
}
#endif