#ifndef SURFACE_LIGHTING_HLSLI
#define SURFACE_LIGHTING_HLSLI

// Artistic approximation used by the original research demo, not a spectral solver.
float3 RainbowBands(float3 value)
{
    return max(1.0f - value * value, 0.0f);
}

float3 SafeNormalize(float3 value, float3 fallback)
{
    float lengthSquared = dot(value, value);
    return lengthSquared > 1e-12f ? value * rsqrt(max(lengthSquared, 1e-12f)) : fallback;
}

float4 EvaluateSurfaceColor(float3 normal, float3 tangent, float3 lightDirection,
                            float3 viewDirection, float4 diffuseColor)
{
    normal = SafeNormalize(normal, float3(0, 0, 1));
    tangent = SafeNormalize(tangent, float3(1, 0, 0));
    float3 light = SafeNormalize(-lightDirection, float3(0, 0, -1));
    float3 view = SafeNormalize(viewDirection, normal);

    // Keep the original L + V scaling: normalizing this sum would change the
    // locations of the colour bands. Guard the grazing/opposing-direction case.
    float3 lightViewSum = light + view;
    float alongTangent = dot(tangent, lightViewSum);
    float alongNormal = dot(normal, lightViewSum);
    const float highlightSharpness = 50.0f;
    float highlightRatio = alongTangent * highlightSharpness / max(abs(alongNormal), 1e-5f);
    float3 anisotropicHighlight = diffuseColor.rgb * exp(-highlightRatio * highlightRatio);

    const float grooveSpacing = 1.0f;
    float phase = abs(alongTangent) * grooveSpacing;
    float3 structuralColor = 0.0f;
    [unroll]
    for (int order = 1; order <= 6; ++order)
    {
        float bandPosition = 2.0f * phase / order - 1.0f;
        structuralColor += RainbowBands(4.0f * (bandPosition - float3(0.75f, 0.5f, 0.25f)));
    }

    float3 diffuse = diffuseColor.rgb * saturate(dot(normal, light));
    return float4(diffuse + anisotropicHighlight + structuralColor, 1.0f);
}
#endif
