#include "Utils.h"
#include "Sky.h"
#include "GeometryGenerator.h"
#include "Camera.h"
#include "Effects.h"
#include "Texture.h"
#include "Transform.h"
#include "SkyRender.h"

#include <vector>

using namespace std;
using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC Sky::InputDesc[1] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

ID3D11InputLayout* Sky::InputLayout = NULL;
SkyEffect* Sky::FX = NULL;

bool Sky::InitClassEnv(ID3D11Device* device)
{
	D3DX11_PASS_DESC passDesc;
	FX = new SkyEffect(device, L"FX/sky.fxo");
	FX->SkyTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(Sky::InputDesc, 1, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &Sky::InputLayout));

	return true;
}

void Sky::DeinitClassEnv()
{
	SafeDelete(Sky::FX);
	SafeRelease(Sky::InputLayout);
}

Sky::Sky(wstring name)
:	ModelObject(name)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_RotateY, I);
}

bool Sky::Init(const wstring& cubemapFilename, const wstring& cubeyunmapFilename, float skySphereRadius)
{
	ID3D11Device *device = Framework::Instance().Device();
	m_CubeMapTex.Load(device, cubemapFilename);
	m_CubeYunMapTex.Load(device, cubeyunmapFilename);

	GeometryGenerator::MeshData sphere;
	GeometryGenerator geoGen;
	geoGen.CreateSphere(skySphereRadius, 30, 30, sphere);

	vector<XMFLOAT3> vertices(sphere.Vertices.size());

	for(size_t i = 0; i < sphere.Vertices.size(); ++i)
	{
		vertices[i] = sphere.Vertices[i].Position;
	}

	vector<USHORT> indices16;
	UINT count = sphere.Indices.size();
	indices16.assign(sphere.Indices.begin(), sphere.Indices.end());

	ModelObject::SetInputLayout(Sky::InputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ModelObject::SetVertexArray(&vertices[0], sizeof(XMFLOAT3) * vertices.size(), sizeof(XMFLOAT3));
	ModelObject::SetIndexArray(&indices16[0], sizeof(USHORT) * count, DXGI_FORMAT_R16_UINT, count);

	SkyRender *render = new SkyRender(L"DefaultRender");
	ModelObject::AddComponent(new Transform(L"DefaultTransform"));
	ModelObject::AddComponent(render);

	render->SetCubeMapSRV(m_CubeMapTex.TexSRV());
	render->SetCubeYunMapSRV(m_CubeYunMapTex.TexSRV());
	render->SetEffectTechnique("SkyTech");
	render->SetAngle(MathHelper::Pi * 2);

	return true;
}

Sky::~Sky()
{
}

void Sky::Draw(FXMMATRIX &viewProj, Camera &cam)
{
	Predraw();

	// center Sky about eye in world space
	XMFLOAT3 eyePos = cam.GetPosition();
	// Transform组件配置
	if (m_Transform)
	{
		// 世界变换
		m_Transform->Translate(eyePos.x, eyePos.y, eyePos.z);
	}

	SkyRender *render = (SkyRender *)m_Render;
	if (render)
	{
		render->SetWorldViewProj(World() * viewProj);
		render->SetEyePosW(cam.GetPosition());
		render->Apply(m_IndexCount);
	}
}

void Sky::Update(float dt)
{
	static float rad = 0.0f;
	rad += dt * 0.05f;
	while (rad > MathHelper::Pi * 2)
	{
		rad -= MathHelper::Pi * 2;
	}

	SkyRender *render = (SkyRender *)m_Render;
	if (render)
	{
		render->SetRotateY(XMMatrixRotationY(rad));
	}
}