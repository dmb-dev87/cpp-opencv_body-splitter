#include "SupportMesh.h"

#include "Layer.h"
#include "SupportTower.h"
#include "Mesh.h"
#include "Settings.h"

//#define CSGJS_HEADER_ONLY
#include "CSGJS.h"

SupportMesh::SupportMesh()
{
	this->thisTool = ToolType::SUPPORT;
}


SupportMesh::~SupportMesh()
{
}

void SupportMesh::addSupportTower(std::shared_ptr<SupportTower> sTower)
{
	m_supportTowers.push_back(sTower);
}

void SupportMesh::removeSupport(std::shared_ptr<SupportTower> sTower)
{
	std::vector<std::shared_ptr<SupportTower>>::iterator it = std::find(m_supportTowers.begin(), m_supportTowers.end(), sTower);
	if (it != m_supportTowers.end())
		m_supportTowers.erase(it);
}

bool SupportMesh::isSupportCut() const
{
	return !m_triangles.empty();
}

void SupportMesh::draw(unsigned int opts, const Colour *colourToDraw) noexcept
{
	glEnable(GL_LIGHTING);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 0);
	glDisable(GL_TEXTURE_2D);
	glTranslatef(m_Offset[X_AXIS], m_Offset[Y_AXIS], 0);
	glColor4f(0.5, 0, 0.5, 0.5);
	if (!m_ptrCSGModel)
	{
		if (!m_supportTowers.empty() && !(opts & DrawingOptions::OBJECT_PICK))
		{
			for (auto &it : m_supportTowers)
			{
				it->draw(0, nullptr);
			}
		}
	}
	else
	{
		// Temp draw cut mesh

		//glDisable(GL_CULL_FACE);
		glBegin(GL_TRIANGLES);
		for (auto it : m_ptrCSGModel->indices)
		{
			glNormal3f(m_ptrCSGModel->vertices[it].normal.x, m_ptrCSGModel->vertices[it].normal.y, m_ptrCSGModel->vertices[it].normal.z);
			glVertex3f(m_ptrCSGModel->vertices[it].pos.x, m_ptrCSGModel->vertices[it].pos.y, m_ptrCSGModel->vertices[it].pos.z);
		}
		glEnd();
		//glEnable(GL_CULL_FACE);
	}
	glTranslatef(-m_Offset[X_AXIS],- m_Offset[Y_AXIS], 0);
}

void SupportMesh::regenerateNormals()
{
	for (unsigned int i = 0; i < m_ptrCSGModel->vertices.size(); i += 3)
	{
		aiVector3D pt0 = aiVector3D(m_ptrCSGModel->vertices[i].pos.x,
			m_ptrCSGModel->vertices[i].pos.y,
			m_ptrCSGModel->vertices[i].pos.z);
		aiVector3D pt1 = aiVector3D(m_ptrCSGModel->vertices[i+1].pos.x,
			m_ptrCSGModel->vertices[i+1].pos.y,
			m_ptrCSGModel->vertices[i+1].pos.z);
		aiVector3D pt2 = aiVector3D(m_ptrCSGModel->vertices[i+2].pos.x,
			m_ptrCSGModel->vertices[i+2].pos.y,
			m_ptrCSGModel->vertices[i+2].pos.z);

		aiVector3D v1 = pt1 - pt0;
		aiVector3D v2 = pt2 - pt0;
		aiVector3D outputNormal = v1^v2;//Cross product
		outputNormal.Normalize();
		csgjs_vector outNormal = csgjs_vector(outputNormal.x, outputNormal.y, outputNormal.z);
		m_ptrCSGModel->vertices[i].normal = outNormal;
		m_ptrCSGModel->vertices[i+1].normal = outNormal;
		m_ptrCSGModel->vertices[i+2].normal = outNormal;
	}
}


//double SupportMesh::getOffsetX() const
//{
//	return m_Offset[0];
//}
//
//double SupportMesh::getOffsetY() const
//{
//	return m_Offset[1];
//}
//
//double SupportMesh::getOffsetZ() const
//{
//	return m_Offset[2];
//}

double SupportMesh::getWidth() const
{
	return getMaxX()-getMinX();
}

double SupportMesh::getHeight() const
{
	return getMaxY()-getMinY();
}

void SupportMesh::generateSkirt()
{

}

