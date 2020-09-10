#include "UserMesh.h"
#include <spdlog/spdlog.h>
#ifdef _DEBUG
#include <wx/log.h>
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <assimp/Exporter.hpp>

#include "OpenglExt.h"
#include "PrintBed.h"

#include "SupportTower.h"
#include "BodySplitterTexture.h"
#include "Settings.h"
#include "ObjImporter.h"

#include "Triangle.h"
#include "AABoundingBox.h"
#include "Vertex.h"
#include "Layer.h"
#include "colour.h"
#include "SupportAnalyzer.h"
#include <thread>

#include <vector>

#include "SupportMesh.h"

#define MAX_BUFFER_SIZE 32*1024*1024 //32 mb

UserMesh::bufferChunk::~bufferChunk()
{
	//glDeleteBuffers(1, &this->bufferID);
}

UserMesh::UserMesh() : Mesh(),m_positions(nullptr), m_normals(nullptr), m_colours(nullptr), scene(nullptr), m_UVMap(nullptr)
{
}


UserMesh::~UserMesh()
{
	if (m_colours)
		delete[] m_colours;
	if (m_normals)
		delete[] m_normals;
	if (m_positions)
		delete[] m_positions;
	if (m_bounds)
		delete m_bounds;
	if (m_UVMap)
		delete[] m_UVMap;
#ifndef CONSOLE
	for (auto chunkIT = bufferChunks.begin(); chunkIT != bufferChunks.end(); ++chunkIT)
	{
		glDeleteBuffers(1, &chunkIT->bufferID);
	}
#endif
}

bool UserMesh::loadMesh(const std::string &fileName, bool dontCentre)
{
	{
		this->drawableName = fileName.substr(fileName.find_last_of('\\') + 1);
		auto logger = spdlog::get("DEBUG_FILE");
		logger->info("Using assimp to load file: " + fileName);
		// Create an instance of the Importer class
		try {

			scene = m_AssImporter.ReadFile(fileName,
				aiProcess_Triangulate |
				aiProcess_SortByPType |
				aiProcess_FixInfacingNormals);


		}
		catch (...)
		{
			logger->error("Assimp failed to load file");
			return false;
		}
		// If the import failed, report it
		if (!scene)
		{
#ifndef _CONSOLE
			wxMessageBox("Error loading model", "Error");
#endif
			auto logger = spdlog::get("DEBUG_FILE");
			logger->error("Error in assimp loading mesh!");
			//	DoTheErrorLogging(importer.GetErrorString());
			return false;
		}
		if (scene->HasMaterials())
		{
			if (scene->mMeshes[0]->HasTextureCoords(0))
			{
				thisType = MeshType::UVMap;
				m_bUseTexture = true;
			}
			else
			{
				thisType = MeshType::noColor;
				m_bUseTexture = false;
			}
		}
		else
			m_bUseTexture = false;

		if (thisType == MeshType::UVMap)
		{
			auto logger = spdlog::get("DEBUG_FILE");
			std::string texFileName;
			logger->info("Loading uvMap" + std::string(fileName.substr(0, fileName.find_last_of('\\') + 1)));
#ifdef _WIN32
			if (!loadTexture(fileName.substr(0, fileName.find_last_of('\\') + 1)))	//Get upto and including the "/"
#else
			if (!loadTexture(fileName.substr(0, fileName.find_last_of('/') + 1)))	//Get upto and including the "/"
#endif
				return false;
		}
		if (1) // commented by freelancer latino  OpenglExt::getSingleton().isSupported("GL_ARB_vertex_buffer_object"))
		{
			createArrayData(!dontCentre);
			logger->info("VBO's created");
		}
		else
		{
			logger->error("VBO not supported");
		}
	}
	//Check mesh size for scaling issues...
	double maxDim = std::max(getMaxZ() - getMinZ(), std::max(getMaxX() - getMinX(), getMaxY() - getMinY()));
	// Why dontCentre here? I'll tell you, dontCentre is used when mutliple meshes are loaded, some may be tiny - see benchy multipart
	if (maxDim < 10 && !dontCentre)
	{
#ifndef CONSOLE
		int answer = wxMessageBox("Mesh appears to be very small. Automatically scale the mesh larger?", "Mesh Size", wxYES_NO);
		if (answer == wxYES)
		{
			this->setXScale(2.5);
			this->setYScale(2.5);
			this->setZScale(2.5);
		}
#endif
	}
	return true;
}

