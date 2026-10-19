#include "inputclass.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace
{
    bool ReadDevice(IDirectInputDevice8* device, DWORD size, void* state)
    {
        const HRESULT result = device->GetDeviceState(size, state);
        if (SUCCEEDED(result))
            return true;
        // Losing focus is normal. Never reuse the previous key press/mouse delta.
        std::memset(state, 0, size);
        if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED ||
            result == DIERR_OTHERAPPHASPRIO)
        {
            device->Acquire();
            return true;
        }
        return false;
    }
}

bool InputClass::Initialize(HINSTANCE instance, HWND window, int width, int height)
{
    Shutdown();
    m_width = width;
    m_height = height;
    ResetView();
    if (FAILED(DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8,
        reinterpret_cast<void**>(m_input.GetAddressOf()), nullptr)))
        return false;
    if (FAILED(m_input->CreateDevice(GUID_SysKeyboard, m_keyboard.GetAddressOf(), nullptr)) ||
        FAILED(m_keyboard->SetDataFormat(&c_dfDIKeyboard)) ||
        FAILED(m_keyboard->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
        return false;
    if (FAILED(m_input->CreateDevice(GUID_SysMouse, m_mouse.GetAddressOf(), nullptr)) ||
        FAILED(m_mouse->SetDataFormat(&c_dfDIMouse)) ||
        FAILED(m_mouse->SetCooperativeLevel(window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
        return false;
    // Frame() retries acquisition if focus changes during initialization.
    m_keyboard->Acquire();
    m_mouse->Acquire();
    return true;
}

void InputClass::Shutdown()
{
    if (m_mouse) m_mouse->Unacquire();
    if (m_keyboard) m_keyboard->Unacquire();
    m_mouse.Reset();
    m_keyboard.Reset();
    m_input.Reset();
    std::memset(m_keyboardState, 0, sizeof(m_keyboardState));
    m_mouseState = {};
}

bool InputClass::Frame()
{
    if (!ReadDevice(m_keyboard.Get(), sizeof(m_keyboardState), m_keyboardState) ||
        !ReadDevice(m_mouse.Get(), sizeof(m_mouseState), &m_mouseState))
        return false;
    m_mouseX = std::clamp(m_mouseX + static_cast<int>(m_mouseState.lX), 0, m_width);
    m_mouseY = std::clamp(m_mouseY + static_cast<int>(m_mouseState.lY), 0, m_height);
    constexpr float degreesPerMouseCount = 0.1f;
    m_yaw = std::remainder(m_yaw + m_mouseState.lX * degreesPerMouseCount, 360.0f);
    m_pitch = std::clamp(m_pitch + m_mouseState.lY * degreesPerMouseCount, -89.0f, 89.0f);
    return true;
}
