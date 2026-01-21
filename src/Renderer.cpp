#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"
#include "GeometryObject.h"
#include <glad/glad.h>
#include <iostream>

Renderer::Renderer()
    : m_LineVAO(0)
    , m_LineVBO(0)
{
}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize() {
    SetupShaders();

    // Setup line rendering
    glGenVertexArrays(1, &m_LineVAO);
    glGenBuffers(1, &m_LineVBO);

    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    std::cout << "Renderer initialized" << std::endl;
    return true;
}

void Renderer::Shutdown() {
    if (m_LineVAO != 0) {
        glDeleteVertexArrays(1, &m_LineVAO);
        m_LineVAO = 0;
    }
    if (m_LineVBO != 0) {
        glDeleteBuffers(1, &m_LineVBO);
        m_LineVBO = 0;
    }

    m_DefaultShader.reset();
    m_LineShader.reset();
    m_PointCloudShader.reset();
}

void Renderer::SetupShaders() {
    // Default shader
    m_DefaultShader = std::make_unique<Shader>();
    std::string defaultVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        out vec3 FragPos;
        out vec3 Normal;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";

    std::string defaultFragmentShader = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        
        uniform vec4 objectColor;
        uniform vec3 lightPos;
        uniform vec3 viewPos;
        
        void main() {
            vec3 lightColor = vec3(1.0, 1.0, 1.0);
            
            // Ambient
            float ambientStrength = 0.3;
            vec3 ambient = ambientStrength * lightColor;
            
            // Diffuse
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            
            // Specular
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * lightColor;
            
            vec3 result = (ambient + diffuse + specular) * objectColor.rgb;
            FragColor = vec4(result, objectColor.a);
        }
    )";

    m_DefaultShader->LoadFromSource(defaultVertexShader, defaultFragmentShader);

    // Line shader
    m_LineShader = std::make_unique<Shader>();
    std::string lineVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        
        uniform mat4 view;
        uniform mat4 projection;
        
        out vec3 Color;
        
        void main() {
            Color = aColor;
            gl_Position = projection * view * vec4(aPos, 1.0);
        }
    )";

    std::string lineFragmentShader = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 Color;
        
        void main() {
            FragColor = vec4(Color, 1.0);
        }
    )";

    m_LineShader->LoadFromSource(lineVertexShader, lineFragmentShader);

    // Point cloud shader (optimized for point rendering)
    m_PointCloudShader = std::make_unique<Shader>();
    std::string pointCloudVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec4 aColor;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        out vec4 vColor;

        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            vColor = aColor;
        }
    )";

    std::string pointCloudFragmentShader = R"(
        #version 330 core
        in vec4 vColor;
        out vec4 FragColor;

        void main() {
            FragColor = vColor;
        }
    )";

    m_PointCloudShader->LoadFromSource(pointCloudVertexShader, pointCloudFragmentShader);

    // Grid shader uses the same shader as line shader
    // We'll use m_LineShader directly for grid rendering
}

void Renderer::BeginFrame() {
}

void Renderer::EndFrame() {
}

void Renderer::Clear(const glm::vec4& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void Renderer::RenderGeometry(GeometryObject* object, Camera* camera) {
    if (!object || !object->IsVisible()) return;

    m_DefaultShader->Use();
    m_DefaultShader->SetMat4("model", object->GetModelMatrix());
    m_DefaultShader->SetMat4("view", camera->GetViewMatrix());
    m_DefaultShader->SetMat4("projection", camera->GetProjectionMatrix());
    m_DefaultShader->SetVec4("objectColor", object->GetColor());
    m_DefaultShader->SetVec3("lightPos", glm::vec3(10.0f, 10.0f, 10.0f));
    m_DefaultShader->SetVec3("viewPos", camera->GetPosition());

    object->Render(m_DefaultShader.get());
}

void Renderer::RenderPointCloud(GeometryObject* object, Camera* camera) {
    if (!object || !object->IsVisible()) return;

    m_PointCloudShader->Use();
    m_PointCloudShader->SetMat4("model", object->GetModelMatrix());
    m_PointCloudShader->SetMat4("view", camera->GetViewMatrix());
    m_PointCloudShader->SetMat4("projection", camera->GetProjectionMatrix());

    object->Render(m_PointCloudShader.get());
}

void Renderer::RenderLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, Camera* camera) {
    float vertices[] = {
        start.x, start.y, start.z, color.r, color.g, color.b,
        end.x, end.y, end.z, color.r, color.g, color.b
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    m_LineShader->Use();
    m_LineShader->SetMat4("view", camera->GetViewMatrix());
    m_LineShader->SetMat4("projection", camera->GetProjectionMatrix());

    glBindVertexArray(m_LineVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