bool UserMesh::loadMesh(const std::vector<unsigned char> &geometryBuffer, const std::vector<unsigned char> &textureBuffer)
{
	auto logger = spdlog::get("DEBUG_FILE");
	try {
		scene = m_AssImporter.ReadFileFromMemory(geometryBuffer.data(), geometryBuffer.size(),
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_FixInfacingNormals);
	}
	catch (...)
	{
		logger->error("Assimp failed to load file");
		return false;
	}
	if (!scene)
	{
#ifndef _CONSOLE
		wxMessageBox("Error loading model", "Error");
#endif
		auto logger = spdlog::get("DEBUG_FILE");
		logger->error("Error in assimp loading mesh!");
		//	DoTheErrorLogging(importer.GetErrorString());
		return false;
	}
	if (scene->HasMaterials())
	{
		if (scene->mMeshes[0]->HasTextureCoords(0))
		{
			thisType = MeshType::UVMap;
			m_bUseTexture = true; // will be overriden when reading properties from the manifest
		}
		else
		{
			thisType = MeshType::noColor;
			m_bUseTexture = false;
		}
	}
	else
		m_bUseTexture = false;
	createArrayData(false);
	if (textureBuffer.empty() == false)
	{
		return loadTexture(textureBuffer);
	}
	else
		return true;
}

