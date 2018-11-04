#include "BoardModel.h"
#include "Utils.h"
#include "MathHelper.h"
#include "Camera.h"
#include "Texture.h"
#include "Framework.h"
#include "BoardRender.h"
#include "Transform.h"

using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC BoardModel::InputDesc[2] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

ID3D11InputLayout* BoardModel::InputLayout = NULL;
BoardEffect* BoardModel::BoardFX = NULL;

bool BoardModel::InitClassEnv(ID3D11Device* device)
{
	D3DX11_PASS_DESC passDesc;

	BoardFX = new BoardEffect(device, L"FX/Board.fxo");
	BoardFX->Light3Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(BoardModel::InputDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &BoardModel::InputLayout));

	return true;
}

void BoardModel::DeinitClassEnv()
{
	SafeDelete(BoardModel::BoardFX);
	SafeRelease(BoardModel::InputLayout);
}

BoardModel::BoardModel(std::wstring name)
:	ModelObject(name),
	m_VettexCount(0)
{
}

BoardModel::~BoardModel()
{
}

void BoardModel::Init(BoardModel::Vertex *vs, UINT size)
{
	SetVertexArray(vs, sizeof(BoardModel::Vertex) * size, sizeof(BoardModel::Vertex));
	m_VettexCount = size;
	AddComponent(new Transform(L"DefaultTransform"));
	m_BoardRender = new BoardRender(L"DefaultRender");
	AddComponent(m_BoardRender);
}

void BoardModel::Predraw()
{
	UINT offset = 0;
	Framework &fw = Framework::Instance();
	fw.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	fw.IASetInputLayout(BoardModel::InputLayout);
	fw.IASetVertexBuffers(0, 1, &m_VB, &m_Stride, &offset);
}

void BoardModel::Draw(FXMMATRIX &viewProj, Camera &cam)
{
	BoardModel::Predraw();
	m_BoardRender->SetViewProj(viewProj);
	m_BoardRender->SetEyePosW(cam.GetPosition());
	m_BoardRender->Apply(m_VettexCount);
}