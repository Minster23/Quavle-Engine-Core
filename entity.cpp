#include <glad/glad.h>
#include "entity.h"
#include "window.h"
#include "game.h"
#include "environment.h"
#include "EngineConfig.h"

#if WITH_EDITOR
#include "interface.h"
using namespace interface;
#endif

#include "scene.h"

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include <imgui.h>
#include <QuavleFormat.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace entity;
using namespace window;
using namespace scene;
using namespace game;
using namespace environment;
using namespace QuaveleShaderFormater;

namespace entity
{
    gameObject gameobjects;
    components componentss;
}

windowsHandler windsHemndl;

std::unordered_map<std::string, components::Data> components::entity_data;
std::unordered_map<std::string, components::Transform> components::entity_transform;
std::unordered_map<std::string, components::Texture> components::entity_texture;
std::unordered_map<std::string, components::Material> components::entity_material;
std::unordered_map<std::string, components::Light> components::entity_light;
std::unordered_map<std::string, components::Mesh> components::entity_mesh;
std::unordered_map<std::string, components::Shader> components::entity_shader;
std::unordered_map<std::string, components::Camera> components::entity_camera;
std::unordered_map<std::string, components::Collusion> components::entity_collusion;
std::vector<components::ComponentsList> components::ComponentsLists;
components::Camera* gameObject::activeCamera = nullptr;
std::unordered_map<std::string, std::vector<glm::vec3>> gameObject::mesh_world_vertices;

#if WITH_EDITOR

struct ShaderLocation {
    std::string MeshFragmentShader;
    std::string MeshVertexShader;

    void Init()
    {
        MeshFragmentShader = gameData.projectLocation + "\\saveData\\shader\\fragment.glsl";
        MeshVertexShader = gameData.projectLocation + "\\saveData\\shader\\vertex.glsl";
    }
}shaderLoc;

#else

struct ShaderLocation {
    std::string MeshFragmentShader = "\\game\\shader\\fragment.quashader";
    std::string MeshVertexShader = "\\game\\shader\\vertex.quashader";
	void Init() {}

}shaderLoc;
#endif

void updateMeshWorldData(const std::string& UUID)
{
    if (!components::entity_mesh.count(UUID)) return;

    auto& mesh = components::entity_mesh[UUID];
    auto& tr = components::entity_transform[UUID];

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);

    float* vertices = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    if (!vertices) return;

    gameobjects.mesh_world_vertices[UUID].clear();

    for (size_t i = 0; i < mesh.indices.size(); i++)
    {
        unsigned int idx = mesh.indices[i];
        glm::vec3 pos(vertices[idx * 8 + 0],
            vertices[idx * 8 + 1],
            vertices[idx * 8 + 2]);

        glm::vec4 wp = tr.worldMatrix * glm::vec4(pos, 1.0f);

        gameobjects.mesh_world_vertices[UUID].push_back(glm::vec3(wp));
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
}

bool RayTriangle(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,float& t)
{
    const float EPS = 0.0000001f;

    glm::vec3 e1 = v1 - v0;
    glm::vec3 e2 = v2 - v0;

    glm::vec3 p = glm::cross(dir, e2);
    float det = glm::dot(e1, p);

    if (fabs(det) < EPS) return false; // paralel

    float inv = 1.0f / det;
    glm::vec3 s = orig - v0;

    float u = inv * glm::dot(s, p);
    if (u < 0 || u > 1) return false;

    glm::vec3 q = glm::cross(s, e1);
    float v = inv * glm::dot(dir, q);
    if (v < 0 || u + v > 1) return false;

    t = inv * glm::dot(e2, q);

    return (t > EPS);
}

float raycastMesh(const std::string& UUID, const glm::vec3& rayOrigin, const glm::vec3& rayDir)
{
    if (!gameobjects.mesh_world_vertices.count(UUID)) return -1.0f;

    float closestT = 999999.0f;
    bool hit = false;

    auto& verts = gameobjects.mesh_world_vertices[UUID];

    for (size_t i = 0; i < verts.size(); i += 3)
    {
        glm::vec3 v0 = verts[i + 0];
        glm::vec3 v1 = verts[i + 1];
        glm::vec3 v2 = verts[i + 2];

        float t;
        if (RayTriangle(rayOrigin, rayDir, v0, v1, v2, t))
        {
            if (t < closestT)
            {
                closestT = t;
                hit = true;
            }
        }
    }

    return hit ? closestT : -1.0f;
}

