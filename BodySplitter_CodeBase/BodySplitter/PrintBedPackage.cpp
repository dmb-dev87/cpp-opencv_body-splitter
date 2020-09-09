#include "PrintBedPackage.h"
#include "Settings.h"
#include "minizip\zip.h"
#include "minizip\unzip.h"

#include "iDrawable.h"
#include "UserMesh.h"
#include "CoasterPrint.h"
#include "BodySplitterTexture.h"


/* Exporter*/

PrintBedPackageExporter::PrintBedPackageExporter(const std::string &filePath, std::list<std::shared_ptr<iDrawable>> &drawables) : 
	m_fileName(filePath), m_ptrDrawables(&drawables)
{
}


PrintBedPackageExporter::~PrintBedPackageExporter()
{
}

bool PrintBedPackageExporter::writeManifest()
{
	if (m_ptrZipFile == nullptr)
		return false;

	if (zipOpenNewFileInZip(m_ptrZipFile, "manifest.rs", NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_BEST_COMPRESSION) != 0)
		return false;
	if (zipWriteInFileInZip(m_ptrZipFile, outputManifest.str().c_str(), outputManifest.str().size()) != 0)
		return false;
	if (zipCloseFileInZip(m_ptrZipFile) != 0)
		return false;
	return true;
}

bool PrintBedPackageExporter::writeFileToZip(const std::string &fileName, const std::vector<unsigned char> &buff, const char *comment)
{
	if (m_ptrZipFile == nullptr)
		return false;
	if (zipOpenNewFileInZip(m_ptrZipFile, fileName.c_str(), NULL, NULL, 0, NULL, 0, comment, Z_DEFLATED, Z_BEST_COMPRESSION) != 0)
		return false;
	if (zipWriteInFileInZip(m_ptrZipFile, buff.data(), buff.size()) != 0)
		return false;
	if (zipCloseFileInZip(m_ptrZipFile) != 0)
		return false;
	return true;
}

/*Writes to a zip file the print bed contents, file must not exist*/
bool PrintBedPackageExporter::exportPrintBed()
{
	// Dont append, file should be check if it exists and dealt with by the caller
	m_ptrZipFile = zipOpen(m_fileName.c_str(), 0);
	if (m_ptrZipFile == NULL)
		return false;
	//if (zipOpenNewFileInZip(m_ptrZipFile, "manifest.rs", NULL, NULL, 0, NULL, 0, "TestZip", 0, Z_DEFAULT_COMPRESSION) != 0)
	//	return false;
	//char buffer[] = "THISISATEST";
	//if (zipWriteInFileInZip(m_ptrZipFile, buffer, sizeof(buffer)) != 0)
	//	return false;
	//if (zipCloseFileInZip(m_ptrZipFile) != 0)
	//	return false;
	if (saveSettings(m_ptrZipFile) == false)
		return false;
	if (saveMeshes(m_ptrZipFile) == false)
		return false;
	if (writeManifest() == false)
		return false;
	if (zipClose(m_ptrZipFile, "BodySplitter") != 0)
		return false;
	return true;
}

bool PrintBedPackageExporter::saveSettings(voidp outFile)
{
	if (zipOpenNewFileInZip(outFile, "settings.ini", NULL, NULL, 0, NULL, 0, "RS Settings", Z_DEFLATED, Z_BEST_COMPRESSION) != 0)
		return false;
	std::ostringstream ss;
	if (Settings::GetSingleton().saveSettingsTo(ss) == false)
		return false;
	if (zipWriteInFileInZip(outFile, ss.str().c_str(), ss.str().length()) != 0)
		return false;
	if (zipCloseFileInZip(outFile) != 0)
		return false;
	return true;
}

