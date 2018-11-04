//***************************************************************************************
// Application.h by Linzer Lee
//***************************************************************************************

#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <DirectXMath.h>
#include "Framework.h"
#include "Timer.h"

#define EXIT_SUCCESS	0
#define EXIT_FAILD		1

class Drawable;

class Application
{
public:
	Application(HINSTANCE hInstance);
	virtual ~Application();
	
	HINSTANCE Instance()const;
	HWND      MainWnd()const;
	float     AspectRatio()const;
	int		  Run();
	
	// Debug
	static Application *g_DebugApp;
	void ErrorBox(LPCWSTR lpText, LPCWSTR lpCaption);
	// Framework methods.  Derived client class overrides these methods to 
	// implement specific application requirements.
	virtual bool Init();
	virtual void Update(float dt) = 0;
	virtual void Predraw();
	virtual void Draw(); 
	virtual void Load() = 0;
	virtual void Unload() = 0;
	virtual void OnResize(); 
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:
	bool InitMainWindow();
	bool InitFramework();
	void ClearScreen(DirectX::XMVECTORF32 color);
	void SetFPS(float fps);
	bool CalculateFrameStats();

	void SetWorld(DirectX::FXMMATRIX &world);
	void SetView(DirectX::FXMMATRIX &view);
	void SetProjection(DirectX::FXMMATRIX &proj);
	void SetEyePos(const DirectX::XMFLOAT3 &pos);

	void DrawModel(Drawable &obj, Camera &cam);
	Terrain *CreateTerrain(Terrain::Info info);
	void DrawTerrain(Terrain *terrain, const DirectX::XMFLOAT3& eyePos, DirectionalLight lights[3]);
protected:

	HINSTANCE m_hInstance;
	HWND      m_hMainWnd;
	Timer	  m_Timer;
	Framework &m_Framework;

	// πÃ∂®‰÷»æπ‹œﬂ
	DirectX::XMMATRIX m_World;
	DirectX::XMMATRIX m_View;
	DirectX::XMMATRIX m_Proj;
	DirectX::XMFLOAT3 m_EyePos;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring m_WndCaption;
	int m_WndWidth;
	int m_WndHeight;
	bool m_AppPaused;
	bool m_Minimized;
	bool m_Maximized;
	bool m_Resizing;
	float m_FPS;
};

#endif // APPLICATION_H