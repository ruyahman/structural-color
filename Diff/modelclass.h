#pragma once
#include "SurfaceMesh.h"
#include <d3d11.h>
#include <wrl/client.h>

// Owns the surface buffers. CPU geometry is retained and updated only when needed.
class ModelClass
{
public:
    ModelClass() = default;
    ModelClass(const ModelClass&) = delete;
    ModelClass& operator=(const ModelClass&) = delete;
    bool Initialize(ID3D11Device* device);
    void Shutdown();
    bool UpdatePattern(ID3D11DeviceContext* context, float frequency);
    void Render(ID3D11DeviceContext* context) const;
    int GetIndexCount() const { return m_indexCount; }

private:
    std::vector<Surface::Vertex> m_vertices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    int m_indexCount = 0;
    float m_uploadedFrequency = 0.0f;
};
