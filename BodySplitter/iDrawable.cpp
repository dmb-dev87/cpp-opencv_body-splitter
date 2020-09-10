#include "iDrawable.h"
#include "BodySplitterTexture.h"

iDrawable::iDrawable()
{
	// no memset since this is mutliplatform
	for (int i = 0; i < 3; i++)
	{
		m_Offset[i] = 0.0;
		m_Rotation[i] = 0.0;
		m_Scale[i] = 1.0;
	}
	m_Colour = Colour(1.0, 1.0, 1.0, 1.0);
}

void iDrawable::savePropertiesToStream(std::ostream &outStream) const
{
	outStream << "[offset]\n";
	outStream << "X = " << m_Offset[X_AXIS] << "\n";
	outStream << "Y = " << m_Offset[Y_AXIS] << "\n";
	outStream << "Z = " << m_Offset[Z_AXIS] << "\n";
	outStream << "[rotation]\n";
	outStream << "X = " << m_Rotation[X_AXIS] << "\n";
	outStream << "Y = " << m_Rotation[Y_AXIS] << "\n";
	outStream << "Z = " << m_Rotation[Z_AXIS] << "\n";
	outStream << "[scale]\n";
	outStream << "X = " << m_Scale[X_AXIS] << "\n";
	outStream << "Y = " << m_Scale[Y_AXIS] << "\n";
	outStream << "Z = " << m_Scale[Z_AXIS] << "\n";
	outStream << "[other]\n";
	outStream << "useTexture = " << m_bUseTexture << "\n";
	outStream << "useCustomColour = " << m_Colour.isSet() << "\n";
	outStream << "drawableName = " << drawableName << "\n";
	outStream << "tool = " << (int)thisTool << "\n";
	outStream << "colour = " << m_Colour.getRGBA() << "\n";
	if (m_TexturePtr)
	{
		outStream << "[texture]\n";
		outStream << "xRes = " << m_TexturePtr->getTargetXSize() << "\n";
		outStream << "yRes = " << m_TexturePtr->getTargetYSize() << "\n";
		outStream << "colourCount = " << m_TexturePtr->getColours().size() << "\n";
	}

	outStream << "[PropertiesEnd]\n";
}

void iDrawable::readPropertiesFromStream(std::istream &inStream)
{
	std::string currentLine;
	const int OFFSET = 1;
	const int SCALE = 2;
	const int ROTATION = 3;
	const int TEXTURE = 4;
	const int OTHER = 20;
	const int END = 100;
	int currentAxis = 0;

	int currentType = -1;
	std::getline(inStream, currentLine); //Covers the case where inStream is empty
	bool colourIsSet = false;

	int xRes, yRes;
	unsigned int colourCount;
	while (!inStream.eof())
	{
		if (currentType == OFFSET && currentAxis < 3)
			m_Offset[currentAxis++] = std::stod(currentLine.substr(4, currentLine.length()));
		if (currentType == ROTATION && currentAxis < 3)
			m_Rotation[currentAxis++] = std::stod(currentLine.substr(4, currentLine.length()));
		if (currentType == SCALE && currentAxis < 3)
			m_Scale[currentAxis++] = std::stod(currentLine.substr(4, currentLine.length()));
		if (currentType == OTHER && currentAxis < 3)
		{
			if (currentLine.find("useTexture") != std::string::npos)
				m_bUseTexture = currentLine.substr(currentLine.find_last_of('=') + 2)[0] == '1';
			if (currentLine.find("useCustomColour") != std::string::npos)
				colourIsSet = currentLine.substr(currentLine.find_last_of('=') + 2)[0] == '1';
			if (currentLine.find("drawableName") != std::string::npos)
				drawableName = currentLine.substr(currentLine.find_last_of('=') + 2);
			if (currentLine.find("tool") != std::string::npos)
				thisTool = (ToolType)std::stoi(currentLine.substr(currentLine.find_last_of('=') + 2));
			if (currentLine.find("colour") != std::string::npos)
				m_Colour = Colour(std::stoul(currentLine.substr(currentLine.find('=') + 2)));
		}
		if (currentType == TEXTURE && currentAxis < 3)
		{
			if (currentLine.find("xRes") != std::string::npos)
				xRes = std::stoi(currentLine.substr(currentLine.find_last_of('=') + 2));
			if (currentLine.find("yRes") != std::string::npos)
				yRes = std::stoi(currentLine.substr(currentLine.find_last_of('=') + 2));
			if (currentLine.find("colourCount") != std::string::npos)
			{
				colourCount = std::stoi(currentLine.substr(currentLine.find_last_of('=') + 2));
				if(colourCount < 30)
					m_TexturePtr->adjustColoursAndRes(colourCount, xRes, yRes);
			}
		}
		if (currentLine.find("[offset]") != std::string::npos)
		{
			currentAxis = 0;
			currentType = OFFSET;
		}
		else if (currentLine.find("[rotation]") != std::string::npos)
		{
			currentAxis = 0;
			currentType = ROTATION;
		}
		else if (currentLine.find("[scale]") != std::string::npos)
		{
			currentAxis = 0;
			currentType = SCALE;
		}
		else if (currentLine.find("[other]") != std::string::npos)
		{
			currentAxis = 0;
			currentType = OTHER;
		}
		else if (currentLine.find("[texture]") != std::string::npos)
		{
			currentAxis = 0;
			currentType = TEXTURE;
		}
		else if (currentLine.find("[PropertiesEnd]") != std::string::npos)
			currentType = END;

		if (currentType == END)
		{
			m_Colour.set(colourIsSet);
			break;
		}

		std::getline(inStream, currentLine);
	}
}