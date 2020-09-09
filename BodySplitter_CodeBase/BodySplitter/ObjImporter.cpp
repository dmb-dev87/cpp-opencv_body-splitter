#include "ObjImporter.h"

#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/log.h>
#include <string>
#include <assimp/mesh.h>
#include <vector>





ObjImporter::ObjImporter()
{
}


ObjImporter::~ObjImporter()
{
}

bool ObjImporter::loadFile(const std::string &fileName)
{


	wxFileInputStream inFile(fileName);
	wxTextInputStream text(inFile, wxT("\x09"), wxConvUTF8);
	if (!inFile.IsOk())
	{
		wxLogDebug("File opening error");
		return false;
	}
	int vertCount = 0;
	while (inFile.IsOk() && !inFile.Eof())
	{
		wxString currentLine;
		currentLine = text.ReadLine();
		//wxLogDebug(currentLine);
		if (currentLine.IsEmpty())
		{

		}
		else if (currentLine[0] == 'v')
		{
			//Read positions
			if (currentLine[1] == 'n')
			{
				//Normal
				wxStringTokenizer tokens(currentLine.SubString(2, currentLine.Length()));
				double data[3];//XYZ RGB
				int i = 0;
				while (tokens.HasMoreTokens())
				{
					tokens.GetNextToken().ToDouble(&data[i++]);
				}
				normals.push_back(aiVector3D(data[0], data[1], data[2]));
			}
			else
			{
				wxStringTokenizer tokens(currentLine.SubString(1,currentLine.Length()));
				if (tokens.CountTokens() < 4)
				{
					wxLogDebug("Token count is wrong");
					//Obj contains no vertex colours
					return false;
				}
				else
				{
					vertCount++;
					double data[6];//XYZ RGB
					int i = 0;
					while (tokens.HasMoreTokens())
					{
						tokens.GetNextToken().ToDouble(&data[i++]);
					}
					positions.push_back(aiVector3D(data[0], data[1], data[2]));
					colour.push_back(aiColor4D(data[3], data[4], data[5], 1.0f));
					//char debugMsg[256];
					//sprintf(debugMsg, "Colour: %2.2f,%2.2f,%2.2f Position:%2.2f,%2.2f,%2.2f");
					//wxLogDebug(debugMsg);
				}
			}
		}
		else if (currentLine[0] == '#')
		{
			//Comment
		}
		else if (currentLine[0] == 'f')
		{
			//Faces
			//Assume triangles
			long vertID[3];
			long normalID[3];
			int i = 0;
			bool lastIsVert = false;
			wxStringTokenizer tokens(currentLine.SubString(2, currentLine.Length()),"//: ");
			while (tokens.HasMoreTokens())
			{
				wxString current = tokens.GetNextToken();
				if (current.empty())
					continue;

				if (!lastIsVert)
					current.ToLong(&vertID[i]);
				else
				{
					current.ToLong(&normalID[i]);
					i++;
				}
				lastIsVert = !lastIsVert;
			}
			aiFace newFace;
			newFace.mNumIndices = 3;
			newFace.mIndices = new unsigned int[3];
			newFace.mIndices[0] = vertID[0]-1; newFace.mIndices[1] = vertID[1]-1; newFace.mIndices[2] = vertID[2]-1;
			faces.push_back(newFace);
		}
	}
	return true;
}