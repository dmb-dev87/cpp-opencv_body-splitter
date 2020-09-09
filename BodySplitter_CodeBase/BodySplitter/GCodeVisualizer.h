#pragma once

#include <vector>
#include <memory>
#include <stdint.h>

// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/glcanvas.h"


#include "iSlicable.h"
#include "PrimeTowerHandler.h"
class Line;
class InnerPerims;

/* GCode visualization window, generates the vbo's to display the line path of a print
TODO: needs better handling of memory errors, should write the gcode to temp file first
then if memory run out, try perimeters only or layers until no memory */

class GCodeVisualizer
{
protected:
	bool needsRebuilding = false;
	const long int MAX_VERTS_PER_SECTION = 262144 * 3 * 3;	//~ 10mb buffer objects...
	unsigned long lastBlockSize;
	unsigned int blocksRequired = 0;
	bool readyToDraw = false;
	GLuint *bufferIDs = nullptr;
	struct BufferChunk {
		unsigned int bufferCount;
		GLfloat *verts = nullptr;
		GLfloat *normals = nullptr;
		GLubyte *colours = nullptr;
		BufferChunk(unsigned int vertscount) : bufferCount(vertscount)
		{
			GLfloat *verts = new GLfloat[vertscount];
			GLfloat *normals = new GLfloat[vertscount];
			GLubyte *colours = new GLubyte[vertscount];
		}
		~BufferChunk()
		{
			if (verts)
				delete[] verts;
			if (normals)
				delete[] normals;
			if (colours)
				delete[] colours;
		}
	};
	std::list<BufferChunk> bufferChunks;
	//Avoid smart ptrs since we just dump this info to openGL

	std::vector<uint16_t> m_vertCount;
	std::vector<GLfloat*> m_positionVect;
	std::vector<GLfloat*> m_normalVect;
	std::vector<GLubyte*> m_colourVect;
	GLfloat **m_pVerts = nullptr;
	GLshort **m_pNormals = nullptr;
	GLubyte **m_pColours = nullptr;
	std::vector<std::pair<unsigned long, unsigned long>> LayerStartLayerEnd;
	unsigned long layerCount = 0;
	iSlicable* m_ptrSlicable;
	std::size_t lineCount = 0;
	unsigned long startVertNumber;
	unsigned long endVertNumber;

	void drawPerimTree(double z, double thickness, double extWidth, double fullInfillExtWidth, std::vector<float> &pts, std::vector<float> &norms, std::vector<GLubyte> &colours, const InnerPerims *set);

	virtual void createVBOs();
	void lineToGLPoints(double z, double thickness, double extWidth, std::vector<float> &pts, std::vector<float> &norms, std::vector<GLubyte> &colours, const Line *inLine, double r = -1, double g = -1, double b = -1, const Line* lastLine = nullptr);
public:
	GCodeVisualizer(iSlicable *owner);
	~GCodeVisualizer();
	void updateMesh()
	{
		readyToDraw = true;
		needsRebuilding = true;
	}
	void draw()noexcept;
	void meshReslice()
	{
		readyToDraw = true;
		needsRebuilding = true;
		//createVBOs();
	}
	void setStartLayer(unsigned int layer); //for preview drawing
	void setEndLayer(unsigned int layer);//For preview drawiung
	void setEndLine(unsigned int line);
	std::size_t getLineCount() const;
};