std::string pickObject(const glm::vec3& rayOrigin, const glm::vec3& rayDir)
{
    float best = 999999.0f;
    std::string hit = "";

    for (auto& [uuid, data] : components::entity_data)
    {
        if (data.typeObject != components::OBJECT)
            continue;

        // update world vertices dulu
        updateMeshWorldData(uuid);

        float t = raycastMesh(uuid, rayOrigin, rayDir);
        if (t > 0 && t < best)
        {
            if (data.Parent.size() != 0)
            {
                best = t;
                hit = data.Parent.c_str();
            }else
            {
                best = t;
                hit = uuid;
            }
        }
    }

    return hit;
}

void updateAABB(const std::string& UUID)
{
    if (components::entity_transform.count(UUID) == 0)
        return;

    auto& tr = components::entity_transform[UUID];

    glm::vec3 localMin(-0.5f, -0.5f, -0.5f);
    glm::vec3 localMax(0.5f, 0.5f, 0.5f);

    glm::vec3 worldMin(99999.0f);
    glm::vec3 worldMax(-99999.0f);

    for (int xi = 0; xi < 2; xi++)
        for (int yi = 0; yi < 2; yi++)
            for (int zi = 0; zi < 2; zi++)
            {
                glm::vec3 corner(
                    xi ? localMax.x : localMin.x,
                    yi ? localMax.y : localMin.y,
                    zi ? localMax.z : localMin.z
                );

                glm::vec4 worldPt = tr.worldMatrix * glm::vec4(corner, 1.0f);

                worldMin = glm::min(worldMin, glm::vec3(worldPt));
                worldMax = glm::max(worldMax, glm::vec3(worldPt));
            }

    components::entity_collusion[UUID].AABBmin = worldMin;
    components::entity_collusion[UUID].AABBmax = worldMax;
}