bool PrintBedPackageExporter::saveMeshes(voidp outFile)
{
	std::unordered_map<std::string, int> fileNameCounter;	//Ensure all file names in the archive are unique
	outputManifest << "meshCount = " << m_ptrDrawables->size() << "\n";
	for (auto it : *m_ptrDrawables)
	{
		auto mesh_ptr = std::dynamic_pointer_cast<UserMesh>(it);
		auto coaster_ptr = std::dynamic_pointer_cast<CoasterPrint>(it);
		// Currently not supporting coasters... fix
		if (mesh_ptr)
		{
			std::string fileName = mesh_ptr->getName().substr(0, mesh_ptr->getName().length() - 4);
			if (fileNameCounter.count(fileName) > 0)
			{
				fileNameCounter[fileName]++;
				fileName += std::to_string(fileNameCounter[fileName]);
			}
			else
				fileNameCounter[fileName] = 0;
			std::string textureName(fileName + ".png");
			std::string supportName(fileName + ".sup");
			fileName += ".dae";
			//std::string fileName = mesh_ptr->getName().substr(0, mesh_ptr->getName().length() - 4) + ".dae";
			std::vector<unsigned char> data = mesh_ptr->exportMesh("dae");
			if (mesh_ptr->hasTexture())
			{
				//std::string textureName = mesh_ptr->getAssimpTexturePath();
				//if (textureName.empty())
				//	textureName = mesh_ptr->getName().substr(0, mesh_ptr->getName().length() - 4) + "tex.png";
				if (saveTexture(mesh_ptr->getBodySplitterTexture(), textureName) == false)
					return false;
			}
			if (writeFileToZip(fileName, data, "RSPolygon") == false)
				return false;
			outputManifest << "[mesh]\n";
			outputManifest << "fileName = " << fileName << "\n";
			if (mesh_ptr->hasTexture())
			{
				//std::string textureName = mesh_ptr->getAssimpTexturePath();
				//if (textureName.empty())
				//	textureName = mesh_ptr->getName().substr(0, mesh_ptr->getName().length() - 4) + "tex.png";
				outputManifest << "textureName = " << textureName << "\n";
			}
			if (mesh_ptr->hasSupport())
			{
				outputManifest << "supportName = " << supportName << "\n";

			}
		}
		else if (coaster_ptr)
		{
			if (saveTexture(coaster_ptr->getBodySplitterTexture(), coaster_ptr->getName()) == false)
				return false;
			outputManifest << "[coaster]\n";
			outputManifest << "fileName = " << coaster_ptr->getName() << "\n";
		}
		it->savePropertiesToStream(outputManifest);
	}
	if (Settings::GetSingleton().hasColourSubs())
	{
		outputManifest << "[colourSub]\n";
		Settings::getSingleton().saveColourSubMap(outputManifest);
	}
	return true;
}

bool PrintBedPackageExporter::saveTexture(std::shared_ptr<BodySplitterTexture> ptrTexture, const std::string &outputFileName)
{
	std::vector<unsigned char> data = ptrTexture->exportImage(".png");
	if (m_savedTextureNames.count(outputFileName.substr(0, outputFileName.length() - 3)))
	{
		//Do we rename this? rename the mesh? assume they're identical?
		return true; // assume they're identical
	}
	else
	{
		m_savedTextureNames.insert(outputFileName.substr(0, outputFileName.length() - 3));
	}
	return writeFileToZip(outputFileName.substr(0, outputFileName.length() - 3) + "png", data);
}



/* ************************************************************************** 
//                           Importer                                       //
*************************************************************************** */

PrintBedPackageImporter::PrintBedPackageImporter(const std::string &inputFile)
{
	//Read the manifest file
	m_ptrZipFile = unzOpen(inputFile.c_str());
}

PrintBedPackageImporter::~PrintBedPackageImporter()
{

}

bool PrintBedPackageImporter::readFile(const std::string &fileName, std::vector<unsigned char> &outBuffer)
{
	if (unzLocateFile(m_ptrZipFile, fileName.c_str(), 1) != UNZ_OK)
		return false;

	unz_file_info file_info;
	char file_name_in_zip[256];
	if (UNZ_OK != unzGetCurrentFileInfo(m_ptrZipFile, &file_info, file_name_in_zip, sizeof(file_name_in_zip), NULL, 0, NULL, 0))
		return false;
	if (UNZ_OK != unzOpenCurrentFile(m_ptrZipFile))
		return false;
	// read zip entry content
	outBuffer.resize(file_info.uncompressed_size + 1);
	
	int total_bytes_read = 0;
	while (true) {
		int bytes_read = unzReadCurrentFile(m_ptrZipFile, &outBuffer[total_bytes_read], outBuffer.size() - total_bytes_read);
		if (bytes_read < 0)
			return false;
		if (bytes_read == 0)
			break;
		total_bytes_read += bytes_read;
	}
	unzCloseCurrentFile(m_ptrZipFile);
	return true;
}

