#ifndef FX_H
#define FX_H

#include <string>
#include <d3d11.h>

#include "D3DUtil.h"
#include "ShaderTypes.h"

class MyD3D;
class Model;

namespace FX
{
	//design pattern - how do we get a 'class enum' safe enumeration where we cna still use the elements as numbers 
	namespace RasterType { enum { CCW_FILLED = 0, CCW_WIRE = 1, CW_FILLED = 2, CW_WIRE = 3, NOCULL_WIRE = 4, NOCULL_FILLED = 5, MAX_STATES = 6 }; }

	/*we've loaded in a "blob" of compiled shader code, it needs to be set up on the gpu as a pixel shader
	* d3dDevice - IN the gpu device to hold this pixel shader
	* pBuff - IN compiled program code
	* buffSz - IN how big the program code is
	* pPS - OUT a pointer reference to a D3D pixel shader object
	*/
	void CreatePixelShader(ID3D11Device& d3dDevice, char* pBuff, unsigned int buffSz, ID3D11PixelShader* &pPS);
	/*we've loaded in a "blob" of compiled shader code, it needs to be set up on the gpu as a vertex shader
	* d3ddevice
	* pBuff - IN compiled program code
	* buffSz - IN how big the program code is
	* pVS - OUT a pointer reference to a D3D vertex shader object
	*/
	void CreateVertexShader(ID3D11Device& d3dDevice, char* pBuff, unsigned int buffSz, ID3D11VertexShader* &pVS);
	/*the input assembler needs to know what is in the vertex buffer, how to get the data outand which vertex shader to give it to
	* d3dDevice
	* vdesc - IN a vertex description
	* numElements - IN how many items are in the vertex description
	* pBuff - IN compiled program code this input layout matches (vertex shader)
	* buffSz - IN how big the program code is
	* pLayout - OUT pointer reference to a D3D input layout object
	*/
	void CreateInputLayout(ID3D11Device& d3dDevice, const D3D11_INPUT_ELEMENT_DESC vdesc[], int numElements, char* pBuff, unsigned int buffSz, ID3D11InputLayout** pLayout);
	/*
	* A constant buffer object can pass constants to a gpu
	* d3dDevice
	* sizeOfBuffer - IN usually this is the size of some struct containing constants to shift to the GPU
	* pBuffer - OUT a d3d object that can hold the constant data we will eventually be passing over
	*/
	void CreateConstantBuffer(ID3D11Device& d3dDevice, UINT sizeOfBuffer, ID3D11Buffer **pBuffer);
	//different hardware supports different instructions, lines of code, number of constants, etc
	//we cheat a bit and just go for shader model 5 which is really common and powerful
	void CheckShaderModel5Supported(ID3D11Device& d3dDevice);
	/*
	* Read data from a file, usually used to load in compiled shader code
	* fileName - IN file to load
	* bytesRead - OUT how much data we got
	*/
	char* ReadAndAllocate(const std::string& fileName, unsigned int& bytesRead);
	//when drawing primitives should they be filled/wireframe/clockwise culled/anti-clockwise culled/not culled at all
	void CreateRasterStates(ID3D11Device& d3dDevice, ID3D11RasterizerState *pStates[RasterType::MAX_STATES]);
	//a sampler takes samples of the texture i.e. looks up texels
	void CreateSampler(ID3D11Device& d3dDevice, ID3D11SamplerState* &pSampler);
	//there can be many blend states, this one uses the blend factors to directly control transparency
	void CreateTransparentBlendState(ID3D11Device& d3dDevice, ID3D11BlendState* &pTransparent);
	//this one uses the texture alpha to control the transparency
	void CreateAlphaTransparentBlendState(ID3D11Device& d3dDevice, ID3D11BlendState* &pBlend);




	//an object to bring state related to shaders together
	class MyFX
	{
	public:
		MyFX(MyD3D& d3d)
			:mD3D(d3d) { }
		~MyFX() {
			Release();
		}
		//load shaders, create constant registers
		bool Init();
		//release all the shader related d3d objects
		void Release();
		/*
		* we don't render low level geometry anymore, we've wrapped that up in Models and Meshes
		* model - IN a model is an instance of a Mesh which holds geometry buffers
		* pOverrideMat - IN Models and Meshes have materials controlling how they look, but we can optionally
		*				pass one in to use instead, so we can make the model look different without altering it
		*/
		void Render(Model& model, Material* pOverrideMat = nullptr);