glm::vec3 getRayDirection(double mouseX, double mouseY, int screenW, int screenH)
{
    float x = (2.0f * mouseX) / screenW - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenH;

    glm::vec4 rayClip(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(gameobjects.activeCamera->projectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 worldRay = glm::normalize(glm::vec3(glm::inverse(gameobjects.activeCamera->viewMatrix) * rayEye));

    return worldRay;
}

void entity::gameObject::childrenHandler()
{
    for (auto& [uuid, data] : components::entity_data)
    {
        auto& tr = components::entity_transform[uuid];

        glm::mat4 T = glm::translate(glm::mat4(1.0f), tr.location);
        glm::mat4 R = glm::yawPitchRoll(
            glm::radians(tr.rotation.y),
            glm::radians(tr.rotation.x),
            glm::radians(tr.rotation.z)
        );
        glm::mat4 S = glm::scale(glm::mat4(1.0f), tr.scale);

        tr.localMatrix = T * R * S;

        if (data.Parent.empty())
        {
            tr.worldMatrix = tr.localMatrix;
            continue;
        }

        if (components::entity_data.count(data.Parent))
        {
            auto& parentTr = components::entity_transform[data.Parent];
            tr.worldMatrix = parentTr.worldMatrix * tr.localMatrix;
        }
    }
}

void components::prepareComponentsList()
{
    ComponentsLists.push_back(
        makeComponentEntry<Data>("Data", entity_data)
    );

    ComponentsLists.push_back(
        makeComponentEntry<Transform>("Transform", entity_transform)
    );

    ComponentsLists.push_back(
        makeComponentEntry<Texture>("Texture", entity_texture)
    );

    ComponentsLists.push_back(
        makeComponentEntry<Material>("Material", entity_material)
    );

    ComponentsLists.push_back(
        makeComponentEntry<Light>("Light", entity_light)
    );

    ComponentsLists.push_back(
        makeComponentEntry<Camera>("Camera", entity_camera)
    );

    ComponentsLists.push_back(
        makeComponentEntry<Audio>("Audio", entity_audio)
    );
}

void gameObject::addObject(const char* UUID, const char* name, components::type type)
{
	shaderLoc.Init();
    if (type == components::type::OBJECT)
    {
        components::Material mat_Default;
        components::entity_data[UUID] = { name, "",UUID, true, components::type::OBJECT };
        components::entity_transform[UUID] = { { 0,0,0 }, { 0,0,0 },  { 1,1,1 } };
        components::entity_material[UUID] = mat_Default;
        std::cout << gameData.projectLocation << std::endl;
        loadShader(shaderLoc.MeshVertexShader.c_str(), shaderLoc.MeshFragmentShader.c_str(), UUID);
        loadMesh(UUID);
    }
    else if (type == components::type::CAMERA)
    {
        components::Camera cam_Default;
        components::entity_data[UUID] = { "Camera", "", UUID, true, components::type::CAMERA };
        components::entity_transform[UUID] = { { 0,0,0 }, { 0,0,0 },  { 1,1,1 } };
        components::entity_camera[UUID] = cam_Default;
        components::entity_camera[UUID].width = 1920;
        components::entity_camera[UUID].height = 1080;
        components::entity_camera[UUID].isActive = true;
        components::entity_camera[UUID].UpdateProjection();
        components::entity_camera[UUID].create_framefuffer();

    }
    else if (type == components::type::LIGHT)
    {
        components::Light light_Default;
        components::entity_data[UUID] = { name, "", UUID, true,components::type::LIGHT };
        components::entity_transform[UUID] = { { 0,0,0 }, { 0,0,0 },  { 1,1,1 } };
        components::entity_light[UUID] = light_Default;

    }
    else if (type == components::type::EMPTY)
    {
        components::entity_data[UUID] = { name, "", UUID, true,components::type::EMPTY };
        components::entity_transform[UUID] = { { 0,0,0 }, { 0,0,0 },  { 1,1,1 } };
    }
    else if (type == components::type::HOLDER)
    {
        components::entity_data[UUID] = { name, "", UUID, true,components::type::HOLDER };
	}
    scenesHandler.inputSceneObject(scenesHandler.currentSceneName().c_str(), UUID);
}

void entity::gameObject::drawShadow(unsigned int shadowProgram, std::string UUID)
{
    auto& mesh = components::entity_mesh[UUID];
    auto& transform = components::entity_transform[UUID];

    if (mesh.VAO == 0) return;
    if (mesh.pass_Shadow == false) return;

    glUseProgram(shadowProgram);

    glUniformMatrix4fv(
        glGetUniformLocation(shadowProgram, "model"),
        1, GL_FALSE,
        glm::value_ptr(transform.worldMatrix)
    );

    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
}

void entity::gameObject::draw()
{
    for (const auto& obj : components::entity_data)
    {
        //!! EDIT THE SHADER
        if (obj.second.typeObject == components::type::OBJECT)
        {
			updateAABB(obj.first);
            std::string UUID = obj.first;
            const auto& data = obj.second;

            if (components::entity_mesh.count(UUID) == 0) continue;
            if (components::entity_shader.count(UUID) == 0) continue;
            if (components::entity_transform.count(UUID) == 0) continue;
            if (components::entity_data[UUID].show_ == false) continue;
            if (scenesHandler.canObjectShown(UUID.c_str()) == false) continue;

            auto& mesh = components::entity_mesh[UUID];
            auto& transform = components::entity_transform[UUID];
            auto& shaderComp = components::entity_shader[UUID];
            auto& material = components::entity_material[UUID];
            auto& texture = components::entity_texture[UUID];

            if (mesh.VAO == 0 || shaderComp.ShaderProgram == 0)
                continue;

            glm::mat4 model = transform.worldMatrix;

            glUseProgram(shaderComp.ShaderProgram);

            if (activeCamera)
            {
                GLint viewLoc = glGetUniformLocation(shaderComp.ShaderProgram, "view");
                GLint projLoc = glGetUniformLocation(shaderComp.ShaderProgram, "projection");
                GLint cameraPosLoc = glGetUniformLocation(shaderComp.ShaderProgram, "cameraPos");

                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(activeCamera->viewMatrix));
                glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(activeCamera->projectionMatrix));
                glUniform3f(cameraPosLoc, activeCamera->position.x, activeCamera->position.y, activeCamera->position.z);
            }

            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
            GLint normalMatrixLoc = glGetUniformLocation(shaderComp.ShaderProgram, "normalMatrix");
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));


            GLint baseColorLoc = glGetUniformLocation(shaderComp.ShaderProgram, "baseColor");
            glUniform3f(baseColorLoc, material.baseColor.r, material.baseColor.g, material.baseColor.b);

            GLint ambientLoc = glGetUniformLocation(shaderComp.ShaderProgram, "ambientStrength");
            GLint specularLoc = glGetUniformLocation(shaderComp.ShaderProgram, "specularStrength");
            GLint shininessLoc = glGetUniformLocation(shaderComp.ShaderProgram, "shininess");

            glUniform1f(ambientLoc, 0.1f);
            glUniform1f(specularLoc, 0.5f);
            glUniform1i(shininessLoc, 32);

            if (texture.Diffuse != 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture.Diffuse);
                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "diffuseMap"), 0);
                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "hasDiffuseMap"), 1);
            }
            else {
                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "hasDiffuseMap"), 0);
            }

            if (texture.Specular != 0) {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texture.Specular);
                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "specularMap"), 1);
                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "hasSpecularMap"), 1);
            }
            else {
                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "hasSpecularMap"), 0);
            }

            if (texture.Normal != 0) {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, texture.Normal);
                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "normalMap"), 2);
                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "hasNormalMap"), 1);
            }
            else {
                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "hasNormalMap"), 0);
            }

            if (texture.Metalness != 0) {
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, texture.Metalness);
                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "metalnessMap"), 3);
            }

            if (texture.emissive != 0) {
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, texture.emissive);
                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "emissiveMap"), 4);
            }


            int i = 0;
            for (auto& l : components::entity_light)
            {
                auto& light = l.second;
                auto& trans = components::entity_transform[l.first];

                std::string prefix = "lights[" + std::to_string(i) + "]";

                glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, (prefix + ".type").c_str()), light.lightType);
                glUniform3fv(glGetUniformLocation(shaderComp.ShaderProgram, (prefix + ".color").c_str()), 1, &light.color[0]);
                glUniform1f(glGetUniformLocation(shaderComp.ShaderProgram, (prefix + ".intensity").c_str()), light.intensity);
                glm::vec3 worldPos = glm::vec3(trans.worldMatrix[3]);
                glUniform3fv(glGetUniformLocation(shaderComp.ShaderProgram, (prefix + ".position").c_str()), 1, &worldPos[0]);
                glUniform3fv(glGetUniformLocation(shaderComp.ShaderProgram, (prefix + ".direction").c_str()), 1, &light.direction[0]);
                glUniform1f(glGetUniformLocation(shaderComp.ShaderProgram, (prefix + ".range").c_str()), light.range);
                glUniform1f(glGetUniformLocation(shaderComp.ShaderProgram, (prefix + ".innerCutoff").c_str()), light.innerCutoff);
                glUniform1f(glGetUniformLocation(shaderComp.ShaderProgram, (prefix + ".outerCutoff").c_str()), light.outerCutoff);

                i++;
            }

            glUniform1i(glGetUniformLocation(shaderComp.ShaderProgram, "lightCount"), i);

            GLint modelLoc = glGetUniformLocation(shaderComp.ShaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glBindVertexArray(mesh.VAO);
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);

        }
    }

