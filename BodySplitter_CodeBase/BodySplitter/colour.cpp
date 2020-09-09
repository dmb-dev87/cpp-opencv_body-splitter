#include "colour.h"

//#define TEST_CLIENT

#ifndef TEST_CLIENT
#include "Settings.h"
#endif

//----------------------DOUBLE--------------------------------//
template<>
double Colour::getA() const noexcept
{
  return ((rgba & 0xFF000000)>>24)/255.0;
}
template<>
double Colour::getR() const noexcept
{
  return ((rgba & 0xFF))/255.0;
}
template<>
double Colour::getG() const noexcept
{
  return ((rgba & 0xFF00)>>8)/255.0;
}
template<>
double Colour::getB() const noexcept
{
  return ((rgba & 0xFF0000)>>16)/255.0;
}
// ---------------------FLOAT----------------------------------//
template<>
float Colour::getA() const noexcept
{
  return ((rgba & 0xFF000000)>>24)/255.0f;
}
template<>
float Colour::getR() const noexcept
{
  return ((rgba & 0xFF))/255.0f;
}
template<>
float Colour::getG() const noexcept
{
  return ((rgba & 0xFF00)>>8)/255.0f;
}
template<>
float Colour::getB() const noexcept
{
  return ((rgba & 0xFF0000)>>16)/255.0f;
}

//---------------------------------uint8_t--------------------------//
template<>
uint8_t Colour::getA() const noexcept
{
  return ((rgba & 0xFF000000)>>24);
}
template<>
uint8_t Colour::getR() const noexcept
{
  return ((rgba & 0xFF));
}
template<>
uint8_t Colour::getG() const noexcept
{
  return ((rgba & 0xFF00)>>8);
}
template<>
uint8_t Colour::getB() const noexcept
{
  return ((rgba & 0xFF0000)>>16);
}
#ifndef TEST_CLIENT
Colour Colour::InfillColour()
{
	if (Settings::GetSingleton().getBoolValue("useCustomInfillColour"))
	{
		return Settings::GetSingleton().getColourValue("infillColour");
	}
	else
	{
		return Colour(-1.0, -1.0, -1.0);
	}
}
#endif

bool Colour::isSingleColour() const
{
	uint8_t cCount = 0;
	auto cmykw = this->getCMYKW();
	for (uint8_t i = 0; i < 5; i++)
	{
		if (cmykw[i] > 0)
			cCount++;
	}
	return cCount <= 1;
}

Colour Colour::colour_rand()
{
	return Colour((rand() % 100) / 100.0, (rand() % 100) / 100.0, (rand() % 100) / 100.0);

}

Colour::Colour(const std::string &input)
{
  //Check if hex
  if(input.at(0) == '#')
  {
    assert(false);
  }
  else{
	size_t last = 0; size_t next = 0;
	uint counter = 0;
	double r,g,b;
	while ((next = input.find(",", last)) != std::string::npos)
	{
		if (counter == 0)
			r = std::stod(input.substr(last, next - last));
		if (counter == 1)
			g = std::stod(input.substr(last, next - last));
		last = next + 1;
		counter++;
		assert(counter < 3);
	}
	b = std::stod(input.substr(last));
	rgba = RGBto32(r*255.0,g*255.0,b*255.0);
  }
  unset = false;
}

Colour::ColourSpace::CMY Colour::getCMY() const
{
	Colour::ColourSpace::CMY output;

	output.c = (1.0 - this->getR<double>()) * 100.0;
	output.m = (1.0 - this->getG<double>()) * 100.0;
	output.y = (1.0 - this->getB<double>()) * 100.0;

	if (output.c < 1 && output.m < 1 && output.y < 1)
	{
		output.c = (1.0 - Settings::GetSingleton().getColourValue("CMYWhiteSub").getR<double>()) * 100.0;
		output.m = (1.0 - Settings::GetSingleton().getColourValue("CMYWhiteSub").getG<double>()) * 100.0;
		output.y = (1.0 - Settings::GetSingleton().getColourValue("CMYWhiteSub").getB<double>()) * 100.0;
	}

	return output;
}

/*
K = 1-max(R', G', B')

The cyan color (C) is calculated from the red (R') and black (K) colors:

C = (1-R'-K) / (1-K)

The magenta color (M) is calculated from the green (G') and black (K) colors:

M = (1-G'-K) / (1-K)

The yellow color (Y) is calculated from the blue (B') and black (K) colors:

Y = (1-B'-K) / (1-K)
*/

Colour::ColourSpace::CMYK Colour::getCMYK() const
{
  ColourSpace::CMYK output;
  output.k = 1.0-std::max(getR<double>(),std::max(getG<double>(),getB<double>()));
  output.c = (1.0-getR<double>()-output.k)/(1.0-output.k);
  output.m = (1.0-getG<double>()-output.k)/(1.0-output.k);
  output.y = (1.0-getB<double>()-output.k)/(1.0-output.k);
  return output;
}