// MUST be called within a valid gl context
void UserMesh::createArrayData(bool moveOrigin)
{
	vertex_count = 0;
	std::deque<float> positions;
	std::deque<float> uvCoords;
	std::deque<float> normals;
	std::deque<float> colours;
	for (uint meshCounter = 0; meshCounter < scene->mNumMeshes; meshCounter++)
	{
		aiMesh* mesh = scene->mMeshes[meshCounter];
		if (moveOrigin)
			centreAIMesh(mesh);
		for (uint k = 0; k < scene->mMeshes[meshCounter]->mNumFaces; k++)
		{
			aiFace face = mesh->mFaces[k];
			for (int j = 0; j < 3; j++)
			{
				//if face isnt 3 verts we are screwed
				aiVector3D pos = mesh->mVertices[face.mIndices[j]];
				aiVector3D v1 = mesh->mVertices[face.mIndices[1]] - mesh->mVertices[face.mIndices[0]];
				aiVector3D v2 = mesh->mVertices[face.mIndices[2]] - mesh->mVertices[face.mIndices[0]];
				aiVector3D outputNormal = v1^v2;//Cross product
				outputNormal.Normalize();
				aiColor4D colour;

				colour = aiColor4D(1, 1, 1, 1);
				positions.push_back(pos.x);
				positions.push_back(pos.y);
				positions.push_back(pos.z);

				normals.push_back(outputNormal.x);
				normals.push_back(outputNormal.y);
				normals.push_back(outputNormal.z);

				colours.push_back(colour.r);
				colours.push_back(colour.g);
				colours.push_back(colour.b);//skip alpha
				if (thisType == MeshType::UVMap)
				{
					aiVector3D text = mesh->mTextureCoords[0][face.mIndices[j]];
					uvCoords.push_back(text.x);
					uvCoords.push_back(text.y);
					m_verts.push_back(Vertex(pos, text));
				}
				else
					m_verts.push_back(Vertex(pos));
				vertex_count++;
			}
			m_triangles.push_back(Triangle(Triangle::MESH, m_verts[m_verts.size() - 3], m_verts[m_verts.size() - 2], m_verts[m_verts.size() - 1]));
		}
	}

	for (uint matCounter = 0; matCounter < scene->mNumMaterials; matCounter++)
	{
		aiMaterial *mat = scene->mMaterials[matCounter];
		mat;
	}

	if (m_colours)
		delete[] m_colours;
	if (m_normals)
		delete[] m_normals;
	if (m_positions)
		delete[] m_positions;
	if (m_bounds)
		delete m_bounds;
	if (m_UVMap)
		delete[] m_UVMap;
	if (bufferData)
		delete[] bufferData;

	unsigned int bufferSize = 0;
	fullStride = 0;
	unsigned int dataPerVert;
	if (thisType == MeshType::UVMap)
	{
		bufferSize = vertex_count * 3 * 2/*normals and points*/ + vertex_count * 2/*textCoords*/;
		dataPerVert = 3 * 2 * 2;
		fullStride = 3 + 3 + 2;
	}
	else if (thisType == MeshType::VertexColor)
	{
		bufferSize = vertex_count * 3 * 3;//normals points and colours
		dataPerVert = 3 * 3 * 3;
		fullStride = 3 + 3 + 3;
	}
	else
	{
		bufferSize = vertex_count * 3 * 2;//normals and points only
		fullStride = 3 + 3;
		dataPerVert = 3 * 3;
	}
	//bufferSize = vertex_count*3;
	//fullStride = 3;

	unsigned int vertsPerBuffer = MAX_BUFFER_SIZE;// / (fullStride * sizeof(float));
	vertsPerBuffer += vertsPerBuffer % 3; //make sure a triangle doesnt get split
	unsigned int vertsToGo = vertex_count;
	bufferChunks.clear();
	int index = 0;
	auto logger = spdlog::get("DEBUG_FILE");
	logger->info("Transfering data into buffers");
	while (vertsToGo > 0)
	{
		bufferChunks.push_back(bufferChunk(std::min(vertsPerBuffer, vertsToGo), fullStride));
		try {
			bufferChunks.back().bufferData = new GLfloat[bufferChunks.back().vertCount*dataPerVert];
		}
		catch (...) {
			//Cant allocate buffer this size
			//	auto logger = spdlog::get("DEBUG_FILE");
			logger->error("Error allocating a buffer chunk, out of memory!");
			logger->flush();
		}
#ifdef USE_ARB_VBO
		bufferCounts++;

#endif
		vertsToGo -= std::min(vertsPerBuffer, vertsToGo);
		for (unsigned int i = 0; i < bufferChunks.back().vertCount; i++)
		{
			//index = i;
			//Vert_pos
			bufferChunks.back().bufferData[i*fullStride + 0] = positions[index * 3 + 0];
			bufferChunks.back().bufferData[i*fullStride + 1] = positions[index * 3 + 1];
			bufferChunks.back().bufferData[i*fullStride + 2] = positions[index * 3 + 2];
			//Normals

			bufferChunks.back().bufferData[i*fullStride + 3] = normals[index * 3 + 0];
			bufferChunks.back().bufferData[i*fullStride + 4] = normals[index * 3 + 1];
			bufferChunks.back().bufferData[i*fullStride + 5] = normals[index * 3 + 2];

			//Color/texture
			if (thisType == MeshType::UVMap)
			{
				bufferChunks.back().bufferData[i*fullStride + 6] = uvCoords[index * 2 + 0];
				bufferChunks.back().bufferData[i*fullStride + 7] = uvCoords[index * 2 + 1];
			}
			else if (thisType == MeshType::VertexColor)
			{
				bufferChunks.back().bufferData[i*fullStride + 6] = colours[index * 3 + 0];
				bufferChunks.back().bufferData[i*fullStride + 7] = colours[index * 3 + 1];
				bufferChunks.back().bufferData[i*fullStride + 8] = colours[index * 3 + 2];
			}
			index++;
		}

	}
	m_bounds = new AABoundingBox(m_verts);
}

void UserMesh::createVBOS()
{
#ifndef CONSOLE
	for (auto &bufferIT : bufferChunks)
	{
		glGenBuffers(1, &(bufferIT.bufferID));
		glBindBuffer(GL_ARRAY_BUFFER_ARB, bufferIT.bufferID);
		glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(float)*fullStride*vertex_count, bufferIT.bufferData, GL_STATIC_DRAW);
	}vbosReady = true;
#endif
}

