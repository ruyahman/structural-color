#include "PositionClass.h"
#include "SurfaceMesh.h"
#include <algorithm>
#include <cmath>

void PositionClass::Update(float deltaSeconds, const Controls& controls)
{
    if (controls.reset)
    {
        *this = PositionClass{};
        return;
    }
    if (!std::isfinite(deltaSeconds) || deltaSeconds <= 0.0f)
        return;

    // Match the original demo's speeds at 60 FPS, but remove frame-rate dependence.
    // Limit a resumed frame so a debugger pause cannot cause a large movement.
    const float dt = (std::min)(deltaSeconds, 0.1f);
    constexpr float rotationSpeed = 1.2f;
    constexpr float movementSpeed = 12.0f;
    constexpr float patternSpeed = 3.0f;
    m_rotation.x = std::remainder(m_rotation.x + controls.pitch * rotationSpeed * dt, DirectX::XM_2PI);
    m_rotation.y = std::remainder(m_rotation.y + controls.yaw * rotationSpeed * dt, DirectX::XM_2PI);
    m_cameraOffset.x += controls.moveX * movementSpeed * dt;
    m_cameraOffset.z += controls.moveZ * movementSpeed * dt;
    if (controls.increasePatternFrequency)
        m_patternFrequency = (std::min)(m_patternFrequency + patternSpeed * dt,
                                       Surface::kMaxPatternFrequency);
}
