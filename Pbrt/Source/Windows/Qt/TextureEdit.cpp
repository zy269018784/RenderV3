#include "textureedit.h"
#include "ui_textureedit.h"
#include <QColorDialog> 
#include <QFiledialog.h>
#include <QDebug>
void SetTextureColor(int index, float& r, float& g, float& b);
void SetTextureImage(int index, std::string filename);
TextureEdit::TextureEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextureEdit)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &TextureEdit::colorEdit);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &TextureEdit::fileEdit);
}

TextureEdit::~TextureEdit()
{
    delete ui;
}



void TextureEdit::colorEdit()
{
    const QColor color = QColorDialog::getColor(Qt::green, this, "Select Color");
    QPalette pe;
    pe.setColor(ui->label_6->backgroundRole(), color);
    pe.setColor(QPalette::WindowText, color);
    ui->label_6->setAutoFillBackground(true);
    ui->label_6->setPalette(pe);
    ui->label_6->setPixmap(QPixmap());

    float r = color.redF() * 255;
    float g = color.greenF() * 255;
    float b = color.blueF() * 255;
    qDebug() << "colorEdit " << r << g << b;
    ::SetTextureColor(index, r, g, b);
    
    emit TextureChanged();
}

void TextureEdit::fileEdit()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".", tr("Image Files (*.png *.jpg)"));

    // 更新渲染器内存
    ::SetTextureImage(index, fileName.toStdString());
    // 更新UI
    SetTextureImage(fileName.toStdString());

    emit TextureChanged();
}

void TextureEdit::SetTextureImage(const std::string& filename)
{
    QPixmap pix;
    pix.load(QString::fromStdString(filename));
    pix = pix.scaled(ui->label_5->size());
    ui->label_5->setPixmap(pix);


}

void TextureEdit::SetTextureName(QString name)
{
    ui->label_2->setText(name);
}

void TextureEdit::SetTextureType(int type)
{
    ui->comboBox->setCurrentIndex(type);
    if (0 == type) {
        ui->pushButton->hide();
        ui->pushButton_2->show();
        ui->label_5->show();
        ui->label_6->hide();
    }
    else if (1 == type) {
        ui->pushButton->show();
        ui->pushButton_2->hide();
        ui->label_5->hide();
        ui->label_6->show();
    }
}

void TextureEdit::SetTextureIndex(int idx)
{
    this->index = idx;
}


void TextureEdit::SetTextureColor(float r, float g, float b)
{ 
    QPalette pe;
    pe.setColor(ui->label_6->backgroundRole(), QColor(r, g, b));
    pe.setColor(QPalette::WindowText, QColor(r, g, b));
    ui->label_6->setAutoFillBackground(true);
    ui->label_6->setPalette(pe);
}