void UserMesh::generateArrayData(const std::vector<aiFace> &faces, const std::vector<aiVector3D> &positions, const std::vector<aiColor4D> &colours)
{
	std::vector<float> fpositions;
	std::vector<float> fuvCoords;
	std::vector<float> fnormals;
	std::vector<float> fcolours;
	vertex_count = 0;
	std::vector<aiVector3D> norms;
	calculateNormals(norms, faces, positions);

	int counter = 0;
	for (auto it = faces.begin(); it != faces.end(); ++it)
	{
		//Find colour mode
		aiColor4D col[3];
		aiColor4D faceColor;
		for (int i = 0; i < 3; i++)
		{
			col[i] = colours[it->mIndices[i]];
		}
		if (col[0] == col[1])
			faceColor = col[0];
		else if (col[1] == col[2])
			faceColor = col[1];
		else if (col[2] == col[0])
			faceColor = col[0];
		else
			faceColor = col[0];
		for (int i = 0; i < 3; i++)
		{
			aiVector3D pos;
			//aiColor4D col;

			pos = positions[it->mIndices[i]];
			//col = colours[it->mIndices[i]];
			fpositions.push_back(pos.x);
			fpositions.push_back(pos.y);
			fpositions.push_back(pos.z);

			fcolours.push_back(faceColor.r);
			fcolours.push_back(faceColor.g);
			fcolours.push_back(faceColor.b);

			fnormals.push_back(norms[it->mIndices[i]].x);
			fnormals.push_back(norms[it->mIndices[i]].y);
			fnormals.push_back(norms[it->mIndices[i]].z);

			m_verts.push_back(Vertex(pos));
			vertex_count++;
		}
		m_triangles.push_back(Triangle(Triangle::MESH, m_verts[m_verts.size() - 3], m_verts[m_verts.size() - 2], m_verts[m_verts.size() - 1]));
	}
	if (m_colours)
		delete[] m_colours;
	if (m_normals)
		delete[] m_normals;
	if (m_positions)
		delete[] m_positions;
	if (m_bounds)
		delete m_bounds;
	try {
		m_colours = new float[vertex_count * 3];
		m_positions = new float[vertex_count * 3];
		m_normals = new float[vertex_count * 3];
	}
	catch (...)
	{
		auto logger = spdlog::get("DEBUG_FILE");
		logger->error("Error allocating memory for vbo's!");
		return;
	}
	std::copy(fnormals.begin(), fnormals.end(), m_normals);
	std::copy(fpositions.begin(), fpositions.end(), m_positions);
	std::copy(fcolours.begin(), fcolours.end(), m_colours);
}

void UserMesh::startSlice(wxFrame *baseFrame)
{
	if (m_ptrSupportMesh)
	{
		if (m_ptrSupportMesh->isSupportCut() == false)
		{
			m_ptrSupportMesh->cutOutMesh(this);
		}
	}
	Mesh::startSlice(baseFrame);
}

