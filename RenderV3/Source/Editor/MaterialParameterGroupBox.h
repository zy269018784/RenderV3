#ifdef USE_QT
#pragma once
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QQuickWidget>
#include <QComboBox>
#include <Scene/Material.h>
#include <sqlite3.h>
#include <string>
class GlfwRender;
using namespace std;
class MaterialParameterGroupBox : public QGroupBox
{
	Q_OBJECT
public:
	MaterialParameterGroupBox(QWidget* parent = nullptr);
    void SetScene(GlfwRender* Scene1);
    void SetMaterialIndex(int Index1);
    /*
        设置当前 厂家材质名, 材质映射名, 材质一级分类
    */
    void AddManufacturerMaterial(string Name, string NameMapping, string FirstClass,
        string Type,
        float R, float G, float B,
        string Texture, string NormalMap,
        float Reflectivity, float Transmissivity,
        float URoughness, float VRoughness, float IOR);

    void ClearManufacturerMaterialList();
public slots:
    void SetManufacturerMaterialIndex(int index);
    /*
        PBR参数导出到XLSX
    */
    void ExportPBRToXLSL();
    
    void TemporaryStorePBRToXLSL();
private:
    void BindUISignal();
signals:
    /*
        刷新UI信号
    */
    void SetWidthLabelText(QString str);
    void SetHeightLabelText(QString str);
    void SetRedText(QString str);
    void SetGreenText(QString str);
    void SetBlueText(QString str);
    void SetReflectivityText(QString str);
    void SetURoughnessText(QString str);
    void SetVRoughnessText(QString str);
    void SetTransmissivityText(QString str);
    void SetIndexOfRefractionText(QString str);
    void SetHDRText(QString str);
    void SetColorButton(QColor c);


public slots:
    /*
        更改材质类型
    */
    void OnCurrentIndexChanged(int index);
    /*
        更换RGB
    */
    void OnColorButtonClicked();
    /*
        更换纹理贴图
    */
    void OnDiffuseTextureButtonClicked();
    /*
        清空纹理贴图
    */
    void OnDiffuseTextureClearButtonClicked();
    /*
        更换法线贴图
    */
    void OnNormalMapTextureButtonClicked();
    /*
        清空法线贴图
    */
    void OnNormalMapTextureClearButtonClicked();
    /*
        更新材质参数: 反射率
    */
    void OnReflectivitySliderMoved(QString p);

    void OnReflectivityTextFieldAccept();
    /*
        更新材质参数: 透过率
    */
    void OnTransmissivitySliderMoved(QString p);

    void OnTransmissivityTextFieldAccept();
    /*
        更新材质参数: 折射率
    */
    void OnIndexOfRefractionSliderMoved(QString p);

    void OnIndexOfRefractionTextFieldAccept();
    /*
        更新材质参数URoughness
    */
    void OnURoughnessSliderMoved(QString p);

    void OnURoughnessTextFieldAccept();
    /*
        更新材质参数VRoughness
    */
    void OnVRoughnessSliderMoved(QString p);

    void OnVRoughnessTextFieldAccept();

    /*
        更新材质参数: Metallic
    */
    void OnMetallicSliderMoved(QString p);

    /*
        更新材质参数: Roughness
    */
    void OnRoughnessSliderMoved(QString p);

    /*
        更新材质参数: Subsurface
    */
    void OnSubsurfaceSliderMoved(QString p);

    /*
        更新材质参数: Specular
    */
    void OnSpecularSliderMoved(QString p);

    /*
        更新材质参数: SpecularTint
    */
    void OnSpecularTintSliderMoved(QString p);

    /*
        更新材质参数: Anisotropy
    */
    void OnAnisotropySliderMoved(QString p);

    /*
        更新材质参数: Sheen
    */
    void OnSheenSliderMoved(QString p);

    /*
        更新材质参数: SheenTint
    */
    void OnSheenTintSliderMoved(QString p);

    /*
        更新材质参数: Clearcoat
    */
    void OnClearcoatSliderMoved(QString p);

    /*
        更新材质参数: ClearcoatGloss
    */
    void OnClearcoatGlossSliderMoved(QString p);

    /*
        更新材质参数: IOR
    */
    void OnIORSliderMoved(QString p);

    /*
        更新材质参数: Transmission
    */
    void OnTransmissionSliderMoved(QString p);

