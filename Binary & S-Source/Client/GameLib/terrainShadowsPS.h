const char* terrainBasePS = R"(
struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float2 UV2 : TEXCOORD1;
    float3 Normal : TEXCOORD2;
    float4 WorldPos : TEXCOORD4;
    float3 Tangent : TEXCOORD5;
    float4 LightViewPosition1 : TEXCOORD6;
    float4 LightViewPosition2 : TEXCOORD7;
    float3 ScreenPos : TEXCOORD8;
    float4 Diffuse : COLOR0;
};

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
float textureDiffusePower : register(c10);
float SpecularLight : register(c11);
float terrainLightMultiply : register(c14);
float alphaOverride : register(c15);
float isSnow : register(c16);
float uv : register(c17);
float textureSpecularPower : register(c18);
float3 characterPosition : register(c86);
float puddleThreshold : register(c87);
float kingdomSelector : register(c88);
float isNight : register(c89);
float isStorm : register(c90);
float time : register(c73);
float3 PrevieousCharacterPosition : register(c93);
float isMoving : register(c94);

sampler2D DiffuseTexture : register(s0);
sampler2D SplatTexture : register(s1);
sampler2D NormalMap : register(s2);
sampler2D shadowMap1 : register(s3);
sampler2D SpecularMap : register(s4);
sampler2D normalMap1 : register(s5);
sampler2D normalMap2 : register(s6);
sampler2D ReflectionTexture : register(s7);
sampler2D foamTexture : register(s8);
sampler2D AttrTexture : register(s9);
sampler2D AttrTexture2 : register(s10);

static const float PI = 3.1416f;

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
}

