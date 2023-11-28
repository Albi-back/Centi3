

#include "LightHelper.hlsl"

#define MAX_LIGHTS 8
#define LIGHT_OFF 0
#define LIGHT_DIR 1
#define LIGHT_POINT 2
#define LIGHT_SPOT 3


cbuffer cbPerFrame : register(b0)  
{
	Light gLights[MAX_LIGHTS];
	float4 gEyePosW;		//ignore w
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
};

cbuffer cbPerMesh : register(b2)
{
	float4x4 gTexTransform;
	Material gMaterial;
}

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap : register(t0);

SamplerState samAnisotropic : register(s0);

//data in and out
struct VertexIn
{
	float3 PosL		: POSITION;
	float3 NormalL	: NORMAL;
	float2 Tex		: TEXCOORD;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 NormalW	: NORMAL;
	float2 Tex		: TEXCOORD;
};

