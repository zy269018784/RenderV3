#ifdef USE_QT
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <QKeyEvent>
#include <QSemaphore>
#include <QThread>
#include "GlfwRender.h"
#include "MainWindow.h"

#include <thread>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
using namespace QXlsx;

MainWindow::MainWindow()
{
    // 厂家材质列表
    ManufacturerMatListGB = new MaterialListGroupBox;
    ManufacturerMatListGB->setTitle(QStringLiteral("厂家材质列表"));
    ManufacturerMatListGB->setFixedWidth(250);
    ManufacturerMatListPanel = ManufacturerMatListGB->GetMaterialListPanel();
    ManufacturerMatCount = 0;

    // 材质列表
    MatListGB = new MaterialListGroupBox;
    MatListGB->setTitle(QStringLiteral("材质列表"));
    MatListGB->setFixedWidth(250);
    matListPanel = MatListGB->GetMaterialListPanel();


    MatListLayout = new QVBoxLayout;
    MatListLayout->addWidget(ManufacturerMatListGB);
    MatListLayout->addWidget(MatListGB);

    // 材质参数
    MatParamGB = new MaterialParameterGroupBox;
    MatParamGB->setFixedWidth(250);

    // HDR参数
    HDRParamGB = new HDRParameterGroupBox;
    HDRParamGB->setFixedWidth(250);

    // 光源列表
    LightListGB = new LightListGroupBox;
    LightListGB->setFixedWidth(250);

    // 光源参数
    LightParamGB = new LightParameterGroupBox;
    LightParamGB->setFixedWidth(250);

    OtherParamGB = new OtherParameterGroupBox;
    OtherParamGB->setFixedWidth(250);

    CentralWidgetLayout = new QHBoxLayout;
    CentralWidgetLayout->addLayout(MatListLayout);
    CentralWidgetLayout->addWidget(MatParamGB);
    CentralWidgetLayout->addWidget(LightListGB);
    CentralWidgetLayout->addWidget(LightParamGB); 
    CentralWidgetLayout->addWidget(HDRParamGB);
    CentralWidgetLayout->addWidget(OtherParamGB);
    CentralWidgetLayout->addStretch(1);


    SaveCompletedMsgBox = new QMessageBox();
    SaveCompletedMsgBox->setText("保存到xlsx和数据库成功！");
    SaveingMsgBox = new QMessageBox();
    SaveingMsgBox->setText("保存中");

    SaveCompletedSem = new QSemaphore(1);
    SaveStartSem = new QSemaphore(0);
    SaveThread = new std::thread([&, this]
    {
        while (1)
        {
            SaveStartSem->acquire();
            MatParamGB->TemporaryStorePBRToXLSL();
            MatParamGB->OnDatabaseSaveButtonClicked();
         //   QThread::sleep(3);
            SaveCompletedSem->release();
            emit SaveCompleted();
        }
    });
    SaveThread->detach();

    // Create a central widget to hold the layout
    CentralWidget = new QWidget(this); 
    CentralWidget->setLayout(CentralWidgetLayout);
    setCentralWidget(CentralWidget);

    CreateActions();
    CreateMenuBar();

    connect(this, SIGNAL(SaveCompleted()), this, SLOT(OnSaveCompleted()));

    connect(matListPanel, &MaterialListPanel::MaterialSelected, this, &MainWindow::OnMaterialSelected);
    connect(LightListGB->GetLightListPanel(), &LightListPanel::LightSelected, this, &MainWindow::OnLightSelected);

    connect(OtherParamGB, SIGNAL(SignalOffScreenRenderCilcked(bool)), this, SLOT(OnSignalOffScreenRenderClicked(bool)));

    connect(ManufacturerMatListPanel, &MaterialListPanel::MaterialSelected, MatParamGB, &MaterialParameterGroupBox::SetManufacturerMaterialIndex);
}

bool MainWindow::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* ke = (QKeyEvent *)event;
        if ((ke->modifiers() & Qt::ControlModifier) != 0 && ke->key() == Qt::Key_S)
        {
            if (SaveCompletedSem->tryAcquire(1))
            {
                qDebug() << "start 1";
                SaveStartSem->release(1);
                qDebug() << "start 2";
            }
            else
            {
                SaveingMsgBox->exec();
            }
        }
    }
    return QWidget::event(event);
}