float random(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float smoothNoise(float2 uv)
{
    float2 i = floor(uv);
    float2 f = frac(uv);
    f = f * f * (3.0 - 2.0 * f);

    float a = random(i);
    float b = random(i + float2(1.0, 0.0));
    float c = random(i + float2(0.0, 1.0));
    float d = random(i + float2(1.0, 1.0));

    return lerp(lerp(a, b, f.x), lerp(c, d, f.x), f.y);
}

float isWithinPuddle(float2 uv, float3 normal)
{
    float noiseScale = 1.0; // Controls the size of the puddles
    float noiseValue = smoothNoise(uv * noiseScale);

    float puddlePresence = smoothstep(puddleThreshold - 0.4, puddleThreshold + 0.4, noiseValue);

    float flatThreshold = 1e-10; 
    float flatnessFactor = 0.15;

    float smoothX = smoothstep(flatThreshold, flatThreshold + flatnessFactor, abs(normal.x));
    float smoothY = smoothstep(flatThreshold, flatThreshold + flatnessFactor, abs(normal.y));

    float isFlatSurface = 0.0f;
    if (smoothX < 1.0f && smoothY < 1.0f)
    {
        isFlatSurface = smoothstep(0.0, 1.0, 1.0f - max(smoothX, smoothY));
    }

    return puddlePresence * isFlatSurface;
}

float circularWave(float distance, float frequency, float speed, float amplitude)
{
    return amplitude * sin(distance * frequency - time * speed);
}

float3 interpolate(float3 start, float3 end, float t)
{
    return start + t * (end - start);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 reflection = 0.5 * (1.0 - input.ScreenPos.xy);

    float2 foamUV;
    foamUV.x = 0.01 + 0.98 * (input.WorldPos.x * 0.001);
    foamUV.y = 0.01 + 0.98 * (input.WorldPos.y * 0.001);

    float2 timeOffset1 = float2(-time * 0.3 * 0.0001975f, 0.0); // Right to left
    float2 animatedUV1 = foamUV + timeOffset1;

    float2 timeOffset2 = float2(time * 0.3 * 0.0001975f, 0.0); // Left to right
    float2 animatedUV2 = foamUV + timeOffset2;

    float3 normalMapSample1 = tex2D(normalMap1, animatedUV1).rgb;
    float3 normalMapSample2 = tex2D(normalMap2, animatedUV2).rgb;

    float3 blendedNormal = normalize((normalMapSample1 * 2.0 - 1.0) + (normalMapSample2 * 2.0 - 1.0));

    float perturbationScale = 0.025; // Adjust the scale as needed
    float2 perturbedUV = reflection + (blendedNormal.xy * 0.5) * perturbationScale;

    float4 Color = tex2D(DiffuseTexture, input.UV / uv);
    float calculatedAlpha = tex2D(SplatTexture, input.UV2).a; // Splat map coordinates remain unchanged
    Color.a = min(1.0, calculatedAlpha * 1.5f);

    float3 NM = normalize((tex2D(NormalMap, input.UV / uv).rgb * 2.0f) - 1.0f);
    float3 N = normalize(input.Normal * NM);
    float3 L = -LightDirection;

    float diffuseLight = saturate(dot(NM, L));

    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition2.z - 0.0001f;

    float shadowIntensity = tex2D(shadowMap1, LightTexCoord1).r; // Sample shadow map (simplified)

    float3 V = normalize(CameraPos - input.WorldPos.xyz);
    float3 R = reflect(-L, NM);
    float specularComponent;

    specularComponent = pow(saturate(dot(V, R)), Shininess * 100) * 10.5f;

    float3 specularLight = float3(0, 0, 0);
    if (fCurrDepth1 < tex2D(shadowMap1, LightTexCoord1).r)
    {
        float3 specularColor = tex2D(SpecularMap, input.UV / uv).r;
        specularLight = specularComponent * (specularColor * textureSpecularPower);
    }
    else
    {
        float3 specularColor = tex2D(SpecularMap, input.UV / uv).r;
        specularLight = specularComponent * (specularColor * (textureSpecularPower / 3));
    }

    float isPuddle = isWithinPuddle(input.UV, input.Normal);
    float transition = smoothstep(0.01f, 0.03f, isPuddle);
    specularLight *= lerp(0.06f, isPuddle, transition);

    float LightIntensity;

    if (isSnow > 0.5f)
        LightIntensity = 0.04f + 0.06f * diffuseLight + 0.01f * (specularLight / 2.5);
    else
        LightIntensity = 0.01f + 0.2f * diffuseLight + 0.01f * (specularLight / 2.5);

    Color.rgb *= (LightIntensity * textureDiffusePower);

    float3 waveStartPosition = PrevieousCharacterPosition;
    float3 waveEndPosition = characterPosition;
    float3 currentPosition = input.WorldPos.xyz;

    float distanceAlongPath = length(waveEndPosition - waveStartPosition);
    float t = saturate(dot(currentPosition - waveStartPosition, waveEndPosition - waveStartPosition) / (distanceAlongPath * distanceAlongPath));
    float3 interpolatedPosition = interpolate(waveStartPosition, waveEndPosition, t);

    float distanceFromPath = distance(currentPosition, interpolatedPosition);
    
    float maxRange = 80.0f;
    float maxRange2 = 300.0f;
    float inRangeFactor1 = saturate(1.0 - (distanceFromPath / maxRange));
    float inRangeFactor2 = saturate(1.0 - (distanceFromPath / maxRange2));
    
    float waveSpeed = 0.02f;       
    float waveFrequency = 0.2f * PI / 1.7f;    
    float waveAmplitude = 0.02f * inRangeFactor1;  
    float waveAmplitude2 = 0.04f * inRangeFactor2; 
    float waveEffect = 0.0f;
    
    if(isMoving > 0.1f)
        waveEffect = circularWave(distanceFromPath, waveFrequency / 3, waveSpeed / 1.5f, waveAmplitude2);
    else
        waveEffect = circularWave(distanceFromPath / 1.2f, waveFrequency/2, waveSpeed/3, waveAmplitude);

    float reflectionInfluence =  saturate(1.0 - (distanceFromPath / 20000.0)); // Adjust the divisor to control the influence range

    float4 reflectionColor = tex2D(ReflectionTexture, perturbedUV + waveEffect);
    Color.rgb = lerp(Color.rgb, reflectionColor.rgb * 1.5f, isPuddle * reflectionInfluence);

    float2 perturbedUVFoam = uv + (blendedNormal.xy * 0.5) * perturbationScale;
    float4 foamColor = tex2D(foamTexture, perturbedUVFoam + waveEffect);
    Color.rgb = lerp(Color.rgb, foamColor.rgb, isPuddle / 3);

    float3 pointLightColor = float3(1.0f, 0.6f, 0.0f); // Hardcoded orange color

    float4 finalColor = Color;
    // Fog calculation
    float fogFactor = saturate((length(CameraPos - input.WorldPos.xyz) - FogRange.x) / (FogRange.y - FogRange.x));
    finalColor = lerp(finalColor, FogColor, fogFactor);
    finalColor.a = Color.a;

    return finalColor;
}
)";











