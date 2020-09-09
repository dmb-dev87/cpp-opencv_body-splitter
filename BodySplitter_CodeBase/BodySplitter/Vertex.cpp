
#include "Vertex.h"
#include <boost/functional/hash.hpp>
#include "ClipperLib/RoVaclipper.hpp"

#if USE_INT_COORDS
using namespace BodySplitter;
#endif

/*
Vertex::Vertex(const aiVector3D &pos, const aiVector3D &texCoords) : Vertex(pos)
{
	u = texCoords.x;
	v = texCoords.y;
}

Vertex::Vertex(const glm::dvec3& in)
{

}

Vertex::Vertex(const Vector& stalgoVector)
{
  x = stalgoVector.x;
  y = stalgoVector.y;
}


#if USE_INT_COORDS
Vertex::Vertex(const aiVector3D &pos, const aiColor4D &colour) : u(0), v(0)
{
	x = to_micrometers(pos.x); y = to_micrometers(pos.y); z = to_micrometers(pos.z);
	r = colour.r; g = colour.g; b = colour.b;
}
#else
Vertex::Vertex(const aiVector3D &pos) : u(0), v(0)
{
	x = (pos.x); y = (pos.y); z = (pos.z);
}
#endif

#if USE_INT_COORDS
Vertex::Vertex(const glm::tvec3<glm::int64> &in) : u(0),v(0),r(0),g(0),b(0),
x(in.x), y(in.y), z(in.z)
{

}
#endif

#if USE_INT_COORDS
glm::tvec3<glm::int64> Vertex::toglm() const
{
	glm::tvec3<glm::int64> out;
	out.x = this->x;
	out.y = this->y;
	out.z = this->z;
	return out;
}

glm::tvec4<glm::int64> Vertex::toglm4() const
{
	glm::tvec4<glm::int64> out;
	out.x = this->x;
	out.y = this->y;
	out.z = this->z;
	out.w = 1;
	return out;
}
#else
glm::dvec3 Vertex::toglm() const
{
	glm::dvec3 out;
	out.x = this->x;
	out.y = this->y;
	out.z = this->z;
	return out;
}

glm::dvec4 Vertex::toglm4() const
{
	glm::dvec4 out;
	out.x = this->x;
	out.y = this->y;
	out.z = this->z;
	out.w = 1;
	return out;
}
#endif

bool Vertex::MatchesLocation(const Vertex &pt1, const Vertex &pt2)
{
#if USE_INT_COORDS
	if (llabs(pt1.x - pt2.x) > THRESHOLD)
		return false;
	if (llabs(pt1.y - pt2.y) > THRESHOLD)
		return false;
#else
	if (fabs(pt1.x - pt2.x) > THRESHOLD)
		return false;
	if (fabs(pt1.y - pt2.y) > THRESHOLD)
		return false;
#endif
	//if (fabs(pt1.z - pt2.z) > THRESHOLD)
	//	return false;
	return true;
}

size_t Vertex::hash() const
{
	//Not working right i dont think
#if USE_INT_COORDS
	std::size_t h1 = std::hash<micrometers>()(x);
	std::size_t h2 = std::hash<micrometers>()(y);
	std::size_t h3 = std::hash<micrometers>()(z);
#else
	std::size_t h1 = std::hash<double>()(round(x*100.0));
	std::size_t h2 = std::hash<double>()(round(y*100.0));
	std::size_t h3 = std::hash<double>()(z);
#endif
	std::size_t seed = h1;
	boost::hash_combine(seed, h2);
	return seed;
}

#if USE_INT_COORDS
micrometers Vertex::distanceToSquared(const Vertex &in) const
#else
double Vertex::distanceToSquared(const Vertex &in) const
#endif
{
	return (in.x - this->x)*(in.x - this->x) + (in.y - this->y)*(in.y - this->y);
}

#if USE_INT_COORDS
Vertex::Vertex(millimeters xIn, millimeters yIn, millimeters zIn):u(0.0),v(0.0)
{
	x = to_micrometers(xIn);
	y = to_micrometers(yIn);
	z = to_micrometers(zIn);
}

Vertex::Vertex(millimeters xIn, millimeters yIn) : z(0), u(0.0), v(0.0)
{
	x = to_micrometers(xIn);
	y = to_micrometers(yIn);
}
Vertex::Vertex(millimeters xIn, millimeters yIn, millimeters zIn,
	double rIn, double gIn, double bIn) : ,u(0.0), v(0.0)
{
	x = to_micrometers(xIn);
	y = to_micrometers(yIn);
	z = to_micrometers(zIn);
}
#endif

Vertex::operator RoVaClipperLib::IntPoint() const
{
	return RoVaClipperLib::IntPoint(this->x*DEDOUBLER, this->y*DEDOUBLER);
}

Vertex::Vertex(const RoVaClipperLib::IntPoint& pt)
{
  x = (double)pt.X/DEDOUBLER; y = (double)pt.Y/DEDOUBLER;
}
*/


//Vertex2D::Vertex2D(const BodySplitter::Vertex &in)
//{
//	this->x = in.x; this->y = in.y;
//}
template <typename T>
vertex_t<T>::operator Vertex2D() const
{
  return Vertex2D(x,y);
}

template <typename T>
vertex_t<T>::vertex_t(const RoVaClipperLib::IntPoint &pt) noexcept :
		x((double)pt.X / DEDOUBLER), y((double)pt.Y / DEDOUBLER){}

Vertex2D::Vertex2D(const RoVaClipperLib::IntPoint& pt) : 
	x((double)pt.X / DEDOUBLER), y((double)pt.Y / DEDOUBLER)
{

}
template <typename T>
vertex_t<T>::operator RoVaClipperLib::IntPoint() const
{
	return RoVaClipperLib::IntPoint(this->x*DEDOUBLER, this->y*DEDOUBLER);
}

template class vertex_t<double>;