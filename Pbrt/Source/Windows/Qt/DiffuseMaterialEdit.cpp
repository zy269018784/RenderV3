#include "DiffuseMaterialEdit.h"
#include "ui_DiffuseMaterialEdit.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QDebug>
void SetTextureColor(int index, float& r, float& g, float& b);
void SetTextureImage(int index, std::string filename);

DiffuseMaterialEdit::DiffuseMaterialEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiffuseMaterialEdit)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &DiffuseMaterialEdit::TextureEdit);
}

DiffuseMaterialEdit::~DiffuseMaterialEdit()
{
    delete ui;
}

void DiffuseMaterialEdit::SetSceneData(Render::SceneData* p) {
    if (!SceneData)
        SceneData = p;
}

void DiffuseMaterialEdit::SetMaterialName(std::string name) {

    MaterialName = name;

    if (!SceneData)
        return;
    TextureName = SceneData->GetTextureNameFromMaterialName(MaterialName);
    TextureIndex = SceneData->GetTextureIndexFromTextureName(TextureName);
    TextureType = SceneData->GetTextureTypeFromTextureIndex(TextureIndex);
    MaterialIndex = SceneData->GetMaterialIndexFromMaterialName(MaterialName);

    Render::DiffuseMaterialParameters& p = SceneData->GetDiffuseMaterialParameters(MaterialIndex);
    qDebug() << "MaterialIndex " << MaterialIndex << " TextureIndex " << TextureIndex << " TextureType " << TextureType;
    if (p.GetHasFile()) {
       
        std::string ImageFielName = SceneData->GetImageTextureFileNameFromTextureIndex(TextureIndex);
        qDebug() << "ImageFielName " << QString::fromStdString(ImageFielName);
        SetUITextureImageFileName(ImageFielName);
    }
    else {
        //设置背景黑色 
        QColor color(p.GetR(), p.GetG(), p.GetB()); 
        qDebug() << color;
        SetUIColor(color);
    }


}

void DiffuseMaterialEdit::TextureEdit() {
    qDebug() << "TextureEdit " << TextureType;
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

void DiffuseMaterialEdit::TextureColorEdit() {
    const QColor color = QColorDialog::getColor(Qt::green, this, "Select Color");
    // 刷新UI
    SetUIColor(color);
    // 更新渲染器内存
    float r = color.redF() * 255;
    float g = color.greenF() * 255;
    float b = color.blueF() * 255;

    ::SetTextureColor(TextureIndex, r, g, b);  

    emit TextureChanged();
}

void DiffuseMaterialEdit::TextureImageEdit() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".", tr("Image Files (*.png *.jpg)"));

    // 刷新UI
    SetUITextureImageFileName(fileName.toStdString());

    // 更新渲染器内存
    ::SetTextureImage(TextureIndex, fileName.toStdString());

    emit TextureChanged();
}

void DiffuseMaterialEdit::SetUIColor(QColor color) {
    QPalette pal(ui->widget_2->palette());
    pal.setColor(QPalette::Background, color);
    ui->widget_2->setPalette(pal);
    ui->widget_2->setAutoFillBackground(true);
}

void DiffuseMaterialEdit::SetUITextureImageFileName(const std::string& filename) {
    QPixmap pix;
    pix.load(QString::fromStdString(filename));
    pix = pix.scaled(ui->widget_2->size());
  
    QPalette pal = ui->widget_2->palette();
    pal.setBrush(QPalette::Background, QBrush(pix));
    ui->widget_2->setPalette(pal);
}
