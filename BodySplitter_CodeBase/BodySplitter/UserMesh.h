#pragma once
#include "Mesh.h"
class UserMesh :
	public Mesh
{
private:
	friend class SupportAnalyzer;
	bool vbosReady = false;
	

	Assimp::Importer m_AssImporter;
	/* This is destroyed when the ai importer is destroyed...*/
	const aiScene* scene = nullptr;
	bool isSliced = false;
	uint64_t vertex_count;
	//For opengl
	GLuint m_vertBuffer;
	unsigned int fullStride = 0;
	struct bufferChunk {
#ifdef USE_ARB_VBO
		GLuint bufferID = 0;
#endif
		GLfloat *bufferData = nullptr;
		unsigned int vertCount = 0;
		bufferChunk(unsigned int verts, unsigned int stride) : vertCount(verts)
		{
			//bufferData = new GLfloat[verts*stride];
		}
#ifdef USE_ARB_VBO
		~bufferChunk();
#endif
	};
	std::vector<std::shared_ptr<SupportTower>> supportTowers;
	std::list<UserMesh::bufferChunk> bufferChunks;
	unsigned int bufferCounts = 0;
	GLfloat *bufferData = nullptr;
	float *m_positions;
	float *m_normals;
	float *m_colours;
	float *m_UVMap;

	std::shared_ptr<SupportMesh> m_ptrSupportMesh;

	void centreAIMesh(aiMesh* meshptr);

	bool loadTexture(const std::string &filePath);
	bool loadTexture(const std::vector<unsigned char> &textureBuffer);

	void generateArrayData(const std::vector<aiFace> &faces, const std::vector<aiVector3D> &positions, const std::vector<aiColor4D> &colours);// , const std::vector<aiVector2D> &textCoords);
	void calculateNormals(std::vector<aiVector3D> &output, const std::vector<aiFace> &faces, const std::vector<aiVector3D> &positions);
	void convertSupportTowersToPolys(unsigned int layerID);

	void createVBOS();
public:
	UserMesh();
	virtual ~UserMesh();
	virtual void startSlice(wxFrame *baseFrame);
	std::vector<std::shared_ptr<SupportTower>> &getSupportTowers() {
		return supportTowers;
	}
	std::shared_ptr<SupportMesh> getSupportMesh() const {
		return m_ptrSupportMesh;
	}
	virtual void scaleMesh(double x, double y, double z);
	void draw(unsigned int opts, const Colour *colourToDraw) noexcept;
	void createArrayData(bool moveOrigin);
	void addSupport(std::shared_ptr<SupportTower> support);
	void removeSupport(std::shared_ptr<SupportTower> tower);
	void cutOutSupport();

	// dontCentre - Used to prevent reconfiguring the model origin- usefull for multipart models
	bool loadMesh(const std::string &fileName, bool dontCentre = false);
	// Load mesh from memory like from a zip file
	bool loadMesh(const std::vector<unsigned char> &geometryBuffer, const std::vector<unsigned char> &textureBuffer);

	static bool isValidExtension(const std::string &filePath);

	void generateSupport();
	//void generateSupportInterfaceLayers() override;
	bool hasSupport() const override;
	void clearSupport();

	//RoVaClipperLib::Paths getEnclosingPerimeters() const override;
	//void generateSkirt() override;

	// The binary blob is destroyed when exporter goes out of scope, we need to copy it, might as well do it the C++ way
	std::vector<unsigned char> exportMesh(const std::string &format) const;
	std::string getAssimpTexturePath() const;
	// void loadPropertiesFromStream(std::ifstream &inStream); commented by freelancerlatino

	virtual void setOFfsetX(double newPos) override;
	virtual void setOFfsetY(double newPos) override;
	virtual void setOFfsetZ(double newPos) override;
};

