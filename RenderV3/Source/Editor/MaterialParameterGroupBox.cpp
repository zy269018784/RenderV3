#ifdef USE_QT
#include "MaterialParameterGroupBox.h"
#include "GlfwRender.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QFile>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
using namespace QXlsx;
MaterialParameterGroupBox::MaterialParameterGroupBox(QWidget* parent) : QGroupBox(parent),
     Scene(nullptr), MaterialIndex(-1)
{
    setTitle(QStringLiteral("材质参数"));
    MtlParamGroupBoxLayout = new QVBoxLayout;
    setLayout(MtlParamGroupBoxLayout);
	CreateMtlNameLayout();
    CreateMtlTypeLayout();
    CreateMtlAlbedoLayout();
    CreateMtlTextureLayout();
    CreateMtlNormalMapLayout();
    CreateReflectivityLayout();
    CreateTransmissivityLayout();
    CreateIndexOfRefractionLayout();
    CreateURoughnessLayout();
    CreateVRoughnessLayout();
    CreateDatabaseSaveLayout();
    CreateDisneyMaterialParameterLayout();
    MtlParamGroupBoxLayout->addStretch(1);
    BindUISignal();

    if (sqlite3_open("Materials.db", &db))
    {
        qDebug() << "open db failed";
    }
}

void MaterialParameterGroupBox::SetScene(GlfwRender* Scene1)
{
    Scene = Scene1;
}

void MaterialParameterGroupBox::SetMaterialIndex(int Index1)
{
   if (!Scene)
       return;
    MaterialIndex = Index1;
    const Rt::Material& Mat = Scene->GetMaterial(Index1);
    
    auto Albedo = Mat.GetAlbedo();
    int Type = Mat.GetMaterialType();
    float T = Mat.GetGlassTransmissivity();
    float R = Mat.GetGlassReflectivity();
    
    
  //  std::cout << "Index1 " << Albedo.r << " " << Albedo.g << " " << Albedo.b << std::endl;
    QColor Albedo1(Albedo.r * 255, Albedo.g * 255, Albedo.b * 255);

    emit SetRedText(QString::number(Albedo.r));
    emit SetGreenText(QString::number(Albedo.g));
    emit SetBlueText(QString::number(Albedo.b));
    emit SetColorButton(Albedo1);
    emit SetTransmissivityText(QString::number(T));
    emit SetReflectivityText(QString::number(R));
}

void MaterialParameterGroupBox::AddManufacturerMaterial(string Name, string NameMapping, string FirstClass,
    string Type, 
    float R, float G, float B,
    string Texture, string NormalMap,
    float Reflectivity, float Transmissivity,
    float URoughness, float VRoughness, float IOR)
{
    ManufacturerMatNames.push_back(Name);
    ManufacturerMatNameMapeds.push_back(NameMapping);
    ManufacturerMatFirstClasss.push_back(FirstClass);
    Rs.push_back(R);
    Gs.push_back(G);
    Bs.push_back(B);
    ManufacturerMatTypes.push_back(Type);
    ManufacturerMatTextures.push_back(Texture);
    ManufacturerMatNormalMaps.push_back(NormalMap);
    ManufacturerMatReflectivitys.push_back(Reflectivity);
    ManufacturerMatTransmissivitys.push_back(Transmissivity);
    ManufacturerMatURoughnesss.push_back(URoughness);
    ManufacturerMatVRoughnesss.push_back(VRoughness);
    ManufacturerMatIORs.push_back(IOR);
}

void MaterialParameterGroupBox::ExportPBRToXLSL()
{
   
    QString filePath = QFileDialog::getSaveFileName(this, QStringLiteral("*.xlsx"), "", "*.xlsx");
    qDebug() << "ExportPBRToXLSL" << filePath;
    //return;
    if (!filePath.isEmpty())
    {
        QFile::setPermissions(filePath, QFile::ReadOther | QFile::WriteOther);
        qDebug() << "remove " << filePath << ":" << QFile::remove(filePath);
        Document xlsx(filePath);
        xlsx.addSheet("pbr");
        xlsx.selectSheet(0);
        auto sheet = xlsx.currentWorksheet();

        sheet->write(1,  1, QString("mtl_name"));
        sheet->write(1,  2, QString("nameMapping"));
        sheet->write(1,  3, QString("一级分类"));
        sheet->write(1,  4, QString(""));
        sheet->write(1,  5, QString(""));
        sheet->write(1,  6, QString(""));
        sheet->write(1,  7, QString(""));
        sheet->write(1,  8, QString("pbr_mtl_type")); 
        sheet->write(1,  9, QString(""));
        sheet->write(1, 10, QString(""));
        sheet->write(1, 11, QString("diffuse_color_r"));
        sheet->write(1, 12, QString("diffuse_color_g"));
        sheet->write(1, 13, QString("diffuse_color_b"));
        sheet->write(1, 14, QString("diffuse_texture"));
        sheet->write(1, 15, QString("normal_map"));
        sheet->write(1, 16, QString("reflectivity"));
        sheet->write(1, 17, QString("transmissivity"));
        sheet->write(1, 18, QString("URoughness"));
        sheet->write(1, 19, QString("VRoughness"));
        sheet->write(1, 20, QString("index_of_refraction"));

        sheet->write(1, 21, QString("%1").arg("Metallic"));
        sheet->write(1, 22, QString("%1").arg("Roughness"));
        sheet->write(1, 23, QString("%1").arg("Subsurface"));
        sheet->write(1, 24, QString("%1").arg("Specular"));
        sheet->write(1, 25, QString("%1").arg("SpecularTint"));
        sheet->write(1, 26, QString("%1").arg("Roughness"));
        sheet->write(1, 27, QString("%1").arg("Anisotropy"));
        sheet->write(1, 28, QString("%1").arg("Sheen"));
        sheet->write(1, 29, QString("%1").arg("SheenTint"));
        sheet->write(1, 30, QString("%1").arg("Clearcoat"));
        sheet->write(1, 31, QString("%1").arg("ClearcoatGloss"));
        sheet->write(1, 32, QString("%1").arg("Tranmission"));
        for (int i = 0; i < ManufacturerMatNames.size(); i++)
        {
            sheet->write(i + 2,  1, QString::fromStdString(ManufacturerMatNames[i]));
            sheet->write(i + 2,  2, QString::fromStdString(ManufacturerMatNameMapeds[i]));
            sheet->write(i + 2,  3, QString::fromStdString(ManufacturerMatFirstClasss[i]));
            sheet->write(i + 2,  8, QString::fromStdString(ManufacturerMatTypes[i]));
            sheet->write(i + 2, 11, QString("%1").arg(Rs[i] / 255));
            sheet->write(i + 2, 12, QString("%1").arg(Gs[i] / 255));
            sheet->write(i + 2, 13, QString("%1").arg(Bs[i] / 255));
            sheet->write(i + 2, 14, QString::fromStdString(ManufacturerMatTextures[i]));
            sheet->write(i + 2, 15, QString::fromStdString(ManufacturerMatNormalMaps[i]));
            sheet->write(i + 2, 16, QString("%1").arg(ManufacturerMatReflectivitys[i]));
            sheet->write(i + 2, 17, QString("%1").arg(ManufacturerMatTransmissivitys[i]));
            sheet->write(i + 2, 18, QString("%1").arg(ManufacturerMatURoughnesss[i]));
            sheet->write(i + 2, 19, QString("%1").arg(ManufacturerMatVRoughnesss[i]));
            sheet->write(i + 2, 20, QString("%1").arg(ManufacturerMatIORs[i]));
            if (ManufacturerMatTypes[i].c_str() == "Disney")
            {
                sheet->write(i + 2, 21, QString("%1").arg(ManufacturerDisneyMaterialMetallic[i]));
                sheet->write(i + 2, 22, QString("%1").arg(ManufacturerDisneyMaterialRoughness[i]));
                sheet->write(i + 2, 23, QString("%1").arg(ManufacturerDisneyMaterialSubsurface[i]));
                sheet->write(i + 2, 24, QString("%1").arg(ManufacturerDisneyMaterialSpecular[i]));
                sheet->write(i + 2, 25, QString("%1").arg(ManufacturerDisneyMaterialSpecularTint[i]));
                sheet->write(i + 2, 26, QString("%1").arg(ManufacturerDisneyMaterialRoughness[i]));
                sheet->write(i + 2, 27, QString("%1").arg(ManufacturerDisneyMaterialAnisotropy[i]));
                sheet->write(i + 2, 28, QString("%1").arg(ManufacturerDisneyMaterialSheen[i]));
                sheet->write(i + 2, 29, QString("%1").arg(ManufacturerDisneyMaterialSheenTint[i]));
                sheet->write(i + 2, 30, QString("%1").arg(ManufacturerDisneyMaterialClearcoat[i]));
                sheet->write(i + 2, 31, QString("%1").arg(ManufacturerDisneyMaterialClearcoatGloss[i]));
                sheet->write(i + 2, 32, QString("%1").arg(ManufacturerDisneyMaterialTranmission[i]));
            }
        }
        xlsx.saveAs(filePath);

    }
}