#if WITH_EDITOR
    if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
    {
        double mx, my;
        glfwGetCursorPos(windsHemndl.window, &mx, &my);

        float vx = (float)mx - sceneViewport.x;
        float vy = (float)my - sceneViewport.y;

        if (vx >= 0 && vy >= 0 && vx <= sceneViewport.width && vy <= sceneViewport.height)
        {
            glm::vec3 rayOrigin = activeCamera->position;
            glm::vec3 rayDir = getRayDirection(vx, vy, 
                                               (int)sceneViewport.width, 
                                               (int)sceneViewport.height);

            std::string hit = pickObject(rayOrigin, rayDir);
            if (!hit.empty())
            {
                Interfaces.setSelectoredObject(hit);
                Interfaces.setDetailId("");
            }
        }
    }

    if (ImGui::IsMouseClicked(1) && !ImGui::IsAnyItemHovered() && ImGui::IsKeyDown(ImGuiKey_LeftShift))
    {
        double mx, my;
        glfwGetCursorPos(windsHemndl.window, &mx, &my);

        float vx = (float)mx - sceneViewport.x;
        float vy = (float)my - sceneViewport.y;

        if (vx >= 0 && vy >= 0 && vx <= sceneViewport.width && vy <= sceneViewport.height)
        {
            glm::vec3 rayOrigin = activeCamera->position;
            glm::vec3 rayDir = getRayDirection(vx, vy,
                (int)sceneViewport.width,
                (int)sceneViewport.height);

            std::string hit = pickObject(rayOrigin, rayDir);
            if (!hit.empty())
            {
                Interfaces.setDetailId(hit);
            }
            else {
                Interfaces.setDetailId("");
            }
        }
    }
