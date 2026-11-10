const char* shadowmapPS = R"(
struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 LightViewSpecular : TEXCOORD4;
    float4 WorldPos : TEXCOORD5;
    float3 Tangent : TEXCOORD6;
};

#define USE_SHADOWS
#define SHADOW_PCF_RADIUS 2
#define SHADOW_PCF_NUM_SAMPLES 10

float3 MaterialDiffuse : register(c0);
float3 LightDirection : register(c1);
float4 LightDiffuse : register(c2);
float4 AmbientColor : register(c3);
float3 CameraPos : register(c4);
float4 FogColor : register(c5);
float2 FogRange : register(c6);
float3 MaterialSpecular : register(c7);
float Shininess : register(c8);
float3 LightSpecular : register(c9);
int ShadowOpacity : register(c10); // 0..100?
float SpecularLight : register(c11);
float Timer : register(c13);
float SpecularPowerMaterial : register(c14);
float specularPower : register(c15);
float kingdomSelector : register(c88);
float isNight : register(c89);
float isStorm : register(c90);

float3 SpecularTrans : register(c91); // Specular translation vector
float SpecularPower : register(c92);  // Specular power value from C++

sampler2D DiffuseTexture : register(s0);
sampler2D SpecularTexture : register(s1);
sampler2D ShadowMap1 : register(s2); // big shadowmap
sampler2D ShadowMap2 : register(s3); // local (small) shadowmap
sampler2D NormalMap : register(s4);

static const float PI = 3.1416f;
static const float2 pcfOffsets[10] = {
    float2(0.0000, 0.0000),
    float2(0.5000, 0.0000),
    float2(0.3536, 0.3536),
    float2(0.0000, 0.5000),
    float2(-0.3536, 0.3536),
    float2(-0.5000, 0.0000),
    float2(-0.3536, -0.3536),
    float2(-0.0000, -0.5000),
    float2(0.3536, -0.3536),
    float2(1.0000, 0.0000), 
};

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 Color = tex2D(DiffuseTexture, input.UV);

    // Fetch and transform normal from normal map
    float2 texelSize = 1.0f / float2(2048.0f, 2048.0f);
    float3 left = tex2D(DiffuseTexture, input.UV + float2(-texelSize.x, 0)).rgb;
    float3 right = tex2D(DiffuseTexture, input.UV + float2(texelSize.x, 0)).rgb;
    float3 top = tex2D(DiffuseTexture, input.UV + float2(0, -texelSize.y)).rgb;
    float3 bottom = tex2D(DiffuseTexture, input.UV + float2(0, texelSize.y)).rgb;

    float3 dx = right - left;
    float3 dy = bottom - top;

    float3 NM;

    NM = normalize(float3(dy.r - dx.r, dy.g - dx.g, 0.25f));

    NM = NM * 0.5 + 0.5; // Adjust to [0, 1] range

    float3 N = normalize(input.Normal * NM);
    float3 L = -LightDirection;
    float3 V = normalize(CameraPos - input.WorldPos.xyz);
    float3 H = normalize(L + V); // Halfway vector

    float LightIntensity = saturate(dot(NM, L));
    float3 LightResult = (AmbientColor.rgb + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity * 0.4f);

    // Specular calculations using reflection vector
    float3 reflectionVector = reflect(-V, N);
    float2 SpecularUV = reflectionVector.xy * 0.4; // Reflection vector to UV space
    SpecularUV += SpecularTrans.xy * 0.5; // Translate UV for animation
    float4 SpecularTex = tex2D(SpecularTexture, SpecularUV);
    float specularIntensity = saturate(dot(N, V));
    float specularFactor = pow(specularIntensity, 0.1f) * SpecularTex.a;

#ifdef USE_SHADOWS
    const float ShadowBias = 0.000025f;
    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias;
	 
    float bigShadow = 0.0f;
    float localShadow = 0.0f;
    const float texelScale1 = 1.0f / 1024.0f;
    const float texelScale2 = 1.0f / 1024.0f;
    const float increment = 1.0f / SHADOW_PCF_NUM_SAMPLES;
   
    for (int i = 0; i < SHADOW_PCF_NUM_SAMPLES; ++i)
    {
        float2 offset = pcfOffsets[i];
        bigShadow += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + offset * texelScale1).r ? increment : 0.0f);
        localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + offset * texelScale2).r ? increment : 0.0f);
    }

    float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);
	 
    float finalShadow = lerp(bigShadow, localShadow, fade);
    float InvertedOpacity = 1.0f - (ShadowOpacity / 120.0f);

    finalShadow = lerp(bigShadow, localShadow, fade);
    finalShadow *= InvertedOpacity;

    LightResult *= (1.0f - finalShadow);

#endif

    // --- INICIO BLOQUE SPECULAR ILUMINADOR ---
    // Specular que ilumina TODO, especialmente sombras
    float3 safeNormal = normalize(input.Normal + float3(0.000001f, 0.000001f, 0.000001f));
    float3 safeView = normalize(CameraPos - input.WorldPos.xyz + float3(0.000001f, 0.000001f, 0.000001f));
    float3 safeLight = normalize(-LightDirection + float3(0.000001f, 0.000001f, 0.000001f));

    // Calcular specular normal
    float3 halfVec = normalize(safeLight + safeView);
    float specDot = saturate(dot(safeNormal, halfVec));
    float specPower = (specDot > 0.001f) ? pow(specDot, 6.0f) : 0.0f;

    // IGNORAR sombras - calcular iluminación base artificial
    float lightInfluence = saturate(dot(safeNormal, safeLight));
    float shadowCompensation = 1.0f - lightInfluence;

    // Crear iluminación artificial más suave
    float3 artificialLight = float3(0.8f, 0.8f, 0.9f);
    float3 specularResult = artificialLight * specPower * 1.5f;

    // Amplificar en sombras para iluminar todo (más suave)
    specularResult += shadowCompensation * artificialLight * 0.2f;

    // Clampear para compatibilidad
    specularResult = clamp(specularResult, 0.0f, 2.0f);

    // Aplicar solo si NO hay sombra
    if (finalShadow < 0.25f && all(isfinite(specularResult)) && !any(isnan(specularResult))) {
        LightResult += specularResult;
    }
    // --- FIN BLOQUE SPECULAR ILUMINADOR ---

    float NdotL = saturate(dot(N, L));
    float3 pointLightColor = float3(1.0f, 0.7f, 0.4f); 
    float3 diffuse = LightIntensity * pointLightColor * NdotL;
    float3 diffuseInverse = 0.2f * (1.0f - NdotL) ;

    float3 R = reflect(-L, N);
    float SpecularIntensity = pow(saturate(dot(V, R)), 8.0f) * 3.5f;
    float3 specular;

    if(finalShadow < 0.25f)
    {
        specular = LightIntensity * SpecularIntensity * pointLightColor * NdotL ;
        LightResult += diffuse;
        LightResult += specular * Color.b * Color.r;
    }

    Color.rgb *= LightResult;

    float3 SpecularColor = SpecularTex.rgb * specularFactor * 2.0f * SpecularPower;
    Color.rgb += SpecularColor * Color.a;

    float contrast = 1.2f;
    Color.rgb = saturate((Color.rgb - 0.5f) * contrast + 0.5f);

    const float fogStart = FogRange.x;
    const float fogEnd = FogRange.y;
    float distance = length(input.WorldPos.xyz - CameraPos);
    float fogFactor = saturate((distance - fogStart) / (fogEnd - fogStart));
    return lerp(Color, FogColor, fogFactor);
}
)";
