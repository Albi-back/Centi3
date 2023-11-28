#pragma once
#ifndef SHADERTYPES_H
#define SHADERTYPES_H

#include <d3d11.h>
#include <string>

#include "SimpleMath.h"


/*
This is what our vertex data will look like
*/
struct VertexPosNormTex
{
	DirectX::SimpleMath::Vector3 Pos;		//local space position of the vertex
	DirectX::SimpleMath::Vector3 Norm;		//colour red, green, blue, alpha
	DirectX::SimpleMath::Vector2 Tex;		//uv texture coordinate

	//a description of this structure that we can pass to d3d
	static const D3D11_INPUT_ELEMENT_DESC sVertexDesc[3];
};

/*
Insted of a colour in each vertex we define a material
for a group of primitves (an entire surface)
We separate these two structs because this bit needs to go to the gpu
*/
struct BasicMaterial
{
	//constructor
	BasicMaterial(const DirectX::SimpleMath::Vector4& d, const DirectX::SimpleMath::Vector4& a, const DirectX::SimpleMath::Vector4& s) {
		Diffuse = d;
		Ambient = a;
		Specular = s;
	}
	BasicMaterial() : Diffuse(1, 1, 1, 1), Ambient(1, 1, 1, 1), Specular(0, 0, 0, 1) {}
	//setter
	void Set(const DirectX::SimpleMath::Vector4& d, const DirectX::SimpleMath::Vector4& a, const DirectX::SimpleMath::Vector4& s) {
		Diffuse = d; Ambient = a; Specular = s;
	}
	//light reflection modulators
	DirectX::SimpleMath::Vector4 Diffuse;
	DirectX::SimpleMath::Vector4 Ambient;
	DirectX::SimpleMath::Vector4 Specular; // w = SpecPower
};

/*
Instead of a colour in each vertex we define a material
for a group of primitves (an entire surface)
*/
struct Material
{
	//textures are transformed by the vertex shader
	struct TexTrsfm
	{
		DirectX::SimpleMath::Vector2 scale = DirectX::SimpleMath::Vector2(1, 1);		//scale
		float angle = 0;																//rotation
		DirectX::SimpleMath::Vector2 translate = DirectX::SimpleMath::Vector2(0, 0);	//translation (scroll)
	};
	//cosntructors
	Material() : flags(TFlags::APPEND_PATH | TFlags::LIT | TFlags::CULL | TFlags::CCW_WINDING) {
		blendFactors[0] = blendFactors[1] = blendFactors[2] = blendFactors[3] = 1;
		pTextureRV = nullptr;
	}
	Material(const BasicMaterial& mat, ID3D11ShaderResourceView* pTex, TexTrsfm texTrsfm, int _flags, const std::string& _name, const std::string& file)
		: gfxData(mat), name(_name), texture(file), flags(_flags) {
		blendFactors[0] = blendFactors[1] = blendFactors[2] = blendFactors[3] = 1;
		pTextureRV = pTex;
	}
	//blend factors for dynamic transparency control
	void SetBlendFactors(float r, float g, float b, float a) {
		blendFactors[0] = r; blendFactors[1] = g; blendFactors[2] = b; blendFactors[3] = a;
	}

	BasicMaterial gfxData;	//this is the original material reflection data that gets passed to the shader

	ID3D11ShaderResourceView* pTextureRV;	//texture - handled by effects texture cache so don't release

	TexTrsfm texTrsfm;			//uv transformation details

	//bit flags, we can have 32 different ones in an int
	typedef enum {
		APPEND_PATH = 1,	//if true then the texture file name is just a name and needs a path pre-appending before use
		TRANSPARENCY = 2,		//use blend factors to force transparency
		LIT = 4,				//reflects light
		ALPHA_TRANSPARENCY = 8,	//use the texture alpha
		CULL = 16,			//remove back facing primitives
		CCW_WINDING = 32,	//counter clockwise winding or clockwise - defines which side is front (for culling)
		WIRE_FRAME = 64		//solid or wireframe?
	} TFlags;
	int flags;

	float blendFactors[4];		//dynamic transparency blend factors
	std::string name;			//material names can come from 3DSMax and can be useful for debugging
	std::string texture;		//file name of texture

	static const Material default; //a default set of values to get you started
};

/*
a light description to be passed to the gpu
type=DIRectional, POINT, SPOTlight
*/
struct Light
{
	Light() : type(OFF), range(0), theta(0), phi(0) {}

	DirectX::SimpleMath::Vector4 Ambient;		//really there should be one ambient light value per render, but putting in each light can be useful
	DirectX::SimpleMath::Vector4 Diffuse;		//light colour
	DirectX::SimpleMath::Vector4 Specular;		//specular colour component w=nothing
	DirectX::SimpleMath::Vector4 Direction;		//what direction is it going w=nothing
	DirectX::SimpleMath::Vector4 Position;		//where does it originate w=nothing
	DirectX::SimpleMath::Vector4 Attenuation;	//how should amplitude decay with distance w=nothing

	typedef enum {
		OFF = 0,		//ignore this light
		DIR = 1,	//directional
		POINT = 2,	//point light
		SPOT = 3	//spot light
	} Type;
	int type;
	float range;	//point at which we terminate the light
	float theta;	//spot light inner and 
	float phi;		//outer cone anlges in radians 
};

//shader variables that don't change within one frame
const int MAX_LIGHTS = 8;
struct GfxParamsPerFrame
{
	Light lights[MAX_LIGHTS];				//all the light descriptions needed to render something
	DirectX::SimpleMath::Vector4 eyePosW;	//where is the camera? world space
};
static_assert((sizeof(GfxParamsPerFrame) % 16) == 0, "CB size not padded correctly");



//shader variables that don't change within one object
struct GfxParamsPerObj
{
	DirectX::SimpleMath::Matrix world;			//local to world space matrix
	DirectX::SimpleMath::Matrix worldInvT;		//inverse transpose used for transforming normals//inverse world matrix
	DirectX::SimpleMath::Matrix worldViewProj;	//the master transform
};
static_assert((sizeof(GfxParamsPerObj) % 16) == 0, "CB size not padded correctly");


//shader variables that don't change within an object's sub-mesh
struct GfxParamsPerMesh
{
	DirectX::SimpleMath::Matrix textureTrsfm;	//do we want the texture manipulated (mag, min, scroll, rotate)
	BasicMaterial material;		//this is the basic material reflection data the gpu needs for lighting calcs
};
static_assert((sizeof(GfxParamsPerMesh) % 16) == 0, "CB size not padded correctly");

#endif
