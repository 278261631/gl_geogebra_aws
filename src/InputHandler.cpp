#include "InputHandler.h"
#include "Camera.h"
#include "Window.h"
#include <iostream>

static InputHandler* s_Instance = nullptr;

InputHandler::InputHandler(Window* window, Camera* camera)
    : m_Window(window)
    , m_Camera(camera)
    , m_LastMouseX(0.0)
    , m_LastMouseY(0.0)
    , m_FirstMouse(true)
    , m_CameraSpeed(5.0f)
    , m_MouseSensitivity(0.1f)
{
    s_Instance = this;
    m_MousePressed[0] = false;
    m_MousePressed[1] = false;
    m_MousePressed[2] = false;
}

InputHandler::~InputHandler() {
    s_Instance = nullptr;
}

void InputHandler::Initialize() {
    GLFWwindow* window = m_Window->GetNativeWindow();
    
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
}

void InputHandler::Update(float deltaTime) {
    GLFWwindow* window = m_Window->GetNativeWindow();

    // Camera movement with WASD
    float velocity = m_CameraSpeed * deltaTime;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 forward = m_Camera->GetForward();
        m_Camera->SetPosition(m_Camera->GetPosition() + forward * velocity);
        m_Camera->SetTarget(m_Camera->GetTarget() + forward * velocity);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 forward = m_Camera->GetForward();
        m_Camera->SetPosition(m_Camera->GetPosition() - forward * velocity);
        m_Camera->SetTarget(m_Camera->GetTarget() - forward * velocity);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 right = m_Camera->GetRight();
        m_Camera->SetPosition(m_Camera->GetPosition() - right * velocity);
        m_Camera->SetTarget(m_Camera->GetTarget() - right * velocity);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 right = m_Camera->GetRight();
        m_Camera->SetPosition(m_Camera->GetPosition() + right * velocity);
        m_Camera->SetTarget(m_Camera->GetTarget() + right * velocity);
    }
}

void InputHandler::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (!s_Instance) return;

    if (button >= 0 && button < 3) {
        s_Instance->m_MousePressed[button] = (action == GLFW_PRESS);
    }
}

void InputHandler::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!s_Instance) return;

    if (s_Instance->m_FirstMouse) {
        s_Instance->m_LastMouseX = xpos;
        s_Instance->m_LastMouseY = ypos;
        s_Instance->m_FirstMouse = false;
        return;
    }

    double deltaX = xpos - s_Instance->m_LastMouseX;
    double deltaY = s_Instance->m_LastMouseY - ypos;

    s_Instance->m_LastMouseX = xpos;
    s_Instance->m_LastMouseY = ypos;

    // Right mouse button - orbit
    if (s_Instance->m_MousePressed[1]) {
        s_Instance->m_Camera->Orbit(
            (float)deltaX * s_Instance->m_MouseSensitivity,
            (float)deltaY * s_Instance->m_MouseSensitivity
        );
    }
    // Middle mouse button - pan
    else if (s_Instance->m_MousePressed[2]) {
        s_Instance->m_Camera->Pan(
            -(float)deltaX * 0.01f,
            (float)deltaY * 0.01f
        );
    }
}

void InputHandler::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!s_Instance) return;
    // Pass yoffset directly for exponential zoom
    s_Instance->m_Camera->Zoom((float)yoffset);
}

void InputHandler::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }
    }
}

void InputHandler::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    
    if (s_Instance && s_Instance->m_Window) {
        s_Instance->m_Window->SetWidth(width);
        s_Instance->m_Window->SetHeight(height);
        s_Instance->m_Camera->SetAspectRatio((float)width / (float)height);
    }
}

