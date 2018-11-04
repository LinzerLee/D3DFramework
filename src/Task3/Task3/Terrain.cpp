#include <fstream>
#include <sstream>
#include <algorithm>

#include "Utils.h"
#include "MathHelper.h"
#include "Terrain.h"
#include "Camera.h"
#include "Texture.h"

using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC Terrain::InputDesc[3] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

ID3D11InputLayout* Terrain::InputLayout = NULL;
TerrainEffect* Terrain::TerrainFX = NULL;

bool Terrain::InitClassEnv(ID3D11Device* device)
{
	D3DX11_PASS_DESC passDesc;

	TerrainFX = new TerrainEffect(device, L"./FX/Terrain.fxo");
	TerrainFX->Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(Terrain::InputDesc, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &Terrain::InputLayout));
	
	return true;
}

void Terrain::DeinitClassEnv()
{
	SafeDelete(Terrain::TerrainFX);
	SafeRelease(Terrain::InputLayout);
}

Terrain::Terrain() :
	m_QuadPatchVB(NULL),
	m_QuadPatchIB(NULL),
	m_LayerMapArrayTex(NULL),
	m_BlendMapTex(NULL),
	m_HeightMapTex(NULL),
	m_NumPatchVertices(NULL),
	m_NumPatchQuadFaces(NULL),
	m_NumPatchVertRows(NULL),
	m_NumPatchVertCols(NULL)
{
	XMStoreFloat4x4(&m_World, XMMatrixIdentity());

	m_Mat.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Mat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
	m_Mat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

Terrain::~Terrain()
{
	SafeRelease(m_QuadPatchVB);
	SafeRelease(m_QuadPatchIB);
	SafeDelete(m_LayerMapArrayTex);
	SafeDelete(m_BlendMapTex);
	SafeDelete(m_HeightMapTex);
}

float Terrain::GetWidth()const
{
	// Total terrain width.
	return (m_Info.HeightmapWidth - 1) * m_Info.CellSpacing;
}

float Terrain::GetDepth()const
{
	// Total terrain depth.
	return (m_Info.HeightmapHeight - 1)  *m_Info.CellSpacing;
}

float Terrain::GetHeight(float x, float z)const
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f * GetWidth()) / m_Info.CellSpacing;
	float d = (z - 0.5f * GetDepth()) / -m_Info.CellSpacing;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = m_Heightmap[row * m_Info.HeightmapWidth + col];
	float B = m_Heightmap[row * m_Info.HeightmapWidth + col + 1];
	float C = m_Heightmap[(row + 1) * m_Info.HeightmapWidth + col];
	float D = m_Heightmap[(row + 1) * m_Info.HeightmapWidth + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s * uy + t * vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}
}

XMMATRIX Terrain::GetWorld()const
{
	return XMLoadFloat4x4(&m_World);
}

void Terrain::SetWorld(CXMMATRIX M)
{
	XMStoreFloat4x4(&m_World, M);
}

void Terrain::Init(const Terrain::Info& info)
{
	ID3D11Device *device = Framework::Instance().Device();
	m_Info = info;

	// Divide heightmap into patches such that each patch has CellsPerPatch.
	m_NumPatchVertRows = ((m_Info.HeightmapHeight - 1) / CellsPerPatch) + 1;
	m_NumPatchVertCols = ((m_Info.HeightmapWidth - 1) / CellsPerPatch) + 1;

	m_NumPatchVertices = m_NumPatchVertRows * m_NumPatchVertCols;
	m_NumPatchQuadFaces = (m_NumPatchVertRows - 1)*(m_NumPatchVertCols - 1);

	LoadHeightmap();
	Smooth();
	CalcAllPatchBoundsY();

	BuildQuadPatchVB(device);
	BuildQuadPatchIB(device);
	BuildHeightmapSRV(device);

	m_LayerMapArrayTex = new Texture(device, m_Info.LayerMapArrayFile);
	m_BlendMapTex = new Texture(device, m_Info.BlendMapFilename);
}

