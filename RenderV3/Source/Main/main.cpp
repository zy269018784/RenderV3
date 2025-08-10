#include <iostream>
#ifdef USE_QT
#include <QApplication>
#include <QVulkanWindow>
#endif
#include "Editor/MainWindow.h"
#include <Editor/VulkanWindow.h>

#include "SceneFileFormat/SceneFileFormat.h"

int VulkanOffscreenRender(char* scene, int GpuIndex);
#ifdef USE_QT
int MainWindowQt(int argc, char** argv);
#endif

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
}

int main(int argc, char **argv)
{
    //TestSceneFileFormat();
    //return 0;
#ifdef USE_QT
	return MainWindowQt(argc, argv);
#else
    int GpuIndex = 0;
    if (argc > 2)
    {
        GpuIndex = atoi(argv[2]);
        std::cout << "main GpuIndex " << GpuIndex << std::endl;  
        
        //int s;
        //cpu_set_t cpuset;
        //pthread_t thread;

        //thread = pthread_self();

        ///* Set affinity mask to include CPUs 0 to 7. */

        //CPU_ZERO(&cpuset);
        ////for (size_t j = 0; j < 8; j++)
        ////    CPU_SET(j, &cpuset);

        //CPU_SET(GpuIndex, &cpuset);

        //s = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
        ////if (s != 0)
        ////    errc(EXIT_FAILURE, s, "pthread_setaffinity_np");

        ///* Check the actual affinity mask assigned to the thread. */

        //s = pthread_getaffinity_np(thread, sizeof(cpuset), &cpuset);
        ////if (s != 0)
        ////    errc(EXIT_FAILURE, s, "pthread_getaffinity_np");

        //printf("Set returned by pthread_getaffinity_np() contained:\n");
        //for (size_t j = 0; j < CPU_SETSIZE; j++)
        //    if (CPU_ISSET(j, &cpuset))
        //        printf("    CPU %zu\n", j);
    }
    return VulkanOffscreenRender(argv[1], GpuIndex);
#endif
}

#ifdef USE_QT
int MainWindowQt(int argc, char** argv)
{
    QApplication app(argc, argv);

    MainWindow w;
    w.resize(1024, 768);
    w.show();
    return app.exec();
}

int MainQt(int argc, char** argv)
{
	QApplication app(argc, argv);
    //! [0]
    QVulkanInstance inst;
    inst.setLayers({ "VK_LAYER_KHRONOS_validation" });
    if (!inst.create())
        qFatal("Failed to create Vulkan instance: %d", inst.errorCode());
    //! [0]

    //! [1]
    VulkanWindow w;
    w.setVulkanInstance(&inst);

    w.resize(1024, 768);
    w.show();
	return app.exec();
}
#endif