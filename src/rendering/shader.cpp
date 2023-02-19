#include "shader.h"


std::string loadShader(const char* path, std::string* out) {
    std::string shaderCode = "";
    std::ifstream shaderFile;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        shaderFile.open(path);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "failed to read shader" << std::endl;
    }
    *out = shaderCode;
    return shaderCode;
}

Shader::Shader(const char* vertPath, const char* fragPath, const char* geomPath) {

    std::string tempVertCode;
    std::string tempFragCode;
    std::string tempGeomCode;
    loadShader(vertPath, &tempVertCode);
    loadShader(fragPath, &tempFragCode);
    loadShader(fragPath, &tempGeomCode);

    const char* vertCode = tempVertCode.c_str();
    const char* fragCode = tempFragCode.c_str();

    GLuint vertID, fragID;
    int success;
    char infoLog[512];

    vertID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertID, 1, &vertCode, NULL);
    glCompileShader(vertID);

    glGetShaderiv(vertID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertID, 512, NULL, infoLog);
        std::cout << "failed to compile vertex shader\n" << infoLog << std::endl;
    }

    fragID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragID, 1, &fragCode, NULL);
    glCompileShader(fragID);

    glGetShaderiv(fragID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragID, 512, NULL, infoLog);
        std::cout << "failed to compile fragment shader\n" << infoLog << std::endl;
    }

    GLuint geomID;
    if (geomPath != nullptr) {
        const char* geomCode = tempGeomCode.c_str();

        geomID = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geomID, 1, &geomCode, NULL);
        glCompileShader(geomID);
        
        glGetShaderiv(geomID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(geomID, 512, NULL, infoLog);
            std::cout << "failed to compile geometry shader\n" << infoLog << std::endl;
        }
    }


    ID = glCreateProgram();
    glAttachShader(ID, vertID);
    glAttachShader(ID, fragID);

    if (geomPath != nullptr) {
        glAttachShader(ID, geomID);
    }

    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << " failed to link program\n" << infoLog << std::endl;
    }

    glDeleteShader(vertID);
    glDeleteShader(fragID);
    if (geomPath != nullptr) glDeleteShader(geomID);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}