    /*
        保存材质参数到数据库
    */
    void OnDatabaseSaveButtonClicked();
private:
    /*
        创建UI
    */
    void CreateMtlNameLayout();
    void CreateMtlTypeLayout();
    void CreateMtlAlbedoLayout();
    void CreateMtlTextureLayout();
    void CreateMtlNormalMapLayout();
    void CreateReflectivityLayout();
    void CreateTransmissivityLayout();
    void CreateIndexOfRefractionLayout();
    void CreateURoughnessLayout();
    void CreateVRoughnessLayout();
    void CreateDatabaseSaveLayout();
    void CreateDisneyMaterialParameterLayout();
private:
    QVBoxLayout* MtlParamGroupBoxLayout;
    /*
        材质名称
    */
    QHBoxLayout* MtlNameLayout;
    QLabel* MtlNameLabel;
    QLabel* MtlName2Label;
    /*
        材质类型
    */
    QHBoxLayout* MtlTypeLayout;
    QLabel*      MtlTypeLabel;
    QComboBox*   MtlTypeComboBox;
    /*
        Albedo
    */
    QHBoxLayout* MtlAlbedoLayout;
    QLabel* MtlAlbedoLabel;
    QLabel* MtlAlbedoRedLabel;
    QLabel* MtlAlbedoGreenLabel;
    QLabel* MtlAlbedoBlueLabel;
    QQuickWidget* RedEdit;
    QQuickWidget* GreenEdit;
    QQuickWidget* BlueEdit;
    QQuickWidget* ColorButton;
    /*
        Texture Layout
    */
    QHBoxLayout* MtlTextureLayout;
    QLabel* DiffuseTexLabel;
    QLabel* DiffuseTextureNameLabel;
    QQuickWidget* DiffuseTextureButton;
    QQuickWidget* DiffuseTextureClearButton;
    /*
        Normalmap Layout
    */
    QHBoxLayout* MtlNormalmapLayout;
    QLabel* MtlNormalmapTexLabel;
    QLabel* MtlNormalmapNameLabel;
    QQuickWidget* NormalTextureClearButton;
    QQuickWidget* NormalTextureButton;
    /*
        反射率
    */
    QHBoxLayout* ReflectivityLayout;
    QLabel* ReflectivityLabel;
    QQuickWidget* ReflectivityEdit;
    QQuickWidget* ReflectivitySlider;
    /*
        透过率
    */
    QHBoxLayout* TransmissivityLayout;
    QLabel* TransmissivityLabel;
    QQuickWidget* TransmissivityEdit;
    QQuickWidget* TransmissivitySlider;

    /*
        折射率
    */
    QHBoxLayout* IndexOfRefractionLayout;
    QLabel* IndexOfRefractionLabel;
    QQuickWidget* IndexOfRefractionEdit;
    QQuickWidget* IndexOfRefractionSlider;
    /*
        粗糙度U
    */
    QHBoxLayout*    URoughnessLayout;
    QLabel*         URoughnessLabel;
    QQuickWidget*   URoughnessEdit;
    QQuickWidget*   URoughnessSlider;

    /*
        粗糙度V
    */
    QHBoxLayout*    VRoughnessLayout;
    QLabel*         VRoughnessLabel;
    QQuickWidget*   VRoughnessEdit;
    QQuickWidget*   VRoughnessSlider;

private:
    /*
        Disney
    */
    QVBoxLayout*  DisneyMaterialParameterLayout;

    QHBoxLayout*  MetallicLayout;
    QLabel*       MetallicLabel;
    QQuickWidget* MetallicEdit;
    QQuickWidget* MetallicSlider;

    QHBoxLayout*  RoughnessLayout;
    QLabel*       RoughnessLabel;
    QQuickWidget* RoughnessEdit;
    QQuickWidget* RoughnessSlider;

    QHBoxLayout*  SubsurfaceLayout;
    QLabel*       SubsurfaceLabel;
    QQuickWidget* SubsurfaceEdit;
    QQuickWidget* SubsurfaceSlider;

    QHBoxLayout*  SpecularLayout;
    QLabel*       SpecularLabel;
    QQuickWidget* SpecularEdit;
    QQuickWidget* SpecularSlider;

    QHBoxLayout*  SpecularTintLayout;
    QLabel*       SpecularTintLabel;
    QQuickWidget* SpecularTintEdit;
    QQuickWidget* SpecularTintSlider;

