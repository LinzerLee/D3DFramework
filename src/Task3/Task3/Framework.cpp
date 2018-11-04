#include <DirectXMath.h>
#include <cassert>

#include "Utils.h"
#include "Framework.h"
#include "Sky.h"
#include "Terrain.h"
#include "BoardModel.h"
#include "ParticleSystem.h"

using namespace DirectX;
// Render State
ID3D11RasterizerState* Framework::WireframeRS = NULL;
ID3D11RasterizerState* Framework::NoCullRS = NULL;
ID3D11RasterizerState* Framework::CullClockwiseRS = NULL;
// Blend State
ID3D11BlendState* Framework::AlphaToCoverageBS = NULL;
ID3D11BlendState* Framework::TransparentBS = NULL;
ID3D11BlendState* Framework::NoRenderTargetWritesBS = NULL;
// Depth Stencil State
ID3D11DepthStencilState* Framework::MarkMirrorDSS = NULL;
ID3D11DepthStencilState* Framework::DrawReflectionDSS = NULL;
ID3D11DepthStencilState* Framework::NoDoubleBlendDSS = NULL;

const D3D11_INPUT_ELEMENT_DESC Framework::BasicInputDesc[4] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

ID3D11InputLayout* Framework::BasicInputLayout = NULL;
BasicEffect* Framework::BasicFX = NULL;

Framework::Framework()
:	m_Format(DXGI_FORMAT_R8G8B8A8_UNORM),
	m_Device(NULL),
	m_DeviceContext(NULL),
	m_SwapChain(NULL),
	m_DepthStencilBuffer(NULL),
	m_RenderTargetView(NULL),
	m_DepthStencilView(NULL),
	m_4xMsaaQuality(0),
	m_Enable4xMsaa(false)
{
	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));
}

Framework::~Framework()
{
	DeinitGlobal();
	DinitDirect3D();
}

bool Framework::InitDirect3D(HWND hwnd, int width, int height)
{
	m_hMainWnd = hwnd;
	m_BufWidth = width;
	m_BufHeight = height;

	// 创建设备和上下文
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		NULL, D3D_DRIVER_TYPE_HARDWARE, 0,
		0, NULL, 0,
		D3D11_SDK_VERSION,
		&m_Device,
		&featureLevel,
		&m_DeviceContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"Create DirectX 11 Device Failed.", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	// 检测设备是否支持4X多重采样
	m_Device->CheckMultisampleQualityLevels(m_Format, 4, &m_4xMsaaQuality);
	assert(m_4xMsaaQuality > 0);

	DXGI_SWAP_CHAIN_DESC swapChain;
	swapChain.BufferDesc.Width = m_BufWidth;
	swapChain.BufferDesc.Height = m_BufHeight;
	swapChain.BufferDesc.RefreshRate.Numerator = 60;
	swapChain.BufferDesc.RefreshRate.Denominator = 1;
	swapChain.BufferDesc.Format = m_Format;
	swapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 是否开启4X多重采样
	if (m_Enable4xMsaa)
	{
		swapChain.SampleDesc.Count = 4;
		swapChain.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		swapChain.SampleDesc.Count = 1;
		swapChain.SampleDesc.Quality = 0;
	}

	swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChain.BufferCount = 1;
	swapChain.OutputWindow = m_hMainWnd;
	swapChain.Windowed = true;
	swapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChain.Flags = 0;

	// 使用创建设备的IDXGIFactory实例
	IDXGIDevice* dxgiDevice = NULL;
	m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter = NULL;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	IDXGIFactory* dxgiFactory = NULL;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	dxgiFactory->CreateSwapChain(m_Device, &swapChain, &m_SwapChain);

	SafeRelease(dxgiDevice);
	SafeRelease(dxgiAdapter);
	SafeRelease(dxgiFactory);

	return true;
}

void Framework::DinitDirect3D()
{
	SafeRelease(m_RenderTargetView);
	SafeRelease(m_DepthStencilView);
	SafeRelease(m_SwapChain);
	SafeRelease(m_DepthStencilBuffer);

	// Restore all default settings.
	if (m_DeviceContext)
		m_DeviceContext->ClearState();

	SafeRelease(m_DeviceContext);
	SafeRelease(m_Device);
}

