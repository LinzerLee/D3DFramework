//***************************************************************************************
// Task3.cpp: 定义应用程序的入口点
//***************************************************************************************
#include "stdafx.h"
#include "Task3App.h"
#include "Utils.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"
#include "Transform.h"
#include "Render.h"
#include "BoardRender.h"
#include "Collider.h"

using namespace std;
using namespace DirectX;

Task3App::Task3App(HINSTANCE hInstance)
:	Application(hInstance),
	m_WalkCamMode(false),
	m_Land(L"Terrain"),
	m_Waves(L"Waves"),
	m_Sky(L"Sky"),
	m_Snowman1(L"Linzer"),
	m_Snowman2(L"Lee"),
	m_Box(L"Box"),
	m_RenderOptions(RenderOptions::TexturesAndFog)
{
	m_LastMousePos.x = 0;
	m_LastMousePos.y = 0;
	m_Camera.SetPosition(10.0f, 5.0f, 10.0f);

	XMMATRIX grassTexScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);
	XMStoreFloat4x4(&m_GrassTexTransform, grassTexScale);

	m_DirLights[0].Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLights[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	m_DirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[1].Diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	m_DirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_DirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	m_DirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	m_LandMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_LandMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_LandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	m_WavesMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_WavesMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	m_WavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

	m_SnowmanMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_SnowmanMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SnowmanMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

	m_ShadowMat.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_ShadowMat.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	m_ShadowMat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);

	m_TreeMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_TreeMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_TreeMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
}

Task3App::~Task3App()
{
	SafeDelete(m_Terrain);
}

bool Task3App::Init()
{
	if (!Application::Init())
		return false;

	// 对依赖Framework的变量初始化
	// TODO
	SetFPS(400.0f);
	return true;
}

static float GetHillHeight(float x, float z)
{
	float y = 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));

	if (x > 0.0f && z > 0.0f) return y;
	// else if (x > 0.0f && z < 0.0f) return sinf(x) + sinf(z);
	else return 0.0f;
}

static XMFLOAT3 GetHillNormal(float x, float z)
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	if (x > 0.0f && z > 0.0f) return n;
	// else if (x > 0.0f && z < 0.0f) return n;
	else return XMFLOAT3(0.0f, 1.0f, 0.0f);
}

