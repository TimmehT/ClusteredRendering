struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float Pad;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;
    
    float3 Attenuation;
    float Pad;
};

struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float3 Direction;
    float Spot;

    float3 Attenuation;
    float Pad;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emmisive;
};

float ComputeAttenuation(float val, float3 atten, float dist)
{
    return val / dot(atten, float3(1.0f, dist, dist * dist));
}

float ComputeSpot(float3 lv, float3 dir, float angle)
{
    return pow(max(dot(lv, dir), 0.0f), angle);
}

void ComputeDirectionalLight(Material mat, DirectionalLight light, float3 norm, float3 toEye,
                                out float4 ambient, out float4 diffuse, out float4 spec)
{
    // Init outputs
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Get light vec
    float3 lightVec = -light.Direction;

    // Get ambient
    ambient = mat.Ambient * light.Ambient;

    // Add diff & spec provided surface is in line of sight
    float diffFactor = dot(lightVec, norm);

    // Flatten to avoid dynamic branching
    [flatten]
    if (diffFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, norm);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

        diffuse = diffFactor * mat.Diffuse * light.Diffuse;
        spec = specFactor * mat.Specular * light.Specular;
    }
}

void ComputePointLight(Material mat, PointLight light, float3 surfPos, float3 surfNorm, float3 toEye,
                        out float4 ambient, out float4 diffuse, out float4 specular)
{
    // Init outputs
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Get vector from surface to light
    float3 lightVec = light.Position - surfPos;

    // Get distance from surface to light
    float dist = length(lightVec);

    // Check if surface is in range
    if (dist > light.Range)
        return;

    // Normalize light vector
    lightVec = normalize(lightVec);

    // Get ambient 
    ambient = mat.Ambient * light.Ambient;

    // Add diffuse and specular term, provided the surface is in line of sight of light
    float diffuseFactor = dot(lightVec, surfNorm);

    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, surfNorm);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

        diffuse = diffuseFactor * mat.Diffuse * light.Diffuse;
        specular = specFactor * mat.Specular * light.Specular;
    }

    diffuse *= ComputeAttenuation(1.0f, light.Attenuation, dist);
    specular *= ComputeAttenuation(1.0f, light.Attenuation, dist);
    
}

void ComputeSpotLight(Material mat, SpotLight light, float3 surfPos, float3 surfNorm, float3 toEye,
                        out float4 ambient, out float4 diffuse, out float4 specular)
{
    // Init outputs
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Get vector from surface to light
    float3 lightVec = light.Position - surfPos;

    // Get distance from surface to light
    float dist = length(lightVec);

    // Check if surface is in range
    if (dist > light.Range)
        return;

    // Normalize light vector
    lightVec = normalize(lightVec);

    // Get ambient 
    ambient = mat.Ambient * light.Ambient;

    // Add diffuse and specular term, provided the surface is in line of sight of light
    float diffuseFactor = dot(lightVec, surfNorm);

    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, surfNorm);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

        diffuse = diffuseFactor * mat.Diffuse * light.Diffuse;
        specular = specFactor * mat.Specular * light.Specular;
    }

    // Scale by spotlight angle and attenuate
    float spot = ComputeSpot(-lightVec, light.Direction, light.Spot);

    ambient *= spot;
    diffuse *= ComputeAttenuation(spot, light.Attenuation, dist);
    specular *= ComputeAttenuation(spot, light.Attenuation, dist);
}