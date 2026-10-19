#include "graphicsclass.h"
using namespace DirectX;

bool GraphicsClass::Initialize(int width, int height, HWND window)
{
    m_d3d = std::make_unique<D3DClass>();
    m_camera = std::make_unique<CameraClass>();
    m_model = std::make_unique<ModelClass>();
    m_surfaceShader = std::make_unique<ShaderClass>();
    m_skyShader = std::make_unique<SkyboxShaderClass>();
    m_text = std::make_unique<TextClass>();
    m_sky = std::make_unique<SkyboxClass>();

    if (!m_d3d->Initialize(width, height, VSYNC_ENABLED, window, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
    {
        MessageBoxW(window, L"Could not initialize Direct3D.", L"Diff", MB_OK | MB_ICONERROR);
        return false;
    }

    m_camera->SetPosition(0, 0, -3);
    m_camera->Render();
    XMMATRIX hudView;
    m_camera->GetViewMatrix(hudView);
    if (!m_text->Initialize(m_d3d->GetDevice(), m_d3d->GetDeviceContext(), window, width, height, hudView))
        return false;
    if (!m_model->Initialize(m_d3d->GetDevice()) ||
        !m_surfaceShader->Initialize(m_d3d->GetDevice(), window))
        return false;
    return m_sky->Initialize(m_d3d->GetDevice(), 30, 30, L"data/skymap.dds", window) &&
           m_skyShader->Initialize(m_d3d->GetDevice(), window);
}

void GraphicsClass::Shutdown()
{
    // Legacy HUD/environment classes still expose explicit Shutdown().
    // Keep cleanup centralized while the core surface uses RAII-managed buffers.
    if (m_skyShader) m_skyShader->Shutdown();
    if (m_sky) m_sky->Shutdown();
    if (m_text) m_text->Shutdown();
    m_skyShader.reset();
    m_sky.reset();
    m_text.reset();
    m_surfaceShader.reset();
    m_model.reset();
    m_camera.reset();
    if (m_d3d) m_d3d->Shutdown();
    m_d3d.reset();
}

bool GraphicsClass::Frame(int mouseX, int mouseY, XMFLOAT3 cameraOffset,
                          float yaw, float pitch, int fps, int cpu, float patternFrequency)
{
    m_camera->SetPosition(cameraOffset.x, cameraOffset.y, cameraOffset.z);
    m_camera->SetRotation(pitch, yaw, 0);
    m_camera->Render();
    auto* context = m_d3d->GetDeviceContext();
    return m_text->SetMousePosition(mouseX, mouseY, context) &&
           m_text->SetFps(fps, context) && m_text->SetCpu(cpu, context) &&
           m_model->UpdatePattern(context, patternFrequency);
}

bool GraphicsClass::Render(XMFLOAT3 rotation)
{
    auto* context = m_d3d->GetDeviceContext();
    m_d3d->BeginScene(0, 0, 0, 1);
    XMMATRIX view, projection, ortho;
    m_camera->GetViewMatrix(view);
    m_d3d->GetProjectionMatrix(projection);
    m_d3d->GetOrthoMatrix(ortho);
    const XMMATRIX world = XMMatrixRotationX(rotation.x) *
                           XMMatrixRotationY(rotation.y) * XMMatrixRotationZ(rotation.z);
    const XMFLOAT3 lightDirection = {0, 0, 1};
    const XMFLOAT4 lightColor = {1, 1, 1, 1};
    const XMFLOAT3 eye = m_camera->GetPosition();
    const Surface::LightConstants light = {lightColor, lightDirection, 0, eye, 0};

    // The demonstration sheet is visible from both sides.
    m_model->Render(context);
    if (!m_surfaceShader->Render(context, m_model->GetIndexCount(), world, view, projection,
                                 light, m_d3d->GetNoCullRasterState()))
        return false;

    // The sky follows camera translation and is drawn at the far depth (skybox.vs).
    XMMATRIX skyView = view;
    skyView.r[3] = g_XMIdentityR3;
    m_sky->Render(context);
    if (!m_skyShader->Render(context, m_sky->GetIndexCount(), XMMatrixIdentity(), skyView, projection,
        m_sky->GetTexture(), lightDirection, lightColor, eye, lightColor, m_d3d->GetNoCullRasterState()))
        return false;

    m_d3d->TurnZBufferOff();
    m_d3d->TurnOnAlphaBlending();
    const bool hudRendered = m_text->Render(context, XMMatrixIdentity(), ortho);
    m_d3d->TurnOffAlphaBlending();
    m_d3d->TurnZBufferOn();
    if (!hudRendered)
        return false;
    m_d3d->EndScene();
    return true;
}