void MaterialParameterGroupBox::TemporaryStorePBRToXLSL()
{
    QString filePath = "temp.xlsx";

    if (!filePath.isEmpty())
    {
        QFile::setPermissions(filePath, QFile::ReadOther | QFile::WriteOther);
        qDebug() << "remove " << filePath  << ":" << QFile::remove(filePath);
        Document xlsx(filePath);
        xlsx.addSheet("pbr");
        xlsx.selectSheet(0);
        auto sheet = xlsx.currentWorksheet();

        sheet->write(1, 1, QString("mtl_name"));
        sheet->write(1, 2, QString("nameMapping"));
        sheet->write(1, 3, QString("一级分类"));
        sheet->write(1, 4, QString(""));
        sheet->write(1, 5, QString(""));
        sheet->write(1, 6, QString(""));
        sheet->write(1, 7, QString(""));
        sheet->write(1, 8, QString("pbr_mtl_type"));
        sheet->write(1, 9, QString(""));
        sheet->write(1, 10, QString(""));
        sheet->write(1, 11, QString("diffuse_color_r"));
        sheet->write(1, 12, QString("diffuse_color_g"));
        sheet->write(1, 13, QString("diffuse_color_b"));
        sheet->write(1, 14, QString("diffuse_texture"));
        sheet->write(1, 15, QString("normal_map"));
        sheet->write(1, 16, QString("reflectivity"));
        sheet->write(1, 17, QString("transmissivity"));
        sheet->write(1, 18, QString("URoughness"));
        sheet->write(1, 19, QString("VRoughness"));
        sheet->write(1, 20, QString("index_of_refraction"));
        for (int i = 0; i < ManufacturerMatNames.size(); i++)
        {
            sheet->write(i + 2, 1, QString::fromStdString(ManufacturerMatNames[i]));
            sheet->write(i + 2, 2, QString::fromStdString(ManufacturerMatNameMapeds[i]));
            sheet->write(i + 2, 3, QString::fromStdString(ManufacturerMatFirstClasss[i]));
            sheet->write(i + 2, 8, QString::fromStdString(ManufacturerMatTypes[i]));
            sheet->write(i + 2, 11, QString("%1").arg(Rs[i] / 255));
            sheet->write(i + 2, 12, QString("%1").arg(Gs[i] / 255));
            sheet->write(i + 2, 13, QString("%1").arg(Bs[i] / 255));
            sheet->write(i + 2, 14, QString::fromStdString(ManufacturerMatTextures[i]));
            sheet->write(i + 2, 15, QString::fromStdString(ManufacturerMatNormalMaps[i]));
            sheet->write(i + 2, 16, QString("%1").arg(ManufacturerMatReflectivitys[i]));
            sheet->write(i + 2, 17, QString("%1").arg(ManufacturerMatTransmissivitys[i]));
            sheet->write(i + 2, 18, QString("%1").arg(ManufacturerMatURoughnesss[i]));
            sheet->write(i + 2, 19, QString("%1").arg(ManufacturerMatVRoughnesss[i]));
            sheet->write(i + 2, 20, QString("%1").arg(ManufacturerMatIORs[i]));
        }
        xlsx.saveAs(filePath);

    }
}

void MaterialParameterGroupBox::SetManufacturerMaterialIndex(int index)
{
    ManufacturerMaterialIndex = index;
    
    if (Scene && MaterialIndex >= 0)
    {
        if (ManufacturerMatTextures[index] != "")
        {
            Scene->updateTexture(MaterialIndex, ManufacturerMatTextures[index].data());
            DiffuseTextureNameLabel->setText(QString::fromStdString(ManufacturerMatTextures[index]).split("/").last());
        }
        else
        {
            Scene->ClearTexture(MaterialIndex);
            DiffuseTextureNameLabel->setText("未选择");
        }

        if (ManufacturerMatNormalMaps[index] != "")
        {
            Scene->updateNormalMapTexture(MaterialIndex, ManufacturerMatNormalMaps[index].data());
            MtlNormalmapNameLabel->setText(QString::fromStdString(ManufacturerMatNormalMaps[index]).split("/").last());
        }
        else
        {
            Scene->ClearNormalMap(MaterialIndex);
            MtlNormalmapNameLabel->setText("未选择");
        }
        Scene->UpdateMaterialAlbedo(MaterialIndex, glm::vec3(Rs[index], Gs[index], Bs[index]));

 
        if ("玻璃" == ManufacturerMatTypes[index])
        {
            Scene->UpdateGlassMaterialReflectivity(MaterialIndex, ManufacturerMatReflectivitys[index]);
            Scene->UpdateMaterialTransmissivity(MaterialIndex, ManufacturerMatTransmissivitys[index]);
            Scene->UpdateMaterialType(MaterialIndex, Rt::MaterialType::Glass);
            MtlTypeComboBox->setCurrentIndex(2);
        }
        else if ("非金属" == ManufacturerMatTypes[index])
        {
            Scene->UpdateDielectricMaterialReflectivity(MaterialIndex, ManufacturerMatReflectivitys[index]);
            Scene->UpdateMaterialType(MaterialIndex, Rt::MaterialType::Dielectric);
            Scene->UpdateMaterialURoughness(MaterialIndex, ManufacturerMatURoughnesss[index]);
            Scene->UpdateMaterialVRoughness(MaterialIndex, ManufacturerMatVRoughnesss[index]);
            MtlTypeComboBox->setCurrentIndex(1);
        }
        else if ("金属" == ManufacturerMatTypes[index])
        {
            Scene->UpdateMaterialURoughness(MaterialIndex, ManufacturerMatURoughnesss[index]);
            Scene->UpdateMaterialVRoughness(MaterialIndex, ManufacturerMatVRoughnesss[index]);
            Scene->UpdateMaterialType(MaterialIndex, Rt::MaterialType::Conductor);
            MtlTypeComboBox->setCurrentIndex(0);
        }
        else if ("Disney" == ManufacturerMatTypes[index])
        {
            MtlTypeComboBox->setCurrentIndex(3);

            Scene->UpdateDisneyMaterialMetallic(MaterialIndex, ManufacturerDisneyMaterialAnisotropy[index]);
            Scene->UpdateDisneyMaterialRoughness(MaterialIndex, ManufacturerDisneyMaterialAnisotropy[index]);
            Scene->UpdateDisneyMaterialSubsurface(MaterialIndex, ManufacturerDisneyMaterialAnisotropy[index]);
            Scene->UpdateDisneyMaterialSpecular(MaterialIndex, ManufacturerDisneyMaterialAnisotropy[index]);
            Scene->UpdateDisneyMaterialSpecularTint(MaterialIndex, ManufacturerDisneyMaterialAnisotropy[index]);
            Scene->UpdateDisneyMaterialAnisotropy(MaterialIndex, ManufacturerDisneyMaterialAnisotropy[index]);
            Scene->UpdateDisneyMaterialSheen(MaterialIndex, ManufacturerDisneyMaterialAnisotropy[index]);
            Scene->UpdateDisneyMaterialSheenTint(MaterialIndex, ManufacturerDisneyMaterialAnisotropy[index]);
            Scene->UpdateDisneyMaterialClearcoat(MaterialIndex, ManufacturerDisneyMaterialAnisotropy[index]);
            Scene->UpdateDisneyMaterialClearcoatGloss(MaterialIndex, ManufacturerDisneyMaterialAnisotropy[index]);
            Scene->UpdateDisneyMaterialTransmission(MaterialIndex, ManufacturerDisneyMaterialAnisotropy[index]);
        }
    }    

    if (ManufacturerMatReflectivitys.size() <= ManufacturerMaterialIndex)
        return;
    //qDebug() << "test 4 " << ManufacturerMatReflectivitys.size() << " " << index;
    QObject* p;
    p = (QObject*)ReflectivitySlider->rootObject();
    p->setProperty("value", ManufacturerMatReflectivitys[index]);
    //qDebug() << "test 5";
    p = (QObject*)TransmissivitySlider->rootObject();
    p->setProperty("value", ManufacturerMatTransmissivitys[index]);
    //qDebug() << "test 6";
    p = (QObject*)IndexOfRefractionSlider->rootObject();
    p->setProperty("value", ManufacturerMatIORs[index]);
   // qDebug() << "test 7";
    p = (QObject*)URoughnessSlider->rootObject();
    p->setProperty("value", ManufacturerMatURoughnesss[index]);
   // qDebug() << "test 8";
    p = (QObject*)VRoughnessSlider->rootObject();
    p->setProperty("value", ManufacturerMatVRoughnesss[index]);
    //qDebug() << "test 9";
    emit SetRedText(QString("%1").arg(Rs[index]));
    emit SetGreenText(QString("%1").arg(Gs[index]));
    emit SetBlueText(QString("%1").arg(Bs[index]));
    emit SetColorButton(QColor(Rs[index] * 255, Gs[index] * 255, Bs[index] * 255));
    emit SetTransmissivityText(QString("%1").arg(ManufacturerMatTransmissivitys[index]));
    emit SetReflectivityText(QString("%1").arg(ManufacturerMatReflectivitys[index]));
    emit SetURoughnessText(QString("%1").arg(ManufacturerMatURoughnesss[index]));
    emit SetVRoughnessText(QString("%1").arg(ManufacturerMatVRoughnesss[index]));
    //qDebug() << "test 10";
}