bool PrintBedPackageImporter::ReadZip(bool loadSettings)
{
	if (readManifest() == false)
		return false;
	if (loadSettings && readSettings() == false)
		return false;
	unzClose(m_ptrZipFile);

	return true;
}

bool PrintBedPackageImporter::readManifest()
{
	if (m_ptrZipFile == NULL)
		return false;
	int manifestFileFound = unzLocateFile(m_ptrZipFile, "manifest.rs", 0);
	if (manifestFileFound != UNZ_OK)
		return false;
	unz_file_info file_info;
	char file_name_in_zip[256];
	if (UNZ_OK != unzGetCurrentFileInfo(m_ptrZipFile, &file_info, file_name_in_zip, sizeof(file_name_in_zip), NULL, 0, NULL, 0))
		return false;
	if (UNZ_OK != unzOpenCurrentFile(m_ptrZipFile))
		return false;
	// read zip entry content
	std::vector< char > buffer(file_info.uncompressed_size + 1);
	int total_bytes_read = 0;
	while (true) {
		int bytes_read = unzReadCurrentFile(m_ptrZipFile, &buffer[total_bytes_read], buffer.size() - total_bytes_read);
		if (bytes_read < 0)
			return false;
		if (bytes_read == 0)
			break;
		total_bytes_read += bytes_read;
	}
	unzCloseCurrentFile(m_ptrZipFile);
	std::istringstream ss(std::string(buffer.data(), buffer.size()));

	std::string currentLine;
	std::getline(ss, currentLine);
	if (currentLine.find("meshCount =") == std::string::npos)
		return false;
	int bedMeshCount = std::stoi(currentLine.substr(currentLine.find('=') + 1));
	for (int i = 0; i < bedMeshCount; i++)
	{
		std::shared_ptr<iDrawable> activeDrawable;
		std::getline(ss, currentLine);
		if (currentLine.find("[mesh") != std::string::npos)
		{
			std::shared_ptr<UserMesh> mesh_ptr = std::make_shared<UserMesh>();
			std::getline(ss, currentLine);
			std::string meshFileName;
			if (currentLine.find("fileName =") == std::string::npos)
				return false;
			meshFileName = currentLine.substr(currentLine.find('=') + 2);
			std::string textureName;
			if (ss.peek() == 't')
			{
				std::getline(ss, currentLine);
				if (currentLine.find("textureName =") == std::string::npos)
					return false;
				textureName = currentLine.substr(currentLine.find('=') + 2);
			}
			if (meshFileName.empty())
				return false;
			std::vector<unsigned char> geomBuffer, textureBuffer;
			if (readFile(meshFileName, geomBuffer) == false)
				return false;
			if (textureName.empty() == false)
			{
				readFile(textureName, textureBuffer);
			}
			mesh_ptr->loadMesh(geomBuffer, textureBuffer);
			activeDrawable = mesh_ptr;
		}
		else if (currentLine.find("[coaster]") != std::string::npos)
		{
			std::shared_ptr<CoasterPrint> coaster_ptr = std::make_shared<CoasterPrint>();
			std::vector<unsigned char> textBuffer;
			std::string fileName;
			std::getline(ss, currentLine);
			if (currentLine.find("fileName =") == std::string::npos)
				return false;
			fileName = currentLine.substr(currentLine.find('=') + 2);
			fileName = fileName.substr(0, fileName.length() - 3) + "png";
			if (readFile(fileName, textBuffer) == false)
				return false;
			if (coaster_ptr->loadImage(textBuffer, fileName) == false)
				return false;
			activeDrawable = coaster_ptr;
		}
		activeDrawable->readPropertiesFromStream(ss);
		objsOnBed.push_back(activeDrawable);
	}
	std::getline(ss, currentLine);
	if (currentLine.find("[colourSub]") != std::string::npos)
	{
		Settings::GetSingleton().loadColourSubMap(ss);
	}
	return true;
}

bool PrintBedPackageImporter::readSettings()
{
	std::vector<unsigned char> buff;
	if (readFile("settings.ini", buff) == false)
		return false;
	std::istringstream ss(std::string((char*)buff.data(), buff.size()));
	return Settings::getSingleton().loadNewSettings(ss);
}