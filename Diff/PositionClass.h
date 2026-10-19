#pragma once
#include <DirectXMath.h>

// Inputs are axis values in [-1, 1]. Units are seconds, radians and world units.
class PositionClass
{
public:
    struct Controls
    {
        float pitch = 0.0f;
        float yaw = 0.0f;
        float moveX = 0.0f;
        float moveZ = 0.0f;
        bool increasePatternFrequency = false;
        bool reset = false;
    };
    void Update(float deltaSeconds, const Controls& controls);
    DirectX::XMFLOAT3 GetModelRotation() const { return m_rotation; }
    DirectX::XMFLOAT3 GetCameraOffset() const { return m_cameraOffset; }
    float GetPatternFrequency() const { return m_patternFrequency; }

private:
    DirectX::XMFLOAT3 m_rotation = {0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 m_cameraOffset = {0.0f, 0.0f, -10.0f};
    float m_patternFrequency = 0.0f;
};