int VulkanOffscreenRender(char* scene, int index);
void MainWindow::OpenScene()
{
    QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("*.pbrt"), "", "*.pbrt");
    if (!filePath.isEmpty())
    {
        if (!Render)
        {
            if (OffScreenRender)
                VulkanOffscreenRender(filePath.toStdString().data(), 0);
            else
            {
#if 0
                QObject* parent = nullptr;
                QProcess* myProcess = new QProcess(parent);
                QString program = "render";
                QStringList arguments;
                arguments << filePath;
                myProcess->start(program, arguments);
#endif
                qDebug() << "open" << filePath.toStdString();
                Render = new GlfwRender(filePath.toStdString());
                MatParamGB->SetScene(Render);
                HDRParamGB->SetScene(Render);
                LightParamGB->SetScene(Render);
                matListPanel->Clear();
                UpdateMaterialListUI();
                LightListGB->GetLightListPanel()->Clear();
                UpdateLightListUI();

                try {
                    Render->Run();
                }
                catch (const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                    return;
                }
                delete Render;
                Render = nullptr;
                MatParamGB->SetScene(nullptr);
                HDRParamGB->SetScene(nullptr);
                LightParamGB->SetScene(nullptr);
            }
        }
    }
}

void MainWindow::CloseScene()
{

}

void MainWindow::OnSaveCompleted()
{
    SaveingMsgBox->close();
    SaveCompletedMsgBox->exec();
}

void MainWindow::ImportMaterialFromXLSL()
{
    qDebug() << "ImportMaterialFromXLSL";
    QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("*.xlsx"), "", "*.xlsx");
    if (!filePath.isEmpty())
    {
        qDebug() << filePath;
        Document xlsxR(filePath);
        if (xlsxR.load())
        {
            xlsxR.selectSheet(0);
            auto sheet = xlsxR.currentWorksheet();
            if (sheet)
            {
                auto dimension = sheet->dimension();
                for (int i = 1; i < dimension.rowCount(); i++)
                {
                    /*
                        厂家材质名
                    */
                    std::string str1 = xlsxR.cellAt(i, 1)->readValue().toString().toStdString();
                    /*
                        材质映射名
                    */
                    std::string str2 = xlsxR.cellAt(i, 2)->readValue().toString().toStdString();
                    /*
                        材质一级分类
                    */
                    std::string str3 = xlsxR.cellAt(i, 19)->readValue().toString().toStdString();
                    /*
                        显示格式: 厂家材质名 | 材质映射名
                    */
                    auto Name = QString::fromStdString(str1 + " | " + str2);
                    /*
                        增加到UI显示
                    */
                    ManufacturerMatListGB->GetMaterialListPanel()->AddItem(Name);

                    /*
                        保存厂家材质名, 材质映射名和材质一级分类, 存到数据库中
                    */
     /*               ManufacturerMatName.push_back(str1);
                    ManufacturerMatNameMaped.push_back(str2);
                    ManufacturerMatFirstClass.push_back(str3);*/

                }
            }
        }
        else
            qDebug() << "load failed";
    }
}

