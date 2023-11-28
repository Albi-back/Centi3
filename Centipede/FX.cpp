#include <iostream>
#include <fstream>

#include "D3D.h"
#include "D3DUtil.h"
#include "FX.h"
#include "WindowUtils.h"
#include "Model.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


namespace FX
{

	void MyFX::CreateConstantBuffers()
	{
		CreateConstantBuffer(mD3D.GetDevice(),sizeof(GfxParamsPerFrame), &mpGfxPerFrame);
		CreateConstantBuffer(mD3D.GetDevice(),sizeof(GfxParamsPerObj), &mpGfxPerObj);
		CreateConstantBuffer(mD3D.GetDevice(),sizeof(GfxParamsPerMesh), &mpGfxPerMesh);
	}

	void MyFX::ReleaseConstantBuffers()
	{
		ReleaseCOM(mpGfxPerFrame);
		ReleaseCOM(mpGfxPerObj);
		ReleaseCOM(mpGfxPerMesh);
	}

	void MyFX::SetPerObjConsts(ID3D11DeviceContext& d3dContext, DirectX::SimpleMath::Matrix& world)
	{
		mGfxPerObj.world = world;
		mGfxPerObj.worldInvT = InverseTranspose(world);
		mGfxPerObj.worldViewProj = world * mView * mProj;

		d3dContext.UpdateSubresource(mpGfxPerObj, 0, nullptr, &mGfxPerObj, 0, 0);
	}

	void MyFX::SetPerFrameConsts(ID3D11DeviceContext& d3DContext, const Vector3& eyePos)
	{
		mGfxPerFrame.eyePosW = Vector4(eyePos.x, eyePos.y, eyePos.z, 0);
		d3DContext.UpdateSubresource(mpGfxPerFrame, 0, nullptr, &mGfxPerFrame, 0, 0);

	}

	void CreateRasterStates(ID3D11Device &device, ID3D11RasterizerState *pStates[RasterType::MAX_STATES])
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FrontCounterClockwise = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_FRONT;
		HR(device.CreateRasterizerState(&desc, &pStates[RasterType::CCW_FILLED]));

		desc.FillMode = D3D11_FILL_WIREFRAME;
		HR(device.CreateRasterizerState(&desc, &pStates[RasterType::CCW_WIRE]));

		desc.FillMode = D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = false;
		HR(device.CreateRasterizerState(&desc, &pStates[RasterType::CW_FILLED]));
		desc.FillMode = D3D11_FILL_WIREFRAME;
		HR(device.CreateRasterizerState(&desc, &pStates[RasterType::CW_WIRE]));

