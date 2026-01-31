#include "pipeline.h"
#include "entity.h"

using namespace pipeline;
using namespace entity;

namespace pipeline
{
    godrays GodRays;
}

void pipeline::prepareTexCam(int width, int height)
{
    if (gameobjects.activeCamera == nullptr) return;

    if (gameobjects.activeCamera->fbo == 0) {
        glGenFramebuffers(1, &gameobjects.activeCamera->fbo);
    }

    if (gameobjects.activeCamera->tex == 0) {
        glGenTextures(1, &gameobjects.activeCamera->tex);
    }

    if (gameobjects.activeCamera->rbo == 0) {
        glGenRenderbuffers(1, &gameobjects.activeCamera->rbo);
    }

    // Setup framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, gameobjects.activeCamera->fbo);

    // Setup texture
    glBindTexture(GL_TEXTURE_2D, gameobjects.activeCamera->tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    // Set texture parameters for post-processing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gameobjects.activeCamera->tex, 0);

    // Setup renderbuffer for depth/stencil
    glBindRenderbuffer(GL_RENDERBUFFER, gameobjects.activeCamera->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gameobjects.activeCamera->rbo);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// ================= SHADERS =================

static const char* GODRAYS_VS = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
out vec2 vUV;
void main()
{
    vUV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

static const char* GODRAYS_FS = R"(#version 330 core
out vec4 FragColor;
in vec2 vUV;

uniform sampler2D uScene;
uniform sampler2D uDepth;
uniform vec3 uRayDirection;
uniform float uExposure;
uniform float uDecay;
uniform float uDensity;
uniform float uWeight;
uniform float uScatterIntensity;
uniform float uDebug;

const int NUM_SAMPLES = 128;
const float MAX_RAY_LENGTH = 100.0;

// Simple noise for variation
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Phase function for directional scattering
float directionalPhase(float cosTheta) {
    // Emphasize rays coming from above
    return max(0.0, cosTheta * 0.5 + 0.5);
}

void main()
{
    vec3 sceneColor = texture(uScene, vUV).rgb;
    float sceneDepth = texture(uDepth, vUV).r;
    
    // Fixed directional ray from top to bottom (world space: (0, -1, 0))
    // Convert to view space for consistency
    vec3 rayDir = normalize(uRayDirection);
    
    // Calculate ray progression based on UV (top-down effect)
    vec2 rayStart = vec2(0.5, 1.0); // Start from top center
    vec2 rayVec = vUV - rayStart;
    
    // Strength based on vertical position (stronger at top, weaker at bottom)
    float verticalStrength = 1.0 - smoothstep(0.0, 1.0, vUV.y);
    
    // Directional factor - stronger when aligned with ray direction
    float directionalFactor = directionalPhase(dot(normalize(vec3(rayVec, 0.0)), vec3(rayDir.xy, 0.0)));
    
    // Early exit for bottom areas
    if (verticalStrength < 0.1) {
        FragColor = vec4(sceneColor, 1.0);
        return;
    }
    
    // Raymarch parameters
    float rayLength = MAX_RAY_LENGTH * verticalStrength;
    float stepSize = rayLength / float(NUM_SAMPLES);
    
    // Initialize scattering
    float scattering = 0.0;
    float transmittance = 1.0;
    
    // Main raymarching loop
    for(int i = 0; i < NUM_SAMPLES; i++) {
        float t = float(i) / float(NUM_SAMPLES);
        
        // Sample position in UV space following the ray direction
        vec2 sampleUV = vUV - vec2(rayDir.xy) * t * 0.5 * verticalStrength;
        
        // Boundary check
        if(sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0) {
            break;
        }
        
        // Atmospheric density with vertical falloff
        float density = uDensity * exp(-t * 3.0) * verticalStrength;
        
        // Add noise for volumetric variation
        float noiseVal = noise(sampleUV * 10.0 + t * 5.0);
        density *= (0.8 + 0.4 * noiseVal);
        
        // Sample depth for occlusion
        float sampleDepth = texture(uDepth, sampleUV).r;
        
        // Simple occlusion check
        if(sampleDepth < 0.999) {
            density *= 0.2; // Attenuate behind objects
        }
        
        // Directional weight
        float dirWeight = 1.0 - t; // Stronger near source
        dirWeight *= directionalFactor;
        
        // Accumulate scattering
        float sampleScattering = density * dirWeight * uWeight;
        scattering += sampleScattering * transmittance;
        
        // Beer's law attenuation
        transmittance *= exp(-density * stepSize * uDecay);
        
        // Early exit
        if(transmittance < 0.01) break;
    }
    
    // Apply intensity
    vec3 godRays = vec3(scattering * uExposure * uScatterIntensity);
    
    // DEBUG MODE: Red tint for visualization
    if (uDebug > 0.5) {
        godRays = vec3(scattering * 2.0, 0.0, 0.0);
    } else {
        // Normal warm sunlight color
        godRays *= vec3(1.0, 0.95, 0.85);
    }
    
    // Add directional glow
    float distFromTop = vUV.y;
    float topGlow = 1.0 - smoothstep(0.0, 0.3, distFromTop);
    godRays += vec3(1.5, 1.2, 0.8) * topGlow * topGlow;
    
    // Blend with scene (additive blending)
    vec3 finalColor = sceneColor + godRays;
    
    // Optional: highlight bright areas
    float sceneBrightness = dot(sceneColor, vec3(0.299, 0.587, 0.114));
    if(sceneBrightness > 0.7) {
        finalColor += godRays * (sceneBrightness - 0.7) * 0.3;
    }
    
    FragColor = vec4(finalColor, 1.0);
}
)";

// ================= UTIL =================

GLuint godrays::compile(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    // Check compilation
    GLint success;
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << "Shader compilation error: " << log << std::endl;
    }

    return s;
}

// ================= INIT =================

void godrays::init(int width, int height)
{
    screenW = width;
    screenH = height;

    // ---- fullscreen quad ----
    float quad[] = {
        // pos      // uv
        -1.f, -1.f, 0.f, 0.f,  // bottom-left
         1.f, -1.f, 1.f, 0.f,  // bottom-right
         1.f,  1.f, 1.f, 1.f,  // top-right

        -1.f, -1.f, 0.f, 0.f,  // bottom-left
         1.f,  1.f, 1.f, 1.f,  // top-right
        -1.f,  1.f, 0.f, 1.f   // top-left
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    // ---- shader ----
    GLuint vs = compile(GL_VERTEX_SHADER, GODRAYS_VS);
    GLuint fs = compile(GL_FRAGMENT_SHADER, GODRAYS_FS);

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Check linking
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program linking error: " << log << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "uScene"), 0);
    glUniform1i(glGetUniformLocation(program, "uDepth"), 1);

    // Get uniform locations
    locRayDirection = glGetUniformLocation(program, "uRayDirection");
    locExposure = glGetUniformLocation(program, "uExposure");
    locDecay = glGetUniformLocation(program, "uDecay");
    locDensity = glGetUniformLocation(program, "uDensity");
    locWeight = glGetUniformLocation(program, "uWeight");
    locScatterIntensity = glGetUniformLocation(program, "uScatterIntensity");
    locDebug = glGetUniformLocation(program, "uDebug");

    glUseProgram(0);
}

// ================= START (DIRECTIONAL VERSION) =================

void godrays::start(GLuint sceneColorTex, GLuint depthTex)
{
    if (sceneColorTex == 0) return;
    if (gameobjects.activeCamera == nullptr) return;

    // Setup blending
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(program);

    // Fixed directional ray from top to bottom (world space)
    // Direction vector: (0, -1, 0) for straight down
    // Slightly angled for better visual effect: (0.1, -0.98, 0.0)
    glm::vec3 rayDirection = glm::normalize(glm::vec3(0.1f, -0.98f, 0.0f));
    glUniform3f(locRayDirection, rayDirection.x, rayDirection.y, rayDirection.z);

    // Check if any directional light exists for debug mode
    bool hasDirectionalLight = false;
    for (const auto& pair : components::entity_light) {
        const components::Light& light = pair.second;
        if (light.lightType == components::LightType::DIRECTIONAL) {
            hasDirectionalLight = true;
            break;
        }
    }

    // Enable debug mode (red tint) if no directional lights found
    float debugMode = hasDirectionalLight ? 0.0f : 1.0f;
    glUniform1f(locDebug, debugMode);

    // Directional god rays parameters
    glUniform1f(locExposure, 0.6f);           // Overall brightness
    glUniform1f(locDecay, 0.02f);             // Atmospheric absorption
    glUniform1f(locDensity, 1.2f);            // Atmospheric density
    glUniform1f(locWeight, 0.07f);            // Scattering weight per sample
    glUniform1f(locScatterIntensity, 2.5f);   // Overall scattering multiplier

    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneColorTex);

    if (depthTex != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTex);
    }

    // Draw fullscreen quad
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Cleanup
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

// ================= CLEAN =================

void godrays::clean()
{
    if (program) glDeleteProgram(program);
    if (vbo)     glDeleteBuffers(1, &vbo);
    if (vao)     glDeleteVertexArrays(1, &vao);

    program = vao = vbo = 0;
}