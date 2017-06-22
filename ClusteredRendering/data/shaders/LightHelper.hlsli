
#define NUM_LIGHTS 8

#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2

struct Light
{
    float4 PositionWS;
	//(16b)
    float4 DirectionWS;
	//(16b)
    float4 Color;
	//(16b)
    float3 Attenuation;
    float SpecIntensity;
	//(16b)
    float Range;
    float SpotAngle;
    bool Enabled;
    uint Type;
	//(16b)
	//(16 * 5 = 80b)
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emmisive;
};

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

float3 ComputeNormalMapping(float3x3 TBN, Texture2D tex, sampler s, float2 texc)
{
    float3 normalMap = tex.Sample(s, texc).xyz;
    normalMap = (2.0f * normalMap) - 1.0f;

    normalMap = mul(normalMap, TBN);
    return normalize(normalMap);
}

float4 ComputeDiffuse(Light light, float3 L, float3 N)
{
    float NdotL = max(0, dot(N, L));
    return light.Color * NdotL;
}

float4 ComputeSpecular(Light light, float3 L, float3 N, float3 V, float a)
{
    float3 R = normalize(reflect(-L, N));
    float RdotV = max(0, dot(R, V));

    return light.Color * pow(RdotV, a);
}

float ComputeAttenuation(float val, float3 atten, float d)
{
    return val / (atten.x + atten.y * d + atten.z * d * d);
}

float ComputeSpotCone(Light light, float3 L)
{
    
    float minCos = cos(radians(light.SpotAngle));
    float maxCos = lerp(minCos, 1, 0.5f);
    float cosAngle = dot(light.DirectionWS.xyz, -L);

    return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult ComputeDirectionalLight(Light light, Material mat, float3 V, float3 N )
{

    LightingResult result;
    // Initialize outputs
    result.Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    result.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    

    // Get light vec
    float3 L = -light.DirectionWS.xyz;

    result.Diffuse = ComputeDiffuse(light, L, N);
    result.Specular = ComputeSpecular(light, L, N, V, mat.Specular.w);

    return result;
}

LightingResult ComputePointLight(Light light, Material mat, float3 V, float3 P, float3 N)
{

    LightingResult result;
    // Init outputs
    result.Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    result.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Get vector from surface to light
    float3 L = light.PositionWS.xyz - P;

    // Get distance from surface to light
    float distance = length(L);

    // Normalize light vector
    L = L / distance;

    // Compute attenuation
    float attenuation = ComputeAttenuation(1.0f, light.Attenuation, distance);

    result.Diffuse = ComputeDiffuse(light, L, N) * attenuation;
    result.Specular = ComputeSpecular(light, L, N, V, mat.Specular.w) * attenuation;

    return result;
}

LightingResult ComputeSpotLight(Light light, Material mat, float3 V, float3 P, float3 N)
{
    LightingResult result;

    // Init outputs
    result.Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    result.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Get vector from surface to light
    float3 L = light.PositionWS.xyz - P;

    // Get distance from surface to light
    float distance = length(L);

    // Normalize light vector
    L = L / distance;

    float attenuation = ComputeAttenuation(1.0f, light.Attenuation, distance);
    float intensity = ComputeSpotCone(light, L);

    result.Diffuse = ComputeDiffuse(light, L, N) * attenuation * intensity;
    result.Specular = ComputeSpecular(light, L, N, V, mat.Specular.w) * attenuation * intensity;

    return result;
}

//LightingResult ComputeLighting(Light lights[], Material mat, float3 eyePos, float3 P, float3 N)
//{
//    float3 V = normalize(eyePos - P);

//    LightingResult totalResult = (LightingResult) 0;

//    [unroll]
//    for (int i = 0; i < NUM_LIGHTS; ++i)
//    {
//        LightingResult result = (LightingResult) 0;

//        // Forget lights that are not enabled
//		//if (!lights[i].Enabled) ;

//        switch (lights[i].Type)
//        {
//        case DIRECTIONAL_LIGHT:
//        {
//            result = ComputeDirectionalLight(lights[i], mat, V, N);
//        }
//        break;
//        case POINT_LIGHT:
//        {
//            result = ComputePointLight(lights[i], mat, V, P, N);
//        }
//        break;
//        case SPOT_LIGHT:
//        {
//            result = ComputeSpotLight(lights[i], mat, V, P, N);
//        }
//        break;
//        }

//        totalResult.Diffuse += result.Diffuse;
//        totalResult.Specular += result.Specular;
//    }

//    return totalResult;
//}