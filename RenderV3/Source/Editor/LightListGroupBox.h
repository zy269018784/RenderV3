#ifdef USE_QT
#pragma once
#include <QGroupBox>
#include <QVBoxLayout>
#include "LightListPanel.h"
class LightListGroupBox : public QGroupBox
{
public:
	LightListGroupBox(QWidget* parent = nullptr);
	LightListPanel* GetLightListPanel();
private:
	QVBoxLayout* Layout;
	LightListPanel* List;
};
#endif