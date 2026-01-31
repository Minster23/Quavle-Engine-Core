#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <any>
#include <unordered_map>
#include <vector>
#include <map>

namespace entity
{
    class components
    {
    public:

        enum type
        {
            OBJECT,
            LIGHT,
            CAMERA,
            HOLDER,
            EMPTY,
			AUDIO,
            OTHER
        };

        enum LightType
        {
            DIRECTIONAL,
            POINT,
            SPOT
        };

        struct Transform
        {
            glm::vec3 location = { 0,0,0 };
            glm::vec3 rotation = { 0,0,0 };
            glm::vec3 scale = { 1,1,1 };

            glm::mat4 localMatrix = glm::mat4(1.0f);
            glm::mat4 worldMatrix = glm::mat4(1.0f);

			void renderUI();

        };

        struct Data
        {
            std::string Name = "Unnamed";
            std::string Parent;
            std::string UUID = "";
            bool show_ = true;
            type typeObject = OBJECT;

			void renderUI();
        };

        struct Texture
        {
            struct Location
            {
                std::string Diffuse = "";
                std::string Specular = "";
                std::string Metalness = "";
                std::string Normal = "";
                std::string Height = "";
                std::string Ambient = "";
                std::string emissive = "";
                std::string opacity = "";
                std::string displacement = "";
            } location;

            GLuint Diffuse = 0;
            GLuint Specular = 0;
            GLuint Metalness = 0;
            GLuint Normal = 0;
            GLuint Height = 0;
            GLuint Ambient = 0;
            GLuint emissive = 0;
            GLuint opacity = 0;
            GLuint displacement = 0;

            void renderUI();
        };

        struct Material
        {
            glm::vec3 baseColor = { 1.0f, 1.0f, 1.0f };

            float roughness = 1.0f;
            float metallic = 0.0f;
            float emissivePower = 0.0f;
            float normalStrength = 1.0f;

            Texture textures;

            void renderUI();
        };

        struct Light
        {
            LightType lightType = DIRECTIONAL;
            glm::vec3 color = { 1.0f, 1.0f, 1.0f };
            float intensity = 1.0f;
            float range = 10.0f;
            float innerCutoff = glm::radians(12.5f);
            float outerCutoff = glm::radians(17.5f);
            glm::vec3 direction = { -0.2f, -1.0f, -0.3f };

            void renderUI();
        };

        struct Mesh
        {
            std::vector<unsigned int> indices;
            GLuint VAO = 0;
            GLuint VBO = 0;
            GLuint EBO = 0;
            bool pass_Shadow = true;
        };

        struct Shader 
        {
            GLuint ShaderProgram;
            bool uploadedToGPU = false;
        };

        struct Camera
        {

            void renderUI();
            float fov = 60.0f;
            float nearPlane = 0.1f;
            float farPlane = 1000.0f;
            float aspect = 16.0f / 9.0f;
            bool isActive = true;
			bool isMain = false;
            GLuint fbo, tex, rbo;

            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 rotation = glm::vec3(0.0f);

            glm::mat4 viewMatrix;
            glm::mat4 projectionMatrix;

            void UpdateProjection()
            {
                projectionMatrix =
                    glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
            }

            void UpdateView()
            {
                glm::vec3 direction;
                direction.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
                direction.y = sin(glm::radians(rotation.x));
                direction.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));

                glm::vec3 target = position + glm::normalize(direction);
                glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

                viewMatrix = glm::lookAt(position, target, upVector);
            }

            void cameraRotation(float xoffset, float yoffset)
            {
                if(this->isActive)
                {
                    this->rotation.y += xoffset;
                    this->rotation.x += yoffset;

                    if (this->rotation.x > 89.0f)
                        this->rotation.x = 89.0f;
                    if (this->rotation.x < -89.0f)
                        this->rotation.x = -89.0f;
                }
            }