double SupportMesh::getMinZ() const
{
	return 0;
}

double SupportMesh::getMaxZ() const
{
	double maxZ = 0;
	for (unsigned int i = 0; i < m_ptrCSGModel->vertices.size(); ++i)
	{
		if (m_ptrCSGModel->vertices[i].pos.z > maxZ)
			maxZ = m_ptrCSGModel->vertices[i].pos.z;
	}
	return maxZ;
}

double SupportMesh::getMinX() const
{
	double minX = std::numeric_limits<double>::max();
	for (unsigned int i = 0; i < m_ptrCSGModel->vertices.size(); ++i)
	{
		if (m_ptrCSGModel->vertices[i].pos.x < minX)
			minX = m_ptrCSGModel->vertices[i].pos.x;
	}
	return minX;
}
double SupportMesh::getMaxX() const
{
	double maxX = std::numeric_limits<double>::min();
	for (unsigned int i = 0; i < m_ptrCSGModel->vertices.size(); ++i)
	{
		if (m_ptrCSGModel->vertices[i].pos.x > maxX)
			maxX = m_ptrCSGModel->vertices[i].pos.x;
	}
	return maxX;
}
double SupportMesh::getMinY() const
{
	double minY = std::numeric_limits<double>::max();
	for (unsigned int i = 0; i < m_ptrCSGModel->vertices.size(); ++i)
	{
		if (m_ptrCSGModel->vertices[i].pos.y < minY)
			minY = m_ptrCSGModel->vertices[i].pos.y;
	}
	return minY;
}
double SupportMesh::getMaxY() const
{
	double maxY = std::numeric_limits<double>::min();
	for (unsigned int i = 0; i < m_ptrCSGModel->vertices.size(); ++i)
	{
		if (m_ptrCSGModel->vertices[i].pos.y > maxY)
			maxY = m_ptrCSGModel->vertices[i].pos.y;
	}
	return maxY;
}

void SupportMesh::cutOutMesh(Mesh *mesh)
{
	convertTowersToCSGMesh();
	mesh->applyTransformations();
	m_ptrCSGModel = std::make_shared<csgjs_model>(csgjs_difference(*m_ptrCSGModel,meshToCSGMesh(mesh)));
	regenerateNormals();
	for (unsigned int i = 0; i < m_ptrCSGModel->vertices.size(); i += 3)
	{
		m_triangles.push_back(Triangle(Triangle::TriangleResult::MESH,
			Vertex(m_ptrCSGModel->vertices[i].pos.x, m_ptrCSGModel->vertices[i].pos.y, m_ptrCSGModel->vertices[i].pos.z),
			Vertex(m_ptrCSGModel->vertices[i + 1].pos.x, m_ptrCSGModel->vertices[i + 1].pos.y, m_ptrCSGModel->vertices[i + 1].pos.z),
			Vertex(m_ptrCSGModel->vertices[i + 2].pos.x, m_ptrCSGModel->vertices[i + 2].pos.y, m_ptrCSGModel->vertices[i + 2].pos.z)));
	}
	mesh->undoTransformations();
}

void SupportMesh::convertTowersToCSGMesh()
{
	m_ptrCSGModel.reset();
	if (m_supportTowers.empty() == false)
	{
		m_ptrCSGModel = std::make_shared<csgjs_model>(towerToCSGMesh(m_supportTowers.front()));
		if (m_supportTowers.size() > 1)
		{
			for (int i = 1; i < m_supportTowers.size(); i++)
			{
				m_ptrCSGModel = std::make_shared<csgjs_model>(csgjs_union(*m_ptrCSGModel, towerToCSGMesh(m_supportTowers[i])));
			}
		}
	}
}

