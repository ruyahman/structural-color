cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct SurfaceInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD0;
    float3 tangent : TEXCOORD1;
};

SurfaceInput LightVertexShader(VertexInput input)
{
    SurfaceInput output;
    float4 worldPosition = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(mul(worldPosition, viewMatrix), projectionMatrix);
    output.worldPosition = worldPosition.xyz;
    // The surface world transform contains rotation only (no non-uniform scale).
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.tangent = mul(input.tangent, (float3x3)worldMatrix);
    return output;
}
