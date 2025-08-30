// Cpp_OpenGL/Cpp_library/src/engine.cpp
#include "cpp_core/engine.hpp"

#include <vector>
#include <string>
#include <cmath>
#include <cstdio>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // perspective, lookAt
#include <glm/gtc/type_ptr.hpp>         // value_ptr

// glad first
// #include <glad/gl.h>
// #include <glad/glad.h>
// glad first
#if __has_include(<glad/gl.h>)      // glad2
  #include <glad/gl.h>
  #define CPP_GLAD_V2 1
#elif __has_include(<glad/glad.h>)   // glad1
  #include <glad/glad.h>
  #define CPP_GLAD_V1 1
#else
  #error "glad headers not found: ensure Cpp_library/third_party/glad/include is in the include path"
#endif
// then GLFW
#include <GLFW/glfw3.h>

namespace cpp_core {


// ===== shader utils =====
static GLuint compile(GLenum type, const char* src){
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok=0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if(!ok){ GLint len=0; glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len); std::string log(len, '\0'); glGetShaderInfoLog(s, len, nullptr, log.data()); glDeleteShader(s); throw std::runtime_error("Shader compile failed: "+log); }
    return s;
}

static GLuint link(GLuint vs, GLuint fs){
    GLuint p = glCreateProgram();
    glAttachShader(p, vs); glAttachShader(p, fs);
    glLinkProgram(p);
    GLint ok=0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if(!ok){ GLint len=0; glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len); std::string log(len, '\0'); glGetProgramInfoLog(p, len, nullptr, log.data()); glDeleteProgram(p); throw std::runtime_error("Program link failed: "+log); }
    glDetachShader(p, vs); glDetachShader(p, fs);
    glDeleteShader(vs); glDeleteShader(fs);
    return p;
}

static const char* kVS = R"GLSL(
#version 450 core
layout(location=0) in vec3 aPos;
uniform mat4 uMVP;
uniform float uPointSize;
void main(){
  gl_Position = uMVP * vec4(aPos, 1.0);
  gl_PointSize = uPointSize;
}
)GLSL";

static const char* kFS = R"GLSL(
#version 450 core
out vec4 FragColor;
uniform vec3 uColor;
void main(){
  // simple round points by discarding corners
  vec2 p = gl_PointCoord*2.0 - 1.0;
  if (dot(p,p) > 1.0) discard;
  FragColor = vec4(uColor, 1.0);
}
)GLSL";

void render_scatter(std::span<const float> points, const Camera& cam, const RenderConfig& cfg){
    if (points.size() % 3 != 0) throw std::runtime_error("points must be N*3 floats");
    size_t count = points.size()/3;


    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); 

    if (!glfwInit()) throw std::runtime_error("Failed to init GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    GLFWwindow* win = glfwCreateWindow(cfg.width, cfg.height, "Cpp_OpenGL Scatter", nullptr, nullptr);
    if (!win){ glfwTerminate(); throw std::runtime_error("Failed to create window"); }

    glfwMakeContextCurrent(win);
    glfwSwapInterval(1); // vsync

    // if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)){
    //     glfwDestroyWindow(win); glfwTerminate();
    //     throw std::runtime_error("Failed to load OpenGL via glad");
    // }

    #ifdef CPP_GLAD_V2
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        glfwDestroyWindow(win); glfwTerminate();
        throw std::runtime_error("Failed to load OpenGL via glad");
    }
    #elif defined(CPP_GLAD_V1)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(win); glfwTerminate();
        throw std::runtime_error("Failed to load OpenGL via glad");
    }
    #endif


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // shader
    GLuint vs = compile(GL_VERTEX_SHADER, kVS);
    GLuint fs = compile(GL_FRAGMENT_SHADER, kFS);
    GLuint prog = link(vs, fs);

    // buffers
    GLuint vao=0, vbo=0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points.size()*sizeof(float), points.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);

    // uniforms
    glUseProgram(prog);
    GLint locMVP = glGetUniformLocation(prog, "uMVP");
    GLint locPS  = glGetUniformLocation(prog, "uPointSize");
    GLint locCol = glGetUniformLocation(prog, "uColor");

    // const float aspect = (float)cfg.width / (float)cfg.height;
    // Mat4 P = perspective(cam.fov_deg, aspect, cam.near_plane, cam.far_plane);
    // Mat4 V = lookAt(cam.eye, cam.center, cam.up);
    // Mat4 VP = multiply(P, V);


    int fb_w_, fb_h_; glfwGetFramebufferSize(win, &fb_w_, &fb_h_);
    glViewport(0,0,fb_w_,fb_h_);
    float aspect = (float)fb_w_ / (float)fb_h_;

    glm::mat4 P = glm::perspective(glm::radians(cam.fov_deg),
                                aspect, cam.near_plane, cam.far_plane);
    glm::mat4 V = glm::lookAt(glm::vec3(cam.eye.x,    cam.eye.y,    cam.eye.z),
                            glm::vec3(cam.center.x, cam.center.y, cam.center.z),
                            glm::vec3(cam.up.x,     cam.up.y,     cam.up.z));
    glm::mat4 VP = P * V;

    // Column-major upload (no transpose)
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, glm::value_ptr(VP));




    glUniform1f(locPS, cfg.point_size);
    glUniform3f(locCol, cfg.color_r, cfg.color_g, cfg.color_b);

    while (!glfwWindowShouldClose(win)){
        glfwPollEvents();

        int fb_w, fb_h; glfwGetFramebufferSize(win, &fb_w, &fb_h);
        glViewport(0,0,fb_w,fb_h);
        glClearColor(cfg.bg_r, cfg.bg_g, cfg.bg_b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // // update projection if window resized
        // float a = (float)fb_w / (float)fb_h;
        // P = perspective(cam.fov_deg, a, cam.near_plane, cam.far_plane);
        // VP = multiply(P, V);
        // glUniformMatrix4fv(locMVP, 1, GL_FALSE, VP.m);
        // // glUniformMatrix4fv(locMVP, 1, GL_TRUE, VP.m);
        glUniformMatrix4fv(locMVP, 1, GL_FALSE, glm::value_ptr(VP));

        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, (GLsizei)count);

        glfwSwapBuffers(win);

        // optional: press ESC to close
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(win, 1);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(prog);

    glfwDestroyWindow(win);
    glfwTerminate();
}

} // namespace cpp_core