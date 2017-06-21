
cbuffer cbPerFrame : register(b0)
{
    
}

cbuffer cbPerObject : register(b1)
{
    float4x4 world;
    float4x4 worldInvTranspose;
    float4x4 worldViewProj;
}

struct AppData
{
    float3 posL : POSITION;
    float3 normalL : NORMAL;
    float2 tex : TEXCOORD0;
};

struct VertexShaderOutput
{
    float4 posH : SV_Position;
    float3 posW : POSITION;
    float3 normalW : NORMAL;
    float2 tex : TEXCOORD0;
    
};

VertexShaderOutput main(AppData IN)
{
    VertexShaderOutput OUT;

    OUT.posW = mul(float4(IN.posL, 1.0f), world).xyz;
    OUT.normalW = mul(IN.normalL, (float3x3) worldInvTranspose);

    OUT.posH = mul(float4(IN.posL, 1.0f), worldViewProj);

    OUT.tex = IN.tex;
 
    return OUT;
}

