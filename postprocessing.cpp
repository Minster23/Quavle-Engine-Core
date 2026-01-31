#include "pipeline.h"

using namespace pipeline;

namespace pipeline
{
	postProcessing postProcessings;
}

// Screen quad vertices for post-processing
float quadVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

// Post-processing shaders
const char* postVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;

        out vec2 TexCoords;

        void main()
        {
            TexCoords = aTexCoords;
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
        }
    )";

const char* postFragmentShader = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoords;

        uniform sampler2D screenTexture;
        uniform vec3 colorFilter;
        uniform float filterStrength;

        void main()
        {
            vec3 color = texture(screenTexture, TexCoords).rgb;
            
            // Apply color filter
            vec3 filtered = mix(color, color * colorFilter, filterStrength);
            
            FragColor = vec4(filtered, 1.0);
        }
    )";

// Member variables for post-processing
unsigned int quadVAO = 0;
unsigned int quadVBO = 0;
unsigned int framebuffer = 0;
unsigned int textureColorbuffer = 0;
unsigned int rbo = 0;
int screenWidth = 1280;
int screenHeight = 720;

void postProcessing::init(int width, int height)
{
    screenWidth = width;
    screenHeight = height;

    // Create and compile shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &postVertexShader, NULL);
    glCompileShader(vertexShader);

    // Check vertex shader
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex Shader Error: " << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &postFragmentShader, NULL);
    glCompileShader(fragmentShader);

    // Check fragment shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment Shader Error: " << infoLog << std::endl;
    }

    Shader = glCreateProgram();
    glAttachShader(Shader, vertexShader);
    glAttachShader(Shader, fragmentShader);
    glLinkProgram(Shader);

    // Check program
    glGetProgramiv(Shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(Shader, 512, NULL, infoLog);
        std::cout << "Shader Program Error: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Setup screen quad VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Create framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create texture attachment
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // Create renderbuffer for depth and stencil
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::cout << "Post-processing initialized!" << std::endl;
}

void postProcessing::BeginPostProcessing()
{
    // Bind to framebuffer and draw scene as normal
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST);

    // Use your normal clear color here (not black)
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void postProcessing::EndPostProcessing()
{
    // Now bind back to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    // Clear the default framebuffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the textured quad with post-processing
    glUseProgram(Shader);
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glUniform1i(glGetUniformLocation(Shader, "screenTexture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Re-enable depth test for UI rendering
    glEnable(GL_DEPTH_TEST);
}

void postProcessing::setColorFilter(glm::vec3 color, float strength)
{
    glUseProgram(Shader);
    glUniform3f(glGetUniformLocation(Shader, "colorFilter"), color.r, color.g, color.b);
    glUniform1f(glGetUniformLocation(Shader, "filterStrength"), strength);
}

void postProcessing::cleanup()
{
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteProgram(Shader);
}