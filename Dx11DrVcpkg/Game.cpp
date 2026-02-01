//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;
    
    auto time = static_cast<float>(m_timer.GetTotalSeconds());
    
    float yaw   = time * 0.4f;
    float pitch = time * 0.7f;
    float roll  = time * 1.1f;
    
    auto quat = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
    auto light = XMVector3Rotate(g_XMOne, quat);
    m_effect->SetLightDirection(0, light);
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context;
    
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    context->RSSetState(m_states->CullNone());
    
    m_effect->Apply(context);
    
    auto sampler = m_states->LinearClamp();
    context->PSSetSamplers(0, 1, &sampler);
    context->IASetInputLayout(m_inputLayout.Get());
    
    m_batch->Begin();
    
    VertexPositionNormalTexture v1(Vector3(400.f, 150.f, 0.f), -Vector3::UnitZ, Vector2(0.5f, 0.f));
    VertexPositionNormalTexture v2(Vector3(600.f, 450.f, 0.f), -Vector3::UnitZ, Vector2(1.f, 1.f));
    VertexPositionNormalTexture v3(Vector3(200.f, 450.f, 0.f), -Vector3::UnitZ, Vector2(0.f, 1.f));
    
    m_batch->DrawTriangle(v1, v2, v3);
    
    m_batch->End();

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
    m_graphicsMemory->Commit();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    const auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    const auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    m_graphicsMemory = std::make_unique<GraphicsMemory>(device);
    
    // TODO: Initialize device dependent objects here (independent of window size).
    device;
    
    m_states = std::make_unique<CommonStates>(device);
    
    auto context = m_deviceResources->GetD3DDeviceContext();
    m_batch = std::make_unique<PrimitiveBatch<VertexType>>(context);
    
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(device, L"../Images/rocks.dds", nullptr,
        m_texture.ReleaseAndGetAddressOf())
    );
    
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"../Images/rocks_normalmap.dds", nullptr,
            m_normalMap.ReleaseAndGetAddressOf())
    );
    
    m_effect = std::make_unique<NormalMapEffect>(device);
    m_effect->SetTexture(m_texture.Get());
    m_effect->SetNormalTexture(m_normalMap.Get());
    
    m_effect->EnableDefaultLighting();
    m_effect->SetLightDiffuseColor(0, Colors::Gray);
    
    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexType>(device, m_effect.get(),
            m_inputLayout.ReleaseAndGetAddressOf())
    );
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
    
    auto size = m_deviceResources->GetOutputSize();
    Matrix proj = Matrix::CreateScale(2.f / float(size.right), 
        -2.f / float(size.bottom), 1.f) * Matrix::CreateTranslation(-1.f, 1.f, 0.f);
    m_effect->SetProjection(proj);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
    m_graphicsMemory.reset();
    
    m_states.reset();
    m_effect.reset();
    m_batch.reset();
    m_inputLayout.Reset();
    m_texture.Reset();
    m_normalMap.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