const char* terrainShadowsPS = R"(
struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 WorldPos : TEXCOORD4;
    float3 Tangent : TEXCOORD5;
};

#define USE_SHADOWS
#define SHADOW_PCF_RADIUS 1
#define SHADOW_PCF_NUM_SAMPLES 10 // 7

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
float Time : register(c12); // Add this to the constants at the bottom
float SpecularLight : register(c11);
float terrainLightMultiply : register(c14);
float3 characterPosition : register(c86);

sampler2D DiffuseTexture : register(s0);
sampler2D ShadowMap1 : register(s1);   // big shadowmap
sampler2D ShadowMap2 : register(s2);   // local (small) shadowmap

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

    // Fetch and transform normal from the diffuse map (pretending it's a normal map)
    float3 NM = normalize((tex2D(DiffuseTexture, input.UV).rgb * 2.0f) - 1.0f);
    float3 N = normalize(input.Normal * NM);
    float3 L = -LightDirection;

    float OriginalLightIntensity = saturate(dot(N, L));

    float distanceToCharacter = length(input.WorldPos.xyz - characterPosition);
    float lightMultiplier = lerp(1.0f, 4.0f, saturate(distanceToCharacter / 10000.0f));
    float LightIntensity = 0.3f + 0.2f * OriginalLightIntensity;
    float3 LightResult = (AmbientColor.rgb + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity * lightMultiplier);



#ifdef USE_SHADOWS
    const float ShadowBias = 0.0001f;
    const float ShadowBias2 = 0.00004f;
    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias2;
	 
    float bigShadow = 0.0f;
    float localShadow = 0.0f;	 
    const float texelScale1 = 1.0f / 1250.0f;
    const float texelScale2 = 1.0f / 600.0f;
	 const float increment = 1.0f / SHADOW_PCF_NUM_SAMPLES;
   
    for (int i = 0; i < SHADOW_PCF_NUM_SAMPLES; ++i) {		 		
            float2 offset = pcfOffsets[i];				
				bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + offset * texelScale1).r ? increment : 0.0f);
				localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + offset * texelScale2).r ? increment : 0.0f);      
    }

	 // Distance to the inner shadow border (0.0 .. 0.5)
    float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);
	 
    float finalShadow = lerp(bigShadow, localShadow, fade);//(shadow2 + (shadow1 * fade)) / 2.0f;
	 //finalShadow = bigShadow + gridVal * 0.8f;
	 float InvertedOpacity = 1.0f - (35.0f / 120.0f);

	finalShadow = lerp(bigShadow, localShadow, fade); 
	finalShadow *= InvertedOpacity;  // Here, if ShadowOpacity is 100 (full), InvertedOpacity will be 0, hence transparent.

	LightResult *= (1.0f - finalShadow);

    if (SpecularLight < 0.5f)  // Use c11 bool to control specular light
    {
        if (finalShadow < 0.30)
        {
            // Compute view direction
            float3 V = normalize(CameraPos - input.WorldPos.xyz);
        
            // Compute reflection vector
            float3 R = reflect(-L, N);
        
            // Compute specular reflection based on alpha intensity
            float SpecularIntensity = pow(saturate(dot(V, R)), Shininess);
        
            // Accumulate specular lighting result
            LightResult += (SpecularIntensity * MaterialSpecular * LightSpecular.rgb)/1.5f;
        }
    }
#endif

    Color.rgb *= LightResult;

    // Fog
    const float fogStart = FogRange.x;
    const float fogEnd = FogRange.y;
    float distance = length(input.WorldPos.xyz - CameraPos);
    float fogFactor = saturate((distance - fogStart) / (fogEnd - fogStart));
    return lerp(Color, FogColor, fogFactor);
}

)";