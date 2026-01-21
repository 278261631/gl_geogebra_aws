#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    ~Shader();

    bool LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);

    void Use() const;
    void Unbind() const;

    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetMat3(const std::string& name, const glm::mat3& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;

    unsigned int GetID() const { return m_ProgramID; }

private:
    bool CompileShader(unsigned int& shader, const std::string& source, unsigned int type);
    bool LinkProgram(unsigned int vertexShader, unsigned int fragmentShader);
    int GetUniformLocation(const std::string& name) const;

    unsigned int m_ProgramID;
};

