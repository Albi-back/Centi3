
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mesh.h"
#include "D3DUtil.h"
#include "FX.h"
#include "D3D.h"
#include "WindowUtils.h"


using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


void SubMesh::Release()
{
	ReleaseCOM(mpVB);
	ReleaseCOM(mpIB);
	mNumIndices = mNumVerts = 0;
}

bool SubMesh::Initialise(MyD3D& d3d, const aiScene* scene, const aiMesh* mesh)
{
	

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	if (scene->HasMaterials()) // Check that there are textures and materials
	{
		aiMaterial* aimaterial = scene->mMaterials[mesh->mMaterialIndex];
		if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) // If there is a diffuse texture
		{
			aiString path;
			if (aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				string textureName = path.C_Str();
				StripPathAndExtension(textureName);
				material.texture = textureName;
				textureName += ".dds";
				material.pTextureRV = d3d.GetCache().LoadTexture(&d3d.GetDevice(),textureName);
			}
		}
		aiString name;
		aimaterial->Get(AI_MATKEY_NAME, name);
		material.name = name.C_Str();

		aiColor3D diffuse(1, 1, 1), ambient(1, 1, 1), specular(1, 1, 1);
		float power = 1;
		aimaterial->Get(AI_MATKEY_SHININESS_STRENGTH, power);
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		aimaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		material.gfxData.Set(Vector4(diffuse.r, diffuse.g, diffuse.b, 1), Vector4(ambient.r, ambient.g, ambient.b, 1), Vector4(specular.r, specular.g, specular.b, power));
	}

	assert(mesh->HasFaces());
	mNumVerts = mNumIndices = mesh->mNumFaces * 3;

	// Create the Mesh using the vertex count that was read in.
	vector<VertexPosNormTex> verts;
	verts.insert(verts.begin(), mNumVerts, VertexPosNormTex());
	vector<unsigned int> indices;
	indices.insert(indices.begin(), mNumIndices, 0);

	int curentVert = 0;
	for (int f = 0; f < (int)mesh->mNumFaces; f++)
	{
		for (int i = 0; i < (int)mesh->mFaces[f].mNumIndices; i++)
		{
			int ind = mesh->mFaces[f].mIndices[i];

			const aiVector3D* pos = &(mesh->mVertices[ind]);
			const aiVector3D* norm = &(mesh->mNormals[ind]);
			const aiVector3D* tex = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][ind]) : &Zero3D;

			verts[curentVert].Pos.x = pos->x;
			verts[curentVert].Pos.y = pos->y;
			verts[curentVert].Pos.z = pos->z;

			verts[curentVert].Norm.x = norm->x;
			verts[curentVert].Norm.y = norm->y;
			verts[curentVert].Norm.z = norm->z;

			verts[curentVert].Tex.x = tex->x;
			verts[curentVert].Tex.y = tex->y;

			indices[curentVert] = curentVert;

			curentVert++;
		}
	}

	CreateVertexBuffer(d3d.GetDevice(), sizeof(VertexPosNormTex)*mNumVerts, &verts[0], mpVB);
	CreateIndexBuffer(d3d.GetDevice(), sizeof(unsigned int)*mNumIndices, &indices[0], mpIB);

	return true;
}




void Mesh::CreateFrom(const string& fileName, MyD3D& d3d)
{
	string path, fstring(fileName);
	StripPathAndExtension(fstring, &path);
	if (mName.empty())
		mName = fstring;
	string oldPath = d3d.GetCache().GetAssetPath();
	d3d.GetCache().SetAssetPath(path);

#if defined(DEBUG) | defined(_DEBUG)
	//disable any memory leak checking, this isn't our code!
	int tmp = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	_CrtSetDbgFlag(0);
#endif

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
		aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
		aiProcess_Triangulate | // Ensure all verticies are triangulated (each 3 vertices are triangle)
		aiProcess_ConvertToLeftHanded | // convert everything to D3D left handed space (by default right-handed, for OpenGL)
		aiProcess_SortByPType | // ?
		aiProcess_RemoveRedundantMaterials | // remove redundant materials
		aiProcess_FindDegenerates | // remove degenerated polygons from the import
		aiProcess_FindInvalidData | // detect invalid model data, such as invalid normal vectors
		aiProcess_GenUVCoords | // convert spherical, cylindrical, box and planar mapping to proper UVs
		aiProcess_TransformUVCoords | // preprocess UV transformations (scaling, translation ...)
		aiProcess_FindInstances | // search for instanced meshes and remove them by references to one master
		aiProcess_OptimizeMeshes | // join small meshes, if possible;
		aiProcess_GenSmoothNormals | //if no normals, make them
		0
	);

	if (scene == NULL) // If there is no valid scene it might be using the old model format
	{
		DBOUT("Couldn't load " << fileName);
		assert(false);
		return;
	}

	//turn any checking back on
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(tmp);
#endif
	if (scene->HasMeshes())
	{
		for (int i = 0; i < (int)scene->mNumMeshes; i++)
		{
			mSubMeshes.push_back(new SubMesh);
			mSubMeshes.back()->Initialise(d3d, scene, scene->mMeshes[i]);
		}
	}

#if defined(DEBUG) | defined(_DEBUG)
	//disable any memory leak checking, this isn't our code!
	_CrtSetDbgFlag(0);
#endif

	importer.FreeScene();

	//turn any checking back on
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(tmp);
#endif

	d3d.GetCache().SetAssetPath(oldPath);
}



//******************************************************************

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
