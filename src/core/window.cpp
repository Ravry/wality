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

Window::Window(unsigned int width, unsigned int height, std::string_view title)
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return;
    }


    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

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

    glfwSwapInterval(1); // Enables VSync

    SetWindowBehindDektopIcons(glfwGetWin32Window(window));
    renderer = std::make_unique<Renderer>();
}

Window::~Window() { 
    if (window) {
        cleanup();
    }
}

void Window::run() {
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        renderer->render();

        glfwSwapBuffers(window);
    }

    cleanup();
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