#endif
}

inline unsigned int gameObject::loadShader(const char* location_Vertex, const char* location_Fragment, const char* UUID)
{
    auto loadShaderSource = [](const char* filePath) -> std::string
        {
#if WITH_EDITOR
            std::ifstream file(filePath, std::ios::binary);
            if (!file.is_open())
            {
                std::cout << "ERROR: Failed to open shader file: " << filePath << std::endl;
                return "";
            }
            std::stringstream stream;
            stream << file.rdbuf();
            std::string source = stream.str();
            if (source.size() >= 3 &&
                static_cast<unsigned char>(source[0]) == 0xEF &&
                static_cast<unsigned char>(source[1]) == 0xBB &&
                static_cast<unsigned char>(source[2]) == 0xBF)
            {
                source.erase(0, 3);
            }
            return source;
#else
            auto shader = QuaveleShaderFormater::ShaderFormatter::loadFromFile(filePath);
            if (!shader)
            {
                std::cout << "ERROR: Failed to open shader file: " << filePath << std::endl;
                return "";
            }

            std::string source = shader->source();
            // ALSO strip UTF-8 BOM if present in the loaded shader
            if (source.size() >= 3 &&
                static_cast<unsigned char>(source[0]) == 0xEF &&
                static_cast<unsigned char>(source[1]) == 0xBB &&
                static_cast<unsigned char>(source[2]) == 0xBF)
            {
                source.erase(0, 3);
            }
            return source;
#endif
        };

    std::string vertexCode = loadShaderSource(location_Vertex);
    std::string fragmentCode = loadShaderSource(location_Fragment);
    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cout << "ERROR: Failed to load shader sources for " << (UUID ? UUID : "") << std::endl;
        return 0;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    GLint success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR: Vertex shader compilation failed for " << UUID << ":\n" << infoLog << std::endl;
    }
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR: Fragment shader compilation failed for " << UUID << ":\n" << infoLog << std::endl;
    }
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR: Shader program linking failed for " << UUID << ":\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (UUID != nullptr && UUID[0] != '\0')
    {
        components::entity_shader[UUID] = components::Shader{
            shaderProgram,
            true
        };
        std::cout << "Shader loaded successfully for: " << UUID << std::endl;
    }
    return shaderProgram;
}

void gameObject::loadMesh(const char* UUID)
{
    float vertices[] = {
        // Positions          // Normals           // TexCoords

        // Front face (Z+)
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,

        // Back face (Z-)
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

         // Left face (X-)
         -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

         // Right face (X+)
          0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

          // Bottom face (Y-)
          -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
           0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
           0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
          -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

          // Top face (Y+)
          -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
           0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
           0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
          -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    // Indices for the cube using two triangles per face
    std::vector<unsigned int> cubeIndices = {
        // Front face
        0, 1, 2,    2, 3, 0,
        // Back face
        4, 5, 6,    6, 7, 4,
        // Left face
        8, 9, 10,   10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Bottom face
        16, 17, 18, 18, 19, 16,
        // Top face
        20, 21, 22, 22, 23, 20
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(unsigned int),
        cubeIndices.data(), GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    components::entity_mesh[UUID] = components::Mesh{
        cubeIndices,
        VAO,
        VBO,
        EBO
    };
}

GLuint components::GLUInit(const std::string& filePath)
{
    int width, height, channels;

    // Load file gambar
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);

    if (!data)
    {
        std::cout << "Failed to load image: " << filePath << std::endl;
        return 0;
    }

    GLenum format;
    if (channels == 1) format = GL_RED;
    else if (channels == 3) format = GL_RGB;
    else if (channels == 4) format = GL_RGBA;
    else format = GL_RGB;

    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    // Upload ke GPU
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Free memory CPU
    stbi_image_free(data);

    return textureID;
}

void entity::components::Camera::create_framefuffer()
{
    std::cout << "FRAMEBUFFER:: Framebuffer Created\n";
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void entity::components::Camera::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
}

void entity::components::Camera::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void entity::components::Camera::rescale_framebuffer(float w, float h)
{
    if (w <= 0 || h <= 0) return;

    width = (GLsizei)w;
    height = (GLsizei)h;
    aspect = w / h;

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}