bool Framework::InitGlobal()
{
	D3DX11_PASS_DESC passDesc;
	BasicFX = new BasicEffect(m_Device, L"FX/Basic.fxo");
	BasicFX->Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(m_Device->CreateInputLayout(BasicInputDesc, 4, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &BasicInputLayout));

	if (!Sky::InitClassEnv(m_Device))
	{
		return false;
	}

	if (!Terrain::InitClassEnv(m_Device))
	{
		return false;
	}
	
	if (!BoardModel::InitClassEnv(m_Device))
	{
		return false;
	}

	if (!ParticleSystem::InitClassEnv(m_Device))
	{
		return false;
	}

	if (!InitStates())
	{
		return false;
	}

	return true;
}

bool Framework::InitStates()
{
	// WireframeRS
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;
	HR(m_Device->CreateRasterizerState(&wireframeDesc, &Framework::WireframeRS));

	// NoCullRS
	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;
	HR(m_Device->CreateRasterizerState(&noCullDesc, &Framework::NoCullRS));

	// CullClockwiseRS
	D3D11_RASTERIZER_DESC cullClockwiseDesc;
	ZeroMemory(&cullClockwiseDesc, sizeof(D3D11_RASTERIZER_DESC));
	cullClockwiseDesc.FillMode = D3D11_FILL_SOLID;
	cullClockwiseDesc.CullMode = D3D11_CULL_BACK;
	cullClockwiseDesc.FrontCounterClockwise = true;
	cullClockwiseDesc.DepthClipEnable = true;
	HR(m_Device->CreateRasterizerState(&cullClockwiseDesc, &Framework::CullClockwiseRS));

	// AlphaToCoverageBS
	D3D11_BLEND_DESC alphaToCoverageDesc = { 0 };
	alphaToCoverageDesc.AlphaToCoverageEnable = true;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(m_Device->CreateBlendState(&alphaToCoverageDesc, &Framework::AlphaToCoverageBS));

	// TransparentBS
	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;
	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(m_Device->CreateBlendState(&transparentDesc, &Framework::TransparentBS));

	// NoRenderTargetWritesBS
	D3D11_BLEND_DESC noRenderTargetWritesDesc = { 0 };
	noRenderTargetWritesDesc.AlphaToCoverageEnable = false;
	noRenderTargetWritesDesc.IndependentBlendEnable = false;
	noRenderTargetWritesDesc.RenderTarget[0].BlendEnable = false;
	noRenderTargetWritesDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	noRenderTargetWritesDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	noRenderTargetWritesDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	noRenderTargetWritesDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	noRenderTargetWritesDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	noRenderTargetWritesDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	noRenderTargetWritesDesc.RenderTarget[0].RenderTargetWriteMask = 0;
	HR(m_Device->CreateBlendState(&noRenderTargetWritesDesc, &Framework::NoRenderTargetWritesBS));

	// MarkMirrorDSS
	D3D11_DEPTH_STENCIL_DESC mirrorDesc;
	mirrorDesc.DepthEnable = true;
	mirrorDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	mirrorDesc.DepthFunc = D3D11_COMPARISON_LESS;
	mirrorDesc.StencilEnable = true;
	mirrorDesc.StencilReadMask = 0xff;
	mirrorDesc.StencilWriteMask = 0xff;
	mirrorDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// 背面消隐下面的设置无关键要
	mirrorDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(m_Device->CreateDepthStencilState(&mirrorDesc, &Framework::MarkMirrorDSS));

	// DrawReflectionDSS
	D3D11_DEPTH_STENCIL_DESC drawReflectionDesc;
	drawReflectionDesc.DepthEnable = false;
	drawReflectionDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	drawReflectionDesc.DepthFunc = D3D11_COMPARISON_LESS;
	drawReflectionDesc.StencilEnable = true;
	drawReflectionDesc.StencilReadMask = 0xff;
	drawReflectionDesc.StencilWriteMask = 0xff;
	drawReflectionDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	// 背面消隐下面的设置无关键要
	drawReflectionDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	HR(m_Device->CreateDepthStencilState(&drawReflectionDesc, &Framework::DrawReflectionDSS));

	// NoDoubleBlendDSS
	D3D11_DEPTH_STENCIL_DESC noDoubleBlendDesc;
	noDoubleBlendDesc.DepthEnable = true;
	noDoubleBlendDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDoubleBlendDesc.DepthFunc = D3D11_COMPARISON_LESS;
	noDoubleBlendDesc.StencilEnable = true;
	noDoubleBlendDesc.StencilReadMask = 0xff;
	noDoubleBlendDesc.StencilWriteMask = 0xff;
	noDoubleBlendDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	// 背面消隐下面的设置无关键要
	noDoubleBlendDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	HR(m_Device->CreateDepthStencilState(&noDoubleBlendDesc, &Framework::NoDoubleBlendDSS));

	return true;
}

