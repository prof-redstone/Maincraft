#define GLEW_STATIC //using static version of GLEW
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>



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
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window){
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR GLEW" << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;
    /* Loop until the user closes the window */

    float positions[6] = {
        -0.5f,-0.5f,
         0.0f, 0.5f,
         0.5f,-0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);


    std::string VertexSource;
    std::string FragmentSource;
    ParseShader(VertexSource, FragmentSource, "shaders/vertex.shader", "shaders/fragment.shader");
    unsigned int shader = CreateShader(VertexSource, FragmentSource);
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


        glDrawArrays(GL_TRIANGLES, 0, 3);   

        /* Swap front and back buffers */ 
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}