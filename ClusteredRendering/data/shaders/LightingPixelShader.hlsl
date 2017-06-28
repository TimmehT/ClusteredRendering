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
Texture2D normalTexture : register(t2);
Texture2D opacityTexture : register(t3);
StructuredBuffer<Light> lights : register(t4);
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
    // Interping normal can unormalize it, so normalize it;
    IN.normalW = normalize(IN.normalW);

    // Vector to the camera
    float3 toEye = eyePosW.xyz - IN.posW;
	 toEye = normalize(toEye);

    // Init colors
    float4 ambientColor = float4(0.01f,0.01f,0.01f,1)/*mat.GlobalAmbient*/;
    float4 diffuseColor = mat.DiffuseColor;
    float4 specularColor = mat.SpecularColor;
    float3 normal = IN.normalW;
    
    // Check for Diffuse Texture
    if(mat.UseDiffuseTexture)
    {
        diffuseColor = diffuseTexture.Sample(SampleType, IN.texc);
        ambientColor *= diffuseColor;
    }

    // Set up alpha and get any clip masks
    float alpha = diffuseColor.a;
    if (mat.UseOpacityTexture)
    {
        alpha = opacityTexture.Sample(SampleType, IN.texc).r;
    }

    // Discard pixel if alpha is below threshold
    if(alpha < mat.AlphaThreshold )
    {
        discard;
    }

    // Check for normal texture otherwise just use vertex norms
    if(mat.UseNormalTexture)
    {
        float3 normalMapSample = normalTexture.Sample(SampleType, IN.texc).rgb;
        normal = ComputeNormalMapping(normalMapSample, IN.normalW, IN.tangentW, IN.binormalW);

		//return float4(normal, 1);
    }


    // Compute Lighting
    LightingResult lighting = ComputeLighting(lights, mat, toEye, IN.posW, normal);

    if(specularColor.w > 1.0f)
    {
        if (mat.UseSpecularTexture)
        {
            specularColor.rgb = specularTexture.Sample(SampleType, IN.texc).rgb;
        }
        specularColor *= float4(lighting.Specular.rgb, 1.0f);
    }

   diffuseColor *= float4(lighting.Diffuse.rgb, 1.0f);
   
   return float4((ambientColor + diffuseColor + specularColor).rgb, alpha);
}