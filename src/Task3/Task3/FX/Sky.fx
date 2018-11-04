//=============================================================================
// Sky.fx by Linzer Lee
//=============================================================================

cbuffer cbPerFrame
{
	float4x4 gWorldViewProj;
	float4x4 gRotateY;
	float gAngle;
};
 
// Nonnumeric values cannot be added to a cbuffer.
TextureCube gCubeMap;
TextureCube gCubeYunMap;

SamplerState samTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
    float3 PosL : POSITION;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj).xyww;
	
	// Use local vertex position as cubemap lookup vector.
	vout.PosL = vin.PosL;
	
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float rad = dot(pin.PosL, up) / (length(pin.PosL) + length(up));
	float4 color1 = gCubeMap.Sample(samTriLinearSam, pin.PosL);
	if (acos(rad) <= gAngle)
	{
		pin.PosL = mul(float4(pin.PosL, 0.0f), gRotateY).xyz;
	}

	float4 color2 = gCubeYunMap.Sample(samTriLinearSam, pin.PosL);

	return color1.a * color1 + (1.0f - color1.a) * color2;
}

RasterizerState NoCull
{
    CullMode = None;
};

DepthStencilState LessEqualDSS
{
	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
    DepthFunc = LESS_EQUAL;
};

technique11 SkyTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
        
        SetRasterizerState(NoCull);
        SetDepthStencilState(LessEqualDSS, 0);
    }
}
