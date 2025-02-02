#define GLEW_STATIC //using static version of GLEW
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

static void ParseShader(std::string& VertexS, std::string& FragmentS, std::string VertexSourceFile, std::string FragmentSourceFile) {
    // Lecture du vertex shader
    std::ifstream vertexStream(VertexSourceFile);
    if (!vertexStream.is_open()) {
        std::cerr << "Erreur lors de l'ouverture du fichier vertex shader." << std::endl;
        return;
    }

    std::string line;
    std::stringstream vertexSS;
    while (getline(vertexStream, line)) {
        vertexSS << line << '\n';
    }
    VertexS = vertexSS.str();
    vertexStream.close();

    // Lecture du fragment shader
    std::ifstream fragmentStream(FragmentSourceFile);
    if (!fragmentStream.is_open()) {
        std::cerr << "Erreur lors de l'ouverture du fichier fragment shader." << std::endl;
        return;
    }

    std::stringstream fragmentSS;
    while (getline(fragmentStream, line)) {
        fragmentSS << line << '\n';
    }
    FragmentS = fragmentSS.str();
    fragmentStream.close();
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
     unsigned int id = glCreateShader(type);
     const char* src = source.c_str();
     glShaderSource(id, 1, &src, nullptr);
     glCompileShader(id);

     int result;
     glGetShaderiv(id, GL_COMPILE_STATUS, &result);
     if (result == GL_FALSE) {
         int length;
         glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
         char* message = (char*)alloca(length*sizeof(char));
         glGetShaderInfoLog(id, length, &length, message);
         std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader." << std::endl;
         std::cout << message << std::endl;
         glDeleteShader(id);
         return 0;
     }

     return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program); 

    glDeleteShader(vs);
    glDeleteShader(fs); //pour nettoyer

    return program;
}

int main(void)
{
    std::cout << "hello" << std::endl;
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window){
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR GLEW" << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;
    /* Loop until the user closes the window */
    {
        float positions[] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f, 0.5f,
            -0.5f, 0.5f
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        unsigned int vao;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));


        VertexArray va;
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);


        IndexBuffer ib(indices, 6);



        //creation shader et application
        std::string VertexSource;
        std::string FragmentSource;
        ParseShader(VertexSource, FragmentSource, "shaders/vertex.shader", "shaders/fragment.shader");
        unsigned int shader = CreateShader(VertexSource, FragmentSource);
        glUseProgram(shader);

        int location = glGetUniformLocation(shader, "u_Color");
        glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f);

        float r = 0.0f;
        float inc = 0.005f;


        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));
            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
            va.Bind();
            ib.Bind();
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            if (r > 1.0f) inc = -inc;
            if (r < 0.0f) inc = -inc;
            r += inc;
            glUniform4f(location, r, 0.3f, 0.8f, 1.0f);


            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        glDeleteProgram(shader);
    }
    glfwTerminate();
    return 0;
}