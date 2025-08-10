#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
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


int PBO()
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

    Program program("PBO.vs", "PBO.fs");
    program.UpdateState();


    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 0.0f,   1.0f, 1.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };



    Buffer VBO;
    VBO.Create();
    VBO.Bind(GL_ARRAY_BUFFER);
    VBO.BufferData(sizeof(vertices), vertices);
    VBO.UpdateState();



     
    VertexArray VAO;
    VAO.Create();
    VAO.Bind();
    VAO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    VAO.EnableVertexAttribArray(0);
    VAO.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO.EnableVertexAttribArray(1);
    VAO.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO.EnableVertexAttribArray(2);

    Buffer EBO;
    EBO.Create();
    EBO.Bind(GL_ELEMENT_ARRAY_BUFFER);
    EBO.BufferData(sizeof(indices), indices);
    EBO.UpdateState();

 

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if 1
    Buffer PBO;
  

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load("aa.jpeg", &width, &height, &nrChannels, 0);
    if (data)
    {  
        for (int i = 0; i < width; i++)
        {
            for (int j = 0; j < height; j++)
            {
               // data[3 * (j * width + i) + 0] = 0.5 * (data[3 * (j * width + i) + 0] + 200);
                data[3 * (j * width + i) + 1] = 0.5 * (data[3 * (j * width + i) + 1] + 200);
               // data[3 * (j * width + i) + 2] +=  0;
            }
        }
        PBO.Create();
        PBO.Bind(GL_PIXEL_UNPACK_BUFFER);
        PBO.BufferData(3 * width * height * sizeof(unsigned char), data);
        PBO.UpdateState();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
   // stbi_image_free(data);
#endif
    // render loop
    // -----------
    int fps = 0;
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (int i = 0; i < width; i++)
        { 
            for (int j = 0; j < height; j++)
            {
                data[3 * (j * width + i) + 0] = 0;
                data[3 * (j * width + i) + 1] = fps % 255;
                data[3 * (j * width + i) + 2] = 0;
            }
        }
        fps += 1;
        PBO.BufferData(3 * width * height * sizeof(unsigned char), data);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glGenerateMipmap(GL_TEXTURE_2D);

        

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
    PBO.Delete();
    EBO.Delete();
    program.Delete();


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

