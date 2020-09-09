#pragma once
/* Used to calculate unsupport triangles */
#include <memory>
#include <stdint.h>
#include <vector>
#include <deque>
class UserMesh;
class Layer;
class Triangle;
class iDrawable;
class SupportTower;

class SupportAnalyzer
{
private:
	bool hasFoundUnsupportedTriangles = false;
	const UserMesh* m_ptrMesh;
	std::vector<Triangle> unsupportedTriangles;
	std::vector<std::shared_ptr<SupportTower>> supportTowers;
	void findUnsupportTriangles();
public:
	SupportAnalyzer(const UserMesh* mesh_ptr);
	void generateSupportTowers(const std::deque<Triangle> &triangles);
	~SupportAnalyzer();
	std::vector<std::shared_ptr<SupportTower>> getSupportTower() {
		return supportTowers;
	}
};

