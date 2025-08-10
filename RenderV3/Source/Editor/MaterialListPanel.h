#ifdef USE_QT
#pragma once
#include <QListWidget>
class MaterialListPanel : public QListWidget
{
    Q_OBJECT
public:
    MaterialListPanel(QWidget* parent = nullptr);
    void AddItem(QString str);
    void AddItem(std::string str);
    void Clear();
public slots:
    void OnItemClicked(QListWidgetItem* item);
signals:
    void MaterialSelected(int MatIndex);
};
#endif