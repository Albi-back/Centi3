
#include "ShaderTypes.h"
#include "GeometryBuilder.h"
#include "Mesh.h"
#include "Model.h"
#include "D3DUtil.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Mesh& BuildQuad(MeshMgr& mgr)
{
	// Create vertex buffer
	VertexPosNormTex vertices[] =
	{	//quad in the XZ plane
		{ Vector3(-1, 0, -1), Vector3(0, 1, 0), Vector2(0, 1) },
		{ Vector3(-1, 0, 1), Vector3(0, 1, 0), Vector2(0, 0) },
		{ Vector3(1, 0, 1), Vector3(0, 1, 0), Vector2(1, 0) },
		{ Vector3(1, 0, -1), Vector3(0, 1, 0), Vector2(1, 1) }
	};

	// Create the index buffer
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3
	};
	Mesh &mesh = mgr.CreateMesh("quad");
	Material mat = Material::default;
	mat.gfxData.Diffuse = Vector4(1, 1, 1, 1);
	mat.gfxData.Ambient = Vector4(0.1f, 0.1f, 0.1f, 1);
	mat.gfxData.Specular = Vector4(1, 1, 1, 10);

	mesh.CreateFrom(vertices, 4, indices, 6, mat, 0, 6);
	return mesh;
}

Mesh& BuildPyramid(MeshMgr& mgr)
{
	// Create vertex buffer
	VertexPosNormTex vertices[] =
	{	//front
		{ Vector3(-1, 0, -1), Vector3(0, 0, 0), Vector2(0, 0) },
		{ Vector3(0, 1, 0), Vector3(0, 0, 0), Vector2(0, 0) },
		{ Vector3(1, 0, -1), Vector3(0, 0, 0), Vector2(0, 0) },
		//left
		{ Vector3(-1, 0, -1), Vector3(0, 0, 0), Vector2(0, 0) },
		{ Vector3(0, 0, 1), Vector3(0, 0, 0), Vector2(0, 0) },
		{ Vector3(0, 1, 0), Vector3(0, 0, 0), Vector2(0, 0) },
		//right
		{ Vector3(1, 0, -1), Vector3(0, 0, 0), Vector2(0, 0) },
		{ Vector3(0, 1, 0), Vector3(0, 0, 0), Vector2(0, 0) },
		{ Vector3(0, 0, 1), Vector3(0, 0, 0), Vector2(0, 0) },
		//bottom
		{ Vector3(-1, 0, -1), Vector3(0, 0, 0), Vector2(0, 0) },
		{ Vector3(1, 0, -1), Vector3(0, 0, 0), Vector2(0, 0) },
		{ Vector3(0, 0, 1), Vector3(0, 0, 0), Vector2(0, 0) }
	};

	// Create the index buffer
	UINT indices[] = {
		// front face
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11
	};
	 
	//make some face normals
	for (int i = 0; i < 4; ++i)
	{
		int idx = i * 3;
		Vector3 a(vertices[idx].Pos - vertices[idx + 1].Pos), b(vertices[idx + 2].Pos - vertices[idx + 1].Pos);
		a.Normalize();
		b.Normalize();
		vertices[idx].Norm = vertices[idx + 1].Norm = vertices[idx + 2].Norm = b.Cross(a);
	}

	Mesh &mesh = mgr.CreateMesh("pyramid");
	Material mat = Material::default;
	mat.gfxData.Diffuse = Vector4(1, 1, 1, 1);
	mat.gfxData.Ambient = Vector4(0.1f, 0.1f, 0.1f, 1);
	mat.gfxData.Specular = Vector4(1,1,1,10);
	mesh.CreateFrom(vertices, 12, indices, 12, mat, 0, 12);
	return mesh;
}

