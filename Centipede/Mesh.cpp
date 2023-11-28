#include "Mesh.h"
#include "D3DUtil.h"
#include "FX.h"
#include "D3D.h"
#include "WindowUtils.h"


void SubMesh::Release()
{
	ReleaseCOM(mpVB);
	ReleaseCOM(mpIB);
	mNumIndices = mNumVerts = 0;
}

Mesh& MeshMgr::GetMesh(const std::string& name)
{
	Meshes::iterator it = mMeshes.find(name);
	assert(it != mMeshes.end());
	return *(*it).second;
}


Mesh& MeshMgr::CreateMesh(const std::string& name)
{
	Meshes::iterator it = mMeshes.find(name);
	assert(it == mMeshes.end());

	Mesh *p = new Mesh(name);
	mMeshes[name] = p;
	return *p;
}

void MeshMgr::Release()
{
	for (auto it : mMeshes)
		delete it.second;
	mMeshes.clear();
}

void Mesh::Release()
{
	for (int i = 0; i < (int)mSubMeshes.size(); ++i)
		delete mSubMeshes[i];
	mSubMeshes.clear();
}

void Mesh::CreateFrom(const VertexPosNormTex verts[], int numVerts, const unsigned int indices[], int numIndices, 
	const Material& mat, int meshStartIndex, int meshNumIndices)
{
	Release();
	SubMesh*p = new SubMesh;
	mSubMeshes.push_back(p);
	p->mNumIndices = meshNumIndices;
	p->mNumVerts = numVerts;
	p->material = mat;
	CreateVertexBuffer(WinUtil::Get().GetD3D().GetDevice(),sizeof(VertexPosNormTex)*numVerts, verts, p->mpVB);
	CreateIndexBuffer(WinUtil::Get().GetD3D().GetDevice(), sizeof(unsigned int)*numIndices, indices, p->mpIB);
}
