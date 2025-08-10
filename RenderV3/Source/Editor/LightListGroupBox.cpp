#ifdef USE_QT
#include "LightListGroupBox.h"

LightListGroupBox::LightListGroupBox(QWidget* parent) : QGroupBox(parent)
{
	setTitle(QStringLiteral("光源列表"));

	List   = new LightListPanel;
	Layout = new QVBoxLayout;
	Layout->addWidget(List);
	setLayout(Layout);
}

LightListPanel* LightListGroupBox::GetLightListPanel()
{
	return List;
}
#endif