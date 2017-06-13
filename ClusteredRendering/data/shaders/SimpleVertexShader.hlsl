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
}

struct AppData
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct VertexShaderOutput
{
    float2 tex : TEXCOORD0;
    float4 position : SV_Position;
};

VertexShaderOutput SimpleVertexShader(AppData IN)
{
    VertexShaderOutput OUT;

    matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
    OUT.position = mul(mvp, float4(IN.position, 1.f));
    OUT.tex = IN.tex;

    return OUT;

}