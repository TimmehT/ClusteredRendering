#include "LightHelper.hlsli"

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer cbLights : register(b0)
{
    Light light;
}

struct PixelShaderInput
{
    float4 position : SV_Position;
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float3 tangentW : TANGENT;
    float3 binormalW : BINORMAL;
};

float4 SimplePixelShader(PixelShaderInput IN) : SV_Target
{
    IN.normal = normalize(IN.normal);
    IN.tangentW = normalize(IN.tangentW);
    IN.binormalW = normalize(IN.tangentW);
    float4 textureColor;

    textureColor = shaderTexture.Sample(SampleType, IN.tex);

    clip(textureColor.a - 0.1f);

    // Init outputs
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Get light vec
    float3 lightVec = -light.DirectionWS.xyz;

    // Get ambient
    ambient = textureColor * float4(0.1f,0.1f,0.1f,1.0f);

    // Add diff & spec provided surface is in line of sight
    float diffFactor = dot(lightVec, IN.normal);

    // Flatten to avoid dynamic branching
    [flatten]
    if (diffFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, IN.normal);
        //float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

        diffuse = diffFactor * textureColor * light.Color;
        spec = textureColor * light.Color;
    }

    float4 final = (ambient + diffuse) + spec;

    //textureColor = float4(IN.normal, 1);


    return final;
}
