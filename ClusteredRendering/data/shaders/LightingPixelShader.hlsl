

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float Pad;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;
    
    float3 Attenuation;
    float Pad;
};

struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float3 Direction;
    float Spot;

    float3 Attenuation;
    float Pad;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emmisive;
};

//float4 ComputeDiffuse()
//{
//    return float4;
//}

//float4 ComputeSpecular()
//{
//    return float4;
//}

float ComputeAttenuation(float3 atten, float d)
{
    return 1.0f / dot(atten, float3(1.0f, d, d * d));
}

void ComputeDirectionalLight()
{
    
}

void ComputePointLight()
{
    
}

void ComputeSpotLight()
{
    
}



float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}