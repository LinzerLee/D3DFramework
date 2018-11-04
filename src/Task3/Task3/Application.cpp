//***************************************************************************************
// Application.cpp by Linzer Lee
//***************************************************************************************
#pragma comment ( lib, "d3d11.lib" )

#include "Application.h"

#include <sstream>
#include <windowsx.h>

#include "Utils.h"
#include "Render.h"
#include "ModelObject.h"

using namespace DirectX;

namespace
{
	Application* g_AppInst = NULL;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (g_AppInst)
	{
		return g_AppInst->MsgProc(hwnd, msg, wParam, lParam);
	} 
	else
	{
		MessageBox(0, L"Create Application Instance Failed.", 0, 0);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

Application::Application(HINSTANCE hInstance)
	: m_hInstance(hInstance),
	m_WndCaption(L"Task3"),
	m_WndWidth(800),
	m_WndHeight(600),
	m_hMainWnd(0),
	m_AppPaused(false),
	m_Minimized(false),
	m_Maximized(false),
	m_Resizing(false),
	m_EyePos(0.0f, 0.0f, 0.0f),
	m_FPS(60.0f),
	m_Framework(Framework::Instance())
{
	g_AppInst = this;

	XMMATRIX I = XMMatrixIdentity();
	m_World = I;
	m_View = I;
	m_Proj = I;
}

Application::~Application()
{
}

HINSTANCE Application::Instance()const
{
	return m_hInstance;
}

HWND Application::MainWnd()const
{
	return m_hMainWnd;
}

float Application::AspectRatio()const
{
	return static_cast<float>(m_WndWidth) / m_WndHeight;
}

int Application::Run()
{
	MSG msg = {0};
 
	m_Timer.Reset();

	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
        {	
			m_Timer.Tick();

			if( !m_AppPaused)
			{
				if (CalculateFrameStats())
				{
					Update(m_Timer.DeltaTime());
				}	
				Predraw();
				Draw();
			}
			else
			{
				Sleep(100);
			}
        }
    }

	return (int)msg.wParam;
}

void Application::ErrorBox(LPCWSTR lpText, LPCWSTR lpCaption)
{
	MessageBox(m_hMainWnd, lpText, lpCaption, 0);
}

void Application::DrawModel(Drawable &obj, Camera &cam)
{
	obj.Draw(m_View * m_Proj, cam);
}

bool Application::Init()
{
	if(!InitMainWindow())
		return false;

	if(!InitFramework())
		return false;

	return true;
}

void Application::OnResize()
{
	if (m_Framework.Test())
		m_Framework.Setup();
}

void Application::Predraw()
{
}

void Application::Draw()
{
	HR(m_Framework.Present(0, 0));
}

LRESULT Application::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	// 窗口失去焦点就暂停
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			m_AppPaused = true;
			m_Timer.Stop();
		}
		else
		{
			m_AppPaused = false;
			m_Timer.Start();
		}
		return 0;

	// 调整窗口大小
	case WM_SIZE:
		m_WndWidth = LOWORD(lParam);
		m_WndHeight = HIWORD(lParam);
		if (wParam == SIZE_MINIMIZED)
		{
			m_AppPaused = true;
			m_Minimized = true;
			m_Maximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			m_AppPaused = false;
			m_Minimized = false;
			m_Maximized = true;
			OnResize();
		}
		else if (wParam == SIZE_RESTORED)
		{
			// 从最小化恢复
			if (m_Minimized)
			{
				m_AppPaused = false;
				m_Minimized = false;
				OnResize();
			}
			// 从最大化恢复
			else if (m_Maximized)
			{
				m_AppPaused = false;
				m_Maximized = false;
				OnResize();
			}
			// Resize期间不进行重绘
			else if (m_Resizing)
			{
			}
			else
			{
				OnResize();
			}
		}
		return 0;

	case WM_ENTERSIZEMOVE:
		m_AppPaused = true;
		m_Resizing  = true;
		m_Timer.Stop();
		return 0;

	case WM_EXITSIZEMOVE:
		m_AppPaused = false;
		m_Resizing  = false;
		m_Timer.Start();
		OnResize();
		return 0;
 
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


bool Application::InitMainWindow()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"WndClassName";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, m_WndWidth, m_WndHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"WndClassName", m_WndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hInstance, 0); 
	if( !m_hMainWnd )
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	return true;
}

bool Application::InitFramework()
{
	bool ret = false;
	if (m_Framework.InitDirect3D(m_hMainWnd, m_WndWidth, m_WndHeight))
	{
		OnResize();
		if (m_Framework.InitGlobal())
		{

			ret = true;
		}
	}

	return ret;
}

void Application::ClearScreen(XMVECTORF32 color)
{
	m_Framework.ClearView(color, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Application::SetFPS(float fps)
{
	if (fps >= 0.0f)
		m_FPS = fps;
}

bool Application::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	static float lastElapsed = 0.0f;

	bool ret = false;
	float current = m_Timer.TotalTime();
	if (current - lastElapsed >= 1.0f / m_FPS)
	{
		ret = true;
		lastElapsed += 1.0f / m_FPS;
		frameCnt ++;
	}

	// Compute averages over one second period.
	if( (current - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;   
		outs.precision(6);
		outs << m_WndCaption << L"    "
			 << L"FPS: " << fps << L"    " 
			 << L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(m_hMainWnd, outs.str().c_str());
		
		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}

	return ret;
}

Terrain *Application::CreateTerrain(Terrain::Info info)
{

	Terrain *terrain = new Terrain();
	if (terrain)
	{
		terrain->Init(info);
	}

	return terrain;
}

void Application::DrawTerrain(Terrain *terrain, const XMFLOAT3& eyePos, DirectionalLight lights[3])
{
	return terrain->Draw(m_View, m_Proj, eyePos, lights);
}

void Application::SetWorld(DirectX::FXMMATRIX &world)
{
	m_World = world;
}

void Application::SetView(DirectX::FXMMATRIX &view)
{
	m_View = view;
}

void Application::SetProjection(DirectX::FXMMATRIX &proj)
{
	m_Proj = proj;
}

void Application::SetEyePos(const XMFLOAT3 &pos)
{
	m_EyePos = pos;
}

Application *Application::g_DebugApp = NULL;

// Auto generation 
#include "Task3App.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Task3App theApp(hInstance);
	Application::g_DebugApp = &theApp;

	if (!theApp.Init())
		return EXIT_FAILD;

	theApp.Load();
	theApp.Run();
	theApp.Unload();

	Framework::Instance().~Framework();
	return EXIT_SUCCESS;
}
