#include "cameraclass.h"
using namespace DirectX;

void CameraClass::SetPosition(float x, float y, float z)
{
    m_offset = {x, y, z};
}

void CameraClass::SetRotation(float pitchDegrees, float yawDegrees, float rollDegrees)
{
    m_rotationDegrees = {pitchDegrees, yawDegrees, rollDegrees};
}

bool CameraClass::Render()
{
    const float pitch = XMConvertToRadians(m_rotationDegrees.x);
    const float yaw = XMConvertToRadians(m_rotationDegrees.y);
    const float roll = XMConvertToRadians(m_rotationDegrees.z);
    const XMMATRIX orientation = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    const XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), orientation);
    const XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), orientation);

    // Preserve the original yaw-relative camera movement. The shader must receive
    // this same world-space eye position, not the unrotated movement offset.
    const XMVECTOR eye = XMVector3TransformCoord(XMLoadFloat3(&m_offset), XMMatrixRotationY(yaw));
    XMStoreFloat3(&m_worldPosition, eye);
    m_view = XMMatrixLookAtLH(eye, eye + forward, up);
    return true;
}
