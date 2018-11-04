//***************************************************************************************
// Framework.h by Linzer Lee
//***************************************************************************************

#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <d3d11.h>

#include "Terrain.h"

class Framework
{
public:
	struct BasicVertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 Tex;
		DirectX::XMFLOAT4 Color;
	};

public:
	~Framework();
	// µ¥Àý
	static Framework & Instance() 
	{ 
		static Framework m_Instance; 
		return m_Instance;
	}

	bool InitDirect3D(HWND hwnd, int width, int height);
	bool InitGlobal();
	bool InitStates();
	bool Test();
	void Setup();
	void ClearView(const DirectX::XMVECTORF32 &color, 
		UINT ClearFlags, FLOAT Depth, UINT8 Stencil);
	ID3D11DeviceContext* DeviceContext();
	ID3D11Device* Device();
	void IASetInputLayout(ID3D11InputLayout *pInputLayout);
	void IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);
	HRESULT Present(UINT SyncInterval, UINT Flags);
	void RSSetState(ID3D11RasterizerState *pRasterizerState);
	void OMSetDepthStencilState(ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef);
	void IASetVertexBuffers(UINT StartSlot, UINT NumBuffers,
		ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides,
		const UINT *pOffsets);
	void IASetIndexBuffer(ID3D11Buffer *pIndexBuffer, DXGI_FORMAT Format,UINT Offset);

private:
	Framework();
	Framework &operator=(const Framework &) = delete;
	Framework(const Framework &) = delete;

	void DeinitGlobal();
	void DinitDirect3D();
	void DinitStates();

public:
	// Render State
	static ID3D11RasterizerState* WireframeRS;
	static ID3D11RasterizerState* NoCullRS;
	static ID3D11RasterizerState* CullClockwiseRS;
	// Blend State
	static ID3D11BlendState* AlphaToCoverageBS;
	static ID3D11BlendState* TransparentBS;
	static ID3D11BlendState* NoRenderTargetWritesBS;
	// Depth Stencil State
	static ID3D11DepthStencilState* MarkMirrorDSS;
	static ID3D11DepthStencilState* DrawReflectionDSS;
	static ID3D11DepthStencilState* NoDoubleBlendDSS;

	static const D3D11_INPUT_ELEMENT_DESC BasicInputDesc[4];
	static ID3D11InputLayout* BasicInputLayout;
	static BasicEffect* BasicFX;

private:
	DXGI_FORMAT m_Format;
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;
	D3D11_VIEWPORT m_ScreenViewport;
	IDXGISwapChain* m_SwapChain;
	ID3D11Texture2D* m_DepthStencilBuffer;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;
	UINT m_4xMsaaQuality;
	bool m_Enable4xMsaa;
	HWND m_hMainWnd;
	int m_BufWidth;
	int m_BufHeight;
};

#endif // FRAMEWORK_H
