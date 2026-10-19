#pragma once
#include <DirectXMath.h>

class CameraClass
{
public:
    void SetPosition(float x, float y, float z);
    void SetRotation(float pitchDegrees, float yawDegrees, float rollDegrees);
    DirectX::XMFLOAT3 GetPosition() const { return m_worldPosition; }
    bool Render();
    void GetViewMatrix(DirectX::XMMATRIX& view) const { view = m_view; }

private:
    DirectX::XMFLOAT3 m_offset = {0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 m_rotationDegrees = {0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 m_worldPosition = {0.0f, 0.0f, 0.0f};
    DirectX::XMMATRIX m_view = DirectX::XMMatrixIdentity();
};
