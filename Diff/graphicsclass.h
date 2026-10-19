#pragma once
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "shaderclass.h"
#include "skyboxshaderclass.h"
#include "textclass.h"
#include "Skyboxclass.h"
#include <memory>

constexpr bool FULL_SCREEN = false;
constexpr bool VSYNC_ENABLED = true;
constexpr float SCREEN_DEPTH = 1000.0f;
constexpr float SCREEN_NEAR = 0.1f;

// Coordinates the three passes: procedural surface, environment, then HUD.
class GraphicsClass
{
public:
    GraphicsClass() = default;
    GraphicsClass(const GraphicsClass&) = delete;
    GraphicsClass& operator=(const GraphicsClass&) = delete;
    ~GraphicsClass() { Shutdown(); }
    bool Initialize(int width, int height, HWND window);
    void Shutdown();
    bool Frame(int mouseX, int mouseY, DirectX::XMFLOAT3 cameraOffset,
               float yaw, float pitch, int fps, int cpu, float patternFrequency);
    bool Render(DirectX::XMFLOAT3 rotation);

private:
    std::unique_ptr<D3DClass> m_d3d;
    std::unique_ptr<CameraClass> m_camera;
    std::unique_ptr<ModelClass> m_model;
    std::unique_ptr<ShaderClass> m_surfaceShader;
    std::unique_ptr<SkyboxShaderClass> m_skyShader;
    std::unique_ptr<TextClass> m_text;
    std::unique_ptr<SkyboxClass> m_sky;
};