void MainWindow::ImportPBRFromXLSL()
{
    QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("*.xlsx"), "", "*.xlsx");
    if (!filePath.isEmpty())
    {
        qDebug() << "filePath" << filePath;
        Document xlsxR(filePath);
        if (xlsxR.load())
        {
            // 清空UI列表
            MatParamGB->ClearManufacturerMaterialList();
            ManufacturerMatListGB->GetMaterialListPanel()->clear();
            ManufacturerMatCount = 0;
            xlsxR.selectSheet(0);
            auto sheet = xlsxR.currentWorksheet(); 
            if (sheet)
            {
                auto dimension = sheet->dimension();
                qDebug() << "dimension.rowCount()" << dimension.rowCount();
                int NameCol = 0, NameMappingCol = 0, FirstClassCol = 0, TypeCol = 0, TextureCol = 0, NormalMapCol = 0;
                float RCol = 0, GCol = 0, BCol = 0, ReflectivityCol = 0, TransmissivityCol = 0, URoughnessCol = 0, VRoughnessCol = 0, IORCol = 0;
               // for (int Col = 0; Col < 30; Col++)
                for (int Col = 1; 1; Col++)
                {         
                    std::cout << "col " << Col << " " << (char)('A' + Col - 1) << std::endl;
                    //std::cout << "xlsxR.read(1, 21).isValid() " << xlsxR.read(1, 21).isValid() << std::endl;
                    //std::cout << "xlsxR.read(1, 22).isValid() " << xlsxR.read(1, 22).isValid() << std::endl;
                    //std::cout << "xlsxR.cellAt(1, 21).get() " << xlsxR.cellAt(1, 21).get() << std::endl;
                    //std::cout << "xlsxR.cellAt(1, 22).get() " << xlsxR.cellAt(1, 22).get() << std::endl;
                    QXlsx::Cell *cell = xlsxR.cellAt(1, Col).get();
                    if (!cell) 
                    {
                        std::cout << "first empty col " << Col << " " << 'A' + Col << std::endl;
                        break;
                    }
     

                    if (xlsxR.read(1, Col).isValid())
                    {
                        std::string Content = xlsxR.cellAt(1, Col)->readValue().toString().toStdString();
                        if (0 == Content.compare("diffuse_color_r"))
                        {
                            RCol = Col;
                        }
                        else if (0 == Content.compare("diffuse_color_g"))
                        {
                            GCol = Col;
                        }
                        else if (0 == Content.compare("diffuse_color_b"))
                        {
                            BCol = Col;
                        }
                        else if (0 == Content.compare("mtl_name"))
                        {
                            NameCol = Col;
                        }
                        else if (0 == Content.compare("nameMapping"))
                        {
                            NameMappingCol = Col;
                        }
                        else if (0 == Content.compare("一级分类"))
                        {
                            FirstClassCol = Col;
                        }
                        else if (0 == Content.compare("mtl_type_"))
                        {
                            TypeCol = Col;
                        }
                        else if (0 == Content.compare("diffuse_texture"))
                        {
                            TextureCol = Col;
                        }
                        else if (0 == Content.compare("normal_map"))
                        {
                            NormalMapCol = Col;
                        }
                        else if (0 == Content.compare("reflectivity"))
                        {
                            ReflectivityCol = Col;
                        }
                        else if (0 == Content.compare("transmissivity"))
                        {
                            TransmissivityCol = Col;
                        }
                        else if (0 == Content.compare("URoughness"))
                        {
                            URoughnessCol = Col;
                        }
                        else if (0 == Content.compare("VRoughness"))
                        {
                            VRoughnessCol = Col;
                        }
                        else if (0 == Content.compare("index_of_refraction"))
                        {
                            IORCol = Col;
                        }
                    }
                }

                std::cout 
                    << "NameCol           "   << NameCol              << "\n"
                    << "NameMappingCol    "   << NameMappingCol       << "\n"
                    << "FirstClassCol     "   << FirstClassCol        << "\n"
                    << "TypeCol           "   << TypeCol              << "\n"
                    << "TextureCol        "   << TextureCol           << "\n"
                    << "NormalMapCol      "   << NormalMapCol         << "\n"
                    << "RCol              "   << RCol                 << "\n"
                    << "GCol              "   << GCol                 << "\n"
                    << "BCol              "   << BCol                 << "\n"
                    << "ReflectivityCol   "   << ReflectivityCol      << "\n"
                    << "TransmissivityCol "   << TransmissivityCol    << "\n"
                    << "URoughnessCol     "   << URoughnessCol        << "\n"
                    << "VRoughnessCol     "   << VRoughnessCol        << "\n"
                    << "IORCol            "   << IORCol               << "\n"
                    << std::endl;
                /*
                   第一行 mtl_name	nameMapping	一级分类	二级分类	process	version	mtl_type_	mtl_id	pbr_mtl_type	texture_size_w	texture_size_h	diffuse_color_r	diffuse_color_g	diffuse_color_b	diffuse_texture	normal_map	reflectivity	transmissivity	URoughness	VRoughness	index_of_refraction深灰色铝合金AJ-02	ANJU-LHJ-02	金属	氟碳漆铝板	20250401-uat-pbr-merge	241227_anju_update	氟碳漆铝板	ANJU-LHJ-02	金属	1000	1000	1	1	1	F:/WXJ_Work/Material_Library/textures/ANJU-LHJ-02-texture.jpg		0		0.2000	0.2000	1.50

                */
                for (int i = 2; i <= dimension.rowCount(); i++)
                {
                    std::string Name, NameMapping, FirstClass, Type, Texture, NormalMap;
                    float R, G, B, Reflectivity, Transmissivity, URoughness, VRoughness, IOR;
                    /*
                        厂家材质名
                    */

   
                    if (!(xlsxR.read(i, 1).isValid() && xlsxR.read(i, 2).isValid() && xlsxR.read(i, 8).isValid()))
                    {
                        qDebug() << "row " << i << "invalid";
                        continue;
                    }
                    ManufacturerMatCount++;
                   // qDebug() << xlsxR.read(i, 1);
    
                    if (xlsxR.read(i, NameCol).isValid())
                        Name = xlsxR.cellAt(i, NameCol)->readValue().toString().toStdString();

                    if (xlsxR.read(i, NameMappingCol).isValid())
                        NameMapping = xlsxR.cellAt(i, NameMappingCol)->readValue().toString().toStdString();

                    if (xlsxR.read(i, FirstClassCol).isValid())
                        FirstClass = xlsxR.cellAt(i, FirstClassCol)->readValue().toString().toStdString();

                    if (xlsxR.read(i, TypeCol).isValid())
                        Type = xlsxR.cellAt(i, TypeCol)->readValue().toString().toStdString();
         
                    if (xlsxR.read(i, RCol).isValid())
                        R = xlsxR.cellAt(i, RCol)->readValue().toFloat();

                    if (xlsxR.read(i, GCol).isValid())
                        G = xlsxR.cellAt(i, GCol)->readValue().toFloat();

                    if (xlsxR.read(i, BCol).isValid())
                        B = xlsxR.cellAt(i, BCol)->readValue().toFloat();
              
                    if (xlsxR.read(i, TextureCol).isValid())
                       Texture = xlsxR.cellAt(i, TextureCol)->readValue().toString().toStdString();
  
                    if (xlsxR.read(i, NormalMapCol).isValid())
                       NormalMap = xlsxR.cellAt(i, NormalMapCol)->readValue().toString().toStdString();

                    //if (i < 10)
                    //std::cout << "Texture " << Texture << " NormalMap " << NormalMap << std::endl;

                    if (xlsxR.read(i, ReflectivityCol).isValid())
                        Reflectivity = xlsxR.cellAt(i, ReflectivityCol)->readValue().toFloat();
         
                    if (xlsxR.read(i, TransmissivityCol).isValid())
                        Transmissivity = xlsxR.cellAt(i, TransmissivityCol)->readValue().toFloat();
                    
                    if (xlsxR.read(i, URoughnessCol).isValid())
                        URoughness = xlsxR.cellAt(i, URoughnessCol)->readValue().toFloat();

                    if (xlsxR.read(i, VRoughnessCol).isValid())
                        VRoughness = xlsxR.cellAt(i, VRoughnessCol)->readValue().toFloat();

                    if (xlsxR.read(i, IORCol).isValid())
                        IOR = xlsxR.cellAt(i, IORCol)->readValue().toFloat();

          
                    MatParamGB->AddManufacturerMaterial(Name, NameMapping, FirstClass, Type, R, G, B, 
                        Texture, NormalMap, Reflectivity, Transmissivity, URoughness, VRoughness, IOR);

  
                    // 强制为0
                    MatParamGB->SetMaterialIndex(0);


                    /*
                      增加到UI显示
                     */

                    ManufacturerMatListGB->GetMaterialListPanel()->AddItem(Name + " | " + NameMapping);

 
                    //qDebug() << "Material " << NameMapping << FirstClass << Type << R << G << B 
                    //    << Texture << NormalMap << Reflectivity << Transmissivity << URoughness << VRoughness
                    //    << IOR;
                  //  qDebug() << "Material " << NameMapping ;
                }

                ManufacturerMatListGB->setTitle(QStringLiteral("厂家材质列表") + QString("[数量: %1]").arg(ManufacturerMatCount));
            }
        }
    }
}