void UserMesh::draw(unsigned int opts, const Colour *colourToDraw) noexcept
{
#ifndef CONSOLE
	if (!vbosReady)
		createVBOS();
	if (fullStride == 0)//Only true if the buffers havent been constructed
		return;
	if (opts & DrawingOptions::OBJECT_PICK)
	{
		glDisable(GL_LIGHTING);
	}
	else
		glEnable(GL_LIGHTING);
	if (thisType == MeshType::VertexColor)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if (opts == DrawingOptions::NONE)
			glEnableClientState(GL_COLOR_ARRAY);
	}
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 40);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if (thisType == MeshType::UVMap && m_bUseTexture && (opts == DrawingOptions::NONE) && thisTool == ToolType::FULL_COLOUR)
	{
		//glDisable(GL_BLEND);
		glColor3f(1, 1, 1);
		glEnable(GL_TEXTURE_2D);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		assert(m_TexturePtr);
		m_TexturePtr->bindTexture(m_TexturePtr->isSimplified() && m_simplifiedTexture); // only use simplified texture if it exists and is selectged
	}
	else
		glDisable(GL_TEXTURE_2D);
	glPushMatrix();

	glTranslated(m_Offset[X_AXIS], m_Offset[Y_AXIS], 0);
	/*
	if (!supportTowers.empty() && !(opts & DrawingOptions::OBJECT_PICK))
	{
	for (auto &it : supportTowers)
	{
	it->draw(0,nullptr);
	}
	}
	*/
	//if (m_ptrSupportMesh && !(opts & DrawingOptions::OBJECT_PICK))
	//{
	//	m_ptrSupportMesh->draw(opts, nullptr);
	//}
	glPopMatrix();
	glPushMatrix();
	glTranslated(m_Offset[X_AXIS], m_Offset[Y_AXIS], m_Offset[Z_AXIS]);
	glRotated((m_Rotation[Z_AXIS]), 0, 0, 1);
	glRotated((m_Rotation[Y_AXIS]), 0, 1, 0);
	glRotated((m_Rotation[X_AXIS]), 1, 0, 0);
	glScaled(m_Scale[X_AXIS], m_Scale[Y_AXIS], m_Scale[Z_AXIS]);

	for (auto chunkIT = bufferChunks.begin(); chunkIT != bufferChunks.end(); ++chunkIT)
	{
		glBindBuffer(GL_ARRAY_BUFFER_ARB, chunkIT->bufferID);
		if (thisType == MeshType::VertexColor)
		{
			if (opts == DrawingOptions::NONE)
				glColorPointer(3, GL_FLOAT, fullStride * sizeof(GLfloat), (GLfloat*)(sizeof(GLfloat) * 6));
		}
		glVertexPointer(3, GL_FLOAT, fullStride * sizeof(GLfloat), 0);
		glNormalPointer(GL_FLOAT, fullStride * sizeof(GLfloat), (GLfloat*)(sizeof(GLfloat) * 3));

		if (thisType == MeshType::UVMap && m_bUseTexture && (opts == DrawingOptions::NONE) && thisTool == ToolType::FULL_COLOUR)
			glTexCoordPointer(2, GL_FLOAT, fullStride * sizeof(GLfloat), (GLfloat*)(sizeof(GLfloat) * 6));

		if (opts & DrawingOptions::OBJECT_PICK)
		{
			if (colourToDraw)
			{
				glColor3ub(colourToDraw->getR<uint8_t>(), colourToDraw->getG<uint8_t>(), colourToDraw->getB<uint8_t>());
			}
			else
				glColor3ub(255, 255, 255);
		}
		else if (!m_bUseTexture && m_Colour.isSet() && thisTool == ToolType::FULL_COLOUR)
			glColor3ub(m_Colour.getR<uint8_t>(), m_Colour.getG<uint8_t>(), m_Colour.getB<uint8_t>());
		else if (thisTool == ToolType::FULL_COLOUR) // defualt to white if the full colour has no specified colour
			glColor3f(1.0f, 1.0f, 1.0f);
		else if (thisTool == ToolType::FLEXIBLE)
			glColor3ub(GET_COLOUR("t1Colour").getR<uint8_t>(), GET_COLOUR("t1Colour").getG<uint8_t>(), GET_COLOUR("t1Colour").getB<uint8_t>());
		else if (thisTool == ToolType::SUPPORT)
			glColor3ub(GET_COLOUR("t2Colour").getR<uint8_t>(), GET_COLOUR("t2Colour").getG<uint8_t>(), GET_COLOUR("t2Colour").getB<uint8_t>());
		else if (thisTool == ToolType::GENERIC1)
			glColor3ub(GET_COLOUR("t3Colour").getR<uint8_t>(), GET_COLOUR("t3Colour").getG<uint8_t>(), GET_COLOUR("t3Colour").getB<uint8_t>());
		else if (thisTool == ToolType::GENERIC2)
			glColor3ub(GET_COLOUR("t4Colour").getR<uint8_t>(), GET_COLOUR("t4Colour").getG<uint8_t>(), GET_COLOUR("t4Colour").getB<uint8_t>());
		else if (thisTool == ToolType::GENERIC3)
			glColor3ub(GET_COLOUR("t5Colour").getR<uint8_t>(), GET_COLOUR("t5Colour").getG<uint8_t>(), GET_COLOUR("t5Colour").getB<uint8_t>());
		else
			glColor3f(0.5, 0.5, 0.5);
		if (m_hideDrawing == false)
			glDrawArrays(GL_TRIANGLES, 0, chunkIT->vertCount);

	}
	glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	glPopMatrix();
	if (thisType == MeshType::UVMap && m_bUseTexture && (opts == DrawingOptions::NONE) && thisTool == ToolType::FULL_COLOUR)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if ((thisType == MeshType::VertexColor) && (opts == DrawingOptions::NONE))
		glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_LIGHTING);
#endif
}

