#include "D3DUtil.h"
#include "D3D.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

static float gTime = 0, gElapsed=0;

float GetClock()
{
	return gTime;
}

float GetElapsedSec()
{
	return gElapsed;
}

void AddSecToClock(float sec)
{
	gTime += sec;
	gElapsed = sec;
}

void CreateProjectionMatrix(Matrix& projM, float fieldOfView, float aspectRatio, float nearZ, float farZ)
{
	projM = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearZ, farZ);
}

void CreateViewMatrix(DirectX::SimpleMath::Matrix& viewM, const DirectX::SimpleMath::Vector3& camPos, const DirectX::SimpleMath::Vector3& camTgt, const DirectX::SimpleMath::Vector3& camUp)
{
	viewM = XMMatrixLookAtLH(camPos, camTgt, camUp);
}

void CreateVertexBuffer(ID3D11Device& d3dDevice, UINT bufferSize, const void *pSourceData, ID3D11Buffer* &pVB)
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = bufferSize;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	ZeroMemory(&vinitData, sizeof(D3D11_SUBRESOURCE_DATA));
	vinitData.pSysMem = pSourceData;
	HR(d3dDevice.CreateBuffer(&vbd, &vinitData, &pVB));
}

void CreateIndexBuffer(ID3D11Device& d3dDevice, UINT bufferSize, const void *pSourceData, ID3D11Buffer* &pIB)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = bufferSize;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = pSourceData;
	HR(d3dDevice.CreateBuffer(&ibd, &iinitData, &pIB));
}


Matrix InverseTranspose(const Matrix& m)
{
	XMMATRIX A = m;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	return XMMatrixTranspose(XMMatrixInverse(&det, A));
}

void StripPathAndExtension(std::string& fileName, std::string* pPath/*=nullptr*/, std::string* pExt/*=nullptr*/)
{
	if (pPath)
		*pPath = "";
	if (pExt)
		*pExt = "";
	//filename only
	std::string::size_type n = fileName.find_last_of("\\/");
	if (n != std::string::npos)
	{
		if (pPath)
			pPath->append(fileName.c_str(), n + 1);
		fileName.erase(0, n + 1);
	}
	n = fileName.find_last_of(".");
	assert(n != std::string::npos);
	if (pExt)
		*pExt = fileName.substr(n);
	fileName.erase(n, fileName.length());
}
