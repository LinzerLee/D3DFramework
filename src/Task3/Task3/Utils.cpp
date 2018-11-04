#include "Utils.h"

using namespace DirectX;

void D3DErrorBox(HRESULT hr, LPCWSTR lpCaption)
{
	WCHAR buf[16];
	wsprintfW(buf, L"0X%X", hr);
	Application::g_DebugApp->ErrorBox(buf, lpCaption);
}