    QHBoxLayout*  AnisotropyLayout;
    QLabel*       AnisotropyLabel;
    QQuickWidget* AnisotropyEdit;
    QQuickWidget* AnisotropySlider;

    QHBoxLayout*  SheenLayout;
    QLabel*       SheenLabel;
    QQuickWidget* SheenEdit;
    QQuickWidget* SheenSlider;

    QHBoxLayout*  SheenTintLayout;
    QLabel*       SheenTintLabel;
    QQuickWidget* SheenTintEdit;
    QQuickWidget* SheenTintSlider;

    QHBoxLayout*  ClearcoatLayout;
    QLabel*       ClearcoatLabel;
    QQuickWidget* ClearcoatEdit;
    QQuickWidget* ClearcoatSlider;

    QHBoxLayout*  ClearcoatGlossLayout;
    QLabel*       ClearcoatGlossLabel;
    QQuickWidget* ClearcoatGlossEdit;
    QQuickWidget* ClearcoatGlossSlider;

    QHBoxLayout*  EmissiveLayout;
    QLabel*       EmissiveLabel;
    QQuickWidget* EmissiveEdit;
    QQuickWidget* EmissiveSlider;

    QHBoxLayout*  IORLayout;
    QLabel*       IORLabel;
    QQuickWidget* IOREdit;
    QQuickWidget* IORSlider;

    QHBoxLayout*  TransmissionLayout;
    QLabel*       TransmissionLabel;
    QQuickWidget* TransmissionEdit;
    QQuickWidget* TransmissionSlider;
private:
    /*
        保存按钮: 保存材质参数到数据库中
    */
    QHBoxLayout* DatabaseSaveLayout;
    QLabel* DatabaseSaveLabel;
    QQuickWidget* DatabaseSaveButton;
private:
    GlfwRender* Scene = nullptr;
    int MaterialIndex = -1;
    sqlite3* db = nullptr;
private:
    /*
        厂家材质名称
        映射名称
        材质一级分类
        纹理文件名
        法线贴图文件名
        当前材质类型
    */
    //QString ManufacturerMatName;
    //QString ManufacturerMatNameMapped;
    //QString ManufacturerMatFirstClass;
    //QString CurrentTextureFileName;
    //QString CurrentNormalMapFileName;
    Rt::MaterialType CurrentMaterialType = Rt::MaterialType::Conductor;

    /*
        xlsx中的参数
    */
    /*
        厂家材质名称
    */
    std::vector<std::string> ManufacturerMatNames;
    /*
        材质映射名称
    */
    std::vector<std::string> ManufacturerMatNameMapeds;
    /*
        材质一级分类
    */
    std::vector<std::string> ManufacturerMatFirstClasss;
    std::vector<float> Rs;
    std::vector<float> Gs;
    std::vector<float> Bs;
    std::vector<std::string> ManufacturerMatTypes;
    std::vector<std::string> ManufacturerMatTextures;
    std::vector<std::string> ManufacturerMatNormalMaps;
    std::vector<float> ManufacturerMatReflectivitys;
    std::vector<float> ManufacturerMatTransmissivitys;
    std::vector<float> ManufacturerMatURoughnesss;
    std::vector<float> ManufacturerMatVRoughnesss;
    std::vector<float> ManufacturerMatIORs;

    std::vector<float> ManufacturerDisneyMaterialMetallic;
    std::vector<float> ManufacturerDisneyMaterialRoughness;
    std::vector<float> ManufacturerDisneyMaterialSubsurface;
    std::vector<float> ManufacturerDisneyMaterialSpecular;
    std::vector<float> ManufacturerDisneyMaterialSpecularTint;
    std::vector<float> ManufacturerDisneyMaterialAnisotropy;
    std::vector<float> ManufacturerDisneyMaterialSheen;
    std::vector<float> ManufacturerDisneyMaterialSheenTint;
    std::vector<float> ManufacturerDisneyMaterialClearcoat;
    std::vector<float> ManufacturerDisneyMaterialClearcoatGloss;
    std::vector<float> ManufacturerDisneyMaterialIOR;
    std::vector<float> ManufacturerDisneyMaterialTranmission;
    /*
        UI厂家材质列表的索引
    */
    int ManufacturerMaterialIndex = -1;
};
#endif