Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct PixelShaderInput
{
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 SimplePixelShader(PixelShaderInput IN) : SV_Target
{
    float4 textureColor;

    textureColor = shaderTexture.Sample(SampleType, IN.tex);

    clip(textureColor.a - 0.4);

    //textureColor = float4(IN.normal,1);

    return textureColor;
}
