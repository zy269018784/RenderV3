#ifdef USE_QT
#include "LightParameterGroupBox.h"
#include "GlfwRender.h"
#include <QColorDialog>
#include <QFileDialog>

LightParameterGroupBox::LightParameterGroupBox(QWidget* parent) : QGroupBox(parent)
{
	setTitle(QStringLiteral("光源参数"));

    Layout = new QVBoxLayout;
    setLayout(Layout);
    CreateLightNameLayout();
    CreateLightTypeLayout();
	CreateLightAlbedoLayout();
    Layout->addStretch(1);
    BindUISignal();
}

void LightParameterGroupBox::SetScene(GlfwRender* Scene1)
{
    Scene = Scene1;
}

void LightParameterGroupBox::SetLightIndex(int Index1)
{
    LightIndex = Index1;
    Rt::Light* light = Scene->GetLight(Index1);
    auto color = light->GetColor();

    //auto Albedo = Mat.GetAlbedo();
    //int Type = Mat.GetMaterialType();
    //float T = Mat.GetGlassTransmissivity();
    //float R = Mat.GetGlassReflectivity();

    //std::cout << "Index1 " << Albedo.r << " " << Albedo.g << " " << Albedo.b << std::endl;
    QColor color1(color.r * 255, color.g * 255, color.b * 255);

    emit SetRedText(QString::number(color.r));
    emit SetGreenText(QString::number(color.g));
    emit SetBlueText(QString::number(color.b));
    emit SetColorButton(color1);
    //emit SetTransmissivityText(QString::number(T));
    //emit SetReflectivityText(QString::number(R));
}

void LightParameterGroupBox::OnColorButtonClicked()
{
    QColor color = QColorDialog::getColor(QColor(0, 0, 0), this, QStringLiteral("选择颜色"));
    if (color.isValid())
    {
        // 更新材质颜色
        if (Scene)
            Scene->UpdateLightColor(LightIndex, glm::vec3(color.redF(), color.greenF(), color.blueF()));
        // 刷新UI
        emit SetRedText(QString("%1").arg(color.red()));
        emit SetGreenText(QString("%1").arg(color.green()));
        emit SetBlueText(QString("%1").arg(color.blue()));
        emit SetColorButton(color);
    }
}

void LightParameterGroupBox::BindUISignal()
{
    connect((QObject*)ColorButton->rootObject(), SIGNAL(signalClicked()), this, SLOT(OnColorButtonClicked()));
    connect(this, SIGNAL(SetColorButton(QColor)), (QObject*)ColorButton->rootObject(), SIGNAL(signalSetColor(QColor)));

    //connect((QObject*)DiffuseTextureButton->rootObject(), SIGNAL(signalClicked()), this, SLOT(OnDiffuseTextureButtonClicked()));
    //connect((QObject*)DiffuseTextureClearButton->rootObject(), SIGNAL(signalClicked()), this, SLOT(OnDiffuseTextureClearButtonClicked()));

    //connect((QObject*)NormalTextureButton->rootObject(), SIGNAL(signalClicked()), this, SLOT(OnNormalMapTextureButtonClicked()));

    //connect((QObject*)ReflectivitySlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnReflectivitySliderMoved(QString)));
    //connect(this, SIGNAL(SetReflectivityText(QString)), (QObject*)ReflectivityEdit->rootObject(), SIGNAL(signalSetText(QString)));

    //connect((QObject*)TransmissivitySlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnTransmissivitySliderMoved(QString)));
    //connect(this, SIGNAL(SetTransmissivityText(QString)), (QObject*)TransmissivityEdit->rootObject(), SIGNAL(signalSetText(QString)));

    //connect((QObject*)IndexOfRefractionSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnIndexOfRefractionSliderMoved(QString)));
    //connect(this, SIGNAL(SetIndexOfRefractionText(QString)), (QObject*)IndexOfRefractionEdit->rootObject(), SIGNAL(signalSetText(QString)));

    //connect((QObject*)RoughnessSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnRoughnessSliderMoved(QString)));
    //connect(this, SIGNAL(SetRoughnessText(QString)), (QObject*)RoughnessEdit->rootObject(), SIGNAL(signalSetText(QString)));

    connect(this, SIGNAL(SetRedText(QString)), (QObject*)RedEdit->rootObject(), SIGNAL(signalSetText(QString)));
    connect(this, SIGNAL(SetGreenText(QString)), (QObject*)GreenEdit->rootObject(), SIGNAL(signalSetText(QString)));
    connect(this, SIGNAL(SetBlueText(QString)), (QObject*)BlueEdit->rootObject(), SIGNAL(signalSetText(QString)));
}

