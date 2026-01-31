#include "shader_s.h"
#include "entity.h"
#include "scene.h"
#include "random.h"
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace entity::importers;
using namespace scene;

namespace entity
{
    namespace importers
    {
        entity_importer Entity_Importer;
    }
}

void texLoader(aiTextureType type, std::string& location, aiMaterial*& mat, GLuint& texture, const std::string& folder)
{
    if (mat->GetTextureCount(type) > 0)
    {
        aiString texPath;
        if (mat->GetTexture(type, 0, &texPath) == AI_SUCCESS)
        {
            location = texPath.C_Str();

            // Fix: Handle absolute vs relative paths properly
            std::string fullPath;
            if (std::filesystem::exists(location))
            {
                // If it's already an absolute path or exists in current directory
                fullPath = location;
            }
            else
            {
                // Combine with folder
                fullPath = folder + "\\" + location;
            }

            std::cout << "Loading texture: " << fullPath << std::endl;

            if (std::filesystem::exists(fullPath))
            {
                texture = entity::componentss.GLUInit(fullPath);
                if (texture == 0)
                {
                    std::cout << "Failed to load texture: " << fullPath << std::endl;
                }
            }
            else
            {
                std::cout << "Texture file not found: " << fullPath << std::endl;
            }
        }
    }
}

void entity::importers::entity_importer::load(const char* location, const char* folder)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        location,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_ImproveCacheLocality |
        aiProcess_SortByPType |
        aiProcess_GenBoundingBoxes
    );

    if (!scene) {
        std::cerr << "Failed to load model: " << importer.GetErrorString() << std::endl;
        return;
    }

    if (scene->mNumMeshes == 0) {
        std::cerr << "No meshes in model: " << location << std::endl;
        return;
    }

    // Generate a root UUID for the entire model
    std::string rootUUID = GenerateRandomID();

    // First, create a root/empty object as parent for all meshes
    components::Data rootData = {
        std::filesystem::path(location).stem().string(), // Use filename as name
        "", // No parent
        rootUUID,
        true,
        components::type::HOLDER
    };

    components::entity_data[rootUUID] = rootData;
    components::entity_transform[rootUUID] = { {0, 0, 0}, {0, 0, 0}, {1, 1, 1} };

    std::cout << "Created root object: " << rootUUID << " (" << rootData.Name << ")" << std::endl;

    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
    {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        std::string UUID = GenerateRandomID();

        std::vector<float> vertices;
        vertices.reserve(mesh->mNumVertices * 8);

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            vertices.insert(vertices.end(), {
                mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z,
                mesh->HasNormals() ? mesh->mNormals[i].x : 0.0f,
                mesh->HasNormals() ? mesh->mNormals[i].y : 1.0f,
                mesh->HasNormals() ? mesh->mNormals[i].z : 0.0f,
                mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i].x : 0.0f,
                mesh->HasTextureCoords(0) ? 1.0f - mesh->mTextureCoords[0][i].y : 0.0f // Flip Y for OpenGL
                });
        }

        std::vector<unsigned int> indices;
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            const aiFace& face = mesh->mFaces[f];
            if (face.mNumIndices == 3) {
                indices.insert(indices.end(), face.mIndices, face.mIndices + 3);
            }
        }

        // Initialize with default texture
        components::Texture texture;
        texture.Diffuse = 0;
        texture.Specular = 0;
        texture.Normal = 0;
        texture.Metalness = 0;
        texture.emissive = 0;

        if (scene->HasMaterials()) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            std::cout << "Loading textures for material index: " << mesh->mMaterialIndex << std::endl;

            // Try to load each texture type
            texLoader(aiTextureType_DIFFUSE, texture.location.Diffuse, material, texture.Diffuse, folder);
            texLoader(aiTextureType_SPECULAR, texture.location.Specular, material, texture.Specular, folder);
            texLoader(aiTextureType_NORMALS, texture.location.Normal, material, texture.Normal, folder);

            // Try different possible metalness texture types
            if (material->GetTextureCount(aiTextureType_METALNESS) > 0)
                texLoader(aiTextureType_METALNESS, texture.location.Metalness, material, texture.Metalness, folder);
            else if (material->GetTextureCount(aiTextureType_UNKNOWN) > 0)
                texLoader(aiTextureType_UNKNOWN, texture.location.Metalness, material, texture.Metalness, folder);

            texLoader(aiTextureType_EMISSIVE, texture.location.emissive, material, texture.emissive, folder);

            // Also try to get some material properties
            aiColor4D diffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
            if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor))
            {
                std::cout << "Diffuse color: " << diffuseColor.r << ", " << diffuseColor.g << ", " << diffuseColor.b << std::endl;
            }
        }
        else
        {
            std::cout << "No materials found in model" << std::endl;
        }

        components::entity_texture[UUID] = texture;

        // Initialize material with default values
        components::Material material;
        material.baseColor = glm::vec3(1.0f, 1.0f, 1.0f);;
        components::entity_material[UUID] = material;

        // Create the mesh data with parent set to rootUUID
        std::string meshName = mesh->mName.C_Str();
        if (meshName.empty())
        {
            meshName = "Mesh_" + std::to_string(meshIndex);
        }

        components::Data data = {
            meshName,
            rootUUID, // All meshes are children of the root
            UUID,
            true,
            components::type::OBJECT
        };

        aiVector3D AABBmax = mesh->mAABB.mMax;
        aiVector3D AABBmin = mesh->mAABB.mMin;

        components::entity_data[UUID] = data;
        components::entity_transform[UUID] = { {0, 0, 0}, {0, 0, 0}, {1, 1, 1} };
        components::entity_collusion[UUID] = { false, {AABBmax.x, AABBmax.y, AABBmax.z}, {AABBmin.x, AABBmin.y, AABBmin.z} };

        GLuint VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        constexpr int stride = 8 * sizeof(float);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        components::entity_mesh[UUID] = { indices, VAO, VBO, EBO, true }; // Added pass_Shadow = true

        gameobjects.loadShader("vertex.glsl", "fragment.glsl", UUID.c_str());
        scenesHandler.inputSceneObject(scenesHandler.currentSceneName().c_str(), UUID.c_str());

        std::cout << "Loaded mesh: " << UUID << " (Parent: " << rootUUID << ")" << std::endl;
    }

    // Also add the root object to the scene
    scenesHandler.inputSceneObject(scenesHandler.currentSceneName().c_str(), rootUUID.c_str());

    std::cout << "Model loading complete. Total meshes: " << scene->mNumMeshes << std::endl;
}