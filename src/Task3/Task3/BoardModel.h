//***************************************************************************************
// BoardModel.h by Linzer Lee
//***************************************************************************************

#ifndef BOARDMODEL_H
#define BOARDMODEL_H

#include <string>
#include <vector>
#include <d3d11.h>

#include "ModelObject.h"
#include "Light.h"
#include "Texture.h"

class BoardEffect;
class BoardRender;

class BoardModel : public ModelObject
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 Size;

		Vertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 size) { Pos = pos; Size = size; };
	};

public:
	static const D3D11_INPUT_ELEMENT_DESC InputDesc[2];
	static ID3D11InputLayout* InputLayout;
	static BoardEffect* BoardFX;
	static bool InitClassEnv(ID3D11Device* device);
	static void DeinitClassEnv();

public:
	BoardModel(std::wstring name = L"Board");
	~BoardModel();

	void Init(BoardModel::Vertex *vs, UINT size);
	virtual void Predraw() override;
	virtual void Draw(DirectX::FXMMATRIX &viewProj, Camera &cam) override;

protected:
	BoardRender *m_BoardRender;
	UINT m_VettexCount;

private:
	BoardModel(const BoardModel& rhs) = delete;
	BoardModel& operator=(const BoardModel& rhs) = delete;
};

#endif // BOARDMODEL_H