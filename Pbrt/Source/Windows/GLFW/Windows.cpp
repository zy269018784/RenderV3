#ifdef OPENGL_RENDER_1
#include <Windows/GLFW/Windows.h>
#include <Render/Render.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

namespace Render {

    Windows* Windows::win = nullptr;
        
    float vertices[] = {
        // positions          // colors           // texture coords
         1.f,  1.f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // top right
         1.f, -1.f, 0.0f,   0.0f, 0.0f, 0.0f,   1.0f, 1.0f, // bottom right
        -1.f, -1.f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, // bottom left
        -1.f,  1.f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    

    Windows::Windows() {
        cout << "Windows C\n";
        // 初始化glfw
        glfwInit();
        // 错误回调函数
        glfwSetErrorCallback(error_callback);
        // 设置opengl
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // To make Apple happy -- should not be needed
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    Windows::Windows(const char* windows_title, int width, int height) : Windows() {
        cout << "Windows B\n";
        // 创建glfw窗口
        window = glfwCreateWindow(width, height, windows_title, nullptr, nullptr);
        if (window) {
            // 设置当前Context
            glfwMakeContextCurrent(window);

            // 关闭垂直同步: A context must be current on the calling thread.
            glfwSwapInterval(1);

            // glad加载opengl
            gladLoadGL();

            //glfwSetWindowRefreshCallback(window, WindowRefreshCallback);
            //glfwSetWindowSizeCallback(window, WindowSizeCallback);
            //glfwSetKeyCallback(window, KeyCallback);
            //glfwSetCursorPosCallback(window, cursor_position_callback);

            // imgui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();

            // Setup Platform/Renderer backends
            const char* glsl_version = "#version 460";
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version);

        }
    }

    Windows::Windows(const char* windows_title, const Scene* scene1) :
        Windows(windows_title, scene1->RenderBuffer->Width(), scene1->RenderBuffer->Height())
    {
        cout << "Windows A\n";
        this->scene = (Scene *)(scene1);
#if 0
        // VBO
        VBO.Create();
        VBO.Bind(GL_ARRAY_BUFFER);
        VBO.BufferData(sizeof(vertices), vertices);

        // VAO
        VAO.Create();
        VAO.Bind();
        VAO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        VAO.EnableVertexAttribArray(0);
        VAO.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        VAO.EnableVertexAttribArray(1);
        VAO.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        VAO.EnableVertexAttribArray(2);

        // EBO
        EBO.Create();
        EBO.Bind(GL_ELEMENT_ARRAY_BUFFER);
        EBO.BufferData(sizeof(indices), indices);
#endif
        // 纹理
        texture = -1;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if 0
        // PBO
        PBO.Create();
        CopyToPBO();
#endif
    }

    Windows::~Windows() {
  
        VAO.Delete();
        VBO.Delete();
        PBO.Delete();
        EBO.Delete();

        // 销毁glfw窗口
        glfwDestroyWindow(window);
        // 退出glfw库
        glfwTerminate();
    }

    const string vShaderSrc =  "#version 460 core\n"
                               "layout(location = 0) in vec3 aPos;\n"
                               "layout(location = 1) in vec3 aColor;\n"
                               "layout(location = 2) in vec2 aTexCoord;\n"
                               "\n"
                               "out vec3 ourColor;\n"
                               "out vec2 TexCoord;\n"
                               "\n"
                               "void main()\n"
                               "{\n"
                               "    gl_Position = vec4(aPos, 1.0);\n"
                               "    ourColor = aColor;\n"
                               "    TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
                               "}\n";
  
    const string fShaderSrc =
        "#version 460 core                               \n"
        "out vec4 FragColor;                             \n"
        "                                                \n"
        "in vec3 ourColor;                               \n"
        "in vec2 TexCoord;                               \n"
        "                                                \n"
        "// texture sampler                              \n"
        "uniform sampler2D texture1;                     \n"
        "                                                \n"
        "void main()                                     \n"
        "{                                               \n"
        "    FragColor = texture(texture1, TexCoord);    \n"
        "}                                               \n";

    void Windows::Run() {

       // Program program("MainWindow.vs", "MainWindow.fs");
       //Program program(vShaderSrc, fShaderSrc, 0);
       //program.Use();

#if 0
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        const char* glsl_version = "#version 460";
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
#endif

        cout << "aaaaaaa\n";


        while (!glfwWindowShouldClose(window)) {        

            glClearColor(0.f, 0.f, 0.f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::SetNextWindowPos({ 0, 0 });
            ImGui::SetNextWindowSize(ImVec2(800, 600));
            if (ImGui::Begin("MainWindow")) {

                ImGui::Text("This is some useful text.");
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwPollEvents();
          //  glfwWaitEvents();
        }
    }

    void Windows::Update() {
  
        // 绘制
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

#if 0
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({ 0, 0 });
        //ImGui::SetNextWindowSize({100, 100});
        ImGui::Begin("Hello, world!"); 
        // we get the screen position of the window
        //ImVec2 pos = ImGui::GetCursorScreenPos();
        //
        //auto size = ImGui::GetWindowSize();
        //
        //glViewport(0, 0, size.x, size.y);
        // and here we can add our created texture as image to ImGui
        // unfortunately we need to use the cast to void* or I didn't find another way tbh
        //ImGui::GetWindowDrawList()->AddImage(
        //    (void*)texture,
        //    ImVec2(pos.x, pos.y),
        //    ImVec2(pos.x + size.x, pos.y + size.y),
        //    ImVec2(0, 1),
        //    ImVec2(1, 0)
        //);

        ImGui::Text("This is some useful text.");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        // 交换缓冲区
        glfwSwapBuffers(window);
    }
   
    void Windows::CopyToPBO() {
        PBO.Bind(GL_PIXEL_UNPACK_BUFFER);
        PBO.BufferData(scene->RenderBuffer->getColorBufferSize(), scene->RenderBuffer->getColorBufferPointer());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scene->RenderBuffer->Width(), scene->RenderBuffer->Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    //void WindowRefreshCallback(GLFWwindow* window) {
    //    if (nullptr == Windows::win)
    //        return;
    //    cout << "window_refresh_callback" << Windows::win->scene->colorBuffer->width() << " " << Windows::win->scene->colorBuffer->height() << endl;
    //    // 渲染
    //    testCuda(Windows::win->scene);
    //    // 拷贝到PBO
    //    Windows::win->CopyToPBO();
    //    // 刷新界面
    //    Windows::win->Update();
    //}
    //
    //void WindowSizeCallback(GLFWwindow* window, int32_t res_x, int32_t res_y) {
    //    if (nullptr == Windows::win)
    //        return;
    //    Windows::win->scene->colorBuffer->resize(res_x, res_y);
    //    cout << "window_size_callback " << res_x << " " << res_y << endl;
    //
    //    glViewport(0, 0, res_x, res_y);
    //    // 交换缓冲区
    //    glfwSwapBuffers(window);
    //}

 
}
#endif
