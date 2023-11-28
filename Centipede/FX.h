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

	//we've loaded in a "blob" of compiled shader code, it needs to be set up on the gpu as a pixel shader
	void CreatePixelShader(ID3D11Device& d3dDevice, char* pBuff, unsigned int buffSz, ID3D11PixelShader* &pPS);
	//we've loaded in a "blob" of compiled shader code, it needs to be set up on the gpu as a vertex shader
	void CreateVertexShader(ID3D11Device& d3dDevice, char* pBuff, unsigned int buffSz, ID3D11VertexShader* &pVS);
	//the input assembler needs to know what is in the vertex buffer, how to get the data out and which vertex shader to give it to
	void CreateInputLayout(ID3D11Device& d3dDevice, const D3D11_INPUT_ELEMENT_DESC vdesc[], int numElements, char* pBuff, unsigned int buffSz, ID3D11InputLayout** pLayout);
	//if we want to give extra information to the shaders then it has to go in a constant buffer
	//once a shader runs, to render a leg or something, the data can't change until it's finished
	//so it's always a constant
	void CreateConstantBuffer(ID3D11Device& d3dDevice, UINT sizeOfBuffer, ID3D11Buffer **pBuffer);
	//different hardware supports different instructions, lines of code, number of constants, etc
	//we cheat a bit and just go for shader model 5 which is really common and powerful
	void CheckShaderModel5Supported(ID3D11Device& d3dDevice);
	//load in and allocate a chunk of binary data
	char* ReadAndAllocate(const std::string& fileName, unsigned int& bytesRead);
	//when drawing primitives should they be filled/wireframe/clockwise culled/anti-clockwise culled/not culled at all
	void CreateRasterStates(ID3D11Device& d3dDevice, ID3D11RasterizerState *pStates[RasterType::MAX_STATES]);
	//a sampler takes samples of the texture i.e. looks up texels
	void CreateSampler(ID3D11Device& d3dDevice, ID3D11SamplerState* &pSampler);
	//there can be many blend states, this one uses the blend factors to directly control transparency
	void CreateTransparentBlendState(ID3D11Device& d3dDevice, ID3D11BlendState* &pTransparent);
	//this one uses the texture alpha to control the transparency
	void CreateAlphaTransparentBlendState(ID3D11Device& d3dDevice, ID3D11BlendState* &pBlend);





	class MyFX
	{
	public:
		MyFX(MyD3D& d3d)
			:mD3D(d3d) { }
		~MyFX() {
			Release();
		}
		bool Init();
		void Release();
		//render	
		//model - one instance of a mesh with position/scale/rotation/etc
		//pD3DContext - handle to D3D
		//flags - control lighting, etc
		//pOverrideMat - if not null it points at a material to use instead of the one in the mesh
		void Render(Model& model, Material* pOverrideMat = nullptr);

		DirectX::SimpleMath::Matrix& GetProjectionMatrix() { return mProj; }
		DirectX::SimpleMath::Matrix& GetViewMatrix() { return mView; }
		//set the constants that change per object
		void SetPerObjConsts(ID3D11DeviceContext& ctx, DirectX::SimpleMath::Matrix &world);
		//set the constants that change each update
		void SetPerFrameConsts(ID3D11DeviceContext& ctx, const DirectX::SimpleMath::Vector3& eyePos);
		/*create a directional light
		lightIdx - which light is it 0->7
		enable - turn it on or off
		direction - normal showing which way the light is shining
		diffuse, specular, ambient light intensities (0->1)
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
		ID3D11RasterizerState *mpRasterStates[RasterType::MAX_STATES];
	};

}

#endif