std::ostream& operator<<(std::ostream &stream, const Colour::ColourSpace::CMYKWT &col)
{
	stream << col.c << "," << col.m << "," << col.y << "," << col.k << "," << col.w << "," << col.t << "\n";
	return stream;
}

Colour::ColourSpace::XYZ Colour::getXYZ() const
{
  double var_R = getR<double>();
  double var_G = getG<double>();
  double var_B = getB<double>();

  if ( var_R > 0.04045 ) 
    var_R = pow( (( var_R + 0.055 ) / 1.055 ), 2.4);
  else 
    var_R = var_R / 12.92;
  if ( var_G > 0.04045 ) 
    var_G = pow(( ( var_G + 0.055 ) / 1.055 ), 2.4);
  else                   
    var_G = var_G / 12.92;
  if ( var_B > 0.04045 ) 
    var_B = pow(( ( var_B + 0.055 ) / 1.055 ) , 2.4);
  else                   
    var_B = var_B / 12.92;

  var_R = var_R * 100.0;
  var_G = var_G * 100.0;
  var_B = var_B * 100.0;
  ColourSpace::XYZ output;
  //Observer. = 2°, Illuminant = D65
  output.x = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
  output.y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
  output.z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;
  
  return output;
}
/*
 * (H)L = 10 * sqrt( Y )
(H)a = 17.5 * ( ( ( 1.02 * X ) - Y ) / sqrt( Y ) )
(H)b = 7 * ( ( Y - ( 0.847 * Z ) ) / sqrt( Y ) )
  */
Colour::ColourSpace::LABS Colour::getLABS() const
{
  /*
  var_X = X / ref_X          //ref_X =  95.047   Observer= 2°, Illuminant= D65
var_Y = Y / ref_Y          //ref_Y = 100.000
var_Z = Z / ref_Z          //ref_Z = 108.883

if ( var_X > 0.008856 ) var_X = var_X ^ ( 1/3 )
else                    var_X = ( 7.787 * var_X ) + ( 16 / 116 )
if ( var_Y > 0.008856 ) var_Y = var_Y ^ ( 1/3 )
else                    var_Y = ( 7.787 * var_Y ) + ( 16 / 116 )
if ( var_Z > 0.008856 ) var_Z = var_Z ^ ( 1/3 )
else                    var_Z = ( 7.787 * var_Z ) + ( 16 / 116 )

CIE-L* = ( 116 * var_Y ) - 16
CIE-a* = 500 * ( var_X - var_Y )
CIE-b* = 200 * ( var_Y - var_Z )
*/
  ColourSpace::LABS output;
  auto XYZ = getXYZ();
  double var_X = XYZ.x/95.047;
  double var_Y = XYZ.y/100.0;
  double var_Z = XYZ.z/108.883;
  
  if(var_X > 0.008856)
    var_X = pow(var_X,1.0/3.0);
  else
    var_X = ( 7.787 * var_X ) + ( 16.0 / 116.0 );
  if(var_Y > 0.008856)
    var_Y = pow(var_Y,1.0/3.0);
  else
     var_Y = ( 7.787 * var_Y ) + ( 16.0 / 116.0 );
  if(var_Z > 0.008856)
    var_Z = pow(var_Z,1.0/3.0);
  else
    var_Z = ( 7.787 * var_Z ) + ( 16.0 / 116.0 );
  
  output.l = (116.0*var_Y)-16.0;
  output.a = 500.0*(var_X-var_Y);
  output.b = 200.0*(var_Y-var_Z);
  
  return output;
}

double Colour::distanceTo(const Colour& in) const
{
  auto C1 = getLABS();
  auto C2 = in.getLABS();
  return sqrt(
    (C1.l-C2.l)*(C1.l-C2.l)+
    (C1.a-C2.a)*(C1.a-C2.a)+
    (C1.b-C2.b)*(C1.b-C2.b)
	 );
}

Colour::ColourSpace::HSV Colour::getHSV() const
{
  ColourSpace::HSV output;
  
  double r = getR<double>();
  double g = getG<double>();
  double b = getB<double>();
  
  double cMax = std::max(r,std::max(g,b));
  double cMin = std::min(r,std::min(g,b));
  double delta = cMax-cMin;
  
  if(cMax == r)
  {
    output.h = 60.0*fmod((g-b)/delta,6);
  }
  else if(cMax == g)
  {
    output.h = 60.0*((b-r)/delta+2);
  }
  else
  {
    output.h = 60.0*((r-g)/delta+4);
  }
  
  if(cMax < 0.00001)
    output.s = 0;
  else
    output.s = delta/cMax;
  output.v = cMax;
  
  return output;
}

