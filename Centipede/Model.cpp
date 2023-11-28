#include "FX.h"
#include "Model.h"
#include "WindowUtils.h"
#include "Mesh.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void Model::Initialise(const std::string& meshFileName)
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	
}


void Model::Initialise(Mesh &mesh)
{
	mpMesh = &mesh;
	mPosition = Vector3(0, 0, 0);
	mScale = Vector3(1, 1, 1);
	mRotation = Vector3(0, 0, 0);
}

void Model::GetWorldMatrix(DirectX::SimpleMath::Matrix& w)
{
	w = Matrix::CreateScale(mScale) * Matrix::CreateRotationX(mRotation.x) *
		Matrix::CreateRotationY(mRotation.y) * Matrix::CreateRotationZ(mRotation.z) *
		Matrix::CreateTranslation(mPosition);
}
