#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

class Camera;
class Window;

class InputHandler {
public:
    InputHandler(Window* window, Camera* camera);
    ~InputHandler();

    void Initialize();
    void Update(float deltaTime);

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    Window* m_Window;
    Camera* m_Camera;

    bool m_MousePressed[3];
    double m_LastMouseX;
    double m_LastMouseY;
    bool m_FirstMouse;

    float m_CameraSpeed;
    float m_MouseSensitivity;
};

