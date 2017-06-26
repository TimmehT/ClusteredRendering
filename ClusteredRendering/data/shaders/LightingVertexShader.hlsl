
cbuffer cbPerObject : register(b0)
{
    float4x4 world;
    float4x4 worldInvTranspose;
    float4x4 worldViewProj;
}

struct AppData
{
    float3 posL : POSITION;
    float3 normalL : NORMAL;
    float2 texc : TEXCOORD0;
    float3 tangentL : TANGENT;
    float3 binormalL : BINORMAL;
};

struct VertexShaderOutput
{
    float4 posH : SV_Position;
    float3 posW : POSITION;
    float3 normalW : NORMAL;
    float2 texc : TEXCOORD0;
    float3 tangentW : TANGENT;
    float3 binormalW : BINORMAL;
};

VertexShaderOutput main(AppData IN)
{
    VertexShaderOutput OUT;

    OUT.posW = mul(IN.posL, (float3x3)world);
    OUT.normalW = mul(IN.normalL,(float3x3)worldInvTranspose);
    OUT.texc = IN.texc;
    OUT.tangentW = mul(IN.tangentL, (float3x3) world);
    OUT.binormalW = mul(IN.binormalL, (float3x3) world);

    OUT.posH = mul(float4(IN.posL, 1.0f), worldViewProj);
 
    return OUT;
}