		desc.CullMode = D3D11_CULL_NONE;
		desc.FillMode = D3D11_FILL_WIREFRAME;
		HR(device.CreateRasterizerState(&desc, &pStates[RasterType::NOCULL_WIRE]));
		desc.CullMode = D3D11_CULL_NONE;
		desc.FillMode = D3D11_FILL_SOLID;
		HR(device.CreateRasterizerState(&desc, &pStates[RasterType::NOCULL_FILLED]));
	}


	void CreateTransparentBlendState(ID3D11Device& d3dDevice, ID3D11BlendState* &pTransparent)
	{
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		D3D11_RENDER_TARGET_BLEND_DESC rtbd;
		ZeroMemory(&rtbd, sizeof(rtbd));

		rtbd.BlendEnable = true;
		//rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
		//rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
		rtbd.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
		rtbd.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;

		rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;

		rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.RenderTarget[0] = rtbd;

		HR(d3dDevice.CreateBlendState(&blendDesc, &pTransparent));
	}


	void CreateAlphaTransparentBlendState(ID3D11Device& d3dDevice, ID3D11BlendState* &pBlend)
	{
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));

		D3D11_RENDER_TARGET_BLEND_DESC rtbd;
		ZeroMemory(&rtbd, sizeof(rtbd));

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;

		rtbd.SrcBlendAlpha = D3D11_BLEND_ZERO;
		rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0] = rtbd;
		//rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

		blendDesc.AlphaToCoverageEnable = false;

		HR(d3dDevice.CreateBlendState(&blendDesc, &pBlend));
	}


	void MyFX::SetupDirectionalLight(int lightIdx, bool enable, const Vector3 &direction,
		const Vector3& diffuse, const Vector3& ambient, const Vector3& specular)
	{
		assert(lightIdx >= 0 && lightIdx < 8);
		if (!enable)
		{
			mGfxPerFrame.lights[lightIdx].type = Light::Type::OFF;
			return;
		}

		Light& l = mGfxPerFrame.lights[lightIdx];
		l.type = Light::Type::DIR;
		l.Diffuse = Vec3To4(diffuse, 0);
		l.Ambient = Vec3To4(ambient, 0);
		l.Specular = Vec3To4(specular, 0);
		l.Direction = Vec3To4(direction, 0);
	}

	void MyFX::SetupPointLight(int lightIdx, bool enable,
		const Vector3& position,
		const Vector3& diffuse,
		const Vector3& ambient,
		const Vector3& specular,
		float range/*=1000.f*/, float atten1/*=0.05f*/)
	{
		assert(lightIdx >= 0 && lightIdx < 8);
		if (!enable)
		{
			mGfxPerFrame.lights[lightIdx].type = Light::Type::OFF;
			return;
		}

		Light& l = mGfxPerFrame.lights[lightIdx];
		l.type = Light::Type::POINT;
		l.Diffuse = Vec3To4(diffuse, 0);
		l.Ambient = Vec3To4(ambient, 0);
		l.Specular = Vec3To4(specular, 0);
		l.Position = Vec3To4(position, 0);
		l.Attenuation = Vector4(0, atten1, 0, 0);
		l.range = range;
	}

	void MyFX::SetupSpotLight(int lightIdx, bool enable,
		const DirectX::SimpleMath::Vector3 &position,
		const DirectX::SimpleMath::Vector3 &direction,
		const DirectX::SimpleMath::Vector3& diffuse,
		const DirectX::SimpleMath::Vector3& ambient,
		const DirectX::SimpleMath::Vector3& specular,
		float range, float atten1, float innerConeTheta, float outerConePhi)
	{
		assert(lightIdx >= 0 && lightIdx < 8);
		if (!enable)
		{
			mGfxPerFrame.lights[lightIdx].type = Light::Type::OFF;
			return;
		}

		Light& l = mGfxPerFrame.lights[lightIdx];
		l.type = Light::Type::SPOT;
		l.Diffuse = Vec3To4(diffuse, 0);
		l.Ambient = Vec3To4(ambient, 0);
		l.Specular = Vec3To4(specular, 0);
		l.Position = Vec3To4(position, 0);
		l.Direction = Vec3To4(direction, 0);
		l.Attenuation = Vector4(0, atten1, 0, 0);
		l.range = range;
		l.theta = innerConeTheta;
		l.phi = outerConePhi;
	}


	void CheckShaderModel5Supported(ID3D11Device& d3dDevice)
	{
		D3D_FEATURE_LEVEL featureLevel = d3dDevice.GetFeatureLevel();
		bool featureLevelOK = true;
		switch (featureLevel)
		{
		case D3D_FEATURE_LEVEL_11_1:
		case D3D_FEATURE_LEVEL_11_0:
			break;
		default:
			featureLevelOK = false;
		}

		if (!featureLevelOK)
		{
			DBOUT("feature level too low for shader model 5");
			assert(false);
		}
	}

	void CreateConstantBuffer(ID3D11Device& d3dDevice, UINT sizeOfBuffer, ID3D11Buffer **pBuffer)
	{
		// Create the constant buffers for the variables defined in the vertex shader.
		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.ByteWidth = sizeOfBuffer;
		constantBufferDesc.CPUAccessFlags = 0;
		constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		HR(d3dDevice.CreateBuffer(&constantBufferDesc, nullptr, pBuffer));

	}

	char* ReadAndAllocate(const string& fileName, unsigned int& bytesRead)
	{
		//open the file for reading
		ifstream infile;
		infile.open(fileName, ios::binary | ios::in);
		if (!infile.is_open() || infile.fail())
		{
			DBOUT("failed to open file: " << fileName);
			assert(false);
			return nullptr;
		}

		//read the whole contents
		infile.seekg(0, ios::end);
		streampos size = infile.tellg();
		if (size > INT_MAX || size <= 0)
		{
			DBOUT("failed to get size of file: " << fileName);
			assert(false);
		}
		char* pBuff = new char[(int)size];
		infile.seekg(0, ios::beg);
		infile.read(pBuff, size);
		if (infile.fail())
		{
			DBOUT("failed to read file: " << fileName);
			assert(false);
		}
		infile.close();
		bytesRead = (int)size;
		return pBuff;
	}

	void CreateInputLayout(ID3D11Device& d3dDevice, const D3D11_INPUT_ELEMENT_DESC vdesc[], int numElements, char* pBuff, unsigned int buffSz, ID3D11InputLayout** pLayout)
	{
		assert(pBuff);
		HR(d3dDevice.CreateInputLayout(vdesc, numElements, pBuff, buffSz, pLayout));

	}

	void CreateVertexShader(ID3D11Device& d3dDevice, char* pBuff, unsigned int buffSz, ID3D11VertexShader* &pVS)
	{
		assert(pBuff);
		HR(d3dDevice.CreateVertexShader(pBuff,
			buffSz,
			nullptr,
			&pVS));
		assert(pVS);
	}

	void CreatePixelShader(ID3D11Device& d3dDevice, char* pBuff, unsigned int buffSz, ID3D11PixelShader* &pPS)
	{
		assert(pBuff);
		HR(d3dDevice.CreatePixelShader(pBuff,
			buffSz,
			nullptr,
			&pPS));
		assert(pPS);
	}

	void CreateSampler(ID3D11Device& d3dDevice, ID3D11SamplerState* &pSampler)
	{
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		HR(d3dDevice.CreateSamplerState(&sampDesc, &pSampler));
	}


	bool MyFX::Init()
	{
		CheckShaderModel5Supported(mD3D.GetDevice());
		CreateSampler(mD3D.GetDevice(),mpSamAnisotropic);
		CreateRasterStates(mD3D.GetDevice(), mpRasterStates);

		char* pBuff = nullptr;
		unsigned int bytes = 0;
		pBuff = ReadAndAllocate("../bin/data/TextureVS.cso", bytes);
		CreateVertexShader(mD3D.GetDevice(),pBuff, bytes, mpVS);
		CreateInputLayout(mD3D.GetDevice(), VertexPosNormTex::sVertexDesc, 3, pBuff, bytes, &mpInputLayout);
		delete[] pBuff;

		pBuff = ReadAndAllocate("../bin/data/PSLitNoTex.cso", bytes);
		CreatePixelShader(mD3D.GetDevice(), pBuff, bytes, mpPSLit);
		delete[] pBuff;

		pBuff = ReadAndAllocate("../bin/data/PSUnlitNoTex.cso", bytes);
		CreatePixelShader(mD3D.GetDevice(), pBuff, bytes, mpPSUnlit);
		delete[] pBuff;

		pBuff = ReadAndAllocate("../bin/data/PSLitTex.cso", bytes);
		CreatePixelShader(mD3D.GetDevice(), pBuff, bytes, mpPSLitTex);
		delete[] pBuff;

		pBuff = ReadAndAllocate("../bin/data/PSUnlitTex.cso", bytes);
		CreatePixelShader(mD3D.GetDevice(), pBuff, bytes, mpPSUnlitTex);
		delete[] pBuff;

		CreateConstantBuffers();
		CreateTransparentBlendState(mD3D.GetDevice(), mpBlendTransparent);
		CreateAlphaTransparentBlendState(mD3D.GetDevice(), mpBlendAlphaTrans);

		return true;
	}

	void MyFX::Release()
	{
		ReleaseCOM(mpVS);
		ReleaseCOM(mpPSLit);
		ReleaseCOM(mpPSUnlit);
		ReleaseCOM(mpPSLitTex);
		ReleaseCOM(mpPSUnlitTex);
		ReleaseCOM(mpInputLayout);
		ReleaseCOM(mpSamAnisotropic);
		ReleaseCOM(mpBlendTransparent);
		ReleaseCOM(mpBlendAlphaTrans);
		ReleaseConstantBuffers();
		for (int i = 0; i < RasterType::MAX_STATES; ++i)
			ReleaseCOM(mpRasterStates[i]);
	}

	void MyFX::Render(Model& model, Material* pOverrideMat)
	{
		//setup shaders
		mD3D.GetDeviceCtx().VSSetShader(mpVS, nullptr, 0);

		Matrix w;
		model.GetWorldMatrix(w);
		SetPerObjConsts(mD3D.GetDeviceCtx(), w);

		Mesh& mesh = model.GetMesh();
		for (int i = 0; i < mesh.GetNumSubMeshes(); ++i)
		{
			//update material
			SubMesh& sm = mesh.GetSubMesh(i);

			mD3D.InitInputAssembler(mpInputLayout, sm.mpVB, sizeof(VertexPosNormTex), sm.mpIB);
			Material *pM;
			if (pOverrideMat)
				pM = pOverrideMat;
			else if (model.HasOverrideMat())
				pM = model.HasOverrideMat();
			else
				pM = &sm.material;


			PreRenderObj(*pM);
			mD3D.GetDeviceCtx().DrawIndexed(sm.mNumIndices, 0, 0);

		}
		mD3D.GetDeviceCtx().OMSetBlendState(0, 0, 0xffffffff);
	}

	void MyFX::PreRenderObj(Material& mat)
	{
		//texture transform
		mGfxPerMesh.material = mat.gfxData;
		mGfxPerMesh.textureTrsfm = Matrix::CreateScale(mat.texTrsfm.scale.x, mat.texTrsfm.scale.y, 1) *
			Matrix::CreateRotationZ(mat.texTrsfm.angle) *
			Matrix::CreateTranslation(mat.texTrsfm.translate.x, mat.texTrsfm.translate.y, 0);
		ID3D11DeviceContext& dc = mD3D.GetDeviceCtx();
		dc.UpdateSubresource(mpGfxPerMesh, 0, nullptr, &mGfxPerMesh, 0, 0);

		//buffers
		dc.VSSetConstantBuffers(0, 1, &mpGfxPerFrame);
		dc.VSSetConstantBuffers(1, 1, &mpGfxPerObj);
		dc.VSSetConstantBuffers(2, 1, &mpGfxPerMesh);

		dc.PSSetConstantBuffers(0, 1, &mpGfxPerFrame);
		dc.PSSetConstantBuffers(1, 1, &mpGfxPerObj);
		dc.PSSetConstantBuffers(2, 1, &mpGfxPerMesh);

		//select pixel shader to use
		ID3D11PixelShader* p;
		if ((mat.flags&Material::TFlags::LIT) != 0)
		{
			if (mat.pTextureRV)
				p = mpPSLitTex;
			else
				p = mpPSLit;
		}
		else
		{
			if (mat.pTextureRV)
				p = mpPSUnlitTex;
			else
				p = mpPSUnlit;
		}
		//do we have a texture
		if (mat.pTextureRV)
		{
			dc.PSSetSamplers(0, 1, &mpSamAnisotropic);
			dc.PSSetShaderResources(0, 1, &mat.pTextureRV);
		}
		dc.PSSetShader(p, nullptr, 0);

		//how is it blended?
		if ((mat.flags&Material::TFlags::TRANSPARENCY) != 0)
			dc.OMSetBlendState(mpBlendTransparent, mat.blendFactors, 0xffffffff);
		else if ((mat.flags&Material::TFlags::ALPHA_TRANSPARENCY) != 0)
		{
			float b[] = { 1, 1, 1, 1 };
			dc.OMSetBlendState(mpBlendAlphaTrans, b, 0xffffffff);
		}

		//should we cull?
		if ((mat.flags&Material::TFlags::CULL) == 0)
			if ((mat.flags&Material::TFlags::WIRE_FRAME) != 0)
				dc.RSSetState(mpRasterStates[RasterType::NOCULL_WIRE]);
			else
				dc.RSSetState(mpRasterStates[RasterType::NOCULL_FILLED]);
		else if ((mat.flags&Material::TFlags::CCW_WINDING) != 0)
			if ((mat.flags&Material::TFlags::WIRE_FRAME) != 0)
				dc.RSSetState(mpRasterStates[RasterType::CCW_WIRE]);
			else
				dc.RSSetState(mpRasterStates[RasterType::CCW_FILLED]);
		else
			if ((mat.flags&Material::TFlags::WIRE_FRAME) != 0)
				dc.RSSetState(mpRasterStates[RasterType::CW_WIRE]);
			else
				dc.RSSetState(mpRasterStates[RasterType::CW_FILLED]);

		dc.OMSetDepthStencilState(nullptr, 1);
	}
}