void UserMesh::calculateNormals(std::vector<aiVector3D> &output, const std::vector<aiFace> &faces, const std::vector<aiVector3D> &positions)
{
	//Normals only used for drawing
	output.resize(positions.size());
	for (auto it = faces.begin(); it != faces.end(); ++it)
	{
		aiVector3D v1 = positions[it->mIndices[1]] - positions[it->mIndices[0]];
		aiVector3D v2 = positions[it->mIndices[2]] - positions[it->mIndices[0]];
		aiVector3D outputNormal = v1^v2;//Cross product
		outputNormal.Normalize();
		output[it->mIndices[0]] = outputNormal;
		output[it->mIndices[1]] = outputNormal;
		output[it->mIndices[2]] = outputNormal;
	}
}

void UserMesh::centreAIMesh(aiMesh* meshptr)
{
	// Need minX, maxX, minY, maxY...z?
	double minX = std::numeric_limits<double>::max();
	double minY = std::numeric_limits<double>::max();
	//double minZ = std::numeric_limits<double>::max();
	double maxX = -std::numeric_limits<double>::max();
	double maxY = -std::numeric_limits<double>::max();
	//double maxZ = -std::numeric_limits<double>::max();
	for (unsigned int i = 0; i < meshptr->mNumVertices; i++)
	{
		if (meshptr->mVertices[i].x < minX)
			minX = meshptr->mVertices[i].x;
		if (meshptr->mVertices[i].y < minY)
			minY = meshptr->mVertices[i].y;
		if (meshptr->mVertices[i].x > maxX)
			maxX = meshptr->mVertices[i].x;
		if (meshptr->mVertices[i].y > maxY)
			maxY = meshptr->mVertices[i].y;
	}
	double xShift = (maxX + minX) / 2.0;
	double yShift = (maxY + minY) / 2.0;
	for (unsigned int i = 0; i < meshptr->mNumVertices; i++)
	{
		meshptr->mVertices[i].x -= xShift;
		meshptr->mVertices[i].y -= yShift;
	}
}

void UserMesh::scaleMesh(double x, double y, double z)
{
	for (auto it = bufferChunks.begin(); it != bufferChunks.end(); ++it)
	{
		for (int i = 0; i < it->vertCount; i++)
		{
			it->bufferData[i*fullStride + 0] *= x;
			it->bufferData[i*fullStride + 1] *= y;
			it->bufferData[i*fullStride + 2] *= z;
		}
	}
	Mesh::scaleMesh(x, y, z);
}

void UserMesh::convertSupportTowersToPolys(unsigned int layerID)
{
	//Each layer...
	RoVaClipperLib::Clipper rClip;
	for (auto it : supportTowers)
	{
		if (it->getTop() >= m_LayerArray[layerID]->getLayerZ() && it->getBottom() <= m_LayerArray[layerID]->getLayerZ())
		{
			rClip.AddPath(it->getPerimetersAsPath(), RoVaClipperLib::ptSubject, true);
		}
	}
	rClip.Execute(RoVaClipperLib::ctUnion, m_LayerArray[layerID]->getSupportPoly(), RoVaClipperLib::pftNonZero);
}

bool UserMesh::loadTexture(const std::vector<unsigned char> &textureBuffer)
{
	m_TexturePtr = std::make_shared<BodySplitterTexture>();
	return m_TexturePtr->openImage(textureBuffer);
}

