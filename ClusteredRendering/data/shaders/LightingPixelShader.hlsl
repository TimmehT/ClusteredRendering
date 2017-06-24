#include "LightHelper.hlsli"

cbuffer cbPerFrame : register(b0)
{
    float3 eyePosW;
    float pad;
};

cbuffer MaterialProperties : register(b1)
{
    Material mat;
};

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
Texture2D emissiveTexture : register(t2);
Texture2D normalTexture : register(t3);
Texture2D opacityTexture : register(t4);
StructuredBuffer<Light> lights : register(t5);
SamplerState SampleType : register(s0);

struct PixelShaderInput
{
    float4 posH : SV_Position;
    float3 posW : POSITION;
    float3 normalW : NORMAL;
    float2 texc : TEXCOORD0;
    float3 tangentW : TANGENT;
    float3 binormalW : BINORMAL;
};

float4 main(PixelShaderInput IN) : SV_TARGET
{
    IN.normalW = normalize(IN.normalW);
    float3 P = IN.posW;

    float3 toEye = eyePosW.xyz - IN.posW;

    float distToEye = length(toEye);

    float4 Ka = mat.AmbientColor;
    Ka *= mat.GlobalAmbient;
    float4 Kd = mat.DiffuseColor;
    float4 Ks = mat.SpecularColor;
    float4 Ke = mat.EmissiveColor;
    float3 N = IN.normalW;
    float opacity = Kd.a;

    if(mat.UseDiffuseTexture)
    {
        Kd = diffuseTexture.Sample(SampleType, IN.texc);
    }

    if(mat.UseSpecularTexture)
    {
        Ks = specularTexture.Sample(SampleType, IN.texc);
    }

    if(mat.UseOpacityTexture)
    {
        opacity = opacityTexture.Sample(SampleType, IN.texc).r;
    }

    if(opacity < mat.AlphaThreshold)
    {
        discard;
    }

    if(mat.UseNormalTexture)
    {
        float3x3 TBN = float3x3(normalize(IN.tangentW), normalize(IN.binormalW), (IN.normalW));

       //N = ComputeNormalMapping(TBN, normalTexture, SampleType, IN.texc);
    }

    LightingResult lighting = ComputeLighting(lights, mat, eyePosW, P, N);

    Kd *= float4(lighting.Diffuse.rgb, 1.0f);
    Ks *= lighting.Specular;
   
    return float4((Ka + Ke + Kd + Ks).rgb, opacity * 1);
}