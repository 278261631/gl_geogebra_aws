#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool Initialize();
    void Shutdown();

    bool ShouldClose() const;
    void SwapBuffers();
    void PollEvents();

    GLFWwindow* GetNativeWindow() { return m_Window; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    float GetAspectRatio() const { return (float)m_Width / (float)m_Height; }

    void SetWidth(int width) { m_Width = width; }
    void SetHeight(int height) { m_Height = height; }

private:
    GLFWwindow* m_Window;
    int m_Width;
    int m_Height;
    std::string m_Title;
};