void Terrain::Draw(FXMMATRIX &view, FXMMATRIX &proj, 
	const XMFLOAT3& eyePos, DirectionalLight lights[3])
{
	Framework &fw = Framework::Instance();
	fw.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	fw.IASetInputLayout(InputLayout);

	UINT stride = sizeof(Terrain::InputLayout);
	UINT offset = 0;
	fw.IASetVertexBuffers(0, 1, &m_QuadPatchVB, &stride, &offset);
	fw.IASetIndexBuffer(m_QuadPatchIB, DXGI_FORMAT_R16_UINT, 0);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);//  cam.ViewProj();
	XMMATRIX world = XMLoadFloat4x4(&m_World);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX worldViewProj = world * viewProj;

	XMFLOAT4 worldPlanes[6];
	MathHelper::ExtractFrustumPlanes(worldPlanes, viewProj);

	// Set per frame constants.
	TerrainFX->SetViewProj(viewProj);
	TerrainFX->SetEyePosW(eyePos/*cam.GetPosition()*/);
	TerrainFX->SetDirLights(lights);
	TerrainFX->SetFogColor(Colors::Silver);
	TerrainFX->SetFogStart(15.0f);
	TerrainFX->SetFogRange(175.0f);
	TerrainFX->SetMinDist(20.0f);
	TerrainFX->SetMaxDist(500.0f);
	TerrainFX->SetMinTess(0.0f);
	TerrainFX->SetMaxTess(6.0f);
	TerrainFX->SetTexelCellSpaceU(1.0f / m_Info.HeightmapWidth);
	TerrainFX->SetTexelCellSpaceV(1.0f / m_Info.HeightmapHeight);
	TerrainFX->SetWorldCellSpace(m_Info.CellSpacing);
	TerrainFX->SetWorldFrustumPlanes(worldPlanes);

	TerrainFX->SetLayerMapArraySRV(m_LayerMapArrayTex->TexSRV());
	TerrainFX->SetBlendMapSRV(m_BlendMapTex->TexSRV());
	TerrainFX->SetHeightMapSRV(m_HeightMapTex->TexSRV());

	TerrainFX->SetMaterial(m_Mat);

	ID3DX11EffectTechnique* tech = TerrainFX->Light1Tech;
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (UINT i = 0; i < techDesc.Passes; ++i)
	{
		ID3DX11EffectPass* pass = tech->GetPassByIndex(i);
		pass->Apply(0, fw.DeviceContext());

		fw.DeviceContext()->DrawIndexed(m_NumPatchQuadFaces * 4, 0, 0);
	}

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	fw.DeviceContext()->HSSetShader(0, 0, 0);
	fw.DeviceContext()->DSSetShader(0, 0, 0);
}

void Terrain::LoadHeightmap()
{
	// A height for each vertex
	std::vector<unsigned char> in(m_Info.HeightmapWidth * m_Info.HeightmapHeight);

	// Open the file.
	std::ifstream inFile;
	inFile.open(m_Info.HeightMapFilename.c_str(), std::ios_base::binary);

	if (inFile.is_open())
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file.
		inFile.close();
	}

	// Copy the array data into a float array and scale it.
	m_Heightmap.resize(m_Info.HeightmapHeight * m_Info.HeightmapWidth, 0);
	for (UINT i = 0; i < m_Info.HeightmapHeight * m_Info.HeightmapWidth; ++i)
	{
		m_Heightmap[i] = (in[i] / 255.0f) * m_Info.HeightScale;
	}
}

void Terrain::Smooth()
{
	std::vector<float> dest(m_Heightmap.size());

	for (UINT i = 0; i < m_Info.HeightmapHeight; ++i)
	{
		for (UINT j = 0; j < m_Info.HeightmapWidth; ++j)
		{
			dest[i * m_Info.HeightmapWidth + j] = Average(i, j);
		}
	}

	// Replace the old heightmap with the filtered one.
	m_Heightmap = dest;
}

bool Terrain::InBounds(int i, int j)
{
	// True if ij are valid indices; false otherwise.
	return
		i >= 0 && i < (int)m_Info.HeightmapHeight &&
		j >= 0 && j < (int)m_Info.HeightmapWidth;
}

