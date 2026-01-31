#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform vec3 cameraPos;
uniform int lightCount;

struct Light {
    int type;
    vec3 color;
    float intensity;
    vec3 position;
    vec3 direction;
    float range;
    float innerCutoff;
    float outerCutoff;
};

uniform Light lights[16];

// Texture uniforms
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D metalnessMap;
uniform sampler2D emissiveMap;
uniform sampler2D shadowMap;

// Texture flags
uniform bool hasDiffuseMap;
uniform bool hasSpecularMap;
uniform bool hasNormalMap;
uniform bool hasMetalnessMap;
uniform bool hasEmissiveMap;
uniform bool shadowsEnabled;

// Material properties
uniform vec3 baseColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform int shininess;

// Additional uniforms from C++ code
uniform mat4 lightSpaceMatrix;

// Calculate shadow
float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    if (!shadowsEnabled) return 0.0;

    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    
    // Calculate bias to prevent shadow acne
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    // PCF (Percentage-Closer Filtering) for softer shadows
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

// Get normal from normal map or use vertex normal
vec3 getNormalFromMap()
{
    if (!hasNormalMap) {
        return normalize(Normal);
    }

    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    // Calculate TBN matrix
    vec3 Q1 = dFdx(FragPos);
    vec3 Q2 = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    // Get base color from diffuse map or uniform
    vec3 diffuseColor = hasDiffuseMap ? texture(diffuseMap, TexCoords).rgb : baseColor;
    
    // Get normal
    vec3 norm = getNormalFromMap();
    vec3 viewDir = normalize(cameraPos - FragPos);
    
    // Get specular intensity from specular map
    float specularIntensity = hasSpecularMap ? texture(specularMap, TexCoords).r : specularStrength;
    
    // Get metalness (affects how reflective the surface is)
    float metalness = hasMetalnessMap ? texture(metalnessMap, TexCoords).r : 0.0;
    
    // Ambient lighting
    vec3 ambient = ambientStrength * diffuseColor;
    
    // Lighting accumulation
    vec3 diffuse = vec3(0.0);
    vec3 specularLight = vec3(0.0);
    
    // Find the main directional light for shadows (assuming first light is main)
    vec3 mainLightDir = vec3(0.0);
    float mainShadow = 0.0;
    
    if (lightCount > 0 && lights[0].type == 1) { // Directional light type
        mainLightDir = normalize(-lights[0].direction);
        if (shadowsEnabled) {
            mainShadow = CalculateShadow(FragPosLightSpace, norm, mainLightDir);
        }
    }
    
    for (int i = 0; i < min(lightCount, 16); i++)
    {
        Light light = lights[i];
        vec3 lightDir;
        float attenuation = 1.0;
        
        // Calculate light direction and attenuation based on type
        if (light.type == 0) { // Point light
            vec3 lightVec = light.position - FragPos;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            
            if (light.range > 0.0) {
                attenuation = clamp(1.0 - (distance / light.range), 0.0, 1.0);
            } else {
                attenuation = 1.0 / (1.0 + distance * distance * 0.01);
            }
        }
        else if (light.type == 1) { // Directional light
            lightDir = normalize(-light.direction);
            attenuation = 1.0;
        }
        else if (light.type == 2) { // Spotlight
            vec3 lightVec = light.position - FragPos;
            float distance = length(lightVec);
            lightDir = normalize(lightVec);
            
            float theta = dot(lightDir, normalize(-light.direction));
            float epsilon = light.innerCutoff - light.outerCutoff;
            float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
            
            attenuation = intensity / (1.0 + distance * distance * 0.01);
        }
        
        // Diffuse lighting
        float diff = max(dot(norm, lightDir), 0.0);
        
        // Specular lighting (Blinn-Phong)
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
        
        // Use shadow only for the main directional light
        float shadow = 0.0;
        if (i == 0 && shadowsEnabled && light.type == 1) {
            shadow = mainShadow;
        }
        
        // Accumulate lighting
        vec3 lightContribution = light.color * light.intensity * attenuation * (1.0 - shadow);
        diffuse += diff * lightContribution;
        specularLight += spec * specularIntensity * lightContribution;
    }
    
    // Apply metalness (metallic surfaces reflect more, diffuse less)
    diffuse = mix(diffuse * diffuseColor, diffuse, metalness);
    specularLight = mix(specularLight, specularLight * diffuseColor, metalness);
    
    // Get emissive color
    vec3 emissive = hasEmissiveMap ? texture(emissiveMap, TexCoords).rgb : vec3(0.0);
    
    // Combine all lighting components
    vec3 result = ambient + diffuse + specularLight + emissive;
    
    FragColor = vec4(result, 1.0);
}