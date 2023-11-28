#ifndef MESH_H
#define MESH_H

#include <vector>
#include <unordered_map>

#include "ShaderTypes.h"

//assimp
struct aiScene;
struct aiMesh;

class MyD3D;

/*
Part of a vertex/index buffer that uses the same
material (colour and texture).
*/
class SubMesh
{
public:
	~SubMesh() { 
		Release();
	}
	void Release();
	bool Initialise(MyD3D& d3d, const aiScene* scene, const aiMesh* mesh);


	//buffer data
	ID3D11Buffer* mpVB = nullptr;
	ID3D11Buffer* mpIB = nullptr;
	int mNumIndices = 0;
	int mNumVerts = 0;

	Material material;
};

/*
A mesh contains all the vertex data for a 3D object
the indices give the triangle order. Where a mesh uses
different materials (colour or textures) it needs to be
rendered on material at a time, so the buffer is split
into different parts.
*/
class Mesh
{
public:
	Mesh(const std::string& name) : mName(name) {}
	~Mesh() { 
		Release(); 
	}
	void Release();
	void CreateFrom(const VertexPosNormTex verts[], int numVerts, const unsigned int indices[],
		int numIndices, const Material& mat, int meshStartIndex, int meshNumIndices);
	void CreateFrom(const std::string& fileName, MyD3D& d3d);
	int GetNumSubMeshes() const {
		return (int)mSubMeshes.size();
	}
	SubMesh& GetSubMesh(int idx) {
		return *mSubMeshes.at(idx);
	}

	
	std::string mName;


private:
	Mesh(const Mesh& m) = delete;
	Mesh& operator=(const Mesh& m) = delete;

	std::vector<SubMesh*> mSubMeshes;
};

/*
A mesh is only ever loaded once, many model instances can use the same
mesh e.g. one tree mesh used to render 1000 trees, all different sizes,
orientations, positions.
*/
class MeshMgr
{
public:
	~MeshMgr() { 
		Release(); 
	}
	void Release();
	Mesh& GetMesh(const std::string& name);
	Mesh& CreateMesh(const std::string& name);

	typedef std::unordered_map<std::string, Mesh*> Meshes;
	Meshes mMeshes;
};

#endif
