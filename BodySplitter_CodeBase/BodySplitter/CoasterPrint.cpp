#include "CoasterPrint.h"
#include "BodySplitterTexture.h"
#include "Settings.h"
#include "Perimeter.h"
#include "colour.h"
#include "RSPolygon.h"
#include "Layer.h"
#include "PrintBed.h"

#ifdef _WIN32
#include <Windows.h>
#endif
// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif


CoasterPrint::CoasterPrint()
{
	dpi = Settings::GetSingleton().getDoubleValue("mmPerPixel");
	widthX = 100;
	heightY = 100;
	zHeight = (Settings::getSingleton().getDoubleValue("coasterBaseLayers")+ Settings::getSingleton().getDoubleValue("coasterSurfaceLayers"))
		* Settings::getSingleton().getDoubleValue("layerThickness");

	m_Offset[0] = 0; m_Offset[1] = 0; m_Offset[2] = 0;
}


CoasterPrint::~CoasterPrint()
{
	if (layerOutput)
		delete[] layerOutput;
}

bool CoasterPrint::loadImage(const std::string &filename)
{
	this->drawableName = filename.substr(filename.find_last_of("\\")+1);
	m_TexturePtr = std::make_shared<CoasterPrintTexture>(filename);
//	texturePtr->adjustColour(Settings::getSingleton().getColours());
	m_TexturePtr->makeSimplified(Settings::GetSingleton().getColours(), 2048);
	widthX = m_TexturePtr->getXSize()*dpi;
	heightY = m_TexturePtr->getYSize()*dpi;
	return (m_TexturePtr->isGood());
}

bool CoasterPrint::loadImage(const std::vector<unsigned char> &buff, const std::string &fileName)
{
	this->drawableName = fileName;
	m_TexturePtr = std::make_shared<CoasterPrintTexture>();
	m_TexturePtr->openImage(buff);
	m_TexturePtr->makeSimplified(Settings::GetSingleton().getColours(), 2048);
	widthX = m_TexturePtr->getXSize()*dpi;
	heightY = m_TexturePtr->getYSize()*dpi;
	return (m_TexturePtr->isGood());
}

void CoasterPrint::startSlice(wxFrame* baseframe)
{
	m_ptrVisualizer = std::make_shared<GCodeVisualizer>(this);
	std::dynamic_pointer_cast<CoasterPrintTexture>(m_TexturePtr)->breakDownImage(widthX*m_Scale[X_AXIS],heightY*m_Scale[Y_AXIS]);
	polygons = std::dynamic_pointer_cast<CoasterPrintTexture>(m_TexturePtr)->getPolys();
	layerOutput = new std::shared_ptr<Layer>[getLayerCount() + 1];
	return;
}

std::unordered_set<uint32_t> CoasterPrint::getColours()
{
	if (m_TexturePtr == nullptr)
	{
		std::unordered_set<uint32_t> temp;
		temp.insert(this->m_Colour);
		return temp;
	}
	return m_TexturePtr->getColours();
}