void Framework::DinitStates()
{
	SafeRelease(Framework::WireframeRS);
	SafeRelease(Framework::NoCullRS);
	SafeRelease(Framework::CullClockwiseRS);

	SafeRelease(Framework::AlphaToCoverageBS);
	SafeRelease(Framework::TransparentBS);
	SafeRelease(Framework::NoRenderTargetWritesBS);

	SafeRelease(Framework::MarkMirrorDSS);
	SafeRelease(Framework::DrawReflectionDSS);
	SafeRelease(Framework::NoDoubleBlendDSS);
}

void Framework::DeinitGlobal()
{
	DinitStates();
	ParticleSystem::DeinitClassEnv();
	BoardModel::DeinitClassEnv();
	Terrain::DeinitClassEnv();
	Sky::DeinitClassEnv();
	SafeDelete(BasicFX);
	SafeRelease(BasicInputLayout);
}

bool Framework::Test()
{
	return !!m_Device;
}

void Framework::Setup()
{
	assert(m_DeviceContext);
	assert(m_Device);
	assert(m_SwapChain);

	SafeRelease(m_RenderTargetView);
	SafeRelease(m_DepthStencilView);
	SafeRelease(m_DepthStencilBuffer);

	// 1. 调整交换链后台缓冲区的大小
	m_SwapChain->ResizeBuffers(1, m_BufWidth, m_BufHeight, m_Format, 0);
	ID3D11Texture2D* backBuffer;
	m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	m_Device->CreateRenderTargetView(backBuffer, 0, &m_RenderTargetView);
	// 每调用一次IDXGISwapChain::GetBuffer方法，后台缓冲区的COM引用计数就会向上递增一次
	SafeRelease(backBuffer);

	// 2. 创建深度/模板缓冲区及其视图
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = m_BufWidth;
	depthStencilDesc.Height = m_BufHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 是否开启4X多重采样
	if (m_Enable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	m_Device->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer);
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer, 0, &m_DepthStencilView);

	// 3. 将视图绑定到输出合并器阶段
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	// 4. 设置视口，后台缓冲区的子矩形区域称为视口（viewport）
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width = static_cast<float>(m_BufWidth);
	m_ScreenViewport.Height = static_cast<float>(m_BufHeight);
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;
	m_DeviceContext->RSSetViewports(1, &m_ScreenViewport);
}

void Framework::ClearView(const XMVECTORF32 &color,UINT ClearFlags, 
	FLOAT Depth, UINT8 Stencil)
{
	assert(m_DeviceContext);
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(&color));
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, ClearFlags, Depth, Stencil);
}

ID3D11DeviceContext* Framework::DeviceContext()
{
	return m_DeviceContext;
}

ID3D11Device* Framework::Device()
{
	return m_Device;
}

void Framework::IASetInputLayout(ID3D11InputLayout *pInputLayout)
{
	return m_DeviceContext->IASetInputLayout(pInputLayout);
}

void Framework::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology)
{
	return m_DeviceContext->IASetPrimitiveTopology(Topology);
}

HRESULT Framework::Present(UINT SyncInterval, UINT Flags)
{
	assert(m_SwapChain);
	return m_SwapChain->Present(SyncInterval, Flags);
}

void Framework::RSSetState(ID3D11RasterizerState *pRasterizerState)
{
	return m_DeviceContext->RSSetState(pRasterizerState);
}

void Framework::OMSetDepthStencilState(ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef)
{
	return m_DeviceContext->OMSetDepthStencilState(pDepthStencilState, StencilRef);
}

void Framework::IASetVertexBuffers(UINT StartSlot, UINT NumBuffers,
	ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides,
	const UINT *pOffsets)
{
	return m_DeviceContext->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void Framework::IASetIndexBuffer(ID3D11Buffer *pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
{
	return m_DeviceContext->IASetIndexBuffer(pIndexBuffer, Format, Offset);
}