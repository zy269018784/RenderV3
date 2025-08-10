#ifdef USE_QT
#include "LightListPanel.h"

LightListPanel::LightListPanel(QWidget* parent) : QListWidget(parent)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(this, &LightListPanel::itemClicked, this, &LightListPanel::OnItemClicked);
}

void LightListPanel::AddItem(QString str)
{
	addItem(str);
}

void LightListPanel::AddItem(std::string str)
{
	addItem(QString::fromStdString(str));
}

void LightListPanel::Clear()
{
	clear();
}

void LightListPanel::OnItemClicked(QListWidgetItem* item)
{
	emit LightSelected(row(item));
}
#endif