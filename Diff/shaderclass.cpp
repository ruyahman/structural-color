#include "shaderclass.h"
#include "ShaderCompiler.h"
#include "SurfaceMesh.h"
#include <cstring>

namespace
{
    bool CreateConstantBuffer(ID3D11Device* device, UINT size,
                              Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer)
    {
        D3D11_BUFFER_DESC description = {};
        description.ByteWidth = size;
        description.Usage = D3D11_USAGE_DYNAMIC;
        description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        return SUCCEEDED(device->CreateBuffer(&description, nullptr, buffer.GetAddressOf()));
    }

    template<class T>
    bool UploadConstants(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const T& value)
    {
        static_assert(sizeof(T) % 16 == 0, "Constant buffer size must be a multiple of 16");
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        if (FAILED(context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
            return false;
        std::memcpy(mapped.pData, &value, sizeof(value));
        context->Unmap(buffer, 0);
        return true;
    }
}

bool ShaderClass::Initialize(ID3D11Device* device, HWND owner)
{
    Shutdown();
    Microsoft::WRL::ComPtr<ID3DBlob> vertexCode;
    Microsoft::WRL::ComPtr<ID3DBlob> pixelCode;
    if (!CompileShader(L"light.vs", "LightVertexShader", "vs_4_0", owner, vertexCode) ||
        !CompileShader(L"light.ps", "LightPixelShader", "ps_4_0", owner, pixelCode))
        return false;

    if (FAILED(device->CreateVertexShader(vertexCode->GetBufferPointer(), vertexCode->GetBufferSize(),
                                          nullptr, m_vertexShader.GetAddressOf())) ||
        FAILED(device->CreatePixelShader(pixelCode->GetBufferPointer(), pixelCode->GetBufferSize(),
                                         nullptr, m_pixelShader.GetAddressOf())))
        return false;

    const D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Surface::Vertex, position),
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Surface::Vertex, normal),
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Surface::Vertex, tangent),
         D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    return SUCCEEDED(device->CreateInputLayout(layout, ARRAYSIZE(layout),
        vertexCode->GetBufferPointer(), vertexCode->GetBufferSize(), m_layout.GetAddressOf())) &&
        CreateConstantBuffer(device, sizeof(Surface::MatrixConstants), m_matrixBuffer) &&
        CreateConstantBuffer(device, sizeof(Surface::LightConstants), m_lightBuffer);
}

void ShaderClass::Shutdown()
{
    m_lightBuffer.Reset();
    m_matrixBuffer.Reset();
    m_layout.Reset();
    m_pixelShader.Reset();
    m_vertexShader.Reset();
}

bool ShaderClass::Render(ID3D11DeviceContext* context, int indexCount,
                         const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view,
                         const DirectX::XMMATRIX& projection, const Surface::LightConstants& light,
                         ID3D11RasterizerState* raster)
{
    Surface::MatrixConstants matrices;
    DirectX::XMStoreFloat4x4(&matrices.world, DirectX::XMMatrixTranspose(world));
    DirectX::XMStoreFloat4x4(&matrices.view, DirectX::XMMatrixTranspose(view));
    DirectX::XMStoreFloat4x4(&matrices.projection, DirectX::XMMatrixTranspose(projection));
    if (!UploadConstants(context, m_matrixBuffer.Get(), matrices) ||
        !UploadConstants(context, m_lightBuffer.Get(), light))
        return false;

    context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());
    context->PSSetConstantBuffers(0, 1, m_lightBuffer.GetAddressOf());
    context->IASetInputLayout(m_layout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    context->RSSetState(raster);
    context->DrawIndexed(indexCount, 0, 0);
    return true;
}