void MaterialParameterGroupBox::BindUISignal()
{
    connect((QObject*)ColorButton->rootObject(), SIGNAL(signalClicked()), this, SLOT(OnColorButtonClicked()));
    connect(this, SIGNAL(SetColorButton(QColor)), (QObject*)ColorButton->rootObject(), SIGNAL(signalSetColor(QColor)));

    connect((QObject*)DiffuseTextureButton->rootObject(), SIGNAL(signalClicked()), this, SLOT(OnDiffuseTextureButtonClicked()));
    connect((QObject*)DiffuseTextureClearButton->rootObject(), SIGNAL(signalClicked()), this, SLOT(OnDiffuseTextureClearButtonClicked()));

    connect((QObject*)NormalTextureButton->rootObject(), SIGNAL(signalClicked()), this, SLOT(OnNormalMapTextureButtonClicked()));
    connect((QObject*)NormalTextureClearButton->rootObject(), SIGNAL(signalClicked()), this, SLOT(OnNormalMapTextureClearButtonClicked()));

    connect((QObject*)ReflectivitySlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnReflectivitySliderMoved(QString)));
    connect(this, SIGNAL(SetReflectivityText(QString)), (QObject*)ReflectivityEdit->rootObject(), SIGNAL(signalSetText(QString)));

    connect((QObject*)TransmissivitySlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnTransmissivitySliderMoved(QString)));
    connect(this, SIGNAL(SetTransmissivityText(QString)), (QObject*)TransmissivityEdit->rootObject(), SIGNAL(signalSetText(QString)));

    connect((QObject*)IndexOfRefractionSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnIndexOfRefractionSliderMoved(QString)));
    connect(this, SIGNAL(SetIndexOfRefractionText(QString)), (QObject*)IndexOfRefractionEdit->rootObject(), SIGNAL(signalSetText(QString)));

    connect((QObject*)URoughnessSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnURoughnessSliderMoved(QString)));
    connect(this, SIGNAL(SetURoughnessText(QString)), (QObject*)URoughnessEdit->rootObject(), SIGNAL(signalSetText(QString)));

    connect((QObject*)VRoughnessSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnVRoughnessSliderMoved(QString)));
    connect(this, SIGNAL(SetVRoughnessText(QString)), (QObject*)VRoughnessEdit->rootObject(), SIGNAL(signalSetText(QString)));

    connect((QObject*)MetallicSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnMetallicSliderMoved(QString)));
    connect((QObject*)RoughnessSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnRoughnessSliderMoved(QString)));
    connect((QObject*)SubsurfaceSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnSubsurfaceSliderMoved(QString)));
    connect((QObject*)SpecularSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnSpecularSliderMoved(QString)));
    connect((QObject*)SpecularTintSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnSpecularTintSliderMoved(QString)));
    connect((QObject*)AnisotropySlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnAnisotropySliderMoved(QString)));
    connect((QObject*)SheenSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnSheenSliderMoved(QString)));
    connect((QObject*)SheenTintSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnSheenTintSliderMoved(QString)));
    connect((QObject*)ClearcoatSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnClearcoatSliderMoved(QString)));
    connect((QObject*)ClearcoatGlossSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnClearcoatGlossSliderMoved(QString)));
    connect((QObject*)IORSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnIORSliderMoved(QString)));
    connect((QObject*)TransmissionSlider->rootObject(), SIGNAL(signalMoved(QString)), this, SLOT(OnTransmissionSliderMoved(QString)));


    connect(this, SIGNAL(SetRedText(QString)), (QObject*)RedEdit->rootObject(), SIGNAL(signalSetText(QString)));
    connect(this, SIGNAL(SetGreenText(QString)), (QObject*)GreenEdit->rootObject(), SIGNAL(signalSetText(QString)));
    connect(this, SIGNAL(SetBlueText(QString)), (QObject*)BlueEdit->rootObject(), SIGNAL(signalSetText(QString)));

    connect(MtlTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnCurrentIndexChanged(int)));

    connect((QObject*)DatabaseSaveButton->rootObject(), SIGNAL(signalClicked()), this, SLOT(OnDatabaseSaveButtonClicked()));

    connect((QObject*)ReflectivityEdit->rootObject(), SIGNAL(accepted()), this, SLOT(OnReflectivityTextFieldAccept()));
    connect((QObject*)TransmissivityEdit->rootObject(), SIGNAL(accepted()), this, SLOT(OnTransmissivityTextFieldAccept()));
    connect((QObject*)IndexOfRefractionEdit->rootObject(), SIGNAL(accepted()), this, SLOT(OnIndexOfRefractionTextFieldAccept()));
    connect((QObject*)URoughnessEdit->rootObject(), SIGNAL(accepted()), this, SLOT(OnURoughnessTextFieldAccept()));
    connect((QObject*)VRoughnessEdit->rootObject(), SIGNAL(accepted()), this, SLOT(OnVRoughnessTextFieldAccept()));
}

