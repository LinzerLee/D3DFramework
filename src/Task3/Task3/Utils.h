//***************************************************************************************
// Utils.h by Linzer Lee
//***************************************************************************************
#ifndef UTILS_H
#define UTILS_H
#include <cassert>
#include <DirectXMath.h>
#include "Application.h"

void D3DErrorBox(HRESULT hr, LPCWSTR lpCaption);

#define SafeRelease(ptr) { if(ptr){ ptr->Release(); ptr = NULL; } }
#define SafeDelete(ptr)  { if(ptr){ delete ptr; ptr = NULL; } }
#define SafeInitialize(ptr) { if(ptr){ ptr->Initialize(); } }
#define SafeRelDel(ptr)  { if(ptr){ ptr->Release(); delete ptr; ptr = NULL; } }

#define HR(exp)			 { HRESULT hr = exp; if(FAILED(hr)) { D3DErrorBox(hr, L" D3DError"); assert(false); } }
#define PTR(exp)		 { void* ptr = exp; assert(!!ptr); }

namespace Colors
{
	XMGLOBALCONST DirectX::XMVECTORF32 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
}

#endif // UTILS_H