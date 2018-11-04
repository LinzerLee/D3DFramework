//***************************************************************************************
// ParticleSystem.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "ParticleSystem.h"
#include "Utils.h"
#include "Effects.h"
#include "Camera.h"
#include "Framework.h"
 
using namespace DirectX;

const D3D11_INPUT_ELEMENT_DESC ParticleSystem::InputDesc[5] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


ID3D11InputLayout* ParticleSystem::InputLayout = NULL;
ParticleEffect* ParticleSystem::FireFX = NULL;
ParticleEffect* ParticleSystem::RainFX = NULL;
ParticleEffect* ParticleSystem::SnowFX = NULL;

bool ParticleSystem::InitClassEnv(ID3D11Device* device)
{
	D3DX11_PASS_DESC passDesc;

	ParticleSystem::FireFX = new ParticleEffect(device, L"FX/Fire.fxo");
	ParticleSystem::RainFX = new ParticleEffect(device, L"FX/Rain.fxo");
	ParticleSystem::SnowFX = new ParticleEffect(device, L"FX/Snow.fxo");

	FireFX->StreamOutTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(ParticleSystem::InputDesc, 5, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &ParticleSystem::InputLayout));

	return true;
}

void ParticleSystem::DeinitClassEnv()
{
	SafeDelete(ParticleSystem::FireFX);
	SafeDelete(ParticleSystem::RainFX);
	SafeDelete(ParticleSystem::SnowFX);
	SafeRelease(ParticleSystem::InputLayout);
}

ParticleSystem::ParticleSystem()
: m_InitVB(0), m_DrawVB(0), m_StreamOutVB(0), m_TexArraySRV(0), m_RandomTexSRV(0)
{
	m_FirstRun = true;
	m_GameTime = 0.0f;
	m_TimeStep = 0.0f;
	m_Age      = 0.0f;

	m_EmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_EmitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);
}

ParticleSystem::~ParticleSystem()
{
	SafeRelease(m_InitVB);
	SafeRelease(m_DrawVB);
	SafeRelease(m_StreamOutVB);
}

float ParticleSystem::GetAge()const
{
	return m_Age;
}

void ParticleSystem::SetEmitPos(const XMFLOAT3& emitPosW)
{
	m_EmitPosW = emitPosW;
}

void ParticleSystem::SetEmitDir(const XMFLOAT3& emitDirW)
{
	m_EmitDirW = emitDirW;
}

void ParticleSystem::Init(ID3D11Device* device, ParticleEffect* fx, ID3D11ShaderResourceView* texArraySRV, 
	                      ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles)
{
	m_MaxParticles = maxParticles;

	m_FX = fx;

	m_TexArraySRV  = texArraySRV;
	m_RandomTexSRV = randomTexSRV;

	BuildVB(device);
}

void ParticleSystem::Reset()
{
	m_FirstRun = true;
	m_Age      = 0.0f;
}

void ParticleSystem::Update(float dt, float gameTime)
{
	m_GameTime = gameTime;
	m_TimeStep = dt;

	m_Age += dt;

	while (m_Age > 1.0f)
		m_Age -= 1.0f;
}

void ParticleSystem::Predraw()
{
	// Set IA stage.
	Framework::Instance().DeviceContext()->IASetInputLayout(ParticleSystem::InputLayout);
	Framework::Instance().DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void ParticleSystem::Draw(FXMMATRIX &viewProj, Camera& cam)
{
	ParticleSystem::Predraw();

	m_FX->SetViewProj(viewProj);
	m_FX->SetGameTime(m_GameTime);
	m_FX->SetTimeStep(m_TimeStep);
	m_FX->SetEyePosW(cam.GetPosition());
	m_FX->SetEmitPosW(m_EmitPosW);
	m_FX->SetEmitDirW(m_EmitDirW);
	m_FX->SetTexArray(m_TexArraySRV);
	m_FX->SetRandomTex(m_RandomTexSRV);

	
	UINT stride = sizeof(ParticleSystem::Vertex);
    UINT offset = 0;
	auto dc = Framework::Instance().DeviceContext();

	// On the first pass, use the initialization VB.  Otherwise, use
	// the VB that contains the current particle list.
	if( m_FirstRun )
		dc->IASetVertexBuffers(0, 1, &m_InitVB, &stride, &offset);
	else
		dc->IASetVertexBuffers(0, 1, &m_DrawVB, &stride, &offset);

	//
	// Draw the current particle list using stream-out only to update them.  
	// The updated vertices are streamed-out to the target VB. 
	//
	dc->SOSetTargets(1, &m_StreamOutVB, &offset);

    D3DX11_TECHNIQUE_DESC techDesc;
	m_FX->StreamOutTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        m_FX->StreamOutTech->GetPassByIndex( p )->Apply(0, dc);
        
		if( m_FirstRun )
		{
			dc->Draw(1, 0);
			m_FirstRun = false;
		}
		else
		{
			dc->DrawAuto();
		}
    }

	// done streaming-out--unbind the vertex buffer
	ID3D11Buffer* bufferArray[1] = {0};
	dc->SOSetTargets(1, bufferArray, &offset);

	// ping-pong the vertex buffers
	std::swap(m_DrawVB, m_StreamOutVB);

	//
	// Draw the updated particle system we just streamed-out. 
	//
	dc->IASetVertexBuffers(0, 1, &m_DrawVB, &stride, &offset);

	m_FX->DrawTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        m_FX->DrawTech->GetPassByIndex( p )->Apply(0, dc);
        
		dc->DrawAuto();
    }

	// 恢复效果文件修改的状态
	dc->RSSetState(NULL);
	dc->OMSetDepthStencilState(NULL, 0);
	dc->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
}

void ParticleSystem::BuildVB(ID3D11Device* device)
{
	//
	// Create the buffer to kick-off the particle system.
	//

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(ParticleSystem::Vertex) * 1;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// The initial particle emitter has type 0 and age 0.  The rest
	// of the particle attributes do not apply to an emitter.
	ParticleSystem::Vertex p;
	ZeroMemory(&p, sizeof(ParticleSystem::Vertex));
	p.Age  = 0.0f;
	p.Type = 0; 
 
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &p;

	HR(device->CreateBuffer(&vbd, &vinitData, &m_InitVB));
	
	//
	// Create the ping-pong buffers for stream-out and drawing.
	//
	vbd.ByteWidth = sizeof(ParticleSystem::Vertex) * m_MaxParticles;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

    HR(device->CreateBuffer(&vbd, 0, &m_DrawVB));
	HR(device->CreateBuffer(&vbd, 0, &m_StreamOutVB));
}