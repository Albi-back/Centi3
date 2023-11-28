#include "ShaderTypes.h"

const Material Material::default {
	{ { 1, 1, 1, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 1 } },
		nullptr,
	{},
		TFlags::APPEND_PATH | TFlags::LIT | TFlags::CULL | TFlags::CCW_WINDING,
		"default",
		""
};


const D3D11_INPUT_ELEMENT_DESC VertexPosNormTex::sVertexDesc[3]{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
