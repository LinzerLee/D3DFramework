#include "WavesModel.h"
#include "Framework.h"
#include "Transform.h"
#include "Render.h"
#include "Utils.h"

using namespace std;
using namespace DirectX;

WavesModel::WavesModel(std::wstring name)
:	ModelObject(name),
	m_WaterTexOffset(0.0f, 0.0f)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_TexTransform, I);
}

WavesModel::~WavesModel()
{
}

void WavesModel::Init(UINT m, UINT n, float dx, float dt, float speed, float damping)
{
	m_Waves.Init(m, n, dx, dt, speed, damping);

	SetInputLayout(Framework::BasicInputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_Stride = sizeof(Framework::BasicVertex);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = m_Stride * m_Waves.VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	HR(Framework::Instance().Device()->CreateBuffer(&vbd, NULL, &m_VB));

	m_Format = DXGI_FORMAT_R32_UINT;
	m_IndexCount = 3 * m_Waves.TriangleCount();

	vector<UINT> indices(m_IndexCount);
	m = m_Waves.RowCount();
	n = m_Waves.ColumnCount();
	int k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (DWORD j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1)*n + j;

			indices[k + 3] = (i + 1)*n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}

	ModelObject::SetIndexArray(&indices[0], sizeof(UINT) * m_IndexCount, m_Format, m_IndexCount);
}

void WavesModel::Update(float dt, bool isDisturb)
{
	//
	// Every time, generate a random wave.
	//
	if (isDisturb)
	{
		DWORD i = 5 + rand() % (m_Waves.RowCount() - 10);
		DWORD j = 5 + rand() % (m_Waves.ColumnCount() - 10);

		float r = MathHelper::RandF(-0.3f, 0.3f);

		m_Waves.Disturb(i, j, r);
	}
	
	m_Waves.Update(dt);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(Framework::Instance().DeviceContext()->Map(m_VB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	Framework::BasicVertex *v = reinterpret_cast<Framework::BasicVertex *>(mappedData.pData);
	for (UINT i = 0; i < m_Waves.VertexCount(); ++i)
	{
		v[i].Pos = m_Waves[i];
		v[i].Normal = m_Waves.Normal(i);

		// Derive tex-coords in [0,1] from position.
		v[i].Tex.x = 0.5f + m_Waves[i].x / m_Waves.Width();
		v[i].Tex.y = 0.5f - m_Waves[i].z / m_Waves.Depth();
	}

	Framework::Instance().DeviceContext()->Unmap(m_VB, 0);

	//
	// Animate water texture coordinates.
	//
	// Tile water texture.
	XMMATRIX wavesScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);

	// Translate texture over time.
	m_WaterTexOffset.y += 0.05f * dt;
	m_WaterTexOffset.x += 0.1f * dt;
	XMMATRIX wavesOffset = XMMatrixTranslation(m_WaterTexOffset.x, m_WaterTexOffset.y, 0.0f);

	// Combine scale and translation.
	XMStoreFloat4x4(&m_TexTransform, wavesScale * wavesOffset);
}

void WavesModel::SetTexture(wstring file)
{
	m_WavesTex.Load(Framework::Instance().Device(), file);
}

void WavesModel::SetMaterial(Material mat)
{
	m_Render->SetMaterial(mat);
}

void WavesModel::AddReflection(Drawable *obj)
{
	for (auto ref : m_Reflections)
	{
		if (ref == obj)
		{
			return;
		}
	}

	m_Reflections.push_back(obj);
}

void WavesModel::Draw(FXMMATRIX &viewProj, Camera &cam)
{
	UINT offset = 0;
	Framework &fw = Framework::Instance();
	fw.IASetInputLayout(m_IL);
	fw.IASetPrimitiveTopology(m_Topology);
	fw.IASetVertexBuffers(0, 1, &m_VB, &m_Stride, &offset);
	fw.IASetIndexBuffer(m_IB, m_Format, 0);

	if (m_Render)
	{
		m_Render->SetWorld(World());
		m_Render->SetWorldViewProj(World() * viewProj);
		m_Render->SetTexTransform(XMLoadFloat4x4(&m_TexTransform));
		m_Render->SetDiffuseMapSRV(m_WavesTex.TexSRV());
		m_Render->Apply(m_IndexCount);
	}
	else
	{
		assert(false);
	}
}
