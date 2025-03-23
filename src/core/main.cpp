#include "window.hpp"

int main(int argv, char** argc)
{
    Window window(800, 600, "Hello, World!");
    window.run();
    return 0;
}