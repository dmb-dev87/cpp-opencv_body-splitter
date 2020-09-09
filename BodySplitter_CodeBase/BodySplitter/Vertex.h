#pragma once
#include <assimp/mesh.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <boost/functional/hash.hpp>

namespace RoVaClipperLib
{
	struct IntPoint;
};

// Consider verts this close to be same position
#define THRESHOLD 1E-7


class Vertex2D;


template <typename T>
class vertex_t
{
public:
	T x, y, z;
public:
	float u, v;
	vertex_t() {};
	vertex_t(double xIn, double yIn) : x(xIn), y(yIn),z(0), u(0.0), v(0.0) {}
	vertex_t(double xIn, double yIn, double zIn) : x(xIn),y(yIn),z(zIn),u(0.0),v(0.0){}
	template <class XYtype>
	vertex_t(const XYtype &xyIn)
	{
		x = static_cast<T>(xyIn.x);
		y = static_cast<T>(xyIn.y);
	}
	vertex_t(const aiVector3D& pos) noexcept : x(pos.x), y(pos.y), z(pos.z) {}
	vertex_t(const aiVector3D& pos, const aiVector3D& texCoords) noexcept : vertex_t(pos) {
	  u = texCoords.x; v = texCoords.y;
	}
	vertex_t(const glm::dvec3 &in) noexcept : x(in.x), y(in.y), z(in.z) {}
	double distanceToSquared(const vertex_t &in) const {
		return (x - in.x)*(x - in.x) + (y - in.y)*(y - in.y) + (z - in.z)*(z - in.z);
	}
	glm::dvec3 toglm() const {
		return glm::dvec3(x, y, z);
	}
	glm::dvec4 toglm4() const {
		return glm::dvec4(x, y, z, 1.0);
	}
	void round(int decimals)
	{
	  x = std::round(x*pow(10.0,decimals))/pow(10.0,decimals);
	  y = std::round(y*pow(10.0,decimals))/pow(10.0,decimals);
	  z = std::round(z*pow(10.0,decimals))/pow(10.0,decimals);
	}

	operator RoVaClipperLib::IntPoint() const;
	operator Vertex2D() const;
	operator glm::vec3() const {
		return glm::vec3(x, y, z);
	}
	vertex_t(const RoVaClipperLib::IntPoint &pt) noexcept;
	~vertex_t() {};
	size_t hash() const {
		std::size_t seedresult = 0;
		boost::hash_combine(seedresult, x);
		boost::hash_combine(seedresult, y);
		boost::hash_combine(seedresult, z);
		return seedresult;
	}

	bool MatchesLocation(const vertex_t &in) const
	{
		if (fabs(x - in.x) > THRESHOLD)
			return false;
		if (fabs(y - in.y) > THRESHOLD)
			return false;
		return true;
	}
	bool operator==(const vertex_t &in) const
	{
		//Slow...
		//Position
		if (fabs(x - in.x) > THRESHOLD)
			return false;
		if (fabs(y - in.y) > THRESHOLD)
			return false;
		if (fabs(z - in.z) > THRESHOLD)
			return false;
		//Textures
		if (fabs(u - in.u) > THRESHOLD)
			return false;
		if (fabs(u - in.u) > THRESHOLD)
			return false;
		return true;
	}
	bool operator!=(const vertex_t &in) const
	{
		//Slow...
		//Position
		if (fabs(x - in.x) > THRESHOLD)
			return true;
		if (fabs(y - in.y) > THRESHOLD)
			return true;
		if (fabs(z - in.z) > THRESHOLD)
			return true;
		//Textures
		if (fabs(u - in.u) > THRESHOLD)
			return true;
		if (fabs(u - in.u) > THRESHOLD)
			return true;
		return false;
	}
	static bool MatchesLocation(const vertex_t &pt1, const vertex_t &pt2) {
		return fabs(pt1.x - pt2.x) < THRESHOLD && fabs(pt1.y - pt2.y) < THRESHOLD;
	}
	static double distanceR2Sqrd(const vertex_t &pt1, const vertex_t &pt2) {
		return (pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y);
	}
};

using Vertex = vertex_t<double>;

class Vertex2D
{
public:
	double x, y;

	//SEL March 2020  In order to support the qualification and subsequent removal of 2D 
	//    vertices, I will add a flag bit here that will stick with the vertex
	bool  bValid=true;
	//SEL end

	template <typename T>
	explicit Vertex2D(const vertex_t<T> &in) noexcept {
		x = in.x; y = in.y;
	}
	explicit Vertex2D(double xIn, double yIn) : x(xIn), y(yIn) {}
	explicit Vertex2D(const RoVaClipperLib::IntPoint &pt);

	static double distanceR2Sqrd(const Vertex2D &pt1, const Vertex2D &pt2) {
		return (pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y);
	}
	template <typename T, typename T2>
	static double distanceR2Sqrd(const T &pt1, const T2 &pt2) {
	  return (pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y);
	}
	bool MatchesLocation(const vertex_t<double> &in) const {
	  return fabs(x - in.x) < THRESHOLD && fabs(y - in.y) < THRESHOLD;
	}
	bool MatchesLocation(const vertex_t<double> &in, double tolerance) const {
	  return fabs(x - in.x) < tolerance && fabs(y - in.y) < tolerance;
	}
	bool MatchesLocation(const Vertex2D &in) const {
	  return fabs(x - in.x) < THRESHOLD && fabs(y - in.y) < THRESHOLD;
	}
	Vertex2D operator+(const Vertex2D& in1) const {
	  return Vertex2D(this->x+in1.x,this->y+in1.y);
	}
	Vertex2D operator-(const Vertex2D& in1) const {
	  return Vertex2D(this->x-in1.x,this->y-in1.y);
	}
	double operator*(const Vertex2D& in) const {
	  return (this->x * in.y - in.x * this->y);
	}
	double Norm()const{
	  return sqrt(this->x * this->x + this->y * this->y);
	}
	size_t hash() const {
		std::size_t seedresult = 0;
		boost::hash_combine(seedresult, x);
		boost::hash_combine(seedresult, y);
		return seedresult;
	}
};



namespace std {
 	template <typename T>
	struct hash<vertex_t<T>> {
		size_t operator () (const vertex_t<T> &f) const { return f.hash(); }
	};
	template <>
	struct hash<Vertex2D> {
		size_t operator () (const Vertex2D &f) const { return f.hash(); }
	};
}