void CoasterPrint::draw(unsigned int opts, const Colour *colourToDraw) noexcept
{
#ifndef CONSOLE
	glPushMatrix();
	float minX = -(float)widthX*m_Scale[X_AXIS] / 2.0f;
	float minY = -(float)heightY*m_Scale[Y_AXIS] / 2.0f;
	float maxX = (float)widthX*m_Scale[X_AXIS]/2.0f;
	float maxY = (float)heightY*m_Scale[Y_AXIS]/2.0f;
	float z = zHeight;//std::max((float)zHeight,1.0f);
	//glRotated(m_Rotation[Z_AXIS], 0, 0, 1); // Rotations are currently not considered for coasters, this probably isnt a hard fix
	glTranslated(m_Offset[0],m_Offset[1],m_Offset[2]);
	if(opts &= DrawingOptions::OBJECT_PICK)
	{
	  glDisable(GL_TEXTURE_2D);
	  glDisable(GL_LIGHTING);
	  if (colourToDraw)
		  glColor3ub(colourToDraw->getR<uint8_t>(), colourToDraw->getG<uint8_t>(), colourToDraw->getB<uint8_t>());
	}
	else
	{
	  glEnable(GL_TEXTURE_2D);
	  m_TexturePtr->bindTexture(m_simplifiedTexture);
	}
	glBegin(GL_QUADS);
	// Front Face
	if(colourToDraw == nullptr)
		glColor3f(1.0, 1.0f, 1.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(minX, minY, z);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(maxX, minY, z);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(maxX, maxY, z);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(minX, maxY, z);  // Top Left Of The Texture and Quad
																// Back Face
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(minX, minY, 0.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(minX, maxY, 0.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(1.0f, 1.0f); glVertex3f(maxX, maxY, 0.0f);  // Top Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(maxX, minY, 0.0f);  // Bottom Left Of The Texture and Quad
								
															 // Top Face
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.99f); glVertex3f(minX, maxY, 0.0f);  // Top Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.99f); glVertex3f(minX, maxY, z);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.99f); glVertex3f(maxX, maxY, z);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(1.0f, 0.99f); glVertex3f(maxX, maxY, 0.0f);  // Top Right Of The Texture and Quad
	
															   // Bottom Face
	glNormal3f(0.0f, -1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(minX, minY, 0.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(maxX, minY, 0.0f);  // Top Left Of The Texture and Quad
	glTexCoord2f(1.0f, 0.0f); glVertex3f(maxX, minY, z);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(minX, minY, z);  // Bottom Right Of The Texture and Quad

																 // Right face
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.99f, 0.0f); glVertex3f(maxX, minY, 0.0f);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(0.99f, 1.0f); glVertex3f(maxX, maxY, 0.0f);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.99f, 1.0f); glVertex3f(maxX, maxY, z);  // Top Left Of The Texture and Quad
	glTexCoord2f(0.99f, 0.0f); glVertex3f(maxX, minY, z);  // Bottom Left Of The Texture and Quad
																// Left Face
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(minX, minY, 0.0f);  // Bottom Left Of The Texture and Quad
	glTexCoord2f(0.0f, 0.0f); glVertex3f(minX, minY, z);  // Bottom Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(minX, maxY, z);  // Top Right Of The Texture and Quad
	glTexCoord2f(0.0f, 1.0f); glVertex3f(minX, maxY, 0.0f);  // Top Left Of The Texture and Quad
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
#endif
}

void CoasterPrint::moveTo(double x, double y, double z)
{
	PrintBed::getSingleton().changesNeedSaving();
  m_Offset[0] = x;
  m_Offset[1] = y;
  m_Offset[2] = z;
}

void CoasterPrint::setOFfsetX(double newPos)
{
	PrintBed::getSingleton().changesNeedSaving();
	m_Offset[0] = newPos;
}

void CoasterPrint::setOFfsetY(double newPos)
{
	PrintBed::getSingleton().changesNeedSaving();
	m_Offset[1] = newPos;
}

void CoasterPrint::setOFfsetZ(double newPos)
{
	PrintBed::getSingleton().changesNeedSaving();
	m_Offset[2] = newPos;
}

double CoasterPrint::getOffsetX() const
{
  return m_Offset[0]-(widthX*m_Scale[X_AXIS]/2.0);
}
double CoasterPrint::getOffsetY() const
{
  return m_Offset[1] - (heightY*m_Scale[Y_AXIS] / 2.0);;
}
double CoasterPrint::getOffsetZ() const
{
  return m_Offset[2];
}


void CoasterPrint::generateSolidLayers()
{

}

void CoasterPrint::createSlice(unsigned int layer)
{
	double minZ = layer*(Settings::getSingleton().getDoubleValue("layerThickness"));
	double angle = 45;
	if (layer % 2)
		angle = 360.0 - 45.0;
	layerOutput[layer] = std::make_shared<Layer>(minZ,layer,thisTool);
	if (layer < getOffsetZ() / Settings::GetSingleton().getDoubleValue("layerThickness"))
	{

	}
	else if (layer - getOffsetZ() / Settings::GetSingleton().getDoubleValue("layerThickness")
		< Settings::GetSingleton().getDoubleValue("coasterBaseLayers"))
	{
		std::shared_ptr<ExRSPolygon> newPoly = std::make_shared<ExRSPolygon>(thisTool);
		std::vector<Line> pLines;
		//T0extrusionWidth
		double exWidth = Settings::GetSingleton().getDoubleValue("T"+ std::to_string((int)thisTool)+"extrusionWidth") / 2.0;
		double wide = widthX *m_Scale[X_AXIS];// Settings::GetSingleton().getDoubleValue("coasterSize");
		double height = heightY*m_Scale[Y_AXIS];
		/*
	
		*/
		double minX = exWidth;
		double maxX = wide - exWidth;
		double minY = exWidth;
		double maxY = height - exWidth;
		pLines.push_back(Line(Vertex(minX, minY),			Vertex(minX, maxY), Line::LineType::PERIM));
		pLines.push_back(Line(Vertex(minX, maxY),			Vertex(maxX, maxY), Line::LineType::PERIM));
		pLines.push_back(Line(Vertex(maxX, maxY),			Vertex(maxX, minY), Line::LineType::PERIM));
		pLines.push_back(Line(Vertex(maxX, minY),			Vertex(minX, minY), Line::LineType::PERIM));
		Perimeter newPerim(pLines);
		newPerim.setColour(Settings::getSingleton().getColourValue("coasterBaseColour"));
		newPoly->setContour(newPerim);
		newPoly->generateInnerPerimeters();
		newPoly->generateInfill(angle, layer%2, 100.0);
		layerOutput[layer]->addRSPoly(newPoly);
	}
	else
	{
		for (auto it : polygons)
		{
			std::shared_ptr<ExRSPolygon> newPoly = std::make_shared<ExRSPolygon>(thisTool);
			newPoly->setContour(it->m_contour);
			for (auto holeIT : it->m_holes)
				newPoly->addHole(holeIT);
			newPoly->generateInnerPerimeters();
			newPoly->generateInfill(angle, layer % 2, 100.0f);
			layerOutput[layer]->addRSPoly(newPoly);
		}
	}
}

RoVaClipperLib::Paths CoasterPrint::getEnclosingPerimeters() const
{
	RoVaClipperLib::cInt wide = (widthX *m_Scale[X_AXIS]*DEDOUBLER);
	RoVaClipperLib::cInt height = (heightY*m_Scale[Y_AXIS]*DEDOUBLER);

	RoVaClipperLib::Paths out;
	out.push_back(RoVaClipperLib::Path());
	out.back().push_back(RoVaClipperLib::IntPoint(-wide/2.0, -height/2.0));
	out.back().push_back(RoVaClipperLib::IntPoint(wide/2.0, -height/2.0));
	out.back().push_back(RoVaClipperLib::IntPoint(wide/2.0, height/2.0));
	out.back().push_back(RoVaClipperLib::IntPoint(-wide/2.0, height/2.0));
	//Is always closed, its a perimeter
	return out;
}

RoVaClipperLib::Paths CoasterPrint::getPerimeters(unsigned int layerID)
{
	return getEnclosingPerimeters();
}

std::shared_ptr<Layer> CoasterPrint::getLayer(unsigned int id)const
{
	return layerOutput[id];
}
unsigned int CoasterPrint::getLayerCount()const
{
	//return Settings::getSingleton().getDoubleValue("bottomSolidLayers") + Settings::getSingleton().getDoubleValue("topSolidLayers");
	return GET_DOUBLE("coasterBaseLayers") + GET_DOUBLE("coasterSurfaceLayers") + getOffsetZ()/Settings::getSingleton().getDoubleValue("layerThickness");
}
unsigned int CoasterPrint::getLineCount() const
{
	return 9; //hmm? we dont use line count anymore so this works... kinda
}
void CoasterPrint::slicePerimeters(unsigned int layerID)
{

}
void CoasterPrint::fillInfill(unsigned int layerID, const std::vector< std::vector< RoVaClipperLib::IntPoint > >& fullInfill, const RoVaClipperLib::Paths *sources, bool skipTopLogic)
{

}
void CoasterPrint::createSurfaces(unsigned int layerID)
{

}
void CoasterPrint::completeLayer(unsigned int layerID)
{

}
void CoasterPrint::generateSupport()
{

}

// SEL add stub for exporting plygons
void CoasterPrint::LogPerimeters()
{

}

void CoasterPrint::updateTextureColour(uint16_t colour)
{
	m_TexturePtr->adjustColour(colour);
}

// iDrawble
void CoasterPrint::applyRotation(double zRotation)
{
	// Not supported
}
void CoasterPrint::applyScale(double amt)
{
	//This makes sense on a mesh, but we use the scale amount to draw our mesh and the math is quick: x*=amt
	//widthX *= amt;
	//heightY *= amt;
	//zHeight *= amt;
}
double CoasterPrint::getXExtent() const
{
	return widthX;
}
double CoasterPrint::getYExtent()  const
{
	return heightY;
}

void CoasterPrint::setMaxSize()
{
	double xRatio = Settings::getSingleton().getDoubleValue("maxX") / widthX;
	double yRatio = Settings::getSingleton().getDoubleValue("maxY") / heightY;
	double minRatio = std::min(xRatio, yRatio);
	m_Scale[X_AXIS] = minRatio;
	m_Scale[Y_AXIS] = minRatio;
}

void CoasterPrint::applyTransformations()
{
	applyRotation(m_Rotation[Z_AXIS]);
	applyScale(std::max(m_Scale[X_AXIS],std::max(m_Scale[Y_AXIS], m_Scale[Z_AXIS])));
	
}

void CoasterPrint::undoTransformations()
{
	applyScale(1.0/(std::max(m_Scale[X_AXIS], std::max(m_Scale[Y_AXIS], m_Scale[Z_AXIS]))));
	applyRotation(-m_Rotation[Z_AXIS]);
}

bool CoasterPrint::isValidExtension(const std::string &path)
{
	const std::vector<std::string> valids{
		"png",
		"bmp",
		"jpg"
	};
	std::string extension = path.substr(path.size() - 3, 3);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	for (auto &it : valids)
	{
		if (it == extension)
			return true;
	}
	return false;
}