Mesh& BuildCube(MeshMgr& mgr)
{
	// Create vertex buffer
	VertexPosNormTex vertices[] =
	{	//front
		{ Vector3(-1, -1, -1), Vector3(0, 0, -1), Vector2(0, 0) },
		{ Vector3(-1, 1, -1), Vector3(0, 0, -1), Vector2(0, 0) },
		{ Vector3(1, 1, -1), Vector3(0, 0, -1), Vector2(0, 0) },
		{ Vector3(1, -1, -1), Vector3(0, 0, -1), Vector2(0, 0) },
		//right
		{ Vector3(1, -1, -1), Vector3(1, 0, 0), Vector2(0, 0) },
		{ Vector3(1, 1, -1), Vector3(1, 0, 0), Vector2(0, 0) },
		{ Vector3(1, 1, 1), Vector3(1, 0, 0), Vector2(0, 0) },
		{ Vector3(1, -1, 1), Vector3(1, 0, 0), Vector2(0, 0) },
		//left
		{ Vector3(-1, -1, 1), Vector3(-1, 0, 0), Vector2(0, 0) },
		{ Vector3(-1, 1, 1), Vector3(-1, 0, 0), Vector2(0, 0) },
		{ Vector3(-1, 1, -1), Vector3(-1, 0, 0), Vector2(0, 0) },
		{ Vector3(-1, -1, -1), Vector3(-1, 0, 0), Vector2(0, 0) },
		//top
		{ Vector3(-1, 1, -1), Vector3(0, 1, 0), Vector2(0, 0) },
		{ Vector3(-1, 1, 1), Vector3(0, 1, 0), Vector2(0, 0) },
		{ Vector3(1, 1, 1), Vector3(0, 1, 0), Vector2(0, 0) },
		{ Vector3(1, 1, -1), Vector3(0, 1, 0), Vector2(0, 0) },
		//bottom
		{ Vector3(1, -1, -1), Vector3(0, -1, 0), Vector2(0, 0) },
		{ Vector3(1, -1, 1), Vector3(0, -1, 0), Vector2(0, 0) },
		{ Vector3(-1, -1, 1), Vector3(0, -1, 0), Vector2(0, 0) },
		{ Vector3(-1, -1, -1), Vector3(0, -1, 0), Vector2(0, 0) },
		//back
		{ Vector3(1, -1, 1), Vector3(0, 0, 1), Vector2(0, 0) },
		{ Vector3(1, 1, 1), Vector3(0, 0, 1), Vector2(0, 0) },
		{ Vector3(-1, 1, 1), Vector3(0, 0, 1), Vector2(0, 0) },
		{ Vector3(-1, -1, 1), Vector3(0, 0, 1), Vector2(0, 0) }
	};

	// Create the index buffer
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,
		//right
		4, 5, 6,
		4, 6, 7,
		//left
		8, 9, 10,
		8, 10, 11,
		//top
		12, 13, 14,
		12, 14, 15,
		//bottom
		16, 17, 18,
		16, 18, 19,
		//back
		20, 21, 22,
		20, 22, 23
	};

	Mesh &mesh = mgr.CreateMesh("box");
	Material mat = Material::default;
	mat.gfxData.Diffuse = Vector4(1, 1, 1, 1);
	mat.gfxData.Ambient = Vector4(0.1f, 0.1f, 0.1f, 1);
	mat.gfxData.Specular = Vector4(1, 1, 1, 10);
	mesh.CreateFrom(vertices, 24, indices, 36, mat, 0, 36);
	return mesh;
}

Mesh& BuildSphere(MeshMgr& mgr, int LatLines, int LongLines)
{
	int NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
	int NumSphereFaces = ((LatLines - 3)*(LongLines)* 2) + (LongLines * 2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<VertexPosNormTex> vertices(NumSphereVertices);
	Vector3 currVertPos(0.0f, 0.0f, 1.0f);
	vertices[0].Pos = Vector3(0, 0, 1);

	for (int i = 0; i < LatLines - 2; ++i)
	{
		spherePitch = (i + 1) * (PI / (LatLines - 1));
		Matrix Rotationx = Matrix::CreateRotationX(spherePitch);
		for (int j = 0; j < LongLines; ++j)
		{
			sphereYaw = j * ((2*PI) / (LongLines));
			Matrix Rotationy = Matrix::CreateRotationZ(sphereYaw);
			currVertPos = Vector3::TransformNormal(Vector3(0, 0, 1), Rotationx * Rotationy);
			currVertPos.Normalize();
			int idx = i*LongLines + j + 1;
			vertices[idx].Pos = currVertPos;
			vertices[idx].Norm = currVertPos;
			vertices[idx].Norm.Normalize();
		}
	}

	vertices[NumSphereVertices - 1].Pos = Vector3(0, 0, -1);


	std::vector<unsigned int> indices(NumSphereFaces * 3);

	int k = 0;
	for (int l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 2] = l + 1;
		indices[k + 1] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 2] = LongLines;
	indices[k + 1] = 1;
	k += 3;

	for (int i = 0; i < LatLines - 3; ++i)
	{
		for (int j = 0; j < LongLines - 1; ++j)
		{
			indices[k] = i*LongLines + j + 1;
			indices[k + 1] = i*LongLines + j + 2;
			indices[k + 2] = (i + 1)*LongLines + j + 1;

			indices[k + 3] = (i + 1)*LongLines + j + 1;
			indices[k + 4] = i*LongLines + j + 2;
			indices[k + 5] = (i + 1)*LongLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i*LongLines) + LongLines;
		indices[k + 1] = (i*LongLines) + 1;
		indices[k + 2] = ((i + 1)*LongLines) + LongLines;

		indices[k + 3] = ((i + 1)*LongLines) + LongLines;
		indices[k + 4] = (i*LongLines) + 1;
		indices[k + 5] = ((i + 1)*LongLines) + 1;

		k += 6;
	}

	for (int l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = NumSphereVertices - 1;
		indices[k + 2] = (NumSphereVertices - 1) - (l + 1);
		indices[k + 1] = (NumSphereVertices - 1) - (l + 2);
		k += 3;
	}

	indices[k] = NumSphereVertices - 1;
	indices[k + 2] = (NumSphereVertices - 1) - LongLines;
	indices[k + 1] = NumSphereVertices - 2;

	Mesh &mesh = mgr.CreateMesh("sphere");
	Material mat = Material::default;
	mat.gfxData.Diffuse = Vector4(0.1f, 0.1f, 0.1f, 1);
	mat.gfxData.Specular = Vector4(1,1,1,10);
	mat.flags &= ~Material::TFlags::CCW_WINDING;
	mesh.CreateFrom(&vertices[0], NumSphereVertices, &indices[0], NumSphereFaces * 3, mat, 0, NumSphereFaces * 3);
	return mesh;
}
