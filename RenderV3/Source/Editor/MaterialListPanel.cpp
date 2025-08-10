#ifdef USE_QT
#include "MaterialListPanel.h"

MaterialListPanel::MaterialListPanel(QWidget* parent) : QListWidget(parent)
{
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(this, &MaterialListPanel::itemClicked, this, &MaterialListPanel::OnItemClicked);
}

void MaterialListPanel::AddItem(QString str)
{
	addItem(str);
}

void MaterialListPanel::AddItem(std::string str)
{
	addItem(QString::fromStdString(str));
}

void MaterialListPanel::Clear()
{
	clear();
}

void MaterialListPanel::OnItemClicked(QListWidgetItem* item)
{
	emit MaterialSelected(row(item));
}
#endif