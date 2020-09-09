#pragma once

#include <memory>

//forward declares
class Mesh;

class MeshPainter
{
private:
	std::shared_ptr<Mesh> m_ptrMesh;
	bool unwrapUV();
public:
	void setMesh(const std::shared_ptr<Mesh> &meshPtr);
	MeshPainter();
	~MeshPainter();
};

