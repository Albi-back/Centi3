#ifndef MESH_H
#define MESH_H

#include <vector>
#include <unordered_map>

#include "ShaderTypes.h"


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

	//buffer data
	ID3D11Buffer* mpVB = nullptr;
	ID3D11Buffer* mpIB = nullptr;
	int mNumIndices = 0;
	int mNumVerts = 0;

	Material material;	//the material describes how the surface reacts to light
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
	/*
	* configure the geometry inside a mesh
	* verts - IN an array of local space vertex data
	* numVerts - IN how many
	* indices - IN an array of vertex indices that define triangles
	* numIndices - IN how many
	* mat - IN a material definining how this geometry reflects light
	*/
	void CreateFrom(const VertexPosNormTex verts[], int numVerts, const unsigned int indices[],
		int numIndices, const Material& mat, int meshStartIndex, int meshNumIndices);

	//getters
	int GetNumSubMeshes() const {
		return (int)mSubMeshes.size();
	}
	SubMesh& GetSubMesh(int idx) {
		return *mSubMeshes.at(idx);
	}

	//give the mesh a name so we can look it up in the library
	std::string mName;


private:
	Mesh(const Mesh& m) = delete;
	Mesh& operator=(const Mesh& m) = delete;
	//a mesh can contain multiple surfaces (geometry), each surface having 
	//potentially different material properties
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
	//look up a mesh by name
	Mesh& GetMesh(const std::string& name);
	//create a new mesh in the library with the given name
	Mesh& CreateMesh(const std::string& name);

	//array of meshes - a clever array where you can look things up with a key
	//and in this case the key is a string
	typedef std::unordered_map<std::string, Mesh*> Meshes;
	Meshes mMeshes;
};

#endif
