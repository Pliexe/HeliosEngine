#pragma once

#include "Helios/Core/Exceptions.h"
#include "pch.h"
#include "GLFW/glfw3.h"

namespace Helios::Internal
{
    class GLFWManager
    {
    public:
        // Initializes GLFW if it was not already 
        static bool Initialize()
        {
            if (!s_initialized)
            {
#ifdef HELIOS_PLATFORM_LINUX
                const char* USE_BACKEND = std::getenv("HELIOS_BACKEND_OVERRIDE");
                
                if (USE_BACKEND)
                {
                    if (std::strcmp(USE_BACKEND, "X11") == 0)
                        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
                    if (std::strcmp(USE_BACKEND, "WAYLAND") == 0)
                        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
                }
#endif
                s_initialized = glfwInit();

                if (s_initialized)
                {
                    std::cout << "GLFW Initialized" << std::endl;
                    std::cout << "GLFW version: " << glfwGetVersionString() << std::endl;
                }
                else std::cout << "Unable to initialize GLFW!" << std::endl;
                
                return s_initialized;
            }
            return true;
        }

        static void Shutdown()
        {
            if (s_initialized)
            {
                glfwTerminate();
                s_initialized = false;
            }
        }

    private:
        inline static bool s_initialized = false;
    };
}