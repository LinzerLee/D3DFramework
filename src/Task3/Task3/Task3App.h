//***************************************************************************************
// Task3App.h by Linzer Lee
//***************************************************************************************

#ifndef TASK3APP_H
#define TASK3APP_H

#include "Application.h"
#include "Camera.h"
#include "Light.h"
#include "ModelObject.h"
#include "Texture.h"
#include "WavesModel.h"
#include "Sky.h"
#include "MeshModel.h"
#include "BoardModel.h"
#include "ParticleSystem.h"

enum RenderOptions
{
	Lighting = 0,
	Textures = 1,
	TexturesAndFog = 2
};

class Task3App : public Application
{
public:
	Task3App(HINSTANCE hInstance);
	~Task3App();

	virtual bool Init() override;
	// Load/Unload Resource
	virtual void Load() override;
	virtual void Unload() override;
	virtual void Update(float dt) override;
	virtual void Predraw() override;
	virtual void Draw() override;

	virtual void OnResize() override;
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
	
private:
	void CreateSnowman(MeshModel &model);

private:
	ModelObject m_Land;
	ModelObject m_Box;
	WavesModel m_Waves;
	MeshModel m_Snowman1;
	MeshModel m_Snowman2;
	Sky m_Sky;
	BoardModel m_Trees[7];
	BoardModel m_Grass[6];
	Terrain *m_Terrain;
	Camera m_Camera;
	DirectionalLight m_DirLights[3];
	bool m_WalkCamMode;
	RenderOptions m_RenderOptions;
	DirectX::XMFLOAT4X4 m_GrassTexTransform;
	Material m_LandMat;
	Material m_WavesMat;
	Material m_SnowmanMat;
	Material m_ShadowMat;
	Material m_TreeMat;
	POINT m_LastMousePos;
	ParticleSystem m_Fire;
	ParticleSystem m_Rain;
	ParticleSystem m_Snow;
	Texture m_GrassMapTex;
	Texture m_SnowTex;
	Texture m_BoxTex;
	Texture m_TreeTexs[7];
	Texture m_GrassTexs[6];
	Texture m_FlareTex;
	Texture m_RainTex;
	Texture m_SnowBallTex;
	Texture m_RandomTex;
};

#endif // TASK3APP_H