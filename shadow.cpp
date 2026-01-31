#include "pipeline.h"
#include "entity.h"
#include "window.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

using namespace pipeline;
using namespace entity;
using namespace window;

namespace pipeline
{
    shadow shadows;
}

// Shadow map resolution - defined once
const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 4096;

// Simple depth vertex shader for shadow mapping
const char* depthVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
)";

// Simple depth fragment shader
const char* depthFragmentShaderSource = R"(
#version 330 core
void main()
{
    // gl_FragDepth is written automatically
}
)";

unsigned int compileDepthShader() {
    // Vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &depthVertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check vertex shader compilation
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADOW::VERTEX_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &depthFragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADOW::FRAGMENT_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADOW::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void pipeline::shadow::init()
{
    // Compile depth shader
    depthShader = compileDepthShader();

    // Create framebuffer
    glGenFramebuffers(1, &depthMapFbo);

    // Create depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    // Use LINEAR for smoother shadows
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::SHADOW::FRAMEBUFFER_NOT_COMPLETE" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "Shadow system initialized: " << SHADOW_WIDTH << "x" << SHADOW_HEIGHT << std::endl;
}

void pipeline::shadow::BeginPassShadow()
{
    if (!shadowsEnabled) return;

    // Find first light (prefer directional, but accept any)
    glm::vec3 lightPos(0.0f, 10.0f, 0.0f);
    glm::vec3 lightDir(0.0f, -1.0f, 0.0f);
    int lightType = 1; // Default to directional
    bool foundLight = false;

    // First try to find a directional light
    for (const auto& kv : components::entity_light)
    {
        const auto& light = kv.second;
        if (light.lightType == 1) { // Directional
            const std::string& uuid = kv.first;
            if (components::entity_transform.count(uuid)) {
                lightPos = components::entity_transform[uuid].location;
            }
            lightDir = light.direction;
            lightType = 1;
            foundLight = true;
            break;
        }
    }

    // If no directional light, use first available light
    if (!foundLight) {
        for (const auto& kv : components::entity_light)
        {
            const std::string& uuid = kv.first;
            const auto& light = kv.second;

            if (components::entity_transform.count(uuid)) {
                lightPos = components::entity_transform[uuid].location;
            }

            lightType = light.lightType;

            // For point/spot lights, calculate direction from light to scene center
            if (lightType == 0 || lightType == 2) {
                glm::vec3 sceneCenter(0.0f, 0.0f, 0.0f);
                lightDir = glm::normalize(sceneCenter - lightPos);
            }
            else {
                lightDir = light.direction;
            }

            foundLight = true;
            break;
        }
    }

    // Build light space matrix
    float near_plane = 0.1f;
    float far_plane = 100.0f;
    float ortho_size = 20.0f;

    glm::mat4 lightProjection = glm::ortho(
        -ortho_size, ortho_size,
        -ortho_size, ortho_size,
        near_plane, far_plane
    );

    glm::vec3 lightTarget = lightPos + lightDir;
    glm::vec3 lightUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // Avoid parallel up vector
    if (abs(glm::dot(lightDir, lightUp)) > 0.99f) {
        lightUp = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, lightUp);

    lightSpaceMatrix = lightProjection * lightView;

    // Save current viewport to restore later
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Configure shadow pass rendering
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Enable depth test for shadow pass
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Optional: cull front faces to reduce peter-panning
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // Activate shadow shader
    glUseProgram(depthShader);
    glUniformMatrix4fv(
        glGetUniformLocation(depthShader, "lightSpaceMatrix"),
        1, GL_FALSE,
        glm::value_ptr(lightSpaceMatrix)
    );

    // Now you can render objects using gameobjects.drawShadow()
}

void pipeline::shadow::EndPassShadow()
{
    if (!shadowsEnabled) return;

    // Restore default state
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowsHandler::width, windowsHandler::height);

    // Reset culling
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);

    glUseProgram(0);

    // Bind shadow map to texture unit 5 for main rendering pass
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, depthMap);
}