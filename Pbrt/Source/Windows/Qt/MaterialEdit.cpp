#include "MaterialEdit.h"
#include "ui_MaterialEdit.h"
#include <QDebug>
MaterialEdit::MaterialEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaterialEdit)
{
    ui->setupUi(this);
    DME = new DiffuseMaterialEdit;
    GME = new GlassMaterialEdit;
    CME = new ConductorMaterialEdit;

    while (ui->stackedWidget->count() > 0)
        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(0));
    ui->stackedWidget->insertWidget(ui->stackedWidget->count(), DME);
    ui->stackedWidget->insertWidget(ui->stackedWidget->count(), GME);
    ui->stackedWidget->insertWidget(ui->stackedWidget->count(), CME);
    ui->stackedWidget->setCurrentWidget(GME);

    connect(GME, &GlassMaterialEdit::TextureChanged, this, &MaterialEdit::TextureChanged);
    connect(DME, &DiffuseMaterialEdit::TextureChanged, this, &MaterialEdit::TextureChanged);
}

MaterialEdit::~MaterialEdit()
{
    delete ui;
}

void MaterialEdit::SetSceneWrapper(Render::SceneWrapper* wrapper) {
    this->SceneWrapper = wrapper;
    GME->SetSceneWrapper(wrapper);
}

void MaterialEdit::SetSceneData(Render::SceneData* p) {
    if (!SceneData)
        SceneData = p;
    GME->SetSceneData(p);
    DME->SetSceneData(p);
}

void MaterialEdit::SetMaterialName(std::string name)
{
    if (!SceneData)
        return;

    MaterialName = name;
    int MaterialIndex = SceneData->GetMaterialIndexFromMaterialName(MaterialName);
    MaterialType = SceneData->GetMaterialTypeFromMaterialIndex(MaterialIndex);
    TextureName = SceneData->GetTextureNameFromMaterialIndex(MaterialIndex);
    TextureIndex = SceneData->GetTextureIndexFromTextureName(TextureName);
    TextureType = SceneData->GetTextureTypeFromTextureIndex(TextureIndex);

    // ui显示材质名称,纹理名称和材质类型
    ui->label_3->setText(QString::fromStdString(MaterialName));
    ui->label_4->setText(QString::fromStdString(TextureName));

    int MaterialTypeIndex = ui->comboBox->findText(QString::fromStdString(MaterialType), Qt::MatchFixedString);
    ui->comboBox->setCurrentIndex(0);
    if (MaterialTypeIndex != -1) {
        ui->comboBox->setCurrentIndex(MaterialTypeIndex);
    }

    if (MaterialType == "diffuse") {
        DME->SetMaterialName(MaterialName);
        ui->stackedWidget->setCurrentWidget(DME);
    }
    else if (MaterialType == "glass") {
        GME->SetMaterialName(MaterialName);
        ui->stackedWidget->setCurrentWidget(GME);
    }
    else if (MaterialType == "conductor") {
        ui->stackedWidget->setCurrentWidget(CME);
    }
}
#if 0
void MaterialEdit::SetMaterialType(std::string name)
{
    MaterialType = name;

    // 全字匹配, 不匹配大小写
    int MaterialTypeIndex = ui->comboBox->findText(QString::fromStdString(MaterialType), Qt::MatchFixedString);
    qDebug() << "TypeIndex " << MaterialTypeIndex << QString::fromStdString(MaterialType);
    ui->comboBox->setCurrentIndex(0);
    if (MaterialTypeIndex != -1) {
        ui->comboBox->setCurrentIndex(MaterialTypeIndex);
    }
    switch (MaterialTypeIndex) {
    case 1:
        ui->stackedWidget->setCurrentWidget(DME);
        break;
    case 2:
        ui->stackedWidget->setCurrentWidget(GME);
        break;
    case 3:
        ui->stackedWidget->setCurrentWidget(CME);
        break;
    default:
        ui->stackedWidget->setCurrentIndex(0);
        break;
    }
}

void MaterialEdit::SetTextureName(std::string name)
{
    TextureName = name;
    ui->label_4->setText(QString::fromStdString(TextureName));
}

void MaterialEdit::SetTextureIndex(int index) {
    TextureIndex = index;

    DME->SetTextureIndex(TextureIndex);
    GME->SetTextureIndex(TextureIndex);
}

void MaterialEdit::SetTextureType(int type) {
    DME->SetTextureType(type);
}

#endif

//void MaterialEdit::SetGlassMaterialParameters(Render::GlassMaterialParameters& p) {
//    GME->SetGlassMaterialParameters(p);
//}