            glm::vec3 Forward() const
            {
                return glm::normalize(glm::vec3(
                    cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x)),
                    sin(glm::radians(rotation.x)),
                    sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x))
                ));
            }

            glm::vec3 Right() const
            {
                return glm::normalize(glm::cross(Forward(), glm::vec3(0, 1, 0)));
            }

            glm::vec3 Up() const
            {
                return glm::normalize(glm::cross(Right(), Forward()));
            }
        };

        struct Collusion
        {
            bool isHit_;
            glm::vec3 AABBmax;
            glm::vec3 AABBmin;
        };

        struct Audio
        {
			void renderUI();
            std::string audioPath = "";
            bool isPlaying = false;
			bool play = false;
            float volume = 1.0f;
            bool loop = false;
			bool spatial = false;
            struct porgress
            {
                float duration = 0.0f;
                float currentTime = 0.0f;
			} progress;
		};

        static std::unordered_map<std::string, Data>      entity_data;
        static std::unordered_map<std::string, Transform> entity_transform;
        static std::unordered_map<std::string, Texture>   entity_texture;
        static std::unordered_map<std::string, Material>  entity_material;
        static std::unordered_map<std::string, Light>     entity_light;
        static std::unordered_map<std::string, Mesh>      entity_mesh;
        static std::unordered_map<std::string, Shader>    entity_shader;
        static std::unordered_map<std::string, Camera>    entity_camera;
        static std::unordered_map<std::string, Collusion> entity_collusion;
        static std::unordered_map<std::string, Audio>     entity_audio;
        GLuint GLUInit(const std::string& filePath);

#define COMPONENT_ENTRY(NAME, MAP, TYPE)                  \
{                                                         \
    NAME,                                                 \
    [](const std::string& id)                             \
    {                                                     \
        if (MAP.contains(id)) return;                     \
        MAP[id] = TYPE{};                                 \
    },                                                    \
    [](const std::string& id)                             \
    {                                                     \
        auto it = MAP.find(id);                           \
        if (it == MAP.end()) return;                      \
        it->second.renderUI();                            \
    }                                                     \
}


        struct CompoenentsList
        {
            std::string name;
            std::function<void(const std::string&)> add;
			std::function<void(const std::string&)> callRender;
        };

		static std::vector<CompoenentsList> ComponentsList;
        void prepareComponentsList();

    };

	class gameObject
	{
    public:
        void addObject(const char* UUID, const char* name, components::type type);
        void draw();
        void drawShadow(unsigned int shadowProgram, std::string UUID);
        unsigned int loadShader(const char* location_Vertex, const char* location_Fragment, const char* UUID);
        void loadMesh(const char* UUID);
        void childrenHandler();
		void addAudio(const char* Location, const char* UUID);
		void playAudio(const char* UUID);
		void stopAudio(const char* UUID);
		void removeAudio(const char* UUID);
        void addListener(const char* UUID);
        void updateAudio();
        static std::unordered_map<std::string, std::vector<glm::vec3>> mesh_world_vertices;
        static components::Camera* activeCamera;
	};

    extern gameObject gameobjects;
    extern components componentss;

    namespace importers
    {
        class entity_importer
        {
        public:
            void load(const char* location, const char* folder);
        };
        extern entity_importer Entity_Importer;
    }

    namespace canvas
    {
        class item 
        {

        public:
            static GLuint textVAO, textVBO;
            struct Character
            {
                GLuint TextureID;
                glm::ivec2 Size;
                glm::ivec2 Bearing;
                GLuint Advance;
            };

            struct Text
            {
                std::string content = "";
                float scale = 1.0f;
                glm::vec3 color = { 1,1,1 };
                std::string renderToCam = "";
				void renderUI();
            };

            struct position
            {
                float x;
                float y;
                void renderUI();
            };

            struct itemData
            {
                std::string UUID = "";
                bool show_ = true;
            };

            struct container
            {
				std::string UUID = "";
				std::vector<std::string> itemListID;
                void renderUI();
            };

            struct Shader
            {
                GLuint ShaderProgram;
                bool uploadedToGPU = false;
            };
            static std::unordered_map<std::string, Text> item_text;
            static std::map<char, Character> characters;
            static std::unordered_map<std::string, position> item_position;
            static std::unordered_map<std::string, itemData> item_itemData;
            static std::unordered_map<std::string, Shader> item_shader;
            static std::unordered_map<std::string, container> item_container;

            enum type
            {
                TEXT,
                HOLDER
			};

            void draw();
			void addItem(const char* UUID, const char* name, type itemType, const char* parentID);
            void LoadFont(const char* Font_Path);
        };
		extern item canvasItems;

    }
}