float Terrain::Average(int i, int j)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	// Use int to allow negatives.  If we use UINT, @ i=0, m=i-1=UINT_MAX
	// and no iterations of the outer for loop occur.
	for (int m = i - 1; m <= i + 1; ++m)
	{
		for (int n = j - 1; n <= j + 1; ++n)
		{
			if (InBounds(m, n))
			{
				avg += m_Heightmap[m * m_Info.HeightmapWidth + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

void Terrain::CalcAllPatchBoundsY()
{
	m_PatchBoundsY.resize(m_NumPatchQuadFaces);

	// For each patch
	for (UINT i = 0; i < m_NumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < m_NumPatchVertCols - 1; ++j)
		{
			CalcPatchBoundsY(i, j);
		}
	}
}

void Terrain::CalcPatchBoundsY(UINT i, UINT j)
{
	// Scan the heightmap values this patch covers and compute the min/max height.

	UINT x0 = j * CellsPerPatch;
	UINT x1 = (j + 1)*CellsPerPatch;

	UINT y0 = i * CellsPerPatch;
	UINT y1 = (i + 1)*CellsPerPatch;

	float minY = +MathHelper::Infinity;
	float maxY = -MathHelper::Infinity;
	for (UINT y = y0; y <= y1; ++y)
	{
		for (UINT x = x0; x <= x1; ++x)
		{
			UINT k = y * m_Info.HeightmapWidth + x;
			minY = MathHelper::Min(minY, m_Heightmap[k]);
			maxY = MathHelper::Max(maxY, m_Heightmap[k]);
		}
	}

	UINT patchID = i * (m_NumPatchVertCols - 1) + j;
	m_PatchBoundsY[patchID] = XMFLOAT2(minY, maxY);
}

void Terrain::BuildQuadPatchVB(ID3D11Device* device)
{
	std::vector<Terrain::Vertex> patchVertices(m_NumPatchVertRows * m_NumPatchVertCols);

	float halfWidth = 0.5f*GetWidth();
	float halfDepth = 0.5f*GetDepth();

	float patchWidth = GetWidth() / (m_NumPatchVertCols - 1);
	float patchDepth = GetDepth() / (m_NumPatchVertRows - 1);
	float du = 1.0f / (m_NumPatchVertCols - 1);
	float dv = 1.0f / (m_NumPatchVertRows - 1);

	for (UINT i = 0; i < m_NumPatchVertRows; ++i)
	{
		float z = halfDepth - i * patchDepth;
		for (UINT j = 0; j < m_NumPatchVertCols; ++j)
		{
			float x = -halfWidth + j * patchWidth;

			patchVertices[i*m_NumPatchVertCols + j].Pos = XMFLOAT3(x, 0.0f, z);

			// Stretch texture over grid.
			patchVertices[i*m_NumPatchVertCols + j].Tex.x = j * du;
			patchVertices[i*m_NumPatchVertCols + j].Tex.y = i * dv;
		}
	}

	// Store axis-aligned bounding box y-bounds in upper-left patch corner.
	for (UINT i = 0; i < m_NumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < m_NumPatchVertCols - 1; ++j)
		{
			UINT patchID = i * (m_NumPatchVertCols - 1) + j;
			patchVertices[i*m_NumPatchVertCols + j].BoundsY = m_PatchBoundsY[patchID];
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Terrain::Vertex) * patchVertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &patchVertices[0];
	device->CreateBuffer(&vbd, &vinitData, &m_QuadPatchVB);
}

void Terrain::BuildQuadPatchIB(ID3D11Device* device)
{
	std::vector<USHORT> indices(m_NumPatchQuadFaces * 4); // 4 indices per quad face

														 // Iterate over each quad and compute indices.
	int k = 0;
	for (UINT i = 0; i < m_NumPatchVertRows - 1; ++i)
	{
		for (UINT j = 0; j < m_NumPatchVertCols - 1; ++j)
		{
			// Top row of 2x2 quad patch
			indices[k] = i * m_NumPatchVertCols + j;
			indices[k + 1] = i * m_NumPatchVertCols + j + 1;

			// Bottom row of 2x2 quad patch
			indices[k + 2] = (i + 1) * m_NumPatchVertCols + j;
			indices[k + 3] = (i + 1) * m_NumPatchVertCols + j + 1;

			k += 4; // next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	device->CreateBuffer(&ibd, &iinitData, &m_QuadPatchIB);
}

void Terrain::BuildHeightmapSRV(ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = m_Info.HeightmapWidth;
	texDesc.Height = m_Info.HeightmapHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	m_HeightMapTex = new Texture(device, m_Heightmap, m_Info.HeightmapWidth, texDesc);
}
