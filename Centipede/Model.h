#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <cassert>
#include "d3d.h"

class Mesh;

class Model
{
public:
	void Initialise(const std::string& meshFileName);
	void Initialise(Mesh &mesh);
	
	DirectX::SimpleMath::Vector3& GetPosition() { return mPosition; }
	DirectX::SimpleMath::Vector3& GetScale() { return mScale; }
	DirectX::SimpleMath::Vector3& GetRotation() { return mRotation; }
	void GetWorldMatrix(DirectX::SimpleMath::Matrix& w);
	Mesh& GetMesh() {
		assert(mpMesh);
		return *mpMesh;
	}
	Material* HasOverrideMat() {
		if (mUseOverrideMat)
			return &mOverrideMaterial; 
		return nullptr;
	}
	void SetOverrideMat(Material *pMat = nullptr) {
		if (!pMat) {
			mUseOverrideMat = false;
			return;
		}
		mUseOverrideMat = true;
		mOverrideMaterial = *pMat;
	}
	Model& operator=(const Model& m)
	{
		mpMesh = m.mpMesh;
		mPosition = m.mPosition;
		mScale = m.mScale;
		mRotation = m.mRotation;
		mOverrideMaterial = m.mOverrideMaterial;
		mUseOverrideMat = m.mUseOverrideMat;
		return *this;
	}
private:

	Mesh *mpMesh = nullptr;
	DirectX::SimpleMath::Vector3 mPosition, mScale, mRotation;
	Material mOverrideMaterial;
	bool mUseOverrideMat = false;
};

#endif

