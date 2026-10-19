#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl/client.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class InputClass
{
public:
    InputClass() = default;
    InputClass(const InputClass&) = delete;
    InputClass& operator=(const InputClass&) = delete;
    ~InputClass() { Shutdown(); }

    bool Initialize(HINSTANCE instance, HWND window, int width, int height);
    void Shutdown();
    bool Frame();
    bool IsKeyDown(unsigned key) const { return key < 256 && (m_keyboardState[key] & 0x80) != 0; }
    void GetMouseLocation(int& x, int& y) const { x = m_mouseX; y = m_mouseY; }
    void GetCameraYawPitch(float& yaw, float& pitch) const { yaw = m_yaw; pitch = m_pitch; }
    void ResetView() { m_yaw = 0.0f; m_pitch = 10.0f; }

private:
    Microsoft::WRL::ComPtr<IDirectInput8> m_input;
    Microsoft::WRL::ComPtr<IDirectInputDevice8> m_keyboard;
    Microsoft::WRL::ComPtr<IDirectInputDevice8> m_mouse;
    unsigned char m_keyboardState[256] = {};
    DIMOUSESTATE m_mouseState = {};
    int m_width = 0, m_height = 0;
    int m_mouseX = 0, m_mouseY = 0;
    float m_yaw = 0.0f, m_pitch = 10.0f;
};
