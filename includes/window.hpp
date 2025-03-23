#pragma once
#include <iostream>
#include <string_view>
#include <memory>
#include <windows.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "renderer.hpp"

class Window {
private:
    GLFWwindow* window;
    std::unique_ptr<Renderer> renderer;
public:
    Window(unsigned int width, unsigned int height, std::string_view title);
    ~Window();
    void run();
    void refactor(unsigned int width, unsigned int height);
    void cleanup();
};