void Task3App::Load()
{
	// 加载资源
	// TODO
	/*
	Terrain::Info info;
	info.HeightMapFilename = L"Textures/terrain.raw";
	info.LayerMapArrayFile = L"Textures/array.dds";
	info.BlendMapFilename = L"Textures/blend.dds";
	
	info.HeightScale = 50.0f;
	info.HeightmapWidth = 2049;
	info.HeightmapHeight = 2049;
	info.CellSpacing = 0.5f;

	PTR(m_Terrain = CreateTerrain(info));
	*/
	m_SnowTex.Load(m_Framework.Device(), L"Textures/snow.dds");
	m_GrassMapTex.Load(m_Framework.Device(), L"Textures/grass.dds");
	m_BoxTex.Load(m_Framework.Device(), L"Textures/WireFence.dds");

	m_Camera.AddComponent(new Transform(L"DefaultTransform"));
	// 加载Land模型资源(后期应由Terrain替代ModelObject来实现地形)
	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;
	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

	UINT indexCount = grid.Indices.size();
	UINT stride = sizeof(Framework::BasicVertex);
	UINT byteWidth = stride * grid.Vertices.size();
	vector<Framework::BasicVertex> vertices(grid.Vertices.size());
	
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		p.y = GetHillHeight(p.x, p.z);

		vertices[i].Pos = p;
		vertices[i].Normal = GetHillNormal(p.x, p.z);
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::White);
	}
	
	m_Land.SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Land.SetVertexArray(&vertices[0], byteWidth, stride);
	m_Land.SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);

	m_Land.AddComponent(new Transform(L"DefaultTransform"));
	m_Land.AddComponent(new Render(L"DefaultRender"));
	
	// Land Transform组件配置
	Transform *transform = m_Land.GetTransform();
	if (transform)
	{
		// 世界变换
	}

	// Land Render组件配置
	Render *render = m_Land.GetRender();
	if (render)
	{
		render->SetTexTransform(XMLoadFloat4x4(&m_GrassTexTransform));
		render->SetMaterial(m_LandMat);
		render->SetDiffuseMapSRV(m_GrassMapTex.TexSRV());
	}
	// 加载Waves模型资源
	m_Waves.Init(80, 80, 1.0f, 0.03f, 3.25f, 0.4f);
	m_Waves.AddComponent(new Transform(L"DefaultTransform"));
	m_Waves.AddComponent(new Render(L"DefaultRender"));

	m_Waves.SetMaterial(m_WavesMat);
	m_Waves.SetTexture(L"Textures/water2.dds");

	// Waves Transform组件配置
	transform = m_Waves.GetTransform();
	if (transform)
	{
		// 世界变换
		transform->Translate(40.0f, 0.0f, 40.0f);
	}

	// Waves Render组件配置
	render = m_Waves.GetRender();
	if (render)
	{
		static float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		render->SetBlendState(Framework::TransparentBS, blendFactor);
	}
	// 加载盒子模型
	geoGen.CreateBox(5.0f, 5.0f, 5.0f, grid);
	indexCount = grid.Indices.size();
	stride = sizeof(Framework::BasicVertex);
	byteWidth = stride * grid.Vertices.size();

	vertices.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::White);
	}
	m_Box.SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Box.SetVertexArray(&vertices[0], byteWidth, stride);
	m_Box.SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);
	m_Box.AddComponent(new Transform(L"DefaultTransform"));
	m_Box.AddComponent(new Render(L"DefaultRender"));
	m_Box.AddComponent(new BoxCollider(L"DefaultCollider", XMFLOAT3(5.0f, 5.0f, 5.0f)));

	// Transform组件配置
	transform = m_Box.GetTransform();
	if (transform)
	{
		// 世界变换
	}
	// Render组件配置
	render = m_Box.GetRender();
	if (render)
	{
		render->SetMaterial(m_SnowmanMat);
		render->SetDiffuseMapSRV(m_BoxTex.TexSRV());
		render->SetRenderState(Framework::NoCullRS);
	}

	// 加载Sky模型资源
	m_Sky.Init(L"Textures/snowcube.dds", L"Textures/yuncube.dds", 5000.0f);

	// 树
	for (int i = 0; i < ARRAYSIZE(m_TreeTexs); ++i)
	{
		wchar_t buf[32];
		wsprintf(buf, L"Textures/tree%d.dds", i);
		m_TreeTexs[i].Load(m_Framework.Device(), buf);
	}

	vector<BoardModel::Vertex> trees;
	for (int i = 0; i < ARRAYSIZE(m_Trees); ++i)
	{
		trees.clear();
		for (int j = 0; j < 20; ++j)
		{
			float size = MathHelper::RandF(5.0f, 15.0f);
			XMFLOAT3 pos;
			// 不在湖中生成树
			do
			{
				pos = XMFLOAT3((MathHelper::RandF() - 0.5f) * 160.0f, 0, (MathHelper::RandF() - 0.5f) * 160.0f);
				pos.y = GetHillHeight(pos.x, pos.z) + size / 2 - 1.0f;
			} while (pos.x > 0 && pos.z > 0 && pos.y < 5);

			trees.push_back(BoardModel::Vertex(pos, XMFLOAT2(size, size)));
		}
		
		m_Trees[i].Init(&trees[0], trees.size());
		BoardRender *boardRender = (BoardRender *)m_Trees[i].GetRender();
		if (boardRender)
		{
			boardRender->SetEffectTechnique("Light3TexAlphaClip");
			boardRender->SetTextureMap(m_TreeTexs[i].TexSRV());
			boardRender->SetDirLights(m_DirLights);
			boardRender->SetFog(Colors::Silver, 15.0f, 175.0f);
			boardRender->SetMaterial(m_TreeMat);
		}
	}

	// 草
	for (int i = 0; i < ARRAYSIZE(m_GrassTexs); ++i)
	{
		wchar_t buf[32];
		wsprintf(buf, L"Textures/grass%d.dds", i);
		m_GrassTexs[i].Load(m_Framework.Device(), buf);
	}

	vector<BoardModel::Vertex> grass;
	for (int i = 0; i < ARRAYSIZE(m_Grass); ++i)
	{
		grass.clear();
		for (int j = 0; j < 7; ++j)
		{
			float size = MathHelper::RandF(1.0f, 3.0f);
			XMFLOAT3 pos;
			// 不在湖中生成草
			do
			{
				pos = XMFLOAT3((MathHelper::RandF() - 0.5f) * 160.0f, 0, (MathHelper::RandF() - 0.5f) * 160.0f);
				pos.y = GetHillHeight(pos.x, pos.z) + size / 2 - 0.1f;
			} while (pos.x > 0 && pos.z > 0 && pos.y < 5);

			grass.push_back(BoardModel::Vertex(pos, XMFLOAT2(size, size)));
		}

		m_Grass[i].Init(&grass[0], grass.size());
		BoardRender *boardRender = (BoardRender *)m_Grass[i].GetRender();
		if (boardRender)
		{
			boardRender->SetEffectTechnique("Light3TexAlphaClip");
			boardRender->SetTextureMap(m_GrassTexs[i].TexSRV());
			boardRender->SetDirLights(m_DirLights);
			boardRender->SetFog(Colors::Silver, 15.0f, 175.0f);
			boardRender->SetMaterial(m_TreeMat);
		}
	}
	// 粒子系统
	m_RandomTex.CreateRandomTexture1D(m_Framework.Device());
	m_FlareTex.Load(m_Framework.Device(), L"Textures/flare0.dds");
	m_RainTex.Load(m_Framework.Device(), L"Textures/raindrop.dds");
	m_SnowBallTex.Load(m_Framework.Device(), L"Textures/snowflake.dds");
	m_Fire.Init(m_Framework.Device(), ParticleSystem::FireFX, m_FlareTex.TexSRV(), m_RandomTex.TexSRV(), 500);
	m_Fire.SetEmitPos(XMFLOAT3(0.0f, 1.0f, 0.0f));
	m_Rain.Init(m_Framework.Device(), ParticleSystem::RainFX, m_RainTex.TexSRV(), m_RandomTex.TexSRV(), 10000);
	m_Snow.Init(m_Framework.Device(), ParticleSystem::SnowFX, m_SnowBallTex.TexSRV(), m_RandomTex.TexSRV(), 10000);

	CreateSnowman(m_Snowman1);
	CreateSnowman(m_Snowman2);
	m_Snowman2.GetTransform()->Translate(-10.0f, 2.5f, -10.0f);
}