/*
        Diffuse = 1,
        Glass = 2,
        Conductor = 3,
        Dielectric = 4
*/
void MaterialParameterGroupBox::OnCurrentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        CurrentMaterialType = Rt::MaterialType::Conductor;
        if (ManufacturerMaterialIndex >= 0)
            ManufacturerMatTypes[ManufacturerMaterialIndex] = "金属";
        qDebug() << "Conductor";
        break;
    case 1:
        CurrentMaterialType = Rt::MaterialType::Dielectric;
        if (ManufacturerMaterialIndex >= 0)
            ManufacturerMatTypes[ManufacturerMaterialIndex] = "非金属";
        qDebug() << "Dielectric";
        break;
    case 2:
        CurrentMaterialType = Rt::MaterialType::Glass;
        if (ManufacturerMaterialIndex >= 0)
            ManufacturerMatTypes[ManufacturerMaterialIndex] = "玻璃";
        qDebug() << "Glass";
        break;
    case 3:
        CurrentMaterialType = Rt::MaterialType::Disney;
        if (ManufacturerMaterialIndex >= 0)
            ManufacturerMatTypes[ManufacturerMaterialIndex] = "迪士尼";
        qDebug() << "Disney";
        break;
    default:
        break;
    }
    qDebug() << "OnCurrentIndexChanged" << index;

    if (Scene && MaterialIndex >= 0)
        Scene->UpdateMaterialType(MaterialIndex, CurrentMaterialType);
}


void MaterialParameterGroupBox::OnColorButtonClicked()
{
    QColor color = QColorDialog::getColor(QColor(0, 0, 0), this, QStringLiteral("选择颜色"));
    if (color.isValid())
    {
        // 更新材质颜色
        if (Scene)
        {
            Scene->UpdateMaterialAlbedo(MaterialIndex, glm::vec3(color.redF(), color.greenF(), color.blueF()));

        }     
        if (ManufacturerMaterialIndex >= 0)
        {
            Rs[ManufacturerMaterialIndex] = color.redF();
            Gs[ManufacturerMaterialIndex] = color.greenF();
            Bs[ManufacturerMaterialIndex] = color.blueF();
        }
        // 刷新UI
        emit SetRedText(QString("%1").arg(color.red()));
        emit SetGreenText(QString("%1").arg(color.green()));
        emit SetBlueText(QString("%1").arg(color.blue()));
        emit SetColorButton(color);
    }
}

void MaterialParameterGroupBox::OnDiffuseTextureButtonClicked()
{
    auto CurrentTextureFileName = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Images (*.png  *.jpg)"));
    qDebug() << CurrentTextureFileName << MaterialIndex;
    if (Scene)
    {
        DiffuseTextureNameLabel->setText(CurrentTextureFileName.split("/").last());
        Scene->updateTexture(MaterialIndex, CurrentTextureFileName.toStdString().data());
        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatTextures[ManufacturerMaterialIndex] = CurrentTextureFileName.toStdString();
        }
    }
}

void MaterialParameterGroupBox::OnDiffuseTextureClearButtonClicked()
{
    if (Scene)
    {
        if (MaterialIndex > -1)
            Scene->UpdateMaterialTexture(MaterialIndex, -1);
    }
    if (ManufacturerMaterialIndex >= 0)
    {
        ManufacturerMatTextures[ManufacturerMaterialIndex].clear();
    }
    DiffuseTextureNameLabel->setText("未选择");
}

void MaterialParameterGroupBox::OnNormalMapTextureButtonClicked()
{
    auto CurrentNormalMapFileName = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Images (*.png  *.jpg)"));
    if (Scene)
    {
        MtlNormalmapNameLabel->setText(CurrentNormalMapFileName.split("/").last());
        Scene->updateNormalMapTexture(MaterialIndex, CurrentNormalMapFileName.toStdString().data());
        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatNormalMaps[ManufacturerMaterialIndex] = CurrentNormalMapFileName.toStdString();
        }
    }
}

void MaterialParameterGroupBox::OnNormalMapTextureClearButtonClicked()
{
    if (Scene)
    {
        if (MaterialIndex > -1)
            Scene->UpdateMaterialNormalMap(MaterialIndex, -1);

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatNormalMaps[ManufacturerMaterialIndex].clear();
            qDebug() << "OnNormalMapTextureClearButtonClicked" << ManufacturerMatNormalMaps[ManufacturerMaterialIndex];
        }

        MtlNormalmapNameLabel->setText("未选择");
    }
}

void MaterialParameterGroupBox::OnReflectivitySliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Glass)
                Scene->UpdateGlassMaterialReflectivity(MaterialIndex, p.toFloat(nullptr));
            else if (CurrentMaterialType == Rt::MaterialType::Dielectric)
                Scene->UpdateDielectricMaterialReflectivity(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatReflectivitys[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
    emit SetReflectivityText(QString("%1").arg(p));
}

void MaterialParameterGroupBox::OnReflectivityTextFieldAccept()
{
    auto p = (QObject*)ReflectivityEdit->rootObject();
    auto p2 = (QObject*)ReflectivitySlider->rootObject();
    float Reflectivity = p->property("text").toFloat();
    p2->setProperty("value", Reflectivity);
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            /*
                区分玻璃和非金属没写
            */
            if (ManufacturerMatTypes[ManufacturerMaterialIndex] == "玻璃")
                Scene->UpdateGlassMaterialReflectivity(MaterialIndex, Reflectivity);
            else if (ManufacturerMatTypes[ManufacturerMaterialIndex] == "非金属")
                Scene->UpdateDielectricMaterialReflectivity(MaterialIndex, Reflectivity);
        }
        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatReflectivitys[ManufacturerMaterialIndex] = Reflectivity;
        }
    }
}

void MaterialParameterGroupBox::OnTransmissivitySliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            Scene->UpdateMaterialTransmissivity(MaterialIndex, p.toFloat(nullptr));
        }
        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatTransmissivitys[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
    emit SetTransmissivityText(QString("%1").arg(p));
}

void MaterialParameterGroupBox::OnTransmissivityTextFieldAccept()
{
    auto p = (QObject*)TransmissivityEdit->rootObject();
    auto p2 = (QObject*)TransmissivitySlider->rootObject();
    float Transmissivity = p->property("text").toFloat();
    p2->setProperty("value", Transmissivity);
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            Scene->UpdateMaterialTransmissivity(MaterialIndex, Transmissivity);
        }
        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatTransmissivitys[ManufacturerMaterialIndex] = Transmissivity;
        }
    }
}

void MaterialParameterGroupBox::OnIndexOfRefractionSliderMoved(QString p)
{
    if (Scene)
    {
        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatIORs[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
    emit SetIndexOfRefractionText(QString("%1").arg(p));
}

void MaterialParameterGroupBox::OnIndexOfRefractionTextFieldAccept()
{
    qDebug() << __FUNCTION__;
    auto p = (QObject*)IndexOfRefractionEdit->rootObject();
    auto p2 = (QObject*)IndexOfRefractionSlider->rootObject();
    float IndexOfRefraction = p->property("text").toFloat();
    p2->setProperty("value", IndexOfRefraction);
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            
        }
        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatIORs[ManufacturerMaterialIndex] = IndexOfRefraction;
        }
    }
}

void MaterialParameterGroupBox::OnURoughnessSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            Scene->UpdateMaterialURoughness(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatURoughnesss[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
    emit SetURoughnessText(QString("%1").arg(p));
}

void MaterialParameterGroupBox::OnURoughnessTextFieldAccept()
{
    auto p = (QObject*)URoughnessEdit->rootObject();
    auto p2 = (QObject*)URoughnessSlider->rootObject();
    float URoughness = p->property("text").toFloat();
    p2->setProperty("value", URoughness);
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            Scene->UpdateMaterialURoughness(MaterialIndex, URoughness);
        }
        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatURoughnesss[ManufacturerMaterialIndex] = URoughness;
        }
    }
}


void MaterialParameterGroupBox::OnVRoughnessSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            Scene->UpdateMaterialVRoughness(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatVRoughnesss[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
    emit SetVRoughnessText(QString("%1").arg(p));
}

void MaterialParameterGroupBox::OnVRoughnessTextFieldAccept()
{
    auto p  = (QObject*)VRoughnessEdit->rootObject();
    auto p2 = (QObject*)VRoughnessSlider->rootObject();
    float VRoughness = p->property("text").toFloat();
    p2->setProperty("value", VRoughness);
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            Scene->UpdateMaterialVRoughness(MaterialIndex, VRoughness);
        }
        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerMatVRoughnesss[ManufacturerMaterialIndex] = VRoughness;
        }
    }
}


