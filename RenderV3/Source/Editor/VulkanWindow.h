#ifdef USE_QT
#pragma once
#include <QVulkanWindow>
#include <QVulkanWindowRenderer>

class VulkanWindow : public QVulkanWindow
{
    Q_OBJECT
public:
    QVulkanWindowRenderer* createRenderer() override;
};
#endif