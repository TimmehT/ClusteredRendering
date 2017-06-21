#include "LightHelper.hlsli"


Texture2D diffuseTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer MaterialProperties : register(b0)
{
    Material mat;
};

cbuffer LightProperties : register(b1)
{
    float4 eyePosW;
    PointLight lights[8];
};

struct PixelShaderInput
{
    float3 posW : POSITION;
    float3 normalW : NORMAL;
    float2 tex : TEXCOORD0;
};

float4 main(PixelShaderInput IN) : SV_TARGET
{
    IN.normalW = normalize(IN.normalW);

    float3 toEye = eyePosW.xyz - IN.posW;

    float distToEye = length(toEye);

    float4 texColor = diffuseTexture.Sample(SampleType, IN.tex);
    clip(texColor.a - 0.1f);

    float4 litColor = texColor;
    if (8 > 0)
    {
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

        [unroll]
        for (int i = 0; i < 8; i++)
        {
            float4 A, D, S;
            ComputePointLight(mat, lights[i],IN.posW, IN.normalW, toEye, A, D, S);

            ambient += A;
            diffuse += D;
            specular += S;
        }

        litColor = texColor * (ambient + diffuse) + specular;
    }

    return litColor;
}