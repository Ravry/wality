#include "window.hpp"

void SetWindowBehindDektopIcons(HWND hwndBG)
{
    // Get the handle of the desktop window
    HWND hwndDesktop = FindWindow("Progman", NULL);
    if (hwndDesktop) {
        // Send a special message to the desktop window to refresh its children
        SendMessageTimeout(hwndDesktop, 0x052C, 0, 0, SMTO_NORMAL, 1000, NULL);

        // Get the handle of the WorkerW window, which contains the icons
        HWND hwndWorkerW = NULL;
        EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
            HWND p = FindWindowEx(hwnd, NULL, "SHELLDLL_DefView", NULL);
        if (p) {
            *(HWND*)lParam = FindWindowEx(NULL, hwnd, "WorkerW", NULL);
        }
        return TRUE;
            }, (LPARAM)&hwndWorkerW);

        if (hwndWorkerW) {
            // Place the background window behind the WorkerW window (which is behind the icons)
            SetParent(hwndBG, hwndWorkerW);
            SetWindowPos(hwndBG, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam)
{
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 
    std::cout << "::ERROR:: " << message <<  std::endl;
}

Window::Window(unsigned int width, unsigned int height, std::string_view title)
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return;
    }


    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  

    window = glfwCreateWindow(800, 600, title.data(), NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window!" << std::endl;
        cleanup();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD!" << std::endl;
        return;
    }
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glfwSwapInterval(1); // Enables VSync

    glViewport(0, 0, width, height);

    // SetWindowBehindDektopIcons(glfwGetWin32Window(window));
    renderer = std::make_unique<Renderer>();

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        Window* windowObjPtr = static_cast<Window*>(glfwGetWindowUserPointer(window));
        windowObjPtr->refactor(width, height);
    });
}

Window::~Window() { 
    if (window) {
        cleanup();
    }
}

void Window::run() {

    float deltaTime { 0.f };
    float lastFrame { 0.f };

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        renderer->render(deltaTime);
        glfwSwapBuffers(window);
        
        // std::cout << int(1.f / deltaTime) << std::endl;
    }

    cleanup();
}

void Window::refactor(unsigned int width, unsigned int height)
{
    renderer->refactor(width, height);
}

void Window::cleanup()
{
    if (window)
    {
        glfwDestroyWindow(window); 
        window = nullptr;    
    }
    glfwTerminate();
}