		//getters
		DirectX::SimpleMath::Matrix& GetProjectionMatrix() { return mProj; }
		DirectX::SimpleMath::Matrix& GetViewMatrix() { return mView; }
		//set the constants that change per object
		void SetPerObjConsts(ID3D11DeviceContext& ctx, DirectX::SimpleMath::Matrix &world);
		//set the constants that change each update
		void SetPerFrameConsts(ID3D11DeviceContext& ctx, const DirectX::SimpleMath::Vector3& eyePos);
		/*
		* A directional light - like the sun
		* lightIdx - IN we pass an array of light info to the gpu, which array element is this one using
		* enable - IN true if we are turning it on
		* direction - which way is it pointing
		* diffuse - colour intensity of the light
		* specular - specular intensity
		* ambient - any ambient light that is meant to leak into the scene
		*/
		void SetupDirectionalLight(int lightIdx, bool enable,
			const DirectX::SimpleMath::Vector3 &direction,
			const DirectX::SimpleMath::Vector3& diffuse = DirectX::SimpleMath::Vector3(1, 1, 1),
			const DirectX::SimpleMath::Vector3& ambient = DirectX::SimpleMath::Vector3(0, 0, 0),
			const DirectX::SimpleMath::Vector3& specular = DirectX::SimpleMath::Vector3(0, 0, 0));

		//create a point light
		//as above but specify a world position and don't bother with a direction
		//range - how far does the light shine
		//atten1 - really there are 3 attenuation values, but we'll just go with one
		//			it's enough to get reasonable control of how the light intensity falls away
		void SetupPointLight(int lightIdx, bool enable,
			const DirectX::SimpleMath::Vector3 &position,
			const DirectX::SimpleMath::Vector3& diffuse = DirectX::SimpleMath::Vector3(1, 1, 1),
			const DirectX::SimpleMath::Vector3& ambient = DirectX::SimpleMath::Vector3(0, 0, 0),
			const DirectX::SimpleMath::Vector3& specular = DirectX::SimpleMath::Vector3(0, 0, 0),
			float range = 1000.f, float atten1 = 0.05f);

		/*spot light
		As above but extra parameters to limit the angle of the inner/outer cone
		innerConeTheta - radius of inner bright cone
		outerConePhi - radius of outer cone as light falls off
		*/
		void SetupSpotLight(int lightIdx, bool enable,
			const DirectX::SimpleMath::Vector3 &position,
			const DirectX::SimpleMath::Vector3 &direction,
			const DirectX::SimpleMath::Vector3& diffuse = DirectX::SimpleMath::Vector3(1, 1, 1),
			const DirectX::SimpleMath::Vector3& ambient = DirectX::SimpleMath::Vector3(0, 0, 0),
			const DirectX::SimpleMath::Vector3& specular = DirectX::SimpleMath::Vector3(0, 0, 0),
			float range = 1000.f, float atten1 = 0.05f, float innerConeTheta = D2R(30), float outerConePhi = D2R(40));

	private:

		MyD3D& mD3D;
		DirectX::SimpleMath::Matrix mView, mProj;	//view and projection matrices
		GfxParamsPerObj mGfxPerObj;					//world matrices for transformation
		GfxParamsPerFrame mGfxPerFrame;				//lights and camera position
		GfxParamsPerMesh mGfxPerMesh;				//texture transform matrix and basic material properties
		ID3D11Buffer *mpGfxPerObj = nullptr, *mpGfxPerFrame = nullptr, *mpGfxPerMesh = nullptr;	//DX equivalent data structures for passing to gpu
		 
		//when passing data to the gpu it goes in constant buffers
		void CreateConstantBuffers();
		void ReleaseConstantBuffers();
		//called before rendering anything	
		void PreRenderObj(Material& mat);
		//mapping between vertex/index buffers and gpu
		ID3D11InputLayout* mpInputLayout = nullptr;
		//a smapler to read the texture
		ID3D11SamplerState *mpSamAnisotropic = nullptr;
		//vertex and pixel shaders
		ID3D11VertexShader* mpVS = nullptr;
		//a complicated one if it's lit, a simple one if it isn't, also a textured option now (lit and unlit)
		ID3D11PixelShader* mpPSLit = nullptr, *mpPSUnlit = nullptr, *mpPSLitTex = nullptr, *mpPSUnlitTex = nullptr;
		//transparency means controlling the blend states beyond default settings
		ID3D11BlendState *mpBlendTransparent = nullptr, *mpBlendAlphaTrans = nullptr;
		//multiple raster states should be available
		ID3D11RasterizerState* mpRasterStates[RasterType::MAX_STATES]{ nullptr };
	};

}

#endif
