#ifdef USE_QT
#pragma once
#include <QGroupBox>
#include <QVBoxLayout>
#include "MaterialListPanel.h"
class MaterialListGroupBox : public QGroupBox
{
public:
	MaterialListGroupBox(QWidget* parent = nullptr);
	MaterialListPanel* GetMaterialListPanel();
private:
	QVBoxLayout* Layout;
	MaterialListPanel* List;
};
#endif