void Task3App::Unload()
{
	// TODO
}

void Task3App::Update(float dt)
{
	// TODO
	if (GetAsyncKeyState('W') & 0x8000)
		m_Camera.Walk(10.0f * dt);

	if (GetAsyncKeyState('S') & 0x8000)
		m_Camera.Walk(-10.0f * dt);

	if (GetAsyncKeyState('A') & 0x8000)
		m_Camera.Strafe(-10.0f * dt);

	if (GetAsyncKeyState('D') & 0x8000)
		m_Camera.Strafe(10.0f * dt);

	// 切换摄像机模式Walk/fly
	if (GetAsyncKeyState('O') & 0x8000)
		m_WalkCamMode = true;

	if (GetAsyncKeyState('P') & 0x8000)
		m_WalkCamMode = false;

	// 将摄像机脱离附着物
	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (m_Camera.AttachMode())
		{
			m_Camera.Attach(NULL);
		}
		else
		{
			Collider *collider = m_Box.GetCollider();
			if (collider->TestPointInside(m_Camera.GetPosition()))
			{
				m_Camera.Attach(&m_Box);
			}
		}
	}

	// 切换渲染模式
	if (GetAsyncKeyState('1') & 0x8000)
		m_RenderOptions = RenderOptions::Lighting;

	if (GetAsyncKeyState('2') & 0x8000)
		m_RenderOptions = RenderOptions::Textures;

	if (GetAsyncKeyState('3') & 0x8000)
		m_RenderOptions = RenderOptions::TexturesAndFog;

	// 地图行走模式
	if (m_WalkCamMode)
	{
		XMFLOAT3 camPos = m_Camera.GetPosition();
		// float y = m_Terrain->GetHeight(camPos.x, camPos.z);
		float y = MathHelper::Max(GetHillHeight(camPos.x, camPos.z), 0.0f);
		m_Camera.SetPosition(camPos.x, y + 2.0f, camPos.z);
	}
	
	static float t_base = 0.0f;
	bool isDisturb = false;
	if ((m_Timer.TotalTime() - t_base) >= 0.05f)
	{
		t_base += 0.05f;
		isDisturb = true;
	}
	
	m_Waves.Update(dt, isDisturb);
	m_Sky.Update(dt);
	m_Fire.Update(dt, m_Timer.TotalTime());
	m_Rain.Update(dt, m_Timer.TotalTime());
	m_Snow.Update(dt, m_Timer.TotalTime());
	
	// 渲染模式
	switch (m_RenderOptions)
	{
	case RenderOptions::Lighting:
		m_Land.GetRender()->SetEffectTechnique("Light3Tex");
		m_Waves.GetRender()->SetEffectTechnique("Light3Tex");
		m_Box.GetRender()->SetEffectTechnique("Light3Tex");
		m_Snowman1.GetSubModel(L"snowman-head")->GetRender()->SetEffectTechnique("Light3Tex");
		m_Snowman2.GetSubModel(L"snowman-head")->GetRender()->SetEffectTechnique("Light3Tex");
		break;

	case RenderOptions::Textures:
		m_Land.GetRender()->SetEffectTechnique("Light3TexAlphaClip");
		m_Waves.GetRender()->SetEffectTechnique("Light3TexAlphaClip");
		m_Box.GetRender()->SetEffectTechnique("Light3TexAlphaClip");
		m_Snowman1.GetSubModel(L"snowman-head")->GetRender()->SetEffectTechnique("Light3TexAlphaClip");
		m_Snowman2.GetSubModel(L"snowman-head")->GetRender()->SetEffectTechnique("Light3TexAlphaClip");
		break;

	case RenderOptions::TexturesAndFog:
		m_Land.GetRender()->SetEffectTechnique("Light3TexAlphaClipFog");
		m_Waves.GetRender()->SetEffectTechnique("Light3TexAlphaClipFog");
		m_Box.GetRender()->SetEffectTechnique("Light3TexAlphaClipFog");
		m_Snowman1.GetSubModel(L"snowman-head")->GetRender()->SetEffectTechnique("Light3TexAlphaClipFog");
		m_Snowman2.GetSubModel(L"snowman-head")->GetRender()->SetEffectTechnique("Light3TexAlphaClipFog");
		break;
	}

	static float rad = 0.0f;
	static float step = 0.001f;
	float radius = 10.0f;
	rad += step;
	if (rad > MathHelper::Pi * 2)
	{
		rad -= MathHelper::Pi * 2;
	}
	
	float x = cos(rad) * radius;
	float z = sin(rad) * radius;
	// 盒子圆周运动
	Transform *transform = m_Box.GetTransform();
	if (transform)
	{
		XMFLOAT3 pos = XMFLOAT3(40 + x, 15.0f, 40 + z);
		transform->Translate(pos);
		// 火焰圆周运动
		m_Fire.SetEmitPos(pos);
		pos.y += 5.0f;
		m_Snowman1.GetTransform()->Translate(pos);
	}

	XMFLOAT4 axis(1.0f, 1.0f, 0.0f, 0.0f);
	m_Snowman1.GetSubModel(L"snowman-fan")->GetTransform()->AttachRotateAxis(XMLoadFloat4(&axis), 0.05f);
	m_Snowman2.GetSubModel(L"snowman-fan")->GetTransform()->AttachRotateAxis(XMLoadFloat4(&axis), 0.05f);
}

