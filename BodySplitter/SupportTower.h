#pragma once
/* These towers are roughly equiv to meshes*/
#include "Vertex.h"
#include <glm/vec3.hpp>
#include "ClipperLib/RoVaclipper.hpp"
class Mesh;
class UserMesh;

class SupportTower
{
private:
	friend class SupportAnalyzer;
	double x, y; //For debug preview in msvc
	double m_width, m_height;
	double m_bottom;
	Vertex m_centrePt;
	enum class stateOfSupport {NORMAL,INVALIDMOVING,VALIDMOVING};
	std::shared_ptr<UserMesh> touchingMeshPtr;
	stateOfSupport state = stateOfSupport::NORMAL;
	bool isHighlighted = false;
public:
	SupportTower(const glm::vec3 &centrePoint, double height, double width);
	~SupportTower();
	void updatePosition(const glm::vec3 &bottom);
	void draw(unsigned int opts, const Colour *colourToDraw) noexcept;
	RoVaClipperLib::Path getPerimetersAsPath() const;
	double getTop() const {
		return m_height;
	}
	double getBottom() const {
		return m_bottom;
	}
	std::shared_ptr<UserMesh> getTouchingMesh() {
		return touchingMeshPtr;
	}
	void addToMesh(Mesh *mesh_ptr);
	void highLight(bool state);
	double getMinX() const {
		return m_centrePt.x - m_width / 2.0;
	}
	double getMaxX() const {
		return m_centrePt.x + m_width / 2.0;
	}
	double getMinY() const {
		return m_centrePt.y - m_width / 2.0;
	}
	double getMaxY() const {
		return m_centrePt.y + m_width / 2.0;
	}
	//double getBottom() const {
	//	return m_bottom;
	//}
	//double getTop() const {
	//	return m_height;
	//}
};

