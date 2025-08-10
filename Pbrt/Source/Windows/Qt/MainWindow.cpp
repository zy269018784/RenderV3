#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <OpenGLWidget.h>
#include <QDebug>
#include <QLabel>
#include <QWidget>
#include <QColorDialog>
#include <QResizeEvent>

#include <Scene/SceneData.h>
void GetTextureColor(int index, float& r, float& g, float& b);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
 
    w = new OpenGLWidget;
    //connect(&te, SIGNAL(TextureChanged()), w, SLOT(ReRender()));
    connect(&te, &TextureEdit::TextureChanged,  w, &OpenGLWidget::ReRender);
    connect(&me, &MaterialEdit::TextureChanged, w, &OpenGLWidget::ReRender);
    connect(ui->SkyColor, &QPushButton::clicked, this, &MainWindow::SetSkyColor);

    ui->widget->layout()->addWidget(w);

    me.setWindowFlag(Qt::WindowStaysOnTopHint);
    te.setWindowFlag(Qt::WindowStaysOnTopHint);
    SceneData = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetSkyColor() {
    const QColor color = QColorDialog::getColor(Qt::green, this, "Select Color");

    float r = color.redF();
    float g = color.greenF();
    float b = color.blueF();

    if (this->SceneWrapper) {
        SceneWrapper->SetSkyBaseColor(r, g, b);
        SceneWrapper->ReRender();
        update();
    }
}

void MainWindow::SetSceneData(Render::SceneData* sd) {
    if (sd && !SceneData) {
        SceneData = sd;
        me.SetSceneData(sd);

        int MaterialCount = sd->GetMaterialNameCount();
        for (int i = 0; i < MaterialCount; i++) {
             ui->listWidget_2->addItem(QString::fromStdString(sd->GetMaterialNameFromMaterialIndex(i)));
        }

        int TextureCount = sd->GetTextureNameCount();
        for (int i = 0; i < TextureCount; i++) {
            ui->listWidget_3->addItem(QString::fromStdString(sd->GetTextureNameFromTextureIndex(i)));
        }   
        // 材质名被双击, UI显示材质信息
        connect(ui->listWidget_2, &QListWidget::itemDoubleClicked, [=](QListWidgetItem* item) {
            std::string MaterialName = item->text().toStdString();
            w->setMouseTracking(false);
            me.SetMaterialName(MaterialName);
            me.show();
        });

        // 纹理名被双击, UI显示纹理信息
        connect(ui->listWidget_3, &QListWidget::itemDoubleClicked, [=](QListWidgetItem* item) {
            std::string TextureName = item->text().toStdString();
            
            int TextureIndex = sd->GetTextureIndexFromTextureName(TextureName);
            int type = 3;                   // random
            if (sd->GetImageTextureOffset() <= TextureIndex && TextureIndex < sd->GetConstTextureOffset()) {
                type = 0;
                std::string file;
                // 获取当前纹理图片的磁盘路径
                file = sd->GetImageTextureFileNameFromTextureIndex(TextureIndex);
                // 显示当前纹理的图片
                te.SetTextureImage(file);
                te.SetTextureIndex(TextureIndex);
            }
            else if (sd->GetConstTextureOffset() <= TextureIndex && TextureIndex < sd->GetNoiseTextureOffset()) {
                type = 1;
                float r, g, b;
                GetTextureColor(TextureIndex, r, g, b);
                qDebug() << "color " << r << g << b;
                te.SetTextureColor(r, g, b);
                te.SetTextureIndex(TextureIndex);
            }
            if (sd->GetNoiseTextureOffset() <= TextureIndex && TextureIndex < sd->GetRandomTextureOffset()) {
                type = 2;
            }
            if (sd->GetRandomTextureOffset() <= TextureIndex) {
                type = 3;
            }
            
            w->setMouseTracking(false);
            te.SetTextureName(QString::fromStdString(TextureName));
            te.SetTextureType(type);
            te.show();
         });
    }


}

void MainWindow::SetSceneWrapper(Render::SceneWrapper * wrapper) {
    SceneWrapper = wrapper;
    w->SetSceneWrapper(wrapper);
    me.SetSceneWrapper(wrapper);
}

void MainWindow::resizeEvent(QResizeEvent* ev) {
    qDebug() << ev->size();
}