void Task3App::Predraw()
{
	// Super Call
	Application::Predraw();

	// TODO
	m_Camera.UpdateViewMatrix();
	SetView(m_Camera.View());
	SetProjection(m_Camera.Proj());
	SetEyePos(m_Camera.GetPosition());

	m_Rain.SetEmitPos(m_Camera.GetPosition());
	m_Snow.SetEmitPos(m_Camera.GetPosition());

	Render::SetGlobalDirLights(m_DirLights);
	Render::SetGlobalFog(Colors::Silver, 15.0f, 175.0f);
}

void Task3App::Draw()
{
	// TODO
	ClearScreen(Colors::LightSteelBlue);
	// DrawTerrain(m_Terrain, m_Camera.GetPosition(), m_DirLights);
	// *绘制地形*
	DrawModel(m_Land, m_Camera);
	// *绘制箱子*
	DrawModel(m_Box, m_Camera);
	// *绘制雪人*
	DrawModel(m_Snowman1, m_Camera);
	
	// Draw Waves到模板缓冲
	Render *render = m_Waves.GetRender();
	static float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Render::SetGlobalBlendState(Framework::NoRenderTargetWritesBS, blendFactor);
	Render::SetGlobalDepthStencilState(Framework::MarkMirrorDSS, 1);
	render->BlendStateEnable(false);
	render->DepthStencilStateEnable(false);
	DrawModel(m_Waves, m_Camera);
	render->BlendStateEnable(true);
	render->DepthStencilStateEnable(true);
	Render::SetGlobalBlendState(NULL, NULL);
	Render::SetGlobalDepthStencilState(NULL, 0);
	
	// Draw雪人倒影
	Transform *transform = m_Snowman1.GetTransform();
	render = m_Snowman1.GetSubModel(L"snowman-body")->GetRender();
	XMMATRIX reflect =  XMMatrixReflect(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));	// xz plane
	transform->Attach(reflect);
	// 调整光照方向
	XMFLOAT3 oldLightDirections[3];
	for (int i = 0; i < 3; ++i)
	{
		oldLightDirections[i] = m_DirLights[i].Direction;
		XMVECTOR lightDir = XMLoadFloat3(&m_DirLights[i].Direction);
		XMVECTOR reflectedLightDir = XMVector3TransformNormal(lightDir, reflect);
		XMStoreFloat3(&m_DirLights[i].Direction, reflectedLightDir);
	}
	Render::SetGlobalDirLights(m_DirLights);
	// Render::SetGlobalRenderState(Framework::CullClockwiseRS);  // 为绘制倒影,反向背面消隐
	Render::SetGlobalDepthStencilState(Framework::DrawReflectionDSS, 1); // 应用模板,只在Waves中绘制
	// render->RenderStateEnable(false);
	render->DepthStencilStateEnable(false);
	// *绘制雪人倒影*
	DrawModel(m_Snowman1, m_Camera);
	// 恢复设置
	render->DepthStencilStateEnable(true);
	// render->RenderStateEnable(true);
	Render::SetGlobalDepthStencilState(NULL, 0);
	// Render::SetGlobalRenderState(NULL);
	for (int i = 0; i < 3; ++i)
	{
		m_DirLights[i].Direction = oldLightDirections[i];
	}
	Render::SetGlobalDirLights(m_DirLights);
	transform->ResetAttach();
	
	// *绘制Waves*
	DrawModel(m_Waves, m_Camera);
	// *绘制天空盒子*
	DrawModel(m_Sky, m_Camera);
	// *绘制树木*
	for (int i = 0; i < ARRAYSIZE(m_Trees); ++i)
		DrawModel(m_Trees[i], m_Camera);
	// *绘制草*
	for (int i = 0; i < ARRAYSIZE(m_Grass); ++i)
		DrawModel(m_Grass[i], m_Camera);

	// *绘制雪人和影子*
	DrawModel(m_Snowman2, m_Camera);
	m_Snowman2.DrawShadow(m_Camera.ViewProj(), m_Camera, m_ShadowMat, m_DirLights[0].Direction);
	/*
	// 雪人影子
	render = m_Snowman.GetSubModel(L"snowman-body")->GetRender();
	transform->Attach(XMMatrixShadow(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -XMLoadFloat3(&m_DirLights[0].Direction)) * XMMatrixTranslation(0.0f, 0.1f, 0.0f));
	render->SetMaterial(m_ShadowMat);
	render->SetDepthStencilState(Framework::NoDoubleBlendDSS, 0);
	render->DepthStencilStateEnable(false);
	// *绘制雪人影子*
	DrawModel(m_Snowman, m_Camera);
	render->DepthStencilStateEnable(true);
	render->SetDepthStencilState(NULL, 0);
	render->SetMaterial(m_SnowmanMat);
	transform->ResetAttach();
	*/
	// *最后绘制粒子*
	DrawModel(m_Fire, m_Camera);
	DrawModel(m_Rain, m_Camera);
	DrawModel(m_Snow, m_Camera);
	// Super Call
	Application::Draw();
}

