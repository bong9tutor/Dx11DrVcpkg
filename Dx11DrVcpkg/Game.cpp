//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace
{
    struct VS_BLOOM_PARAMETERS
    {
        float bloomThreshold;
        float blurAmount;
        float bloomIntensity;
        float baseIntensity;
        float bloomSaturation;
        float baseSaturation;
        uint8_t na[8];
    };
    
    static_assert(!(sizeof(VS_BLOOM_PARAMETERS) % 16), "VS_BLOOM_PARAMETERS needs to be 16 bytes aligned");
    
    struct VS_BLUR_PARAMETERS
    {
        static constexpr size_t SAMPLE_COUNT = 15;
        
        XMFLOAT4 sampleOffsets[SAMPLE_COUNT];
        XMFLOAT4 sampleWeights[SAMPLE_COUNT];
        
        void SetBlurEffectParameters(float dx, float dy, const VS_BLOOM_PARAMETERS& params)
        {
            sampleWeights[0].x = ComputeGaussian(0, params.blurAmount);
            sampleOffsets[0].x = sampleOffsets[0].y = 0.f;
            
            float totalWeights = sampleWeights[0].x;
            
            for (size_t i = 0; i < SAMPLE_COUNT / 2; i++)
            {
                float weight = ComputeGaussian(float(i + 1.f), params.blurAmount);
                
                sampleWeights[i * 2 + 1].x = weight;
                sampleWeights[i * 2 + 2].x = weight;
                
                totalWeights += weight * 2;
                
                float sampleOffset = float(i) * 2.f + 1.5f;
                
                Vector2 delta = Vector2(dx, dy) * sampleOffset;
                
                sampleOffsets[i * 2 + 1].x =  delta.x;
                sampleOffsets[i * 2 + 1].y =  delta.y;
                sampleOffsets[i * 2 + 2].x = -delta.x;
                sampleOffsets[i * 2 + 2].y = -delta.y;
            }
            
            for (size_t i = 0; i < SAMPLE_COUNT; i++)
            {
                sampleWeights[i].x /= totalWeights;
            }
        }
        
    private:
        float ComputeGaussian(float n, float theta)
        {
            return (float)((1.0 / sqrtf(2 * XM_PI * theta)) * expf(-(n * n) / (2 * theta * theta)));
        }
    };
    
    static_assert(!(sizeof(VS_BLUR_PARAMETERS) & 16), "VS_BLUR_PARAMETERS needs to be 16 bytes aligned");
    
    enum BloomPresets
    {
        Default = 0,
        Soft,
        Desaturated,
        Saturated,
        Blurry,
        Subtle,
        None
    };
    
    BloomPresets g_Bloom = Saturated;
    
    static const VS_BLOOM_PARAMETERS g_BloomPresets[] =
    {
        { 0.25f, 4, 1.25f, 1,    1, 1 }, // Default
        { 0,     3, 1,     1,    1, 1 }, // Soft
        { 0.5f,  8, 2,     1,    0, 1 }, // Desaturated
        { 0.25f, 4, 2,     1,    2, 0 }, // Saturated
        { 0,     2, 1,     0.1f, 1, 1 }, // Blurry
        { 0.5f,  2, 1,     1,    0, 1 }, // Subtle
        { 0.25f, 4, 1.25f, 1,    2, 0 }  // None
    };
}

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false) :
    m_fullscreenRect{},
    m_bloomRect{}
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
    
    const auto format = m_deviceResources->GetBackBufferFormat();
    m_offscreenTexture = std::make_unique<DX::RenderTexture>(format);
    m_renderTarget1 = std::make_unique<DX::RenderTexture>(format);
    m_renderTarget2 = std::make_unique<DX::RenderTexture>(format);
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
    
    auto totalTime = static_cast<float>(timer.GetTotalSeconds());
    
    m_world = Matrix::CreateRotationZ(totalTime / 2.f)
        * Matrix::CreateRotationY(totalTime)
        * Matrix::CreateRotationX(totalTime / 2.f);
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
    
    m_spriteBatch->Begin();
    m_spriteBatch->Draw(m_background.Get(), m_fullscreenRect);
    m_spriteBatch->End();

    m_shape->Draw(m_world, m_view, m_proj);

    m_deviceResources->PIXEndEvent();

    PostProcess();
    
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
    auto renderTarget = m_offscreenTexture->GetRenderTargetView();
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
    
    SetCurrentDirectory(L"../Images");
    DX::ThrowIfFailed(CreateWICTextureFromFile(device, L"sunset.jpg", nullptr, m_background.ReleaseAndGetAddressOf()));
    
    // Load HLSL Shaders
    auto blob = DX::ReadData(L"BloomExtract.cso");
    DX::ThrowIfFailed(device->CreatePixelShader(blob.data(), blob.size(), nullptr, m_bloomExtractPS.ReleaseAndGetAddressOf()));
    
    blob = DX::ReadData(L"BloomCombine.cso");
    DX::ThrowIfFailed(device->CreatePixelShader(blob.data(), blob.size(), nullptr, m_bloomCombinePS.ReleaseAndGetAddressOf()));
    
    blob = DX::ReadData(L"GaussianBlur.cso");
    DX::ThrowIfFailed(device->CreatePixelShader(blob.data(), blob.size(), nullptr, m_gaussianBlurPS.ReleaseAndGetAddressOf()));
    
    {
        CD3D11_BUFFER_DESC cbDesc(sizeof(VS_BLOOM_PARAMETERS), D3D11_BIND_CONSTANT_BUFFER);
        D3D11_SUBRESOURCE_DATA initData = { &g_BloomPresets[g_Bloom], 0, 0 };
        DX::ThrowIfFailed(device->CreateBuffer(&cbDesc, &initData, m_bloomParams.ReleaseAndGetAddressOf()));
    }
    
    {
        CD3D11_BUFFER_DESC cbDesc(sizeof(VS_BLUR_PARAMETERS), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(device->CreateBuffer(&cbDesc, nullptr, m_blurParamsWidth.ReleaseAndGetAddressOf()));
        DX::ThrowIfFailed(device->CreateBuffer(&cbDesc, nullptr, m_blurParamsHeight.ReleaseAndGetAddressOf()));
    }
    
    m_offscreenTexture->SetDevice(device);
    m_renderTarget1->SetDevice(device);
    m_renderTarget2->SetDevice(device);
    
    m_states = std::make_unique<CommonStates>(device);
    
    auto context = m_deviceResources->GetD3DDeviceContext();
    m_spriteBatch = std::make_unique<SpriteBatch>(context);
    m_shape = GeometricPrimitive::CreateTorus(context);
    
    m_view = Matrix::CreateLookAt(Vector3(0.f, 3.f, -3.f), Vector3::Zero, Vector3::UnitY);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
    
    auto size = m_deviceResources->GetOutputSize();
    m_fullscreenRect = size;
    
    auto context = m_deviceResources->GetD3DDeviceContext();
    
    VS_BLUR_PARAMETERS blurData = {};
    blurData.SetBlurEffectParameters(1.f / (float(size.right) / 2), 0, g_BloomPresets[g_Bloom]);
    context->UpdateSubresource(m_blurParamsWidth.Get(), 0, nullptr, &blurData, sizeof(VS_BLUR_PARAMETERS), 0);
    
    blurData.SetBlurEffectParameters(0, 1.f / (float(size.bottom) / 2), g_BloomPresets[g_Bloom]);
    context->UpdateSubresource(m_blurParamsHeight.Get(), 0, nullptr, &blurData, sizeof(VS_BLUR_PARAMETERS), 0);
    
    m_offscreenTexture->SetWindow(size);
    
    // Half-size blurring render targets
    m_bloomRect = { 0, 0, size.right / 2, size.bottom / 2 };
    
    m_renderTarget1->SetWindow(m_bloomRect);
    m_renderTarget2->SetWindow(m_bloomRect);
    
    m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PIDIV4, float(size.right) / float(size.bottom), 0.01f, 100.f);
}

