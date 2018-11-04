//***************************************************************************************
// Terrain.h by Linzer Lee
//   
// Class that renders a terrain using hardware tessellation and multitexturing.
//***************************************************************************************

#ifndef TERRAIN_H
#define TERRAIN_H

#include <string>
#include <vector>
#include <Windows.h>
#include <d3d11.h>

#include "MathHelper.h"
#include "Light.h"
#include "Effects.h"

class Camera;
class Texture;
class TerrainEffect;
struct DirectionalLight;

#pragma region Terrain
class Terrain
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 Tex;
		DirectX::XMFLOAT2 BoundsY;
	};

	struct Info
	{
		std::wstring HeightMapFilename;
		std::wstring LayerMapArrayFile;
		std::wstring BlendMapFilename;
		float HeightScale;
		UINT HeightmapWidth;
		UINT HeightmapHeight;
		float CellSpacing;
	};

public:
	static const D3D11_INPUT_ELEMENT_DESC InputDesc[3];
	static ID3D11InputLayout* InputLayout;
	static TerrainEffect* TerrainFX;
	static bool InitClassEnv(ID3D11Device* device);
	static void DeinitClassEnv();

public:
	Terrain();
	~Terrain();

	float GetWidth()const;
	float GetDepth()const;
	float GetHeight(float x, float z)const;

	DirectX::XMMATRIX GetWorld()const;
	void SetWorld(DirectX::CXMMATRIX M);

	void Init(const Terrain::Info& initInfo);

	void Draw(DirectX::FXMMATRIX &view, DirectX::FXMMATRIX &proj, 
		const DirectX::XMFLOAT3& eyePos, DirectionalLight lights[3]);

private:
	void LoadHeightmap();
	void Smooth();
	bool InBounds(int i, int j);
	float Average(int i, int j);
	void CalcAllPatchBoundsY();
	void CalcPatchBoundsY(UINT i, UINT j);
	void BuildQuadPatchVB(ID3D11Device* device);
	void BuildQuadPatchIB(ID3D11Device* device);
	void BuildHeightmapSRV(ID3D11Device* device);

private:
	ID3D11Buffer* m_QuadPatchVB;
	ID3D11Buffer* m_QuadPatchIB;
	Texture* m_LayerMapArrayTex;
	Texture* m_BlendMapTex;
	Texture* m_HeightMapTex;
	Terrain::Info m_Info;
	UINT m_NumPatchVertices;
	UINT m_NumPatchQuadFaces;
	UINT m_NumPatchVertRows;
	UINT m_NumPatchVertCols;
	DirectX::XMFLOAT4X4 m_World;
	Material m_Mat;
	std::vector<DirectX::XMFLOAT2> m_PatchBoundsY;
	std::vector<float> m_Heightmap;

	// Divide heightmap into patches such that each patch has CellsPerPatch cells
	// and CellsPerPatch+1 vertices.  Use 64 so that if we tessellate all the way 
	// to 64, we use all the data from the heightmap.  
	static const int CellsPerPatch = 64;
};
#pragma endregion

#endif // TERRAIN_H