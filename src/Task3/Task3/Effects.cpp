#include <fstream>
#include <vector>

#include "Utils.h"
#include "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: m_FX(NULL)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, device, &m_FX));
}

Effect::~Effect()
{
	SafeRelease(m_FX);
}

ID3DX11EffectTechnique *Effect::GetTechniqueByName(const char *techName)
{
	return m_FX->GetTechniqueByName(techName);
}
#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light0Tech	  = m_FX->GetTechniqueByName("Light0");
	Light1Tech    = m_FX->GetTechniqueByName("Light1");
	Light2Tech    = m_FX->GetTechniqueByName("Light2");
	Light3Tech    = m_FX->GetTechniqueByName("Light3");
					 
	Light0TexTech = m_FX->GetTechniqueByName("Light0Tex");
	Light1TexTech = m_FX->GetTechniqueByName("Light1Tex");
	Light2TexTech = m_FX->GetTechniqueByName("Light2Tex");
	Light3TexTech = m_FX->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = m_FX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = m_FX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = m_FX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = m_FX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech    = m_FX->GetTechniqueByName("Light1Fog");
	Light2FogTech    = m_FX->GetTechniqueByName("Light2Fog");
	Light3FogTech    = m_FX->GetTechniqueByName("Light3Fog");
						
	Light0TexFogTech = m_FX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = m_FX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = m_FX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = m_FX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = m_FX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = m_FX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = m_FX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = m_FX->GetTechniqueByName("Light3TexAlphaClipFog");

	Light1ReflectTech    = m_FX->GetTechniqueByName("Light1Reflect");
	Light2ReflectTech    = m_FX->GetTechniqueByName("Light2Reflect");
	Light3ReflectTech    = m_FX->GetTechniqueByName("Light3Reflect");
							
	Light0TexReflectTech = m_FX->GetTechniqueByName("Light0TexReflect");
	Light1TexReflectTech = m_FX->GetTechniqueByName("Light1TexReflect");
	Light2TexReflectTech = m_FX->GetTechniqueByName("Light2TexReflect");
	Light3TexReflectTech = m_FX->GetTechniqueByName("Light3TexReflect");

	Light0TexAlphaClipReflectTech = m_FX->GetTechniqueByName("Light0TexAlphaClipReflect");
	Light1TexAlphaClipReflectTech = m_FX->GetTechniqueByName("Light1TexAlphaClipReflect");
	Light2TexAlphaClipReflectTech = m_FX->GetTechniqueByName("Light2TexAlphaClipReflect");
	Light3TexAlphaClipReflectTech = m_FX->GetTechniqueByName("Light3TexAlphaClipReflect");

	Light1FogReflectTech    = m_FX->GetTechniqueByName("Light1FogReflect");
	Light2FogReflectTech    = m_FX->GetTechniqueByName("Light2FogReflect");
	Light3FogReflectTech    = m_FX->GetTechniqueByName("Light3FogReflect");
							   
	Light0TexFogReflectTech = m_FX->GetTechniqueByName("Light0TexFogReflect");
	Light1TexFogReflectTech = m_FX->GetTechniqueByName("Light1TexFogReflect");
	Light2TexFogReflectTech = m_FX->GetTechniqueByName("Light2TexFogReflect");
	Light3TexFogReflectTech = m_FX->GetTechniqueByName("Light3TexFogReflect");

	Light0TexAlphaClipFogReflectTech = m_FX->GetTechniqueByName("Light0TexAlphaClipFogReflect");
	Light1TexAlphaClipFogReflectTech = m_FX->GetTechniqueByName("Light1TexAlphaClipFogReflect");
	Light2TexAlphaClipFogReflectTech = m_FX->GetTechniqueByName("Light2TexAlphaClipFogReflect");
	Light3TexAlphaClipFogReflectTech = m_FX->GetTechniqueByName("Light3TexAlphaClipFogReflect");

	WorldViewProj     = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = m_FX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = m_FX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = m_FX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = m_FX->GetVariableByName("gFogColor")->AsVector();
	FogStart          = m_FX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = m_FX->GetVariableByName("gFogRange")->AsScalar();
	DirLights         = m_FX->GetVariableByName("gDirLights");
	Mat               = m_FX->GetVariableByName("gMaterial");
	DiffuseMap        = m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap           = m_FX->GetVariableByName("gCubeMap")->AsShaderResource();
}

BasicEffect::~BasicEffect()
{
}
#pragma endregion