void Game::PostProcess()
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    ID3D11ShaderResourceView* null[] = { nullptr, nullptr };

    if (g_Bloom == None)
    {
        // Pass-through test
        context->CopyResource(m_deviceResources->GetRenderTarget(),
            m_offscreenTexture->GetRenderTarget());
    }
    else
    {
        // scene -> RT1 (downsample)
        auto rt1RT = m_renderTarget1->GetRenderTargetView();
        context->OMSetRenderTargets(1, &rt1RT, nullptr);
        m_spriteBatch->Begin(SpriteSortMode_Immediate,
            nullptr, nullptr, nullptr, nullptr,
            [=](){
                context->PSSetConstantBuffers(0, 1, m_bloomParams.GetAddressOf());
                context->PSSetShader(m_bloomExtractPS.Get(), nullptr, 0);
            });
        auto rtSRV = m_offscreenTexture->GetShaderResourceView();
        m_spriteBatch->Draw(rtSRV, m_bloomRect);
        m_spriteBatch->End();

        // RT1 -> RT2 (blur horizontal)
        auto rt2RT = m_renderTarget2->GetRenderTargetView();
        context->OMSetRenderTargets(1, &rt2RT, nullptr);
        m_spriteBatch->Begin(SpriteSortMode_Immediate,
            nullptr, nullptr, nullptr, nullptr,
            [=](){
                context->PSSetShader(m_gaussianBlurPS.Get(), nullptr, 0);
                context->PSSetConstantBuffers(0, 1,
                    m_blurParamsWidth.GetAddressOf());
            });
        auto rt1SRV = m_renderTarget1->GetShaderResourceView();
        m_spriteBatch->Draw(rt1SRV, m_bloomRect);
        m_spriteBatch->End();

        context->PSSetShaderResources(0, 2, null);

        // RT2 -> RT1 (blur vertical)
        context->OMSetRenderTargets(1, &rt1RT, nullptr);
        m_spriteBatch->Begin(SpriteSortMode_Immediate,
            nullptr, nullptr, nullptr, nullptr,
            [=](){
                context->PSSetShader(m_gaussianBlurPS.Get(), nullptr, 0);
                context->PSSetConstantBuffers(0, 1,
                    m_blurParamsHeight.GetAddressOf());
            });
        auto rt2SRV = m_renderTarget2->GetShaderResourceView();
        m_spriteBatch->Draw(rt2SRV, m_bloomRect);
        m_spriteBatch->End();

        // RT1 + scene
        auto renderTarget = m_deviceResources->GetRenderTargetView();
        context->OMSetRenderTargets(1, &renderTarget, nullptr);
        m_spriteBatch->Begin(SpriteSortMode_Immediate,
            nullptr, nullptr, nullptr, nullptr,
            [=](){
                context->PSSetShader(m_bloomCombinePS.Get(), nullptr, 0);
                context->PSSetShaderResources(1, 1, &rt1SRV);
                context->PSSetConstantBuffers(0, 1, m_bloomParams.GetAddressOf());
            });
        m_spriteBatch->Draw(rtSRV, m_fullscreenRect);
        m_spriteBatch->End();
    }

    context->PSSetShaderResources(0, 2, null);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
    m_graphicsMemory.reset();
    
    m_states.reset();
    m_spriteBatch.reset();
    m_shape.reset();
    m_background.Reset();
    
    m_bloomExtractPS.Reset();
    m_bloomCombinePS.Reset();
    m_gaussianBlurPS.Reset();
    
    m_bloomParams.Reset();
    m_blurParamsWidth.Reset();
    m_blurParamsHeight.Reset();
    
    m_offscreenTexture->ReleaseDevice();
    m_renderTarget1->ReleaseDevice();
    m_renderTarget2->ReleaseDevice();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
