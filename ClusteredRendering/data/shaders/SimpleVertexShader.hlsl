cbuffer PerApplication : register(b0)
{
    matrix projectionMatrix;
}

cbuffer PerFrame : register(b1)
{
    matrix viewMatrix;
}

cbuffer PerObject : register(b2)
{
    matrix worldMatrix;
    matrix invWM;
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

    matrix mvp = mul(mul(worldMatrix,viewMatrix), projectionMatrix);
    OUT.position = mul( float4(IN.position, 1.f), mvp);
    OUT.tex = IN.tex;
    OUT.normal = mul(IN.normal,(float3x3)invWM);
    OUT.tangentW = IN.tangent;
    OUT.binormalW = IN.binormal;

    return OUT;

}