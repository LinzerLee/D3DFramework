#include <algorithm>
#include "./3rd/DirectXTex/DDSTextureLoader/DDSTextureLoader.h"
#include "./3rd/DirectXTex/WICTextureLoader/WICTextureLoader.h"

#include "Utils.h"
#include "Texture.h"

using namespace std;
using namespace DirectX;
using namespace PackedVector;

Texture::Texture()
:	m_Device(NULL),
	m_Type(Texture::Type::NONE),
	m_Texture(NULL),
	m_TexSRV(NULL)
{
}

Texture::Texture(ID3D11Device* device, wstring filepath)
:	m_Device(device),
	m_Type(Texture::Type::NONE),
	m_Texture(NULL),
	m_TexSRV(NULL)
{
	Load(device, filepath);
}

Texture::Texture(ID3D11Device* device,
	ID3D11DeviceContext* context,
	vector<wstring> &filepaths)
{
	LoadTexArray(device, context, filepaths);
}

Texture::Texture(ID3D11Device* device, 
	vector<float> &heightMap, 
	UINT mapWidth,
	D3D11_TEXTURE2D_DESC &texDesc)
{
	LoadHeightMap(device, heightMap, mapWidth, texDesc);
}

Texture::Texture(Texture&& tex)
{
	m_Device = tex.m_Device;
	m_FilePath = move(tex.m_FilePath);
	m_Texture = tex.m_Texture;
	m_TexSRV = tex.m_TexSRV;
	m_Type = tex.m_Type;

	tex.m_Device = NULL;
	tex.m_Texture = NULL;
	tex.m_TexSRV = NULL;
}

Texture::~Texture()
{
	SafeRelease(m_Texture);
	SafeRelease(m_TexSRV);
}

bool Texture::Load(ID3D11Device* device, wstring filepath)
{
	assert(device);
	m_Device = device;
	wstring::size_type pos = filepath.rfind('.');
	if (wstring::npos != pos)
	{
		wstring type = filepath.substr(pos + 1, filepath.size() - pos - 1);
		transform(type.begin(), type.end(), type.begin(), ::toupper);
		if (L"DDS" == type)
		{
			m_Type = Type::DDS;
			m_FilePath = filepath;

			ID3D11Resource* texResource = NULL;
			HR(CreateDDSTextureFromFile(m_Device, m_FilePath.c_str(), &texResource, &m_TexSRV));
			m_Texture = (ID3D11Texture2D *)texResource;
		}
		else if (L"PNG" == type)
		{
			m_Type = Type::WIC;
			m_FilePath = filepath;

			ID3D11Resource* texResource = NULL;
			HR(CreateWICTextureFromFile(m_Device, m_FilePath.c_str(), &texResource, &m_TexSRV));
			m_Texture = (ID3D11Texture2D *)texResource;
		}
	}

	return true;
}

bool Texture::LoadHeightMap(ID3D11Device* device,
	vector<float> &heightMap,
	UINT mapWidth,
	D3D11_TEXTURE2D_DESC &texDesc)
{
	assert(device);
	m_Device = device;
	std::vector<HALF> hmap(heightMap.size());
	std::transform(heightMap.begin(), heightMap.end(), hmap.begin(), XMConvertFloatToHalf);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &hmap[0];
	data.SysMemPitch = mapWidth * sizeof(HALF);
	data.SysMemSlicePitch = 0;

	ID3D11Texture2D* tex = 0;
	HR(device->CreateTexture2D(&texDesc, &data, &tex));
	m_Texture = tex;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	HR(device->CreateShaderResourceView(m_Texture, &srvDesc, &m_TexSRV));

	m_Type = Type::HMP;

	return true;
}

bool Texture::LoadTexArray(ID3D11Device* device,
	ID3D11DeviceContext* context,
	vector<wstring> &filepaths)
{
	assert(device);
	assert(context);
	m_Device = device;
	size_t size = filepaths.size();
	vector<Texture> textures;
	for (auto file : filepaths)
	{
		textures.push_back(Texture(device, file));
	}

	D3D11_TEXTURE2D_DESC texElementDesc;
	textures[0].Get2DTexDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	HR(device->CreateTexture2D(&texArrayDesc, 0, &texArray));

	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			HR(context->Map(textures[texElement].Tex2D(), mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

			context->UpdateSubresource(texArray,
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			context->Unmap(textures[texElement].Tex2D(), mipLevel);
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	ID3D11ShaderResourceView* texArraySRV = 0;
	HR(device->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV));

	m_Texture = texArray;
	m_TexSRV = texArraySRV;

	return true;
}

bool Texture::CreateRandomTexture1D(ID3D11Device* device)
{
	// 创建随机数据
	XMFLOAT4 randomValues[1024];

	for (int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].y = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].z = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].w = MathHelper::RandF(-1.0f, 1.0f);
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = 0;

	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;
	HR(device->CreateTexture1D(&texDesc, &initData, &randomTex));
	m_Texture = randomTex;

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	HR(device->CreateShaderResourceView(randomTex, &viewDesc, &m_TexSRV));

	return true;
}

ID3D11ShaderResourceView *Texture::TexSRV()
{
	return m_TexSRV;
}

ID3D11Texture1D *Texture::Tex1D()
{
	return (ID3D11Texture1D *)m_Texture;
}

ID3D11Texture2D *Texture::Tex2D()
{
	return (ID3D11Texture2D *)m_Texture;
}

void Texture::Get1DTexDesc(D3D11_TEXTURE1D_DESC *desc)
{
	Tex1D()->GetDesc(desc);
}

void Texture::Get2DTexDesc(D3D11_TEXTURE2D_DESC *desc)
{
	Tex2D()->GetDesc(desc);
}