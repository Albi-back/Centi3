#ifndef GEOMETRYBUILDER_H
#define GEOMETRYBUILDER_H

class MeshMgr;
class Model;
class Mesh;

Mesh& BuildQuad(MeshMgr& mgr);

Mesh& BuildPyramid(MeshMgr& mgr);

Mesh& BuildCube(MeshMgr& mgr);

Mesh& BuildSphere(MeshMgr& mgr, int LatLines, int LongLines);

#endif
