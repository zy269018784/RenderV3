#ifdef USE_QT
#include <Editor/VulkanWindow.h>
#include <Editor/RTRenderer.h>

QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
	return new RTRenderer(this, true); // try MSAA, when available
}
#endif