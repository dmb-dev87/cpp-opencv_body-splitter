#pragma once
#include <opencv2/core/core.hpp>
/* Colour information is primarily stored in an unsigned 32bit value
This class can convert from this to RGB, HSV, CMYK, CMYK-W-travis' code, not tested heavily
It also currently doesnt consider colour weighting*/
class Colour
{
private:
	uint32_t rgba;
	bool unset = true;
	// 	double r, g, b;
	// 	double h, s, v;
	// 	double c, m, y, k, w, t;
public:
	struct ColourSpace {
		struct CMY {
			double c, m, y;
			double& operator[](const unsigned int x) {
				if (x == 0)
					return c;
				else if (x == 1)
					return m;
				else if (x == 2)
					return y;
			};
		};
		struct CMYK {
			double c, m, y, k;
			double& operator[](const unsigned int x) {
				if (x == 0) return c;
				if (x == 1) return m;
				if (x == 2) return y;
				if (x == 3) return k;
			}
		};
		struct CMYKW {
			double c = 0.0, m = 0.0, y = 0.0, k = 0.0, w = 0.0;
			double& operator[](const unsigned int x){
				if (x == 0) return c;
				if (x == 1) return m;
				if (x == 2) return y;
				if (x == 3) return k;
				if (x == 4) return w;
			}
		};
		struct CMYKWT {
			double c = 0.0, m = 0.0, y = 0.0, k = 0.0, w = 0.0, t=0.0;
			double& operator[](const unsigned int x) {
				if (x == 0) return c;
				if (x == 1) return m;
				if (x == 2) return y;
				if (x == 3) return k;
				if (x == 4) return w;
				if (x == 5) return t;
			}
		};
		struct LABS {
			double l, a, b;
			double& operator[](const unsigned int x) {
				if (x == 0) return l;
				if (x == 1) return a;
				if (x == 2) return b;
			}
		};
		struct XYZ {
			double x, y, z;
			double& operator[](const unsigned int in) {
				if (in == 0) return x;
				if (in == 1) return y;
				if (in == 2) return z;
			}
		};
		struct HSV {
			double h, s, v;
			double& operator[](const unsigned int in) {
				if (in == 0) return h;
				if (in == 1) return s;
				if (in == 2) return v;
			}
		};
	};

	explicit Colour(double rIn, double gIn, double bIn, double aIn = 0.0)
	{
		if (rIn > 1.)
			rIn = 1.;
		if (gIn > 1.)
			gIn = 1.;
		if (bIn > 1.)
			bIn = 1.;
		if (aIn > 1.)
			aIn = 1.;
		rgba = RGBAto32((uint8_t)round(rIn*255.0), (uint8_t)round(gIn*255.0), (uint8_t)round(bIn*255.0), (uint8_t)round(aIn*255.0));
		unset = false;
	}
	explicit Colour(uint8_t rIn, uint8_t gIn, uint8_t bIn)
	{
		rgba = RGBto32(rIn, gIn, bIn);
		unset = false;
	}
	explicit Colour(const cv::Vec3b &cvcolour) {
		// For fun opencv is BGR
		rgba = RGBto32(cvcolour[2], cvcolour[1], cvcolour[0]);
		unset = false;
	}
	explicit Colour(const uint32_t in) : rgba(in) {};
	Colour() : rgba(0) {  };
	Colour(const std::string &string); //Format r,g,b
	bool operator ==(const Colour &in) const
	{
		return rgba == in.rgba;
	}
	bool operator !=(const Colour &in) const
	{
		return rgba != in.getRGBA();
	}
	ColourSpace::CMY getCMY() const;
	ColourSpace::HSV getHSV() const;
	ColourSpace::CMYK getCMYK() const;
	ColourSpace::CMYKW getCMYKW() const;
	ColourSpace::LABS getLABS() const;
	ColourSpace::XYZ getXYZ() const;
	
	static Colour InfillColour();
	static Colour colour_rand();
	double sqrdDistanceTo(const Colour &in) const;
	double distanceTo(const Colour &in) const;

	bool isSingleColour() const;

	bool isSet() const noexcept {
	  return !unset;
	}
	void set(bool newSetting)
	{
		unset = !newSetting;
	}
	
	template <typename T>
	T getA() const noexcept;
	template <typename T>
	T getG() const noexcept;
	template <typename T>
	T getB() const noexcept;
	template <typename T>
	T getR() const noexcept;
	
	uint32_t getRGBA() const noexcept{
	  return rgba;
	}
	
	std::size_t hash() const
	{
	  return rgba;
	}
	
	operator uint32_t() const
	{
		return rgba;
	}
	inline static uint32_t RGBto32(uint8_t r, uint8_t g, uint8_t b)
	{
	  return r | g<<8 | b<<16;
	}
	inline static uint32_t RGBAto32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
	  return r | g<<8 | b<<16 | a<<24;
	}
};

std::ostream& operator<<(std::ostream &stream, const Colour::ColourSpace::CMYKWT &col);

namespace std {

	template <>
	struct hash<Colour>
	{
		std::size_t operator()(const Colour& k) const
		{
		  return k.getRGBA();
		}
	};

}
