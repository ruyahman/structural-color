#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <cstddef>

namespace Surface
{
    struct MatrixConstants
    {
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4X4 view;
        DirectX::XMFLOAT4X4 projection;
    };
    struct LightConstants
    {
        DirectX::XMFLOAT4 diffuseColor;
        DirectX::XMFLOAT3 lightDirection;
        float lightPadding = 0.0f;
        DirectX::XMFLOAT3 cameraPosition;
        float cameraPadding = 0.0f;
    };
    static_assert(sizeof(MatrixConstants) == 192, "HLSL matrix buffer layout");
    static_assert(sizeof(LightConstants) == 48, "HLSL light buffer layout");
    static_assert(offsetof(LightConstants, cameraPosition) == 32, "HLSL register alignment");
}

// Renders the procedural surface. No texture/sampler is needed by this material.
class ShaderClass
{
public:
    ShaderClass() = default;
    ShaderClass(const ShaderClass&) = delete;
    ShaderClass& operator=(const ShaderClass&) = delete;
    bool Initialize(ID3D11Device* device, HWND owner);
    void Shutdown();
    bool Render(ID3D11DeviceContext* context, int indexCount,
                const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view,
                const DirectX::XMMATRIX& projection, const Surface::LightConstants& light,
                ID3D11RasterizerState* raster);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_matrixBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightBuffer;
};