//void MainWindow::OnManufacturerMaterialSelected(int MatIndex)
//{
//    MatParamGB->SetManufacturerMatNameMapped(
//        QString::fromStdString(ManufacturerMatName[MatIndex]),
//        QString::fromStdString(ManufacturerMatNameMaped[MatIndex]),
//        QString::fromStdString(ManufacturerMatFirstClass[MatIndex]),
//        QString::fromStdString(Types[MatIndex]),
//        QString::fromStdString(Rs[MatIndex]),
//        QString::fromStdString(Gs[MatIndex]),
//        QString::fromStdString(Bs[MatIndex]),
//        QString::fromStdString(Textures[MatIndex]),
//        QString::fromStdString(NormalMaps[MatIndex]),
//        QString::fromStdString(Reflectivitys[MatIndex]),
//        QString::fromStdString(Transmissivitys[MatIndex]),
//        QString::fromStdString(URoughnesss[MatIndex]),
//        QString::fromStdString(VRoughnesss[MatIndex]),
//        QString::fromStdString(IORs[MatIndex])
//    );
//}

void MainWindow::OnMaterialSelected(int MatIndex)
{
    MatParamGB->SetMaterialIndex(MatIndex);
    HDRParamGB->SetMaterialIndex(MatIndex);
}

void MainWindow::OnLightSelected(int LightIndex)
{
    LightParamGB->SetLightIndex(LightIndex);
}

