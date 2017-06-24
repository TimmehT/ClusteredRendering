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

//Texture2D diffuseTexture : register(t0);
//Texture2D specularTexture : register(t1);
//Texture2D emissiveTexture : register(t2);
//Texture2D normalTexture : register(t3);
//Texture2D opacityTexture : register(t4);
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

    float3 toEye = eyePosW.xyz - IN.posW;

    float distToEye = length(toEye);

    float4 diffuse = mat.DiffuseColor;
    //if(mat.UseDiffuseTexture)
    //{
    //    float4 diffTex = diffuseTexture.Sample(SampleType, IN.texc);
    //    if(any(diffuse.rgb))
    //    {
    //        diffuse = diffTex;
    //    }
    //    else
    //    {
    //        diffuse = diffTex;
    //    }
    //}

    float alpha = diffuse.a;
    //if(mat.UseOpacityTexture)
    //{
    //    alpha = opacityTexture.Sample(SampleType, IN.texc).r;
    //}

    float4 ambient = mat.AmbientColor;
    
    ambient *= mat.GlobalAmbient;

    float4 emissive = mat.EmissiveColor;
    //if(mat.UseEmmisiveTexture)
    //{
    //    float4 emissiveTex = emissiveTexture.Sample(SampleType, IN.texc);
    //    if(any(emissive.rgb))
    //    {
    //        emissive *= emissiveTex;
    //    }
    //    else
    //    {
    //        emissive = emissiveTex;
    //    }
    //}

    float3 N;

    if(mat.UseNormalTexture)
    {
        float3x3 TBN = float3x3(normalize(IN.tangentW), normalize(IN.binormalW), (IN.normalW));

       // N = ComputeNormalMapping(TBN, normalTexture, SampleType, IN.texc);
    }
    else
    {
        N = IN.normalW;
    }

    float3 P = IN.posW;

    LightingResult lighting = ComputeLighting(lights, mat, eyePosW, P, N);

    diffuse *= float4(lighting.Diffuse.rgb, 1.0f);

    float4 specular = 0;
    //if(mat.SpecularColor.w > 1.0f)
    //{
        specular = mat.SpecularColor;
        //if(mat.UseSpecularTexture)
        //{
        //    float4 specularTex = specularTexture.Sample(SampleType, IN.texc);
        //    if(any(specular.rgb))
        //    {
        //        specular *= specularTex;
        //    }
        //    else
        //    {
        //        specular = specularTex;
        //    }
        //}

        specular *= lighting.Specular;
   // }
   // return float4(IN.normalW, 1);
    //return diffuse;
    return float4((ambient + emissive + diffuse + specular).rgb, alpha * 1);
}