#pragma region TerrainEffect
TerrainEffect::TerrainEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech = m_FX->GetTechniqueByName("Light1");
	Light2Tech = m_FX->GetTechniqueByName("Light2");
	Light3Tech = m_FX->GetTechniqueByName("Light3");
	Light1FogTech = m_FX->GetTechniqueByName("Light1Fog");
	Light2FogTech = m_FX->GetTechniqueByName("Light2Fog");
	Light3FogTech = m_FX->GetTechniqueByName("Light3Fog");

	ViewProj = m_FX->GetVariableByName("gViewProj")->AsMatrix();
	EyePosW = m_FX->GetVariableByName("gEyePosW")->AsVector();
	FogColor = m_FX->GetVariableByName("gFogColor")->AsVector();
	FogStart = m_FX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = m_FX->GetVariableByName("gFogRange")->AsScalar();
	DirLights = m_FX->GetVariableByName("gDirLights");
	Mat = m_FX->GetVariableByName("gMaterial");

	MinDist = m_FX->GetVariableByName("gMinDist")->AsScalar();
	MaxDist = m_FX->GetVariableByName("gMaxDist")->AsScalar();
	MinTess = m_FX->GetVariableByName("gMinTess")->AsScalar();
	MaxTess = m_FX->GetVariableByName("gMaxTess")->AsScalar();
	TexelCellSpaceU = m_FX->GetVariableByName("gTexelCellSpaceU")->AsScalar();
	TexelCellSpaceV = m_FX->GetVariableByName("gTexelCellSpaceV")->AsScalar();
	WorldCellSpace = m_FX->GetVariableByName("gWorldCellSpace")->AsScalar();
	WorldFrustumPlanes = m_FX->GetVariableByName("gWorldFrustumPlanes")->AsVector();

	LayerMapArray = m_FX->GetVariableByName("gLayerMapArray")->AsShaderResource();
	BlendMap = m_FX->GetVariableByName("gBlendMap")->AsShaderResource();
	HeightMap = m_FX->GetVariableByName("gHeightMap")->AsShaderResource();
}

TerrainEffect::~TerrainEffect()
{
}
#pragma endregion

#pragma region SkyEffect
SkyEffect::SkyEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SkyTech       = m_FX->GetTechniqueByName("SkyTech");
	WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap       = m_FX->GetVariableByName("gCubeMap")->AsShaderResource();
	CubeYunMap	  = m_FX->GetVariableByName("gCubeYunMap")->AsShaderResource();
	RotateY		  = m_FX->GetVariableByName("gRotateY")->AsMatrix();
	Angle		  = m_FX->GetVariableByName("gAngle")->AsScalar();
}

SkyEffect::~SkyEffect()
{
}
#pragma endregion

#pragma region BoardEffect
BoardEffect::BoardEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light3Tech = m_FX->GetTechniqueByName("Light3");
	Light3TexAlphaClipTech = m_FX->GetTechniqueByName("Light3TexAlphaClip");
	Light3TexAlphaClipFogTech = m_FX->GetTechniqueByName("Light3TexAlphaClipFog");

	ViewProj = m_FX->GetVariableByName("gViewProj")->AsMatrix();
	EyePosW = m_FX->GetVariableByName("gEyePosW")->AsVector();
	FogColor = m_FX->GetVariableByName("gFogColor")->AsVector();
	FogStart = m_FX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = m_FX->GetVariableByName("gFogRange")->AsScalar();
	DirLights = m_FX->GetVariableByName("gDirLights");
	Mat = m_FX->GetVariableByName("gMaterial");
	TextureMap = m_FX->GetVariableByName("gTreeMap")->AsShaderResource();
}

BoardEffect::~BoardEffect()
{
}
#pragma endregion

#pragma region ParticleEffect
ParticleEffect::ParticleEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	StreamOutTech = m_FX->GetTechniqueByName("StreamOutTech");
	DrawTech = m_FX->GetTechniqueByName("DrawTech");

	ViewProj = m_FX->GetVariableByName("gViewProj")->AsMatrix();
	GameTime = m_FX->GetVariableByName("gGameTime")->AsScalar();
	TimeStep = m_FX->GetVariableByName("gTimeStep")->AsScalar();
	EyePosW = m_FX->GetVariableByName("gEyePosW")->AsVector();
	EmitPosW = m_FX->GetVariableByName("gEmitPosW")->AsVector();
	EmitDirW = m_FX->GetVariableByName("gEmitDirW")->AsVector();
	TexArray = m_FX->GetVariableByName("gTexArray")->AsShaderResource();
	RandomTex = m_FX->GetVariableByName("gRandomTex")->AsShaderResource();
}

ParticleEffect::~ParticleEffect()
{
}
#pragma endregion