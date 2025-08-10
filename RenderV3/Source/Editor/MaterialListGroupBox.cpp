#ifdef USE_QT
#include "MaterialListGroupBox.h"

MaterialListGroupBox::MaterialListGroupBox(QWidget* parent) : QGroupBox(parent)
{

	List = new MaterialListPanel;

	Layout = new QVBoxLayout;
	Layout->addWidget(List);
	setLayout(Layout);
}

MaterialListPanel* MaterialListGroupBox::GetMaterialListPanel()
{
	return List;
}
#endif