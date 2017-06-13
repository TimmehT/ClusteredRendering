Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct PixelShaderInput
{
    float2 tex : TEXCOORD0;
};

float4 SimplePixelShader(PixelShaderInput IN) : SV_Target
{
    float4 textureColor;

    textureColor = shaderTexture.Sample(SampleType, IN.tex);

    return textureColor;
}
