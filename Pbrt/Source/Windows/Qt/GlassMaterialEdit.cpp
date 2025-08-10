#include "GlassMaterialEdit.h"
#include "ui_GlassMaterialEdit.h"
#include <QColorDialog> 
#include <QFileDialog>
#include <QDebug>

void SetTextureColor(int index, float& r, float& g, float& b);
void SetTextureImage(int index, std::string filename);
GlassMaterialEdit::GlassMaterialEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GlassMaterialEdit)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &GlassMaterialEdit::TextureEdit);
    connect(ui->horizontalSlider_2, &QSlider::sliderReleased, this, &GlassMaterialEdit::TransmittanceChanged);
    connect(ui->horizontalSlider_3, &QSlider::sliderReleased, this, &GlassMaterialEdit::ReflectivityChanged);

}

GlassMaterialEdit::~GlassMaterialEdit()
{
    delete ui;
}

void GlassMaterialEdit::SetSceneWrapper(Render::SceneWrapper* wrapper) {
    this->SceneWrapper = wrapper;
}

void GlassMaterialEdit::SetSceneData(Render::SceneData* p) {
    if (!SceneData)
        SceneData = p;
}

void GlassMaterialEdit::SetMaterialName(std::string name) {
    MaterialName = name;

    if (!SceneData)
        return;

    MaterialIndex  = SceneData->GetMaterialIndexFromMaterialName(MaterialName);
    TextureIndex   = SceneData->GetTextureIndexFromMaterialIndex(MaterialIndex);
    TextureType = SceneData->GetTextureTypeFromTextureIndex(TextureIndex);
    Render::GlassMaterialParameters& p = SceneData->GetGlassMaterialParameters(MaterialIndex);
    // UI显示反射率和透射率
    SetTransmittance(p.GetTransmissivity());
    SetReflectivity(p.GetReflectivity());
    if (p.GetHasFile()) {
        std::string ImageFielName = SceneData->GetImageTextureFileNameFromTextureIndex(TextureIndex);
        qDebug() << "ImageFielName " << QString::fromStdString(ImageFielName);
        SetUITextureImageFileName(ImageFielName);
    }
    else {
         //设置背景黑色 
        QColor color(p.GetR(), p.GetG(), p.GetB());
        SetUIColor(color);
    }
}

//void GlassMaterialEdit::colorEdit() {
//    QColor color = QColorDialog::getColor(Qt::green, this, "Select Color");
//    SetUIColor(color);
//    if (TextureIndex != -1) {
//        float r = color.redF() * 255;
//        float g = color.greenF() * 255;
//        float b = color.blueF() * 255;
//        ::SetTextureColor(TextureIndex, r, g, b);
//
//        emit TextureChanged();
//    }
//}

void GlassMaterialEdit::TextureEdit() {
    switch (TextureType) {
    case 0:
        TextureImageEdit();
        break;
    case 1:
        TextureColorEdit();
    default:
        break;
    }
    emit TextureChanged();
}

void GlassMaterialEdit::TransmittanceChanged() {
    float t = ui->horizontalSlider_2->value() / 100.f;
    float r = ui->horizontalSlider_3->value() / 100.f;
    if (SceneWrapper) {
        SceneWrapper->SetGlassMaterial(MaterialIndex, r, t);
        emit TextureChanged();
    }
}

void GlassMaterialEdit::ReflectivityChanged() {
    float t = ui->horizontalSlider_2->value() / 100.f;
    float r = ui->horizontalSlider_3->value() / 100.f;
    if (SceneWrapper) {
        SceneWrapper->SetGlassMaterial(MaterialIndex, r, t);
        emit TextureChanged();
    }
}

void GlassMaterialEdit::TextureColorEdit() {
    const QColor color = QColorDialog::getColor(Qt::green, this, "Select Color");

    // 刷新UI
    SetUIColor(color);
    // 更新渲染器内存
    float r = color.redF() * 255;
    float g = color.greenF() * 255;
    float b = color.blueF() * 255;
    if (-1 != TextureIndex)
        ::SetTextureColor(TextureIndex, r, g, b);

    emit TextureChanged();
}

void GlassMaterialEdit::TextureImageEdit() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".", tr("Image Files (*.png *.jpg)"));

    // 刷新UI
    SetUITextureImageFileName(fileName.toStdString());

    // 更新渲染器内存
    ::SetTextureImage(TextureIndex, fileName.toStdString());

    emit TextureChanged();
}

void GlassMaterialEdit::SetUIColor(QColor color) {
    QPalette pal(ui->widget_2->palette());
    pal.setColor(QPalette::Background, color);
    ui->widget_2->setPalette(pal);
    ui->widget_2->setAutoFillBackground(true);
}


void GlassMaterialEdit::SetUITextureImageFileName(const std::string& filename) {
    QPixmap pix;
    pix.load(QString::fromStdString(filename));
    pix = pix.scaled(ui->widget_2->size());

    QPalette pal = ui->widget_2->palette();
    pal.setBrush(QPalette::Background, QBrush(pix));
    ui->widget_2->setPalette(pal);
    ui->widget_2->setAutoFillBackground(true);
}

void GlassMaterialEdit::SetTransmittance(float t) {
    disconnect(ui->horizontalSlider_2, &QSlider::valueChanged, this, &GlassMaterialEdit::TransmittanceChanged);
    ui->horizontalSlider_2->setValue(t * 100);
    connect(ui->horizontalSlider_2, &QSlider::valueChanged, this, &GlassMaterialEdit::TransmittanceChanged);
    ui->label_5->setText(QString("%1").arg(t));
}


void GlassMaterialEdit::SetReflectivity(float r) {
    disconnect(ui->horizontalSlider_3, &QSlider::valueChanged, this, &GlassMaterialEdit::ReflectivityChanged);
    ui->horizontalSlider_3->setValue(r * 100);
    connect(ui->horizontalSlider_3, &QSlider::valueChanged, this, &GlassMaterialEdit::ReflectivityChanged);
    ui->label_6->setText(QString("%1").arg(r));
}
void GlassMaterialEdit::SetIOR(float ior) {
    ui->horizontalSlider_1->setValue(ior * 100);

}