void Task3App::OnResize()
{
	// TODO
	m_Camera.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 3000.0f);

	// Super Call
	Application::OnResize();
}

void Task3App::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(m_hMainWnd);
}

void Task3App::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Task3App::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		m_Camera.Pitch(dy);
		m_Camera.RotateY(dx);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void Task3App::CreateSnowman(MeshModel &model)
{
	// 加载雪人模型
	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;
	UINT indexCount = 0;
	UINT stride = 0;
	UINT byteWidth = 0;
	vector<Framework::BasicVertex> vertices;

	model.AddComponent(new Transform(L"DefaultTransform"));
	Transform *transform = NULL;
	Render *render = NULL;
	ModelObject *submod = NULL;

	// 加载雪人模型-帽檐
	geoGen.CreateCylinder(2.0f, 2.0f, 0.1f, 15, 15, grid);
	indexCount = grid.Indices.size();
	stride = sizeof(Framework::BasicVertex);
	byteWidth = stride * grid.Vertices.size();

	vertices.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::Black);
	}

	submod = model.AddSubModel(NULL, L"snowman-hatbrim");
	submod->SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	submod->SetVertexArray(&vertices[0], byteWidth, stride);
	submod->SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);

	submod->AddComponent(new Transform(L"DefaultTransform"));
	submod->AddComponent(new Render(L"DefaultRender"));

	// hatbrim Transform组件配置
	transform = submod->GetTransform();
	if (transform)
	{
		// SubMode2局部坐标变换
		transform->Translate(0.0f, 5.0, 0.0f);
	}
	// hatbrim Render组件配置
	render = submod->GetRender();
	if (render)
	{
		render->SetMaterial(m_SnowmanMat);
		render->SetEffectTechnique("Light3");
		render->SetRenderState(Framework::NoCullRS);
	}

	// 加载雪人模型-帽子
	geoGen.CreateCylinder(1.0f, 1.2f, 1.5f, 15, 15, grid);
	indexCount = grid.Indices.size();
	stride = sizeof(Framework::BasicVertex);
	byteWidth = stride * grid.Vertices.size();

	vertices.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::Black);
	}

	submod = model.AddSubModel(NULL, L"snowman-hat");
	submod->SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	submod->SetVertexArray(&vertices[0], byteWidth, stride);
	submod->SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);

	submod->AddComponent(new Transform(L"DefaultTransform"));
	submod->AddComponent(new Render(L"DefaultRender"));

	// hat Transform组件配置
	transform = submod->GetTransform();
	if (transform)
	{
		// SubMode3局部坐标变换
		transform->Translate(0.0f, 5.75, 0.0f);
	}
	// hat Render组件配置
	render = submod->GetRender();
	if (render)
	{
		render->SetMaterial(m_SnowmanMat);
		render->SetEffectTechnique("Light3");
		render->SetRenderState(Framework::NoCullRS);
	}

	// 加载雪人模型-围脖
	geoGen.CreateCylinder(1.5f, 1.5f, 0.5f, 15, 15, grid);
	indexCount = grid.Indices.size();
	stride = sizeof(Framework::BasicVertex);
	byteWidth = stride * grid.Vertices.size();

	vertices.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::Red);
	}

	submod = model.AddSubModel(NULL, L"snowman-scarf");
	submod->SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	submod->SetVertexArray(&vertices[0], byteWidth, stride);
	submod->SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);

	submod->AddComponent(new Transform(L"DefaultTransform"));
	submod->AddComponent(new Render(L"DefaultRender"));

	// scarf Transform组件配置
	transform = submod->GetTransform();
	if (transform)
	{
		// SubMode4局部坐标变换
		transform->Translate(0.0f, 2.5, 0.0f);
	}
	// scarf Render组件配置
	render = submod->GetRender();
	if (render)
	{
		render->SetMaterial(m_SnowmanMat);
		render->SetEffectTechnique("Light3");
		render->SetRenderState(Framework::NoCullRS);
	}

	// 加载雪人模型-左眼
	geoGen.CreateSphere(0.2f, 15, 15, grid);
	indexCount = grid.Indices.size();
	stride = sizeof(Framework::BasicVertex);
	byteWidth = stride * grid.Vertices.size();

	vertices.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::Black);
	}

	submod = model.AddSubModel(NULL, L"snowman-lefteye");
	submod->SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	submod->SetVertexArray(&vertices[0], byteWidth, stride);
	submod->SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);

	submod->AddComponent(new Transform(L"DefaultTransform"));
	submod->AddComponent(new Render(L"DefaultRender"));

	// lefteye Transform组件配置
	transform = submod->GetTransform();
	if (transform)
	{
		// SubMode5局部坐标变换
		transform->Translate(-0.6f, 4.0, -1.3f);
	}
	// lefteye Render组件配置
	render = submod->GetRender();
	if (render)
	{
		render->SetMaterial(m_SnowmanMat);
		render->SetEffectTechnique("Light3");
		render->SetRenderState(Framework::NoCullRS);
	}

	// 加载雪人模型-右眼
	geoGen.CreateSphere(0.2f, 15, 15, grid);
	indexCount = grid.Indices.size();
	stride = sizeof(Framework::BasicVertex);
	byteWidth = stride * grid.Vertices.size();

	vertices.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::Black);
	}

	submod = model.AddSubModel(NULL, L"snowman-righteye");
	submod->SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	submod->SetVertexArray(&vertices[0], byteWidth, stride);
	submod->SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);

	submod->AddComponent(new Transform(L"DefaultTransform"));
	submod->AddComponent(new Render(L"DefaultRender"));

	// righteye Transform组件配置
	transform = submod->GetTransform();
	if (transform)
	{
		// SubMode6局部坐标变换
		transform->Translate(0.6f, 4.0, -1.3f);
	}
	// righteye Render组件配置
	render = submod->GetRender();
	if (render)
	{
		render->SetMaterial(m_SnowmanMat);
		render->SetEffectTechnique("Light3");
		render->SetRenderState(Framework::NoCullRS);
	}

	// 加载雪人模型-鼻子
	geoGen.CreateCylinder(0.01f, 0.5f, 1.5f, 15, 15, grid);
	indexCount = grid.Indices.size();
	stride = sizeof(Framework::BasicVertex);
	byteWidth = stride * grid.Vertices.size();

	vertices.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::Yellow);
	}

	submod = model.AddSubModel(NULL, L"snowman-righteye");
	submod->SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	submod->SetVertexArray(&vertices[0], byteWidth, stride);
	submod->SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);

	submod->AddComponent(new Transform(L"DefaultTransform"));
	submod->AddComponent(new Render(L"DefaultRender"));

	// righteye Transform组件配置
	transform = submod->GetTransform();
	if (transform)
	{
		// SubMode6局部坐标变换
		transform->Translate(0.0f, 3.8f, -1.3f);
		transform->RotateX(MathHelper::Pi / 2);
	}
	// righteye Render组件配置
	render = submod->GetRender();
	if (render)
	{
		render->SetMaterial(m_SnowmanMat);
		render->SetEffectTechnique("Light3");
		render->SetRenderState(Framework::NoCullRS);
	}

	// 加载雪人模型-木棍
	geoGen.CreateCylinder(0.1f, 0.1f, 4.0f, 15, 15, grid);
	indexCount = grid.Indices.size();
	stride = sizeof(Framework::BasicVertex);
	byteWidth = stride * grid.Vertices.size();

	vertices.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::Magenta);
	}

	submod = model.AddSubModel(NULL, L"snowman-stick");
	submod->SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	submod->SetVertexArray(&vertices[0], byteWidth, stride);
	submod->SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);

	submod->AddComponent(new Transform(L"DefaultTransform"));
	submod->AddComponent(new Render(L"DefaultRender"));

	// stick Transform组件配置
	transform = submod->GetTransform();
	if (transform)
	{
		// SubMode6局部坐标变换
		transform->Translate(3.0f, 1.0f, 0.0f);
		transform->RotateZ(- MathHelper::Pi / 4);
	}
	// stick Render组件配置
	render = submod->GetRender();
	if (render)
	{
		render->SetMaterial(m_SnowmanMat);
		render->SetEffectTechnique("Light3");
		render->SetRenderState(Framework::NoCullRS);
	}

	// 加载雪人模型-扇子
	geoGen.CreateCylinder(2.0f, 2.0f, 0.01f, 15, 15, grid);
	indexCount = grid.Indices.size();
	stride = sizeof(Framework::BasicVertex);
	byteWidth = stride * grid.Vertices.size();

	vertices.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::Blue);
	}

	submod = model.AddSubModel(NULL, L"snowman-fan");
	submod->SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	submod->SetVertexArray(&vertices[0], byteWidth, stride);
	submod->SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);

	submod->AddComponent(new Transform(L"DefaultTransform"));
	submod->AddComponent(new Render(L"DefaultRender"));

	// fan Transform组件配置
	transform = submod->GetTransform();
	if (transform)
	{
		// SubMode6局部坐标变换
		transform->Translate(5.0f, 3.0f, 0.0f);
		transform->RotateX(MathHelper::Pi / 2);
	}
	// fan Render组件配置
	render = submod->GetRender();
	if (render)
	{
		render->SetMaterial(m_SnowmanMat);
		render->SetEffectTechnique("Light3");
		render->SetRenderState(Framework::NoCullRS);
	}

	// 加载雪人模型-Head
	geoGen.CreateSphere(1.5, 30, 30, grid);
	indexCount = grid.Indices.size();
	stride = sizeof(Framework::BasicVertex);
	byteWidth = stride * grid.Vertices.size();

	vertices.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::White);
	}

	submod = model.AddSubModel(NULL, L"snowman-head");
	submod->SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	submod->SetVertexArray(&vertices[0], byteWidth, stride);
	submod->SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);

	submod->AddComponent(new Transform(L"DefaultTransform"));
	submod->AddComponent(new Render(L"DefaultRender"));

	// Head Transform组件配置
	transform = submod->GetTransform();
	if (transform)
	{
		// SubModel局部坐标变换
		transform->Translate(0.0f, 3.8f, 0.0f);
	}

	render = submod->GetRender();

	// 加载雪人模型-Body
	geoGen.CreateSphere(2.5, 30, 30, grid);
	indexCount = grid.Indices.size();
	stride = sizeof(Framework::BasicVertex);
	byteWidth = stride * grid.Vertices.size();

	vertices.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
		XMStoreFloat4(&vertices[i].Color, Colors::White);
	}

	submod = model.AddSubModel(NULL, L"snowman-body");
	submod->SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	submod->SetVertexArray(&vertices[0], byteWidth, stride);
	submod->SetIndexArray(&grid.Indices[0], sizeof(UINT) * indexCount, DXGI_FORMAT_R32_UINT, indexCount);

	submod->AddComponent(new Transform(L"DefaultTransform"));
	submod->AddComponent(render);

	// Body Transform组件配置
	transform = submod->GetTransform();
	if (transform)
	{
		// 世界变换
	}

	// Body Render组件配置
	render = submod->GetRender();
	if (render)
	{
		render->SetMaterial(m_SnowmanMat);
		render->SetDiffuseMapSRV(m_SnowTex.TexSRV());
		render->SetRenderState(Framework::NoCullRS);
	}
}