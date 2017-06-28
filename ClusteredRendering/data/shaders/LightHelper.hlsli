
#define NUM_LIGHTS 50

#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2

struct Light
{
    float3 PositionWS;
	float Pad;
	//(16b)
    float4 DirectionWS;
	//(16b)
    float4 Color;
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
    float4 GlobalAmbient;
		//-------------------------(16b)
    float4 AmbientColor;
		//-------------------------(16b)
    float4 DiffuseColor;
		//-------------------------(16b)
    float4 SpecularColor;
		//-------------------------(16b)
    bool UseDiffuseTexture;
    bool UseSpecularTexture;
    bool UseNormalTexture;
    bool UseOpacityTexture;
		//-------------------------(16b)
    float AlphaThreshold;
    float3 Pad;
		//-------------------------(16b)
		// 7 * 16 = 112 bytes
};

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

float3 ComputeNormalMapping(float3 normalMapSample, float3 unitNormalW, float3 tangentW, float3 binormalW )
{
    float3 normalT = 2.0f * normalMapSample - 1.0f;

    float3 N = unitNormalW;
    float3 T = normalize(tangentW);
    float3 B = normalize(binormalW);

    if(dot(cross(N, T), B) < 0.0f)
    {
        T *= -1;
    }

    float3x3 TBN = float3x3(T, B, N);

    float3 bumpedNormalW = mul(normalT, TBN);

    return normalize(bumpedNormalW);
}

float ComputeAttenuation(Light light, float distance)
{
    return 1.0f - smoothstep(0, light.Range, distance);
}

float ComputeAtten2(float d, float r)
{

	return 1.0f / pow(((d / r) + 1.0f), 2.0f);
}

float ComputeSpotCone(Light light, float3 L)
{
    
    float minCos = cos(radians(light.SpotAngle));
    float maxCos = lerp(minCos, 1, 0.5f);
    float cosAngle = dot(light.DirectionWS.xyz, -L);

    return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult ComputeDirectionalLight(Light light, Material mat, float3 toEye, float3 normal )
{

    LightingResult result;
    // Initialize outputs
    result.Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    result.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    

    // Get light vec
    float3 lightVec = -light.DirectionWS.xyz;

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.SpecularColor.w);

		result.Diffuse = diffuseFactor * light.Color;
		result.Specular = specFactor * light.Color;
	}

    return result;
}

LightingResult ComputePointLight(Light light, Material mat, float3 toEye, float3 pos, float3 normal)
{

    LightingResult result;
    // Init outputs
    result.Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    result.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Get vector from surface to light
    float3 lightVec = light.PositionWS - pos;

    // Get distance from surface to light
    float distance = length(lightVec);

	if (distance > light.Range)
		return result;

    // Normalize light vector
	lightVec /= distance;

    // Compute attenuation
    float attenuation = ComputeAttenuation(light, distance);

	// Compute diff & spec
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.SpecularColor.w);

		result.Diffuse = diffuseFactor * light.Color;
		result.Specular = specFactor * light.Color;
	}

    result.Diffuse *= attenuation;
    result.Specular *= attenuation;

    return result;
}

LightingResult ComputeSpotLight(Light light, Material mat, float3 toEye, float3 pos, float3 normal)
{
    LightingResult result;

    // Init outputs
    result.Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    result.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Get vector from surface to light
    float3 lightVec = light.PositionWS - pos;

    // Get distance from surface to light
    float distance = length(lightVec);

	if (distance > light.Range)
		return result;

    // Normalize light vector
	lightVec /= distance;

	float diffuseFactor = dot(lightVec, normal);

    float spot = ComputeSpotCone(light, lightVec);
	float attenuation = ComputeAttenuation(light, distance);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.SpecularColor.w);

		result.Diffuse = diffuseFactor * light.Color;
		result.Specular = specFactor * light.Color;
	}

    result.Diffuse *= attenuation * spot;
    result.Specular *= attenuation * spot;

    return result;
}

LightingResult ComputeLighting(StructuredBuffer<Light> lights, Material mat, float3 toEye, float3 pos, float3 normal)
{
    LightingResult totalResult = (LightingResult) 0;

    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        LightingResult result = (LightingResult) 0;

        // Forget lights that are not enabled
		if (!lights[i].Enabled) continue;

        switch (lights[i].Type)
        {
        case DIRECTIONAL_LIGHT:
        {
            result = ComputeDirectionalLight(lights[i], mat, toEye, normal);
        }
        break;
        case POINT_LIGHT:
        {
            result = ComputePointLight(lights[i], mat, toEye, pos, normal);
        }
        break;
        case SPOT_LIGHT:
        {
            result = ComputeSpotLight(lights[i], mat, toEye, pos, normal);
        }
        break;
        }

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
    }

    return totalResult;
}