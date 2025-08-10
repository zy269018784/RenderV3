#ifndef CONDUCTORMATERIALEDIT_H
#define CONDUCTORMATERIALEDIT_H

#include <QWidget>

namespace Ui {
class ConductorMaterialEdit;
}

class ConductorMaterialEdit : public QWidget
{
    Q_OBJECT

public:
    explicit ConductorMaterialEdit(QWidget *parent = 0);
    ~ConductorMaterialEdit();

private:
    Ui::ConductorMaterialEdit *ui;
};

#endif // CONDUCTORMATERIALEDIT_H