csgjs_model SupportMesh::towerToCSGMesh(std::shared_ptr<SupportTower> sTower)
{
	csgjs_model result;

	csgjs_vertex tempVert;
	int counter = 0;
	double maxX = sTower->getMaxX() + 0.1;
	double maxY = sTower->getMaxY() + 0.1;
	double minX = sTower->getMinX() - 0.1;
	double minY = sTower->getMinY() - 0.1;
	// --------- Top -----------
	tempVert.normal = csgjs_vector(0, 0, 1.0f);
	//2
	tempVert.pos = csgjs_vector(maxX, maxY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//1
	tempVert.pos = csgjs_vector(minX, maxY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//0
	tempVert.pos = csgjs_vector(minX, minY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);

	//3
	tempVert.pos = csgjs_vector(maxX, minY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//2
	tempVert.pos = csgjs_vector(maxX, maxY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//0
	tempVert.pos = csgjs_vector(minX, minY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	
	
	
	// Side A
	tempVert.normal = csgjs_vector(-1.0f, 0, 0);
	//0
	tempVert.pos = csgjs_vector(minX, minY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//1
	tempVert.pos = csgjs_vector(minX, maxY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//4
	tempVert.pos = csgjs_vector(minX, minY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	
	//4
	tempVert.pos = csgjs_vector(minX, minY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//1
	tempVert.pos = csgjs_vector(minX, maxY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//5
	tempVert.pos = csgjs_vector(minX, maxY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	
	
	// Side B
	tempVert.normal = csgjs_vector(0, -1.0f, 0);
	//7
	tempVert.pos = csgjs_vector(maxX, minY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//3
	tempVert.pos = csgjs_vector(maxX, minY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//0
	tempVert.pos = csgjs_vector(minX, minY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);

	//0
	tempVert.pos = csgjs_vector(minX, minY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//4
	tempVert.pos = csgjs_vector(minX, minY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//7
	tempVert.pos = csgjs_vector(maxX, minY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	
	
	// Side C
	tempVert.normal = csgjs_vector(1.0f, 0, 0);
	//6
	tempVert.pos = csgjs_vector(maxX, maxY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//2
	tempVert.pos = csgjs_vector(maxX, maxY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//3
	tempVert.pos = csgjs_vector(maxX, minY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);

	//7
	tempVert.pos = csgjs_vector(maxX, minY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//6
	tempVert.pos = csgjs_vector(maxX, maxY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//2
	tempVert.pos = csgjs_vector(maxX, minY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);

	
	// Side D
	tempVert.normal = csgjs_vector(0, 1, 0);
	//1
	tempVert.pos = csgjs_vector(minX, maxY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//2
	tempVert.pos = csgjs_vector(maxX, maxY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//6
	tempVert.pos = csgjs_vector(maxX, maxY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);

	//6
	tempVert.pos = csgjs_vector(maxX, maxY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//5
	tempVert.pos = csgjs_vector(minX, maxY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//1
	tempVert.pos = csgjs_vector(minX, maxY, sTower->getTop());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	
	// Bottom
	tempVert.normal = csgjs_vector(0, 0, -1.0f);
	//4
	tempVert.pos = csgjs_vector(minX, minY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//5
	tempVert.pos = csgjs_vector(minX, maxY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//6
	tempVert.pos = csgjs_vector(maxX, maxY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);

	//6
	tempVert.pos = csgjs_vector(maxX, maxY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//7
	tempVert.pos = csgjs_vector(maxX, minY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	//4
	tempVert.pos = csgjs_vector(minX, minY, sTower->getBottom());
	result.vertices.push_back(tempVert);
	result.indices.push_back(counter++);
	
	return result;
}

csgjs_model SupportMesh::meshToCSGMesh(const Mesh* mesh)
{
	csgjs_model result;
	auto triangles = mesh->getTriangles();
	int counter = 0;
	for (auto it : triangles)
	{
		csgjs_vertex vert;
		vert.normal = csgjs_vector(it.getNormal().x+mesh->getOffsetX(), it.getNormal().y + mesh->getOffsetY(), it.getNormal().z + mesh->getOffsetZ());
		for (uint8_t i = 0; i < 3; i++)
		{
			result.indices.push_back(counter++);
			vert.pos = csgjs_vector(it.getPt(i).x, it.getPt(i).y, it.getPt(i).z);
			result.vertices.push_back(vert);
		}
	}
	return result;
}

void SupportMesh::fillInfill(unsigned int layerID, const RoVaClipperLib::Paths &fullInfill, const RoVaClipperLib::Paths *sources, bool skipTopLogic)
{
	if (Settings::getSingleton().getBoolValue("useInfill"))
	{
		if (m_LayerArray[layerID]->hasPerimeter)
			m_LayerArray[layerID]->InfillPerimeters(Settings::GetSingleton().getDoubleValue("infillDensity"), fullInfill, skipTopLogic, sources);
	}
}