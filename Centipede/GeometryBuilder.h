#ifndef GEOMETRYBUILDER_H
#define GEOMETRYBUILDER_H

class MeshMgr;
class Model;
class Mesh;

/*
* build the geometry for some common shapes and store in the MeshMgr library
* only one of each allowed, identified by text name
*/
Mesh& BuildQuad(MeshMgr& mgr);
Mesh& BuildPyramid(MeshMgr& mgr);
Mesh& BuildCube(MeshMgr& mgr);
Mesh& BuildSphere(MeshMgr& mgr, int LatLines, int LongLines);

#endif
