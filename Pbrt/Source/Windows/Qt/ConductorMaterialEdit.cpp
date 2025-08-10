#include "ConductorMaterialEdit.h"
#include "ui_ConductorMaterialEdit.h"

ConductorMaterialEdit::ConductorMaterialEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConductorMaterialEdit)
{
    ui->setupUi(this);
}

ConductorMaterialEdit::~ConductorMaterialEdit()
{
    delete ui;
}
