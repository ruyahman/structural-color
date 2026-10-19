#include "modelclass.h"
#include <cstring>
#include <utility>

bool ModelClass::Initialize(ID3D11Device* device)
{
    Shutdown();
    auto mesh = Surface::BuildGrid();
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

    D3D11_BUFFER_DESC vertexDesc = {};
    vertexDesc.ByteWidth = static_cast<UINT>(mesh.vertices.size() * sizeof(Surface::Vertex));
    vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = mesh.vertices.data();
    if (FAILED(device->CreateBuffer(&vertexDesc, &vertexData, vertexBuffer.GetAddressOf())))
        return false;

    D3D11_BUFFER_DESC indexDesc = {};
    indexDesc.ByteWidth = static_cast<UINT>(mesh.indices.size() * sizeof(std::uint32_t));
    indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = mesh.indices.data();
    if (FAILED(device->CreateBuffer(&indexDesc, &indexData, indexBuffer.GetAddressOf())))
        return false;

    m_vertices = std::move(mesh.vertices);
    m_indexCount = static_cast<int>(mesh.indices.size());
    m_vertexBuffer = std::move(vertexBuffer);
    m_indexBuffer = std::move(indexBuffer);
    return true;
}

void ModelClass::Shutdown()
{
    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
    m_vertices.clear();
    m_indexCount = 0;
    m_uploadedFrequency = 0.0f;
}

bool ModelClass::UpdatePattern(ID3D11DeviceContext* context, float frequency)
{
    if (!m_vertexBuffer || !Surface::IsValidFrequency(frequency))
        return false;
    if (frequency == m_uploadedFrequency)
        return true;

    Surface::UpdateTangents(m_vertices, frequency);
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (FAILED(context->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        return false;
    std::memcpy(mapped.pData, m_vertices.data(), m_vertices.size() * sizeof(Surface::Vertex));
    context->Unmap(m_vertexBuffer.Get(), 0);
    m_uploadedFrequency = frequency;
    return true;
}

void ModelClass::Render(ID3D11DeviceContext* context) const
{
    const UINT stride = sizeof(Surface::Vertex);
    const UINT offset = 0;
    ID3D11Buffer* vertexBuffer = m_vertexBuffer.Get();
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
