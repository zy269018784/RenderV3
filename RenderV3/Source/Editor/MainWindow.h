#ifdef USE_QT
#pragma once
#include <QMainWindow>
#include <QMenuBar>
#include <QQuickWidget>
#include <QSemaphore>
#include <QMessageBox>
#include <thread>

#include "MaterialListGroupBox.h"
#include "MaterialParameterGroupBox.h"
#include "MaterialListPanel.h"
#include "LightParameterGroupBox.h"
#include "LightListGroupBox.h"
#include "HDRParameterGroupBox.h"
#include "OtherParameterGroupBox.h"

class GlfwRender;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    virtual bool event(QEvent* event) override;
signals:
    void SaveMaterial();
    void MaterialListViewAppend(QString str);
    void SaveCompleted();
public slots:
    void OpenScene();
    void CloseScene();
    void OnSaveCompleted();

    /*
        从XLSL导入厂家材质信息
    */
    void ImportMaterialFromXLSL();
    /*
        从XLSX导入PBR参数信息
    */
    void ImportPBRFromXLSL();


    /*
        选中厂家材质  
    */
    //void OnManufacturerMaterialSelected(int MatIndex);

    void OnMaterialSelected(int MatIndex);
    void OnLightSelected(int LightIndex);
    void OnSignalOffScreenRenderClicked(bool c);
private:
    void CreateActions();
    void CreateMenuBar();
    void UpdateMaterialListUI();
    void UpdateLightListUI();
private:
    QWidget* CentralWidget;
    QHBoxLayout* CentralWidgetLayout;
    QMenuBar* menuBar;
    /*
        HDR参数列表
    */
    HDRParameterGroupBox* HDRParamGB;

    /*
         Layout:
             厂家材质列表
             场景材质列表
    */
    QVBoxLayout* MatListLayout;
    /*
        厂家材质列表
    */
    MaterialListGroupBox* ManufacturerMatListGB;
    MaterialListPanel* ManufacturerMatListPanel;
    int  ManufacturerMatCount;

    /*
        场景材质列表
    */
    MaterialListGroupBox*       MatListGB;
    MaterialListPanel* matListPanel;
    MaterialParameterGroupBox*   MatParamGB;
    /*
        光源参数列表
    */
    LightListGroupBox*          LightListGB;
    LightParameterGroupBox*     LightParamGB;

    OtherParameterGroupBox* OtherParamGB;

    bool  OffScreenRender = false;
    
    /*
        保存
    */
    QSemaphore* SaveStartSem;
    QSemaphore *SaveCompletedSem;
    QMessageBox* SaveCompletedMsgBox;
    QMessageBox* SaveingMsgBox;
    std::thread *SaveThread;
/*
    Actions
*/
private:
    QAction* OpenSceneAction;
    QAction* CloseSceneAction;
    QAction* ImportMaterialAction;
    QAction* ImportPBRAction;
    QAction* ExportPBRAction;
private:
    GlfwRender* Render;
};
#endif