void MainWindow::OnSignalOffScreenRenderClicked(bool c)
{
    qDebug() << "OnSignalOffScreenRenderChecked " << c;
    OffScreenRender = c;
}

void MainWindow::CreateActions()
{
    OpenSceneAction = new QAction(QStringLiteral("打开"));
    CloseSceneAction = new QAction(QStringLiteral("关闭"));
    ImportMaterialAction = new QAction(QStringLiteral("导入厂家材质")); 
    ImportPBRAction = new QAction(QStringLiteral("导入PBR参数"));
    ExportPBRAction = new QAction(QStringLiteral("保存PBR参数"));

    connect(OpenSceneAction,  &QAction::triggered, this, &MainWindow::OpenScene);
    connect(CloseSceneAction, &QAction::triggered, this, &MainWindow::CloseScene);
    connect(ImportMaterialAction, &QAction::triggered, this, &MainWindow::ImportMaterialFromXLSL);
    connect(ImportPBRAction, &QAction::triggered, this, &MainWindow::ImportPBRFromXLSL);
    connect(ExportPBRAction, &QAction::triggered, MatParamGB, &MaterialParameterGroupBox::ExportPBRToXLSL);
}

void MainWindow::CreateMenuBar()
{
    menuBar = new QMenuBar();

    QMenu* m1 = new QMenu(QStringLiteral("项目"));
    m1->addAction(OpenSceneAction);
    m1->addAction(CloseSceneAction);
    m1->addAction(ImportMaterialAction);
    m1->addAction(ImportPBRAction);
    m1->addAction(ExportPBRAction);

    QMenu* m2 = new QMenu(QStringLiteral("帮助"));

    menuBar->addMenu(m1);
    menuBar->addMenu(m2);

    setMenuBar(menuBar);
}

void MainWindow::UpdateMaterialListUI()
{
    if (Render)
    {
        const std::vector<Rt::RtMaterial*>& Mat = Render->GetMaterials();
        for (const auto& pMaterial : Mat)
            matListPanel->AddItem(pMaterial->name_);
    }
}

void MainWindow::UpdateLightListUI()
{
    if (Render)
    {
        const std::vector<Rt::Light*>& Lights = Render->GetLights();
        int i = 0;
        for (const auto& pLight : Lights) {
            std::string name = to_string(i);
            LightListGB->GetLightListPanel()->AddItem(name);
            i++;
        } 
    }
}
#endif