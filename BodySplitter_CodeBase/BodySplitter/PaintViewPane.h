#pragma once
/* TODO: This*/
#include "View3DPane.h"
#include <memory>

class Mesh;

class PaintViewPane :
	public View3DPane
{
private:
	std::shared_ptr<Mesh> m_ptrMesh;
public:
	PaintViewPane(wxFrame* parent, int* args);
	virtual ~PaintViewPane();

	void setMesh(const std::shared_ptr<Mesh> &mesh);

	void draw() override;
};

