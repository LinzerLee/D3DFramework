//***************************************************************************************
// Waves.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Waves.h"
#include <algorithm>
#include <vector>
#include <cassert>

using namespace DirectX;

Waves::Waves()
: m_NumRows(0), m_NumCols(0), m_VertexCount(0), m_TriangleCount(0),
  m_K1(0.0f), m_K2(0.0f), m_K3(0.0f), m_TimeStep(0.0f), m_SpatialStep(0.0f),
  m_PrevSolution(nullptr), m_CurrSolution(nullptr), m_Normals(nullptr), m_TangentX(nullptr)
{
}

Waves::~Waves()
{
	delete[] m_PrevSolution;
	delete[] m_CurrSolution;
	delete[] m_Normals;
	delete[] m_TangentX;
}

UINT Waves::RowCount()const
{
	return m_NumRows;
}

UINT Waves::ColumnCount()const
{
	return m_NumCols;
}

UINT Waves::VertexCount()const
{
	return m_VertexCount;
}

UINT Waves::TriangleCount()const
{
	return m_TriangleCount;
}

float Waves::Width()const
{
	return m_NumCols * m_SpatialStep;
}

float Waves::Depth()const
{
	return m_NumRows * m_SpatialStep;
}

void Waves::Init(UINT m, UINT n, float dx, float dt, float speed, float damping)
{
	m_NumRows  = m;
	m_NumCols  = n;

	m_VertexCount   = m*n;
	m_TriangleCount = (m-1)*(n-1)*2;

	m_TimeStep    = dt;
	m_SpatialStep = dx;

	float d = damping*dt+2.0f;
	float e = (speed*speed)*(dt*dt)/(dx*dx);
	m_K1     = (damping*dt-2.0f)/ d;
	m_K2     = (4.0f-8.0f*e) / d;
	m_K3     = (2.0f*e) / d;

	// In case Init() called again.
	delete[] m_PrevSolution;
	delete[] m_CurrSolution;
	delete[] m_Normals;
	delete[] m_TangentX;

	m_PrevSolution = new XMFLOAT3[m*n];
	m_CurrSolution = new XMFLOAT3[m*n];
	m_Normals      = new XMFLOAT3[m*n];
	m_TangentX     = new XMFLOAT3[m*n];

	// Generate grid vertices in system memory.

	float halfWidth = (n-1)*dx*0.5f;
	float halfDepth = (m-1)*dx*0.5f;
	for(UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dx;
		for(UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			m_PrevSolution[i*n+j] = XMFLOAT3(x, 0.0f, z);
			m_CurrSolution[i*n+j] = XMFLOAT3(x, 0.0f, z);
			m_Normals[i*n+j]      = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_TangentX[i*n+j]     = XMFLOAT3(1.0f, 0.0f, 0.0f);
		}
	}
}

void Waves::Update(float dt)
{
	static float t = 0;

	// Accumulate time.
	t += dt;

	// Only update the simulation at the specified time step.
	if( t >= m_TimeStep )
	{
		// Only update interior points; we use zero boundary conditions.
		for(UINT i = 1; i < m_NumRows-1; ++i)
		{
			for(UINT j = 1; j < m_NumCols-1; ++j)
			{
				// After this update we will be discarding the old previous
				// buffer, so overwrite that buffer with the new update.
				// Note how we can do this inplace (read/write to same element) 
				// because we won't need prev_ij again and the assignment happens last.

				// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
				// Moreover, our +z axis goes "down"; this is just to 
				// keep consistent with our row indices going down.

				m_PrevSolution[i*m_NumCols+j].y =
					m_K1*m_PrevSolution[i*m_NumCols+j].y +
					m_K2*m_CurrSolution[i*m_NumCols+j].y +
					m_K3*(m_CurrSolution[(i+1)*m_NumCols+j].y +
					     m_CurrSolution[(i-1)*m_NumCols+j].y +
					     m_CurrSolution[i*m_NumCols+j+1].y + 
						 m_CurrSolution[i*m_NumCols+j-1].y);
			}
		}

		// We just overwrote the previous buffer with the new data, so
		// this data needs to become the current solution and the old
		// current solution becomes the new previous solution.
		std::swap(m_PrevSolution, m_CurrSolution);

		t = 0.0f; // reset time

		//
		// Compute normals using finite difference scheme.
		//
		for(UINT i = 1; i < m_NumRows-1; ++i)
		{
			for(UINT j = 1; j < m_NumCols-1; ++j)
			{
				float l = m_CurrSolution[i*m_NumCols+j-1].y;
				float r = m_CurrSolution[i*m_NumCols+j+1].y;
				float t = m_CurrSolution[(i-1)*m_NumCols+j].y;
				float b = m_CurrSolution[(i+1)*m_NumCols+j].y;
				m_Normals[i*m_NumCols+j].x = -r+l;
				m_Normals[i*m_NumCols+j].y = 2.0f*m_SpatialStep;
				m_Normals[i*m_NumCols+j].z = b-t;

				XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&m_Normals[i*m_NumCols+j]));
				XMStoreFloat3(&m_Normals[i*m_NumCols+j], n);

				m_TangentX[i*m_NumCols+j] = XMFLOAT3(2.0f*m_SpatialStep, r-l, 0.0f);
				XMVECTOR T = XMVector3Normalize(XMLoadFloat3(&m_TangentX[i*m_NumCols+j]));
				XMStoreFloat3(&m_TangentX[i*m_NumCols+j], T);
			}
		}
	}
}

void Waves::Disturb(UINT i, UINT j, float magnitude)
{
	// Don't disturb boundaries.
	assert(i > 1 && i < m_NumRows-2);
	assert(j > 1 && j < m_NumCols-2);

	float halfMag = 0.5f * magnitude;

	// Disturb the ijth vertex height and its neighbors.
	m_CurrSolution[i*m_NumCols+j].y     += magnitude;
	m_CurrSolution[i*m_NumCols+j+1].y   += halfMag;
	m_CurrSolution[i*m_NumCols+j-1].y   += halfMag;
	m_CurrSolution[(i+1)*m_NumCols+j].y += halfMag;
	m_CurrSolution[(i-1)*m_NumCols+j].y += halfMag;
}
	
