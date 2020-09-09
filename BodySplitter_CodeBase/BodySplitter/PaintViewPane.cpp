#include "PaintViewPane.h"
#include "Mesh.h"
#include "Settings.h"

#ifndef CONSOLE

PaintViewPane::PaintViewPane(wxFrame* parent, int* args) : View3DPane(parent, args)
{
}


PaintViewPane::~PaintViewPane()
{
}

void PaintViewPane::setMesh(const std::shared_ptr<Mesh> &mesh)
{
	m_ptrMesh = mesh;
}

void PaintViewPane::draw()
{
	initializeDrawing();
	if (m_ptrMesh)
		m_ptrMesh->draw(0,nullptr);

	glFlush();
	SwapBuffers();
}

#endif