/*
    更新材质参数: Metallic
*/
void MaterialParameterGroupBox::OnMetallicSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                Scene->UpdateDisneyMaterialMetallic(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialMetallic[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
   // emit SetReflectivityText(QString("%1").arg(p));
}

/*
    更新材质参数: Roughness
*/
void MaterialParameterGroupBox::OnRoughnessSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                Scene->UpdateDisneyMaterialRoughness(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialRoughness[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
}

/*
    更新材质参数: Subsurface
*/
void MaterialParameterGroupBox::OnSubsurfaceSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                Scene->UpdateDisneyMaterialSubsurface(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialSubsurface[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
}

/*
    更新材质参数: Specular
*/
void MaterialParameterGroupBox::OnSpecularSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                Scene->UpdateDisneyMaterialSpecular(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialSpecular[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
}

/*
    更新材质参数: SpecularTint
*/
void MaterialParameterGroupBox::OnSpecularTintSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                Scene->UpdateDisneyMaterialSpecularTint(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialSpecularTint[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
}

/*
    更新材质参数: Anisotropy
*/
void MaterialParameterGroupBox::OnAnisotropySliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                Scene->UpdateDisneyMaterialAnisotropy(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialAnisotropy[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
}

/*
    更新材质参数: Sheen
*/
void MaterialParameterGroupBox::OnSheenSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                Scene->UpdateDisneyMaterialSheen(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialSheen[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
}

/*
    更新材质参数: SheenTint
*/
void MaterialParameterGroupBox::OnSheenTintSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                Scene->UpdateDisneyMaterialSheenTint(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialSheenTint[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
}

/*
    更新材质参数: Clearcoat
*/
void MaterialParameterGroupBox::OnClearcoatSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                Scene->UpdateDisneyMaterialClearcoat(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialClearcoat[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
}

/*
    更新材质参数: ClearcoatGloss
*/
void MaterialParameterGroupBox::OnClearcoatGlossSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                Scene->UpdateDisneyMaterialClearcoatGloss(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialClearcoatGloss[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
}

/*
    更新材质参数: IOR
*/
void MaterialParameterGroupBox::OnIORSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                //Scene->UpdateDisneyMaterialIOR(MaterialIndex, p.toFloat(nullptr));
                Scene->UpdateDisneyMaterialIOR(MaterialIndex, 1.45);
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialIOR[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
}

/*
    更新材质参数: Transmission
*/
void MaterialParameterGroupBox::OnTransmissionSliderMoved(QString p)
{
    if (Scene)
    {
        if (MaterialIndex >= 0)
        {
            if (CurrentMaterialType == Rt::MaterialType::Disney)
                Scene->UpdateDisneyMaterialIOR(MaterialIndex, p.toFloat(nullptr));
        }

        if (ManufacturerMaterialIndex >= 0)
        {
            ManufacturerDisneyMaterialIOR[ManufacturerMaterialIndex] = p.toFloat();
        }
    }
}

void MaterialParameterGroupBox::OnDatabaseSaveButtonClicked()
{
    if (!Scene)
        return;
    //if (ManufacturerMaterialIndex < 0)
    //    return;
    qDebug() << "OnDatabaseSaveButtonClicked " << ManufacturerMatNames.size();
    for (int i = 0; i < ManufacturerMatNames.size(); i++)
    {
        std::string sql;
        std::string sql2 = "delete from ";
        std::string tableName;
        if (ManufacturerMatTypes[i] == "玻璃")
        {
            tableName = "GlassParameter";
            sql = "insert into " + tableName + " Values(\"" +
                ManufacturerMatNames[i] + "\", " +
                "\"" + ManufacturerMatNameMapeds[i] + "\", " +
                to_string(ManufacturerMatReflectivitys[i]) + "," +
                to_string(ManufacturerMatTransmissivitys[i]) + "," +
                "\"" + to_string(Rs[i]) + "  " + to_string(Gs[i]) + "  " + to_string(Bs[i]) + "\", " +
                "\"" + QString::fromStdString(ManufacturerMatTextures[i]).split(u'/').last().toStdString() + "\", " +
                "\"" + QString::fromStdString(ManufacturerMatNormalMaps[i]).split(u'/').last().toStdString() + "\", " +
                "\"" + ManufacturerMatFirstClasss[i] + "\");";
        }
        else if (ManufacturerMatTypes[i] == "金属")
        {
            tableName = "ConductorParameter";
            sql = "insert into " + tableName + " Values(\"" +
                ManufacturerMatNames[i] + "\", " +
                "\"" + ManufacturerMatNameMapeds[i] + "\", " +
                to_string(ManufacturerMatURoughnesss[i]) + "," +
                to_string(ManufacturerMatVRoughnesss[i]) + "," +
                "\"" + to_string(Rs[i]) + "  " + to_string(Gs[i]) + "  " + to_string(Bs[i]) + "\", " +
                "\"" + QString::fromStdString(ManufacturerMatTextures[i]).split(u'/').last().toStdString() + "\", " +
                "\"" + QString::fromStdString(ManufacturerMatNormalMaps[i]).split(u'/').last().toStdString() + "\", " +
                "\"" + ManufacturerMatFirstClasss[i] + "\");";
        }
        else if (ManufacturerMatTypes[i] == "非金属")
        {
            tableName = "DielectricParameter";
            sql = "insert into " + tableName + " Values(\"" +
                ManufacturerMatNames[i] + "\", " +
                "\"" + ManufacturerMatNameMapeds[i] + "\", " +
                to_string(ManufacturerMatURoughnesss[i]) + "," +
                to_string(ManufacturerMatVRoughnesss[i]) + "," +
                to_string(ManufacturerMatReflectivitys[i]) + "," +
                "\"" + to_string(Rs[i]) + "  " + to_string(Gs[i]) + "  " + to_string(Bs[i]) + "\", " +
                "\"" + QString::fromStdString(ManufacturerMatTextures[i]).split(u'/').last().toStdString() + "\", " +
                "\"" + QString::fromStdString(ManufacturerMatNormalMaps[i]).split(u'/').last().toStdString() + "\", " +
                "\"" + ManufacturerMatFirstClasss[i] + "\");";
        }

        QString dstTextureFileName = "material/textures/" + QString::fromStdString(ManufacturerMatTextures[i]).split(u'/').last();
        QFile::copy(QString::fromStdString(ManufacturerMatTextures[i]), dstTextureFileName);

        QString dstNormalMapFileName = "material/textures/" + QString::fromStdString(ManufacturerMatNormalMaps[i]).split(u'/').last();
        QFile::copy(QString::fromStdString(ManufacturerMatNormalMaps[i]), dstNormalMapFileName);

        sql2 += tableName + " " + "WHERE \"Material Name\"=" + " \"" + ManufacturerMatNames[i] + "\";";

       // qDebug() << sql2;
       // qDebug() << "\n\n";
       // qDebug() << sql;

#if 1
        char* errmsg;
        int ret;
        sql2 = std::string("delete from GlassParameter WHERE \"Material Name Mapped\"=") + " \"" + ManufacturerMatNameMapeds[i] + "\";";
        ret = sqlite3_exec(db, sql2.data(), nullptr, nullptr, &errmsg);
        qDebug() << "delete" << ret << errmsg;

        sql2 = std::string("delete from ConductorParameter WHERE \"Material Name Mapped\"=") + " \"" + ManufacturerMatNameMapeds[i] + "\";";
        ret = sqlite3_exec(db, sql2.data(), nullptr, nullptr, &errmsg);
        qDebug() << "delete" << ret << errmsg;

        sql2 = std::string("delete from DielectricParameter WHERE \"Material Name Mapped\"=") + " \"" + ManufacturerMatNameMapeds[i] + "\";";
        ret = sqlite3_exec(db, sql2.data(), nullptr, nullptr, &errmsg);
        qDebug() << "delete" << ret << errmsg;

        ret = sqlite3_exec(db, sql.data(), nullptr, nullptr, &errmsg);
        qDebug() << "sqlite3_exec" << ret << errmsg;
#endif
    }
}


void MaterialParameterGroupBox::CreateMtlNameLayout()
{
    MtlNameLabel = new QLabel(QStringLiteral("材质名称"));
    MtlName2Label = new QLabel(QStringLiteral("xxxx"));
    MtlNameLayout = new QHBoxLayout;
    MtlNameLayout->addWidget(MtlNameLabel);
    MtlNameLayout->addStretch(1);
    MtlNameLayout->addWidget(MtlName2Label);
    MtlParamGroupBoxLayout->addLayout(MtlNameLayout);
}

void MaterialParameterGroupBox::CreateMtlTypeLayout()
{
    MtlTypeLabel = new QLabel(QStringLiteral("材质类型"));

    MtlTypeComboBox = new QComboBox;
    MtlTypeComboBox->addItem(QStringLiteral("金属"));
    MtlTypeComboBox->addItem(QStringLiteral("非金属"));
    MtlTypeComboBox->addItem(QStringLiteral("玻璃"));
    MtlTypeComboBox->addItem(QStringLiteral("Disney"));

    MtlTypeLayout = new QHBoxLayout;
    MtlTypeLayout->addWidget(MtlTypeLabel);
    MtlTypeLayout->addStretch(1);
    MtlTypeLayout->addWidget(MtlTypeComboBox);
    MtlParamGroupBoxLayout->addLayout(MtlTypeLayout);
}

void MaterialParameterGroupBox::CreateMtlAlbedoLayout()
{
    MtlAlbedoLabel = new QLabel(QStringLiteral("颜色"));
    MtlAlbedoRedLabel = new QLabel(QStringLiteral("R"));
    MtlAlbedoGreenLabel = new QLabel(QStringLiteral("G"));
    MtlAlbedoBlueLabel = new QLabel(QStringLiteral("B"));

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

    MtlAlbedoLayout = new QHBoxLayout;
    MtlAlbedoLayout->addWidget(MtlAlbedoLabel);
    MtlAlbedoLayout->addWidget(MtlAlbedoRedLabel);
    MtlAlbedoLayout->addWidget(RedEdit);
    MtlAlbedoLayout->addWidget(MtlAlbedoGreenLabel);
    MtlAlbedoLayout->addWidget(GreenEdit);
    MtlAlbedoLayout->addWidget(MtlAlbedoBlueLabel);
    MtlAlbedoLayout->addWidget(BlueEdit);
    MtlAlbedoLayout->addWidget(ColorButton);
    MtlParamGroupBoxLayout->addLayout(MtlAlbedoLayout);
}

void MaterialParameterGroupBox::CreateMtlTextureLayout()
{
    DiffuseTexLabel = new QLabel(QStringLiteral("贴图"));


    QUrl ButtonSource("qrc:Resource/QML/Button.qml");
    QUrl ClearButtonSource("qrc:Resource/QML/ClearButton.qml");

    DiffuseTextureButton = new QQuickWidget(this);
    DiffuseTextureButton->setFixedSize(30, 30);
    DiffuseTextureButton->setResizeMode(QQuickWidget::SizeRootObjectToView);
    DiffuseTextureButton->setSource(ButtonSource);

    DiffuseTextureClearButton = new QQuickWidget(this);
    DiffuseTextureClearButton->setFixedSize(30, 30);
    DiffuseTextureClearButton->setResizeMode(QQuickWidget::SizeRootObjectToView);
    DiffuseTextureClearButton->setSource(ClearButtonSource);

    // 创建一个标签来显示选中的贴图名称
    DiffuseTextureNameLabel = new QLabel(QStringLiteral("未选择"));
    DiffuseTextureNameLabel->setFixedWidth(150);
    DiffuseTextureNameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    MtlTextureLayout = new QHBoxLayout;
    MtlTextureLayout->addWidget(DiffuseTexLabel);
    MtlTextureLayout->addWidget(DiffuseTextureNameLabel);
    MtlTextureLayout->addStretch(1);
    MtlTextureLayout->addWidget(DiffuseTextureClearButton);
    MtlTextureLayout->addWidget(DiffuseTextureButton);

    MtlParamGroupBoxLayout->addLayout(MtlTextureLayout);
}

void MaterialParameterGroupBox::CreateMtlNormalMapLayout()
{
    MtlNormalmapTexLabel = new QLabel(QStringLiteral("法线贴图"));
    MtlNormalmapNameLabel = new QLabel(QStringLiteral("未选择"));

    QUrl ButtonSource("qrc:Resource/QML/Button.qml");
    QUrl ClearButtonSource("qrc:Resource/QML/ClearButton.qml");

    NormalTextureButton = new QQuickWidget(this);
    NormalTextureButton->setFixedSize(30, 30);
    NormalTextureButton->setResizeMode(QQuickWidget::SizeRootObjectToView);
    NormalTextureButton->setSource(ButtonSource);

    NormalTextureClearButton = new QQuickWidget(this);
    NormalTextureClearButton->setFixedSize(30, 30);
    NormalTextureClearButton->setResizeMode(QQuickWidget::SizeRootObjectToView);
    NormalTextureClearButton->setSource(ClearButtonSource);

    MtlNormalmapLayout = new QHBoxLayout;
    MtlNormalmapLayout->addWidget(MtlNormalmapTexLabel);
    MtlNormalmapLayout->addWidget(MtlNormalmapNameLabel);
    MtlNormalmapLayout->addStretch(1);
    MtlNormalmapLayout->addWidget(NormalTextureClearButton);
    MtlNormalmapLayout->addWidget(NormalTextureButton);

    MtlParamGroupBoxLayout->addLayout(MtlNormalmapLayout);
}

void MaterialParameterGroupBox::CreateReflectivityLayout()
{
    ReflectivityLabel = new QLabel(QStringLiteral("反射率: "));

    QUrl Source("qrc:Resource/QML/LineEdit.qml");
    ReflectivityEdit = new QQuickWidget(this);
    ReflectivityEdit->setFixedWidth(50);
    ReflectivityEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ReflectivityEdit->setSource(Source);

    QUrl Source2("qrc:Resource/QML/Slider.qml");
    ReflectivitySlider = new QQuickWidget(this);
    ReflectivitySlider->setMinimumHeight(30);
    ReflectivitySlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ReflectivitySlider->setSource(Source2);

    ReflectivityLayout = new QHBoxLayout;
    ReflectivityLayout->addWidget(ReflectivityLabel);
    ReflectivityLayout->addStretch(1);
    ReflectivityLayout->addWidget(ReflectivityEdit);
    MtlParamGroupBoxLayout->addLayout(ReflectivityLayout);
    MtlParamGroupBoxLayout->addWidget(ReflectivitySlider);
}

void MaterialParameterGroupBox::CreateTransmissivityLayout()
{
    TransmissivityLabel = new QLabel(QStringLiteral("透射率: "));

    QUrl Source("qrc:Resource/QML/LineEdit.qml");

    TransmissivityEdit = new QQuickWidget(this);
    TransmissivityEdit->setFixedWidth(50);
    TransmissivityEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    TransmissivityEdit->setSource(Source);

    /*
        透射率Slider
    */
    QUrl Source1("qrc:Resource/QML/Slider.qml");
    TransmissivitySlider = new QQuickWidget(this);
    TransmissivitySlider->setMinimumHeight(30);
    TransmissivitySlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    TransmissivitySlider->setSource(Source1);

    TransmissivityLayout = new QHBoxLayout;
    TransmissivityLayout->addWidget(TransmissivityLabel);
    TransmissivityLayout->addStretch(1);
    TransmissivityLayout->addWidget(TransmissivityEdit);
    MtlParamGroupBoxLayout->addLayout(TransmissivityLayout);
    MtlParamGroupBoxLayout->addWidget(TransmissivitySlider);
}

void MaterialParameterGroupBox::CreateIndexOfRefractionLayout()
{
    IndexOfRefractionLabel = new QLabel(QStringLiteral("折射率: "));

    QUrl Source("qrc:Resource/QML/LineEdit.qml");

    IndexOfRefractionEdit = new QQuickWidget(this);
    IndexOfRefractionEdit->setFixedWidth(50);
    IndexOfRefractionEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    IndexOfRefractionEdit->setSource(Source);

    /*
        折射率Slider
    */
    QUrl Source1("qrc:Resource/QML/Slider.qml");
    IndexOfRefractionSlider = new QQuickWidget(this);
    IndexOfRefractionSlider->setMinimumHeight(30);
    IndexOfRefractionSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    IndexOfRefractionSlider->setSource(Source1);

    IndexOfRefractionLayout = new QHBoxLayout;
    IndexOfRefractionLayout->addWidget(IndexOfRefractionLabel);
    IndexOfRefractionLayout->addStretch(1);
    IndexOfRefractionLayout->addWidget(IndexOfRefractionEdit);
    MtlParamGroupBoxLayout->addLayout(IndexOfRefractionLayout);
    MtlParamGroupBoxLayout->addWidget(IndexOfRefractionSlider);
}

void MaterialParameterGroupBox::CreateURoughnessLayout()
{
    URoughnessLabel = new QLabel(QStringLiteral("U粗糙度: "));

    QUrl Source("qrc:Resource/QML/LineEdit.qml");
    URoughnessEdit = new QQuickWidget(this);
    URoughnessEdit->setFixedWidth(50);
    URoughnessEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    URoughnessEdit->setSource(Source);

    /*
        粗糙度Slider
    */
    QUrl Source1("qrc:Resource/QML/Slider.qml");
    URoughnessSlider = new QQuickWidget(this);
    URoughnessSlider->setMinimumHeight(30);
    URoughnessSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    URoughnessSlider->setSource(Source1);

    URoughnessLayout = new QHBoxLayout;
    URoughnessLayout->addWidget(URoughnessLabel);
    URoughnessLayout->addStretch(1);
    URoughnessLayout->addWidget(URoughnessEdit);
    MtlParamGroupBoxLayout->addLayout(URoughnessLayout);
    MtlParamGroupBoxLayout->addWidget(URoughnessSlider);
}

void MaterialParameterGroupBox::CreateVRoughnessLayout()
{
    VRoughnessLabel = new QLabel(QStringLiteral("V粗糙度: "));

    QUrl Source("qrc:Resource/QML/LineEdit.qml");
    VRoughnessEdit = new QQuickWidget(this);
    VRoughnessEdit->setFixedWidth(50);
    VRoughnessEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    VRoughnessEdit->setSource(Source);

    /*
        粗糙度Slider
    */
    QUrl Source1("qrc:Resource/QML/Slider.qml");
    VRoughnessSlider = new QQuickWidget(this);
    VRoughnessSlider->setMinimumHeight(30);
    VRoughnessSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    VRoughnessSlider->setSource(Source1);

    VRoughnessLayout = new QHBoxLayout;
    VRoughnessLayout->addWidget(VRoughnessLabel);
    VRoughnessLayout->addStretch(1);
    VRoughnessLayout->addWidget(VRoughnessEdit);
    MtlParamGroupBoxLayout->addLayout(VRoughnessLayout);
    MtlParamGroupBoxLayout->addWidget(VRoughnessSlider);
}

void MaterialParameterGroupBox::CreateDatabaseSaveLayout()
{
    DatabaseSaveLabel = new QLabel(QStringLiteral("保存到数据库"));

    QUrl ButtonSource("qrc:Resource/QML/Button.qml");

    DatabaseSaveButton = new QQuickWidget(this);
    DatabaseSaveButton->setFixedSize(30, 30);
    DatabaseSaveButton->setResizeMode(QQuickWidget::SizeRootObjectToView);
    DatabaseSaveButton->setSource(ButtonSource);

    DatabaseSaveLayout = new QHBoxLayout;
    DatabaseSaveLayout->addWidget(DatabaseSaveLabel);
    DatabaseSaveLayout->addWidget(DatabaseSaveButton);

    MtlParamGroupBoxLayout->addLayout(DatabaseSaveLayout);
}

void MaterialParameterGroupBox::CreateDisneyMaterialParameterLayout()
{    
    
    QUrl Source("qrc:Resource/QML/LineEdit.qml");
    QUrl Source1("qrc:Resource/QML/Slider.qml");

    DisneyMaterialParameterLayout = new QVBoxLayout;

    /*
        金属度
    */
    MetallicEdit = new QQuickWidget(this);
    MetallicEdit->setFixedWidth(50);
    MetallicEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    MetallicEdit->setSource(Source);

    MetallicSlider = new QQuickWidget(this);
    MetallicSlider->setMinimumHeight(30);
    MetallicSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    MetallicSlider->setSource(Source1);

    MetallicLabel = new QLabel(QStringLiteral("金属度: "));

    MetallicLayout = new QHBoxLayout;
    MetallicLayout->addWidget(MetallicLabel);
    MetallicLayout->addStretch(1);
    MetallicLayout->addWidget(MetallicEdit);

    DisneyMaterialParameterLayout->addLayout(MetallicLayout);
    DisneyMaterialParameterLayout->addWidget(MetallicSlider);

    /*
        粗糙度
    */
    RoughnessEdit = new QQuickWidget(this);
    RoughnessEdit->setFixedWidth(50);
    RoughnessEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    RoughnessEdit->setSource(Source);

    RoughnessSlider = new QQuickWidget(this);
    RoughnessSlider->setMinimumHeight(30);
    RoughnessSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    RoughnessSlider->setSource(Source1);

    RoughnessLabel = new QLabel(QStringLiteral("粗糙度: "));

    RoughnessLayout = new QHBoxLayout;
    RoughnessLayout->addWidget(RoughnessLabel);
    RoughnessLayout->addStretch(1);
    RoughnessLayout->addWidget(RoughnessEdit);

    DisneyMaterialParameterLayout->addLayout(RoughnessLayout);
    DisneyMaterialParameterLayout->addWidget(RoughnessSlider);

    /*
        次表面散射
    */
    SubsurfaceEdit = new QQuickWidget(this);
    SubsurfaceEdit->setFixedWidth(50);
    SubsurfaceEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    SubsurfaceEdit->setSource(Source);

    SubsurfaceSlider = new QQuickWidget(this);
    SubsurfaceSlider->setMinimumHeight(30);
    SubsurfaceSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    SubsurfaceSlider->setSource(Source1);

    SubsurfaceLabel = new QLabel(QStringLiteral("次表面散射: "));

    SubsurfaceLayout = new QHBoxLayout;
    SubsurfaceLayout->addWidget(SubsurfaceLabel);
    SubsurfaceLayout->addStretch(1);
    SubsurfaceLayout->addWidget(SubsurfaceEdit);

    DisneyMaterialParameterLayout->addLayout(SubsurfaceLayout);
    DisneyMaterialParameterLayout->addWidget(SubsurfaceSlider);

    /*
        高光强度
    */
    SpecularEdit = new QQuickWidget(this);
    SpecularEdit->setFixedWidth(50);
    SpecularEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    SpecularEdit->setSource(Source);

    SpecularSlider = new QQuickWidget(this);
    SpecularSlider->setMinimumHeight(30);
    SpecularSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    SpecularSlider->setSource(Source1);

    SpecularLabel = new QLabel(QStringLiteral("高光强度: "));

    SpecularLayout = new QHBoxLayout;
    SpecularLayout->addWidget(SpecularLabel);
    SpecularLayout->addStretch(1);
    SpecularLayout->addWidget(SpecularEdit);

    DisneyMaterialParameterLayout->addLayout(SpecularLayout);
    DisneyMaterialParameterLayout->addWidget(SpecularSlider);

    /*
        SpecularTint
    */
    SpecularTintEdit = new QQuickWidget(this);
    SpecularTintEdit->setFixedWidth(50);
    SpecularTintEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    SpecularTintEdit->setSource(Source);

    SpecularTintSlider = new QQuickWidget(this);
    SpecularTintSlider->setMinimumHeight(30);
    SpecularTintSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    SpecularTintSlider->setSource(Source1);

    SpecularTintLabel = new QLabel(QStringLiteral("SpecularTint: "));

    SpecularTintLayout = new QHBoxLayout;
    SpecularTintLayout->addWidget(SpecularTintLabel);
    SpecularTintLayout->addStretch(1);
    SpecularTintLayout->addWidget(SpecularTintEdit);

    DisneyMaterialParameterLayout->addLayout(SpecularTintLayout);
    DisneyMaterialParameterLayout->addWidget(SpecularTintSlider);

    /*
        Anisotropy
    */
    AnisotropyEdit = new QQuickWidget(this);
    AnisotropyEdit->setFixedWidth(50);
    AnisotropyEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    AnisotropyEdit->setSource(Source);

    AnisotropySlider = new QQuickWidget(this);
    AnisotropySlider->setMinimumHeight(30);
    AnisotropySlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    AnisotropySlider->setSource(Source1);

    AnisotropyLabel = new QLabel(QStringLiteral("各向异性: "));

    AnisotropyLayout = new QHBoxLayout;
    AnisotropyLayout->addWidget(AnisotropyLabel);
    AnisotropyLayout->addStretch(1);
    AnisotropyLayout->addWidget(AnisotropyEdit);

    DisneyMaterialParameterLayout->addLayout(AnisotropyLayout);
    DisneyMaterialParameterLayout->addWidget(AnisotropySlider);

    /*
        Sheen
    */
    SheenEdit = new QQuickWidget(this);
    SheenEdit->setFixedWidth(50);
    SheenEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    SheenEdit->setSource(Source);

    SheenSlider = new QQuickWidget(this);
    SheenSlider->setMinimumHeight(30);
    SheenSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    SheenSlider->setSource(Source1);

    SheenLabel = new QLabel(QStringLiteral("Sheen: "));

    SheenLayout = new QHBoxLayout;
    SheenLayout->addWidget(SheenLabel);
    SheenLayout->addStretch(1);
    SheenLayout->addWidget(SheenEdit);

    DisneyMaterialParameterLayout->addLayout(SheenLayout);
    DisneyMaterialParameterLayout->addWidget(SheenSlider);

    /*
        Sheen
    */
    SheenTintEdit = new QQuickWidget(this);
    SheenTintEdit->setFixedWidth(50);
    SheenTintEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    SheenTintEdit->setSource(Source);

    SheenTintSlider = new QQuickWidget(this);
    SheenTintSlider->setMinimumHeight(30);
    SheenTintSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    SheenTintSlider->setSource(Source1);

    SheenTintLabel = new QLabel(QStringLiteral("SheenTint: "));

    SheenTintLayout = new QHBoxLayout;
    SheenTintLayout->addWidget(SheenTintLabel);
    SheenTintLayout->addStretch(1);
    SheenTintLayout->addWidget(SheenTintEdit);

    DisneyMaterialParameterLayout->addLayout(SheenTintLayout);
    DisneyMaterialParameterLayout->addWidget(SheenTintSlider);

    /*
        Clearcoat
    */
    ClearcoatEdit = new QQuickWidget(this);
    ClearcoatEdit->setFixedWidth(50);
    ClearcoatEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ClearcoatEdit->setSource(Source);

    ClearcoatSlider = new QQuickWidget(this);
    ClearcoatSlider->setMinimumHeight(30);
    ClearcoatSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ClearcoatSlider->setSource(Source1);

    ClearcoatLabel = new QLabel(QStringLiteral("Clearcoat: "));

    ClearcoatLayout = new QHBoxLayout;
    ClearcoatLayout->addWidget(ClearcoatLabel);
    ClearcoatLayout->addStretch(1);
    ClearcoatLayout->addWidget(ClearcoatEdit);

    DisneyMaterialParameterLayout->addLayout(ClearcoatLayout);
    DisneyMaterialParameterLayout->addWidget(ClearcoatSlider);

    /*
        ClearcoatGloss
    */
    ClearcoatGlossEdit = new QQuickWidget(this);
    ClearcoatGlossEdit->setFixedWidth(50);
    ClearcoatGlossEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ClearcoatGlossEdit->setSource(Source);

    ClearcoatGlossSlider = new QQuickWidget(this);
    ClearcoatGlossSlider->setMinimumHeight(30);
    ClearcoatGlossSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ClearcoatGlossSlider->setSource(Source1);

    ClearcoatGlossLabel = new QLabel(QStringLiteral("ClearcoatGloss: "));

    ClearcoatGlossLayout = new QHBoxLayout;
    ClearcoatGlossLayout->addWidget(ClearcoatGlossLabel);
    ClearcoatGlossLayout->addStretch(1);
    ClearcoatGlossLayout->addWidget(ClearcoatGlossEdit);

    DisneyMaterialParameterLayout->addLayout(ClearcoatGlossLayout);
    DisneyMaterialParameterLayout->addWidget(ClearcoatGlossSlider);

    /*
        IOR
    */
    IOREdit = new QQuickWidget(this);
    IOREdit->setFixedWidth(50);
    IOREdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    IOREdit->setSource(Source);

    IORSlider = new QQuickWidget(this);
    IORSlider->setMinimumHeight(30);
    IORSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    IORSlider->setSource(Source1);

    IORLabel = new QLabel(QStringLiteral("IOR: "));

    IORLayout = new QHBoxLayout;
    IORLayout->addWidget(IORLabel);
    IORLayout->addStretch(1);
    IORLayout->addWidget(IOREdit);

    DisneyMaterialParameterLayout->addLayout(IORLayout);
    DisneyMaterialParameterLayout->addWidget(IORSlider);

    /*
        Transmission
    */
    TransmissionEdit = new QQuickWidget(this);
    TransmissionEdit->setFixedWidth(50);
    TransmissionEdit->setResizeMode(QQuickWidget::SizeRootObjectToView);
    TransmissionEdit->setSource(Source);

    TransmissionSlider = new QQuickWidget(this);
    TransmissionSlider->setMinimumHeight(30);
    TransmissionSlider->setResizeMode(QQuickWidget::SizeRootObjectToView);
    TransmissionSlider->setSource(Source1);

    TransmissionLabel = new QLabel(QStringLiteral("Transmission: "));

    TransmissionLayout = new QHBoxLayout;
    TransmissionLayout->addWidget(TransmissionLabel);
    TransmissionLayout->addStretch(1);
    TransmissionLayout->addWidget(TransmissionEdit);

    DisneyMaterialParameterLayout->addLayout(TransmissionLayout);
    DisneyMaterialParameterLayout->addWidget(TransmissionSlider);


    MtlParamGroupBoxLayout->addLayout(DisneyMaterialParameterLayout);
}

void MaterialParameterGroupBox::ClearManufacturerMaterialList()
{
    ManufacturerMatNames.clear();
    ManufacturerMatNameMapeds.clear();
    ManufacturerMatFirstClasss.clear();
    Rs.clear();
    Gs.clear();
    Bs.clear();
    ManufacturerMatTypes.clear();
    ManufacturerMatTextures.clear();
    ManufacturerMatNormalMaps.clear();
    ManufacturerMatReflectivitys.clear();
    ManufacturerMatTransmissivitys.clear();
    ManufacturerMatURoughnesss.clear();
    ManufacturerMatVRoughnesss.clear();
    ManufacturerMatIORs.clear();
    ManufacturerMaterialIndex = -1;
}
#endif