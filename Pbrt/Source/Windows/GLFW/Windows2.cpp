#ifdef OPENGL_RENDER_1
    #include <Windows/GLFW/Windows2.h>
    #include <Render/Render.h>
    #include "imgui.h"
    #include "imgui_impl_glfw.h"
    #include "imgui_impl_opengl3.h"
    //#include <Imgui/ImGuiFileDialog.h>
    //#include <Imgui/imfilebrowser.h>
    #include <stdio.h>
    #define GL_SILENCE_DEPRECATION
    #if defined(IMGUI_IMPL_OPENGL_ES2)
    #include <GLES2/gl2.h>
#endif

#include <Windows/GLFW/Callbak.h>

#include <Camera/PerspectiveCamera.h>

void test();
namespace Render {

    //Windows2* Windows2::win = nullptr;
    
    bool Windows2::TrylockRend() {
        return mtx.try_lock();
    }

    void Windows2::LockRend() {
        mtx.lock();
    }

    void Windows2::UnlockRend() {
        mtx.unlock();
    }

    void Windows2::Rend() {
        while (!GUIClosed) {

            //cout << "Rend start" << endl;
            LockRend();
            if (needRend) {
                cout << "Rend start "
                     << scene->RenderBuffer->Width()  << " " 
                     << scene->RenderBuffer->Height() << " " << endl;
                // 渲染
                RenderCuda(scene);
            
                needCopyToPBO = true;
                needRend = false;  
                cout << "Rend end" << endl;
            }
            UnlockRend();
        
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    Windows2::Windows2(const char* windows_title, int width, int height) 
         {
        // 初始化glfw
        glfwInit();
        // 错误回调函数
        glfwSetErrorCallback(error_callback);
        // 设置opengl
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // To make Apple happy -- should not be needed
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // 创建glfw窗口
        window = glfwCreateWindow(width, height, windows_title, nullptr, nullptr);

        cout << "glfwCreateWindow AAA\n";
        //_sleep(100000);
        cout << "glfwCreateWindow BBB\n";
        if (window) {
            // 设置当前Context
            glfwMakeContextCurrent(window);

            // 关闭垂直同步: A context must be current on the calling thread.
            glfwSwapInterval(1);

            // glad加载opengl
            gladLoadGL();

            glfwSetWindowUserPointer(window, this);

            glfwSetKeyCallback(window, KeyCallback2);
            //glfwSetWindowRefreshCallback(window, WindowRefreshCallback2);
            //glfwSetWindowSizeCallback(window, WindowSizeCallback2);
        }
    }

    Windows2::Windows2(const char* windows_title, const Scene* scene1)
        : Windows2(windows_title, scene1->RenderBuffer->Width(), scene1->RenderBuffer->Height())
    {
        cout << "Windows2 C\n";
        this->scene = (Scene *)(scene1);
        // opengl
        if (window) {
            glGenTextures(1, &texture[0]);
            glBindTexture(GL_TEXTURE_2D, texture[0]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //glGenTextures(1, &texture[1]);
            //glBindTexture(GL_TEXTURE_2D, texture[1]);
            //
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            //
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //uint8_t* ptr = new uint8_t[400 * 300 * 3];
            //
            //for (int i = 0; i < 400; i++) {
            //    for (int j = 0; j < 300; j++) {
            //        int offset = 3 * (j * 400 + i);
            //        ptr[offset + 0] = 0;
            //        ptr[offset + 1] = 0;
            //        ptr[offset + 2] = 0;
            //        if (j > 150) {
            //            ptr[offset + 0] = 255;
            //            ptr[offset + 1] = 0;
            //            ptr[offset + 2] = 0;
            //        }
            //    }
            //}

            PBO.Create();
            //PBO.Bind(GL_PIXEL_UNPACK_BUFFER);
            //PBO.BufferData(400 * 300 * 3, ptr);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 400, 300, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            //glGenerateMipmap(GL_TEXTURE_2D);
            //PBO.Unbind(GL_PIXEL_UNPACK_BUFFER);
        }

        // imgui
        if (window) {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            //(void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();

            // Setup Platform/Renderer backends
            const char* glsl_version = "#version 460";
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version);
        }

        if (window) {
            std::thread myThread(&Windows2::Rend, this);
            myThread.detach();
        }
        
    }

    Windows2::~Windows2() {
 
        PBO.Delete();

        // 销毁glfw窗口
        glfwDestroyWindow(window);
        // 退出glfw库
        glfwTerminate();
    }
    
    void  Windows2::CopyFromPBOToTextureObject() {

        cout << "  BufferData" << scene->RenderBuffer->Width() << " " << scene->RenderBuffer->Height() << endl;
        PBO.Bind(GL_PIXEL_UNPACK_BUFFER);
        PBO.BufferData(scene->RenderBuffer->getColorBufferSize(), scene->RenderBuffer->getColorBufferPointer());
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scene->RenderBuffer->Width(), scene->RenderBuffer->Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glGenerateMipmap(GL_TEXTURE_2D);
        PBO.Unbind(GL_PIXEL_UNPACK_BUFFER);

    }

    void Windows2::Run() {

        if (window) {
            glBindTexture(GL_TEXTURE_2D, texture[0]);
            while (!glfwWindowShouldClose(window)) {

                glClearColor(0.f, 0.f, 0.f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                int width, height;
                glfwGetWindowSize(window, &width, &height);

                GuiMainWindow(width, height);

                glfwSwapBuffers(window);

                //glfwWaitEvents();
                glfwPollEvents();
            } // while (!glfwWindowShouldClose(window)) 

            GUIClosed = true;

        }
    }

    void Windows2::GuiMainWindow(int width, int height) {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize(ImVec2(width, height));

        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

        if (ImGui::Begin("MainWindow", nullptr, window_flags)) {

            GuiMenuBar();

            ObjectWindow();

            ImGui::SameLine();

            OpenGLWindow();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    }

    void Windows2::GuiMenuBar() {
        if (ImGui::BeginMenuBar()) {

            if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("Main menu bar", nullptr, nullptr);
                ImGui::MenuItem("Main menu bar", nullptr, nullptr);
                ImGui::MenuItem("Main menu bar", nullptr, nullptr);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    void Windows2::ObjectWindow() {
        if (ImGui::BeginChild("ObjectWindow", ImVec2(200, 0), true)) {
            auto ObjectWindowPos = ImGui::GetWindowPos();
            auto ObjectWindowSize = ImGui::GetWindowSize();

            //cout << "ObjectWindowPos " << ObjectWindowPos.x << " " << ObjectWindowPos.y << " "
            //    << "ObjectWindowSize " << ObjectWindowSize.x << " " << ObjectWindowSize.y << endl;

            GuiMeshes();
            GuiMaterials();
            GuiTextures();
            GuiLights();

            ImGui::EndChild();
        }
    }

    void Windows2::OpenGLWindow() {
        ImGuiIO& io = ImGui::GetIO();
        if (ImGui::BeginChild("OpenGLWindow", ImVec2(0, 0), true))
        {
            auto MainWindowSize = io.DisplaySize;
            auto OpenGLWindowPos  = ImGui::GetWindowPos();
            auto OpenGLWindowSize = ImGui::GetWindowSize();


            //cout 
            //     << "io.DisplaySize "   << io.DisplaySize.x   << " "   << io.DisplaySize.y   << " "
            //     << "OpenGLWindowPos "  << OpenGLWindowPos.x  << " "   << OpenGLWindowPos.y  << " "
            //     << "OpenGLWindowSize " << OpenGLWindowSize.x << " "   << OpenGLWindowSize.y << endl;

            if (main_w != MainWindowSize.x ||
                main_h != MainWindowSize.y
                ) {
                if (TrylockRend()) {
                    main_w = MainWindowSize.x;
                    main_h = MainWindowSize.y;

                    // 渲染缓冲区resize
                    scene->RenderBuffer->Resize(OpenGLWindowSize.x, OpenGLWindowSize.y);
                    // 出图resize
                    scene->GetOutputImagePointer()->p8 = (uint8_t*)scene->RenderBuffer->getColorBufferPointer();
                    Point2i res(OpenGLWindowSize.x, OpenGLWindowSize.y);
                    char* channels[] = { "R", "G", "B" };
                    new (scene->GetOutputImagePointer())Image((uint8_t*)scene->RenderBuffer->getColorBufferPointer(), res, channels, 3);


                    // 更新相机
                    Float frame = scene->RenderBuffer->Width() * 1.f / scene->RenderBuffer->Height();
                    scene->SetScreenWindow(Bounds2f({ -frame, -1 }, { frame, 1 }));

                    RGBFilm* film = (RGBFilm*)scene->GetFilm().ptr();
                    film->Resize({ (int)scene->RenderBuffer->Width(), (int)scene->RenderBuffer->Height() });

                    // 相机矩阵
                    Transform lookat = LookAt(scene->eye, scene->look, scene->up);
                    scene->cameraFromWorld = lookat;
                    scene->worldFromCamera = Inverse(lookat);

                    // 相机参数
                    CameraBaseParameters cbp(scene->cameraFromWorld, scene->worldFromCamera, scene->GetFilm(), scene->GetMedium());

                    // 相机
                    PerspectiveCamera* pc = (PerspectiveCamera*)scene->camera.ptr();

                    // 重构相机
                    new (scene->camera.ptr()) PerspectiveCamera(cbp, scene->fov, scene->GetScreenWindow(), 0.f, 0.f);

                    // 刷新标志
                    needRend = true;
                    cout << "needRend = true;" << endl;
                    UnlockRend();
                }
            }

            if (needCopyToPBO) {

                CopyFromPBOToTextureObject();
                needCopyToPBO = false;
            }
            glViewport(OpenGLWindowPos.x, OpenGLWindowPos.y, OpenGLWindowPos.x + OpenGLWindowSize.x, OpenGLWindowPos.y + OpenGLWindowSize.y);

            // 绘制纹理
            ImGui::GetWindowDrawList()->AddImage(
                (void*)texture[0],
                ImVec2(OpenGLWindowPos.x, OpenGLWindowPos.y),
                ImVec2(OpenGLWindowPos.x + OpenGLWindowSize.x, OpenGLWindowPos.y + OpenGLWindowSize.y),
                ImVec2(0, 0),
                ImVec2(1, 1)
            );

            //glBindTexture(GL_TEXTURE_2D, -1);

            // 绘制文本
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

            ImGui::EndChild();
        }
    }

    void Windows2::GuiMeshes() {
        static bool select = false;
        static string currentMeshName;
        if (ImGui::CollapsingHeader("Meshes")) {
            if (ImGui::BeginListBox("")) {
                for (int n = 0; n < 10; n++) {
                    if (ImGui::Selectable(to_string(n).c_str(), false)) {
                        cout << "select " << currentMeshName << endl;
                        currentMeshName = to_string(n);
                        select = true;
                    }
                }          
                ImGui::EndListBox();

            }
        }               
        if (select) { 
            cout << currentMeshName << endl;
            ImGui::OpenPopup("Mesh");
            
            //auto center = ImGui::GetMainViewport()->GetCenter();
            bool open = true;
            ImGui::SetNextWindowPos({200, 200});
            ImGui::SetNextWindowSize({ 200, 200 });
            if (ImGui::BeginPopupModal("Mesh", &open, ImGuiWindowFlags_AlwaysAutoResize)) {
                string str = "Mesh: ";
                str += currentMeshName;
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), str.c_str());
                string str2 = "Material: ";
                str2 += currentMeshName;
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), str2.c_str());
            }
            else {
                select = false;
            }
            ImGui::EndPopup();
        }
    }

    void Windows2::GuiMaterials() {
        static bool select = false;
        static string currentMaterialName;
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        if (ImGui::CollapsingHeader("Materials")) {
            if (ImGui::BeginListBox("")) {
                for (int n = 0; n < 10; n++) {
                    if (ImGui::Selectable(to_string(n).c_str(), false)) {
                        cout << "select " << currentMaterialName << endl;
                        currentMaterialName = to_string(n);
                        select = true;
                    }
                }
                ImGui::EndListBox();

            }
        }

        GuiMaterialEdit(select, currentMaterialName);
    }

    void Windows2::GuiMaterialEdit(bool& opened, const string& materialName) {
       static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
       if (opened) {
           ImGui::OpenPopup("Material");

           bool open = true;
           if (ImGui::BeginPopupModal("Material", &open)) {
               string str = "Material: ";
               str += materialName;
               ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), str.c_str());

               ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Color: ");
               ImGui::SameLine();
               ImGui::ColorEdit3("", (float*)&clear_color);

               ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "texture: ");

           }
           else {
               opened = false;
           }

           ImGui::EndPopup();
       }
    }

    void Windows2::GuiTextures() {
        if (ImGui::CollapsingHeader("Textures")) {
            if (ImGui::BeginListBox("")) {
                for (int n = 0; n < 10; n++) {
                    ImGui::Selectable(to_string(n + 200).c_str(), false);
                }
                ImGui::EndListBox();
            }
        }
    }

    void Windows2::GuiLights() {
        if (ImGui::CollapsingHeader("Lights")) {
            if (ImGui::BeginListBox("")) {
                for (int n = 0; n < 10; n++) {
                    ImGui::Selectable(to_string(n + 100).c_str(), false);
                }
                ImGui::EndListBox();
            }
        }
    }
}
#endif
