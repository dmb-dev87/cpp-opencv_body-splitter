#pragma once

/* Class to handle importing and exporting the bed contents*/

#include <string>
#include <vector>
#include <memory>
#include <list>
#include <unordered_set>
#include <sstream>

class BodySplitterTexture;
class iDrawable;

class PrintBedPackageExporter
{
private:
	std::stringstream outputManifest;
	std::list<std::shared_ptr<iDrawable>> *m_ptrDrawables;
	std::unordered_set<std::string> m_savedMeshNames;
	std::unordered_set<std::string> m_savedTextureNames;
	std::string m_fileName;
	void* m_ptrZipFile = nullptr;
	
	bool writeFileToZip(const std::string &fileName, const std::vector<unsigned char> &buffer, const char *comment = NULL);

	bool saveTexture(std::shared_ptr<BodySplitterTexture> ptrTexture, const std::string &outputFileName);
	bool saveSettings(void* zipFile);
	bool writeManifest();
	bool saveMeshes(void* zipFile);
	bool saveSupports(void* zipFile);
public:
	bool exportPrintBed();
	//static bool importPrintBed(const std::string &filePath);

	PrintBedPackageExporter(const std::string &filePath, std::list<std::shared_ptr<iDrawable>> &drawables);
	~PrintBedPackageExporter();
};

class PrintBedPackageImporter
{
private:
	void* m_ptrZipFile = nullptr;
	std::istringstream manifestFile;
	std::list<std::shared_ptr<iDrawable>> objsOnBed;
	bool readManifest();
	bool readSettings();
	bool readFile(const std::string &fileName, std::vector<unsigned char> &buffer);
public:
	PrintBedPackageImporter(const std::string &filePath);
	~PrintBedPackageImporter();
	bool ReadZip(bool readSettings);
	std::list<std::shared_ptr<iDrawable>> &getDrawables() {
		return objsOnBed;
	}
	bool isGood() const {
		return m_ptrZipFile != nullptr;
	}
};