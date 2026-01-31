#pragma once
#include "shader_s.h"
#include <glm/glm.hpp>
#include <vector>

namespace pipeline
{
	void prepareTexCam(int width, int height);

    class shadow
    {
    public:
        void init();
        void BeginPassShadow();
        void EndPassShadow();

        glm::mat4 lightSpaceMatrix;
        unsigned int depthMap = 0;
        unsigned int depthMapFbo = 0;
        unsigned int depthShader = 0;
        bool shadowsEnabled = true;
    };

    class postProcessing
    {
    public:
        void init(int width, int height);
        void BeginPostProcessing();
        void EndPostProcessing();
        void setColorFilter(glm::vec3 color, float strength);
        void cleanup();
        unsigned int Shader;

    private:
        unsigned int quadVAO = 0;
        unsigned int quadVBO = 0;
        unsigned int framebuffer = 0;
        unsigned int textureColorbuffer = 0;
        unsigned int rbo = 0;
        int screenWidth = 1280;
        int screenHeight = 720;
    };

    class godrays
    {
    public:
        void init(int width, int height);
        void start(GLuint sceneColorTex, GLuint depthTex);
        void clean();

    private:
        // GL objects
        GLuint program = 0;
        GLuint vao = 0;
        GLuint vbo = 0;

        // uniforms
        GLint locLightPos = -1;
        GLint locExposure = -1;
        GLint locDecay = -1;
        GLint locDensity = -1;
        GLint locCameraPos = -1;
        GLint locScatterIntensity = -1;
        GLint locViewMatrix = -1;
        GLint locProjectionMatrix = -1;
        GLint locInvViewProjection = -1;
        GLint locWeight = -1;
		GLint locDebug = -1;
		GLint locRayDirection = -1;

        // internal
        GLuint compile(GLenum type, const char* src);

        int screenW = 0;
        int screenH = 0;
    };

    extern postProcessing postProcessings;
    extern godrays GodRays;
    extern shadow shadows;
}