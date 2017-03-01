struct PixelShaderInput
{
    float4 color : COLOR;
};

float4 SimplePixelShader(PixelShaderInput IN) : SV_Target
{
    return IN.color;
}