Colour::ColourSpace::CMYKW Colour::getCMYKW() const
{
  ColourSpace::CMYKW output;
  if (rgba == 0)
  {
	  output.k = 100;
	  return output;
  }
  ColourSpace::CMYK cmyk = getCMYK();
  double white = std::max(1.0 - (cmyk.c + cmyk.m + cmyk.y + cmyk.k),0.0);
  output.c = cmyk.c*100.0;
  output.m = cmyk.m*100.0;
  output.y = cmyk.y*100.0;
  output.k = cmyk.k*100.0;
  output.w = white*100.0;
  return output;

  auto hsv = getHSV();
  double k1,w1,t1;
  
  k1 = pow(1-hsv.v,2);
  w1 = (1.0-hsv.s)*hsv.v;
  t1 = hsv.s*hsv.v;
  
  double kn = k1/(k1+w1+t1);
  double wn = w1/(k1+w1+t1);
  double tn = t1/(k1+w1+t1);
  double cn,mn,yn;
  if(hsv.h <= 60.0)
  {
    cn = 0;
    mn = 0.5-hsv.h/120.0;
    yn = hsv.h/120.0+0.5;
  }
  else if(hsv.h <= 180.0)
  {
    cn = (hsv.h-120.0)/120.0+0.5;
    mn = 0;
    yn = 0.5-(hsv.h-120.0)/120.0;
  }
  else if(hsv.h <= 300)
  {
    cn = 0.5-(hsv.h-240.0)/120.0;
    mn = (hsv.h-240.0)/120.0+0.5;
    yn = 0;
  }
  else
  {
    cn = 0;
    mn = 0.5-(hsv.h-360.0)/120.0;
    yn = (hsv.h-360.0)/120.0+0.5;
  }
  output.c = std::max(cn*tn*100.0,0.0);
  output.m = std::max(mn*tn*100.0,0.0);
  output.y = std::max(yn*tn*100.0,0.0);
  output.k = std::max(kn*100.0,0.0);
  output.w = std::max(wn*100.0,0.0);
  
  return output;
}

/* RGB Distance */
double Colour::sqrdDistanceTo(const Colour &in) const
{
  return (getR<double>()-in.getR<double>())*(getR<double>()-in.getR<double>()) +
	  (getG<double>()-in.getG<double>())*(getG<double>()-in.getG<double>()) +
	  (getB<double>()-in.getB<double>())*(getB<double>()-in.getB<double>()) +
	  (getA<double>()-in.getA<double>())*(getA<double>()-in.getA<double>());
	//Hmmm should be CMYK or LABS not RGB, we can fix this later
	//RGB
	//return (pow((this->r - in.r), 2) + pow(this->g - in.g, 2) + pow(this->b - in.b, 2));

	//CMYKW
// 	return fabs(in.c - c) + fabs(in.m - m) + fabs(in.y - y) + fabs(in.k - k) + fabs(in.w - w);
// 	return (in.c - c)*(in.c - c) + (in.m - m)*(in.m - m) + (in.y - y)*(in.y - y) + (in.k - k)*(in.k - k) + (in.w - w)*(in.w - w);
	/*
	dh = min(abs(h1-h0), 360-abs(h1-h0)) / 180.0
ds = abs(s1-s0)
dv = abs(v1-v0) / 255.0
*/
	//Tone
	//return (in.k - k)*(in.k - k) + (in.w - w)*(in.w - w) + (in.t - t);
// 	double dh = std::min(fabs(in.h - h), 360.0 - fabs(in.h - h)) / 180.0;
// 	double ds = fabs(in.s - s);
// 	double dv = fabs(in.v - v);
// 	return dh*dh + ds*ds + dv*dv;
	//return (pow((this->h - in.h), 2) + pow(this->s - in.g, s) + pow(this->v - in.b, v));
}

////Hash specialization
//std::size_t std::hash<Colour>::operator()(const Colour& k) const
//{
//	return k.getRGBA();
//}

#ifdef TEST_CLIENT
#ifdef _CONSOLE
#error "This client is able to run in linux only, with the _CONSOLE build flag disabled"
#endif
#include <iostream>

int main(int argc, char** args)
{
  uint8_t rIn = std::stoi(args[1]);
  uint8_t gIn = std::stoi(args[2]);
  uint8_t bIn = std::stoi(args[3]);
  Colour testColour(rIn,gIn,bIn);
  std::cout << "Converting R:"<<(int)rIn << " G:"<<(int)gIn<<" B:"<<(int)bIn<<"\n";
  std::cout << "--------------HSV------------\n";
  auto hsv = testColour.getHSV();
  std::cout << "H: "<< hsv.h<<"\n";
  std::cout << "S: "<< hsv.s<<"\n";
  std::cout << "V: "<< hsv.v<<"\n";
  
  
  std::cout << "--------------CMYKW----------\n";
  auto cmykw = testColour.getCMYKW();
  std::cout << "C: "<<cmykw.c<<"\n";
  std::cout << "M: "<<cmykw.m<<"\n";
  std::cout << "Y: "<<cmykw.y<<"\n";
  std::cout << "K: "<<cmykw.k<<"\n";
  std::cout << "w: "<<cmykw.w<<"\n";
}
#endif
