//***************************************************************************************
// Texture.h by Linzer Lee
//***************************************************************************************
#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXPackedVector.h>

class Texture
{
public:
	Texture();
	Texture(ID3D11Device* device, std::wstring filepath = L"");
	// 创建纹理数组,该函数实现有问题
	Texture(ID3D11Device* device,
		ID3D11DeviceContext* context,
		std::vector<std::wstring> &filepaths);
	// for Height Map
	Texture(ID3D11Device* device,
		std::vector<float> &heightMap,
		UINT mapWidth,
		D3D11_TEXTURE2D_DESC &texDesc);
	Texture(Texture&& tex);
	~Texture();
	
	bool Load(ID3D11Device* device, std::wstring filepath);
	bool LoadHeightMap(ID3D11Device* device,
		std::vector<float> &heightMap,
		UINT mapWidth,
		D3D11_TEXTURE2D_DESC &texDesc);
	bool LoadTexArray(ID3D11Device* device,
		ID3D11DeviceContext* context,
		std::vector<std::wstring> &filepaths);
	bool CreateRandomTexture1D(ID3D11Device* device);


	ID3D11ShaderResourceView *TexSRV();
	ID3D11Texture1D *Tex1D();
	ID3D11Texture2D *Tex2D();
	void Get1DTexDesc(D3D11_TEXTURE1D_DESC *desc);
	void Get2DTexDesc(D3D11_TEXTURE2D_DESC *desc);
private:
	enum Type
	{
		NONE,
		DDS,
		WIC,
		HMP
	};

	ID3D11Device* m_Device;
	std::wstring m_FilePath;
	ID3D11Resource* m_Texture;
	ID3D11ShaderResourceView* m_TexSRV;
	Type m_Type;
};

#endif // TEXTURE_H