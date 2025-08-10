#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <Common/Program.h>
#include <Common/Shader.h>
#include <Common/Buffer.h>
#include <Common/VertexArray.h>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


int VAO()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Program program("VAO.vs", "VAO.fs");
    program.UpdateState();


    float vertices[] = {
       0.5f,  0.5f, 0.0f, 1.f, 1.f, 0.f,// top right
       0.5f, -0.5f, 0.0f, 1.f, 1.f, 0.f,// bottom right
      -0.5f, -0.5f, 0.0f, 1.f, 1.f, 0.f,// bottom left
      -0.5f,  0.5f, 0.0f, 1.f, 1.f, 0.f // top left 
    };

    unsigned int indices[] = {  // note that we start from 0!
      0, 1, 3,  // first Triangle
      1, 2, 3   // second Triangle
    };



    Buffer VBO;
    VBO.Create();
    VBO.Bind(GL_ARRAY_BUFFER);
    VBO.BufferData(sizeof(vertices), vertices);
    VBO.UpdateState();

    Buffer EBO;
    EBO.Create();
    EBO.Bind(GL_ELEMENT_ARRAY_BUFFER);
    EBO.BufferData(sizeof(indices), indices);
    EBO.UpdateState();

    VertexArray VAO;
    VAO.Create();
    VAO.Bind();
    VAO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    VAO.EnableVertexAttribArray(0);
    VAO.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO.EnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    VBO.Unbind(GL_ARRAY_BUFFER);
    EBO.Unbind(GL_ELEMENT_ARRAY_BUFFER);
    VAO.Unbind();


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        program.Use();
        VAO.Bind();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    VAO.Delete();
    VBO.Delete();
    EBO.Delete();
    program.Delete();


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

