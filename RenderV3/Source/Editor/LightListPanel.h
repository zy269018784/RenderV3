#ifdef USE_QT
#pragma once
#include <QListWidget>
class LightListPanel : public QListWidget
{
    Q_OBJECT
public:
    LightListPanel(QWidget* parent = nullptr);
    void AddItem(QString str);
    void AddItem(std::string str);
    void Clear();
public slots:
    void OnItemClicked(QListWidgetItem* item);
signals:
    void LightSelected(int MatIndex);
};
#endif