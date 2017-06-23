cbuffer PerObject : register(b0)
{
    float4x4 world;
    float4x4 worldInvTranspose;
    float4x4 worldViewProj;
}

struct AppData
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct VertexShaderOutput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float3 tangentW : TANGENT;
    float3 binormalW : BINORMAL;
    
};

VertexShaderOutput SimpleVertexShader(AppData IN)
{
    VertexShaderOutput OUT;

    OUT.position = mul( float4(IN.position, 1.f),worldViewProj);
    OUT.tex = IN.tex;
    OUT.normal = mul(IN.normal,(float3x3)worldInvTranspose);
    OUT.tangentW = mul(IN.tangent, (float3x3)world);
    OUT.binormalW = mul(IN.binormal, (float3x3)world);

    return OUT;

}