#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3  cameraPos;
uniform int   lightCount;

struct Light {
    int   type;
    vec3  color;
    float intensity;
    vec3  position;
    vec3  direction;
    float range;
    float innerCutoff;
    float outerCutoff;
};

uniform Light lights[16];

// ── Texture samplers ──────────────────────────────────────────────────────────
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D metalnessMap;   // unit 3
uniform sampler2D roughnessMap;   // unit 4
uniform sampler2D emissiveMap;    // unit 5
uniform sampler2D aoMap;          // unit 6

// ── Texture presence flags ────────────────────────────────────────────────────
uniform bool hasDiffuseMap;
uniform bool hasNormalMap;
uniform bool hasMetalnessMap;
uniform bool hasRoughnessMap;
uniform bool hasEmissiveMap;
uniform bool hasAoMap;

// ── PBR material fallback / dummy values ──────────────────────────────────────
//    These are used when the corresponding texture is not present.
//    C++ side sends: roughness=0.5, metallic=0.0, ao=1.0 as defaults.
uniform vec3  baseColor;    // albedo when no diffuseMap
uniform float roughness;    // dummy default: 0.5
uniform float metallic;     // dummy default: 0.0
uniform float ao;           // ambient occlusion dummy default: 1.0

// ── Constants ─────────────────────────────────────────────────────────────────
const float PI = 3.14159265359;

// ── Normal mapping ────────────────────────────────────────────────────────────
vec3 getNormalFromMap()
{
    if (!hasNormalMap) return normalize(Normal);

    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// ── PBR: GGX Normal Distribution Function ────────────────────────────────────
float DistributionGGX(vec3 N, vec3 H, float r)
{
    float a      = r * r;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / max(denom, 0.0001);
}

// ── PBR: Schlick-GGX Geometry sub-term ───────────────────────────────────────
float GeometrySchlickGGX(float NdotV, float r)
{
    float k = (r + 1.0);
    k = (k * k) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

// ── PBR: Smith combined Geometry term ────────────────────────────────────────
float GeometrySmith(vec3 N, vec3 V, vec3 L, float r)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, r) * GeometrySchlickGGX(NdotL, r);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    // ── Sample material inputs ────────────────────────────────────────────────
    vec3  albedo    = hasDiffuseMap   ? texture(diffuseMap,   TexCoords).rgb : baseColor;
    float metallicV = hasMetalnessMap ? texture(metalnessMap, TexCoords).r   : metallic;
    float roughV    = hasRoughnessMap ? texture(roughnessMap, TexCoords).r   : roughness;
    float aoV       = hasAoMap        ? texture(aoMap,        TexCoords).r   : ao;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(cameraPos - FragPos);

    // ── Dielectric / metal base reflectivity (F0) ────────────────────────────
    //    Dielectrics use 0.04; metals tint with their albedo.
    vec3 F0 = mix(vec3(0.04), albedo, metallicV);

    // ── Accumulate radiance from all lights (Cook-Torrance BRDF) ─────────────
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < min(lightCount, 16); i++)
    {
        Light light = lights[i];
        vec3  L;
        float attenuation = 1.0;

        if (light.type == 0) // Point light
        {
            vec3  lightVec = light.position - FragPos;
            float dist     = length(lightVec);
            L = normalize(lightVec);

            if (light.range > 0.0)
                attenuation = clamp(1.0 - (dist / light.range), 0.0, 1.0);
            else
                attenuation = 1.0 / (1.0 + dist * dist * 0.01);
        }
        else if (light.type == 1) // Directional light
        {
            L           = normalize(-light.direction);
            attenuation = 1.0;
        }
        else if (light.type == 2) // Spotlight
        {
            vec3  lightVec = light.position - FragPos;
            float dist     = length(lightVec);
            L = normalize(lightVec);

            float theta   = dot(L, normalize(-light.direction));
            float epsilon = light.innerCutoff - light.outerCutoff;
            float coneAtt = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
            attenuation   = coneAtt / (1.0 + dist * dist * 0.01);
        }

        vec3  H     = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);

        // Per-light radiance
        vec3 radiance = light.color * light.intensity * attenuation;

        // Cook-Torrance specular BRDF terms
        float NDF = DistributionGGX(N, H, roughV);
        float G   = GeometrySmith(N, V, L, roughV);
        vec3  F   = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3  numerator   = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
        vec3  specular    = numerator / denominator;

        // Energy-conserving diffuse (metals have no diffuse)
        vec3 kD = (vec3(1.0) - F) * (1.0 - metallicV);

        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // ── Ambient (simplified – no IBL) ─────────────────────────────────────────
    //    Replace with image-based lighting when available.
    vec3 ambient = vec3(0.03) * albedo * aoV;

    // ── Emissive ──────────────────────────────────────────────────────────────
    vec3 emissive = hasEmissiveMap ? texture(emissiveMap, TexCoords).rgb : vec3(0.0);

    // ── Combine + Reinhard tone map + gamma correction ────────────────────────
    vec3 result = ambient + Lo + emissive;
    result = result / (result + vec3(1.0));           // Reinhard
    result = pow(result, vec3(1.0 / 2.2));            // gamma

    FragColor = vec4(result, 1.0);
}