bool UserMesh::loadTexture(const std::string &filePath)
{
	int texIndex = 0;
	aiString path;
	scene->mMaterials[scene->mMeshes[0]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
	std::string fullPath(filePath);
	fullPath += path.data;
	if (m_TexturePtr)
		m_TexturePtr.reset();
	m_TexturePtr = std::make_shared<BodySplitterTexture>();
	if (m_TexturePtr->openImage(fullPath) == false)
	{
		// Holy tight coupling batman
#ifndef CONSOLE
		int answer = wxMessageBox("Failed to open. Manually specify texture file?", "Error Loading Texture", wxYES_NO | wxCANCEL);
#else
		int answer = wxNO;
#endif
		//Option to manually load texture
		if (answer == wxCANCEL)
		{
			return false;
		}
		if (answer == wxNO)
		{
			this->thisType = MeshType::noColor;
			m_TexturePtr.reset();
			return true;
		}
		if (answer == wxYES)
		{
			//open file dialog
			wxFileDialog
				openFileDialog(wxGetActiveWindow(), _("Open image file"), "", "",
					"PNG and jpeg files (*.png;*.jpg;*.jpeg;*.bmp;*.tif)|*.png;*.jpg;*.jpeg;*.bmp;*.tif", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
			if (openFileDialog.ShowModal() == wxID_CANCEL)
			{
				//Cancel loading the mesh
				return false;
			}
			else
			{
				if (m_TexturePtr->openImage(openFileDialog.GetPath().ToStdString()) == false)
				{
					m_TexturePtr.reset();
					return false;
				}

				return true;
			}
		}
	}

	return true;
}

std::vector<unsigned char> UserMesh::exportMesh(const std::string &format) const
{
	Assimp::Exporter exporter;
	const char* exportFormatID = nullptr;
	for (int i = 0; i < exporter.GetExportFormatCount(); i++)
	{
		if (format == std::string(exporter.GetExportFormatDescription(i)->fileExtension))
		{
			exportFormatID = exporter.GetExportFormatDescription(i)->id;
			break;
		}
	}
	if (exportFormatID == nullptr)
		return std::vector<unsigned char>();

	const aiExportDataBlob* blobData = exporter.ExportToBlob(scene, exportFormatID);
	unsigned char *buff = (unsigned char*)blobData->data;
	std::vector<unsigned char> output(buff, buff + blobData->size);
	return output;
}

void UserMesh::addSupport(std::shared_ptr<SupportTower> support)
{
	support->addToMesh(this);
	supportTowers.push_back(support);
	if (!m_ptrSupportMesh)
	{
		m_ptrSupportMesh = std::make_shared<SupportMesh>();
		m_ptrSupportMesh->setOFfsetX(this->m_Offset[X_AXIS]);
		m_ptrSupportMesh->setOFfsetY(this->m_Offset[X_AXIS]);
		PrintBed::getSingleton().addMeshToBed(m_ptrSupportMesh);
	}
	m_ptrSupportMesh->addSupportTower(support);
}

void UserMesh::removeSupport(std::shared_ptr<SupportTower> tower)
{
	if (m_ptrSupportMesh)
		m_ptrSupportMesh->removeSupport(tower);
	auto towerIT = std::find(supportTowers.begin(), supportTowers.end(), tower);
	if (towerIT != supportTowers.end())
		supportTowers.erase(towerIT);
}

void UserMesh::clearSupport() {
	supportTowers.clear();
	m_ptrSupportMesh.reset();
}

void UserMesh::cutOutSupport()
{
	if (m_ptrSupportMesh)
		m_ptrSupportMesh->cutOutMesh(this);
}

void UserMesh::generateSupport()
{
	m_ptrSupportMesh.reset();
	PrintBed::GetSingleton().removeObject(m_ptrSupportMesh);
	m_ptrSupportMesh = std::make_shared<SupportMesh>();
	m_ptrSupportMesh->setOFfsetX(this->m_Offset[X_AXIS]);
	m_ptrSupportMesh->setOFfsetY(this->m_Offset[X_AXIS]);
	supportTowers.clear();
	SupportAnalyzer supportAnal(this);
	supportTowers = supportAnal.getSupportTower();
	for (auto it : supportTowers)
		m_ptrSupportMesh->addSupportTower(it);
	PrintBed::getSingleton().addMeshToBed(m_ptrSupportMesh);
}

bool UserMesh::hasSupport() const
{
	return !supportTowers.empty();
}

std::string UserMesh::getAssimpTexturePath() const
{
	aiString path;
	scene->mMaterials[scene->mMeshes[0]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
	return path.C_Str();
}

bool UserMesh::isValidExtension(const std::string &path)
{
	const std::vector<std::string> valids{
		"obj",
		"stl",
		"dae"
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

void UserMesh::setOFfsetX(double newPos)
{
	m_Offset[X_AXIS] = newPos;
	if (m_ptrSupportMesh)
		m_ptrSupportMesh->setOFfsetX(newPos);
}
void UserMesh::setOFfsetY(double newPos)
{
	m_Offset[Y_AXIS] = newPos;
	if (m_ptrSupportMesh)
		m_ptrSupportMesh->setOFfsetY(newPos);
}
void UserMesh::setOFfsetZ(double newPos)
{
	m_Offset[Z_AXIS] = newPos;
	if (m_ptrSupportMesh)
		m_ptrSupportMesh->setOFfsetZ(newPos);
}