void LightParameterGroupBox::CreateLightNameLayout()
{
    LightNameLabel = new QLabel(QStringLiteral("光源名称"));
    LightName2Label = new QLabel(QStringLiteral("xxxx"));

    LightNameLayout = new QHBoxLayout;
    LightNameLayout->addWidget(LightNameLabel);
    LightNameLayout->addStretch(1);
    LightNameLayout->addWidget(LightName2Label);

    Layout->addLayout(LightNameLayout);
}

void LightParameterGroupBox::CreateLightTypeLayout()
{
    LightTypeLabel = new QLabel(QStringLiteral("光源类型"));

    LightTypeComboBox = new QComboBox;
    LightTypeComboBox->addItem(QStringLiteral("点光源"));
    LightTypeComboBox->addItem(QStringLiteral("聚光灯"));
    LightTypeComboBox->addItem(QStringLiteral("方向光"));
    LightTypeComboBox->addItem(QStringLiteral("面光源"));

    LightTypeLayout = new QHBoxLayout;
    LightTypeLayout->addWidget(LightTypeLabel);
    LightTypeLayout->addStretch(1);
    LightTypeLayout->addWidget(LightTypeComboBox);
    Layout->addLayout(LightTypeLayout);
}

void LightParameterGroupBox::CreateLightAlbedoLayout()
{
    LightAlbedoLabel = new QLabel(QStringLiteral("颜色"));
    LightAlbedoRedLabel = new QLabel(QStringLiteral("R"));
    LightAlbedoGreenLabel = new QLabel(QStringLiteral("G"));
    LightAlbedoBlueLabel = new QLabel(QStringLiteral("B"));

    QUrl Source("qrc:Resource/QML/LineEdit.qml");
    QUrl ColorButtonSource("qrc:Resource/QML/ColorButton.qml");

    RedEdit = new QQuickWidget(this);
    RedEdit->setFixedWidth(40);
    RedEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    RedEdit->setSource(Source);

    GreenEdit = new QQuickWidget(this);
    GreenEdit->setFixedWidth(40);
    GreenEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    GreenEdit->setSource(Source);

    BlueEdit = new QQuickWidget(this);
    BlueEdit->setFixedWidth(40);
    BlueEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    BlueEdit->setSource(Source);

    ColorButton = new QQuickWidget(this);
    ColorButton->setFixedSize(30, 30);
    ColorButton->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ColorButton->setSource(ColorButtonSource);

    LightAlbedoLayout = new QHBoxLayout;
    LightAlbedoLayout->addWidget(LightAlbedoLabel);
    LightAlbedoLayout->addWidget(LightAlbedoRedLabel);
    LightAlbedoLayout->addWidget(RedEdit);
    LightAlbedoLayout->addWidget(LightAlbedoGreenLabel);
    LightAlbedoLayout->addWidget(GreenEdit);
    LightAlbedoLayout->addWidget(LightAlbedoBlueLabel);
    LightAlbedoLayout->addWidget(BlueEdit);
    LightAlbedoLayout->addWidget(ColorButton);
    Layout->addLayout(LightAlbedoLayout);
}
#endif