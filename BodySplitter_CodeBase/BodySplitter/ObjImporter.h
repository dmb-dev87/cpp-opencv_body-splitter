#pragma once
/* Deprecated: was used to load objs with vertex colours, easy to paint 
hard to print */
#include <vector>
#include <string>
#include <assimp/mesh.h>


class ObjImporter
{
public:
	ObjImporter();
	bool loadFile(const std::string &file);
	~ObjImporter();

	std::vector<aiVector3D> positions;
	std::vector<aiColor4D> colour;
	std::vector<aiVector3D> normals;
	std::vector<aiFace> faces;
};

