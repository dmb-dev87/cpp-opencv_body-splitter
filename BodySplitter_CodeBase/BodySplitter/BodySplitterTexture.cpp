#include "BodySplitterTexture.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <unordered_set>

#include "Perimeter.h"
#include "RSPolygon.h"
#include "Settings.h"

// include OpenGL
#include <wx/wx.h>
#ifndef CONSOLE
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#endif

#include "OpenglExt.h"

#include "Line.h"

#include "colour.h"

//#include "EdgesSubPix\EdgesSubPix.h"

cv::RNG rng(12345);

extern int gl_nSEL; //SEL Debug - verbose logging when true

BodySplitterTexture::BodySplitterTexture() : sourceImage(nullptr), simplifiedImage(nullptr), contours(nullptr),
	simplifiedTexture(0), sourceTexture(0),contourTexture(0), isOpen(false)
{

}

BodySplitterTexture::BodySplitterTexture(const std::string &fullPath) : sourceImage(nullptr), simplifiedImage(nullptr), contours(nullptr),
	simplifiedTexture(0), sourceTexture(0),contourTexture(0), isOpen(false)
{
	openImage(fullPath);
}

bool BodySplitterTexture::openImage(const std::string &fullPath)
{
	if (sourceImage)
		delete sourceImage;
	sourceImage = new cv::Mat(cv::imread(fullPath));
	if (sourceImage->empty())
	{
		delete sourceImage;
		sourceImage = nullptr;
		return false;
	}
	//if ((sourceImage->cols & (sourceImage->cols - 1)) == 0 && (sourceImage->rows & (sourceImage->rows - 1)) == 0)
	//{
	//  if(sourceImage->cols > 1024 || sourceImage->rows > 1024)
	//  {
	//    //Scale texture automatically?
	//  }
	//}
	//else
	//{
	//	//Check if hardware suports NPOT
	//	if (!OpenglExt::getSingleton().isSupported("GL_ARB_texture_non_power_of_two"))
	//	{
	//		//Fail here until we have an NPOT to POT converter
	//		wxMessageBox("Hardware doesnt support non power of two textures!", "Error");
	//		return false;
	//	}
	//	//Resize

	//}
	targetXRes = sourceImage->cols; targetYRes = sourceImage->rows;
	sourceX = sourceImage->cols; sourceY = sourceImage->rows;
	isOpen = true;
	//imwrite("sourceImage.jpg", *sourceImage);
	cv::flip(*sourceImage, *sourceImage, 0);	//Flip for opengl's upsidedownness
	//imwrite("flippedsourceImage.jpg", *sourceImage);
	generateTexture(sourceImage, sourceTexture); //SEL this isn't needed
	m_fileName = fullPath;
	return true;
}

void BodySplitterTexture::removeSimplified()
{
	isSimple = false;
	if (simplifiedImage)
		delete simplifiedImage;
	simplifiedImage = nullptr;
}

void BodySplitterTexture::generateTexture(const cv::Mat* source, GLuint &index)
{
#ifndef CONSOLE
	glDeleteTextures(1, &index);
	glGenTextures(1, &index);
	
	glBindTexture(GL_TEXTURE_2D, index);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	cv::Mat resized;
	// Give the image to OpenGL
	if ((source->cols & (source->cols - 1)) == 0 && (source->rows & (source->rows - 1)) == 0)
	{
	}
	else
	{
		int cols, rows;
		if ((source->cols & (source->cols - 1)) != 0)
			cols = pow(2, ceil(log(source->cols / log(2))));
		if((source->rows & (source->rows - 1)) != 0)
			rows = pow(2, ceil(log(source->rows / log(2))));
		resized.create(cols, rows, source->type());
		cv::resize(*source, resized, cv::Size(cols, rows), 0, 0, cv::INTER_LANCZOS4);
		source = &resized;
	}
	cv::Mat temp;
	if (source->type() != CV_8UC(3))
	{
		temp.create(source->rows,source->cols, source->type());
		cv::cvtColor(*source, temp, CV_GRAY2BGR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, source->cols, source->rows, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, source->ptr());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp.cols, temp.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, temp.ptr());
	}
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, source->cols, source->rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, source->ptr());

#endif
}

std::vector<uchar> BodySplitterTexture::exportImage(const std::string &format)
{
	std::vector<uchar> buff;
	//cv::InputArray tempArray(*sourceImage);
	cv::imencode(format, *sourceImage, buff);
	return buff;
}

bool BodySplitterTexture::openImage(const std::vector<unsigned char> &textureBuffer)
{
	if (sourceImage)
		delete sourceImage;
	cv::InputArray tempArray(textureBuffer);
	sourceImage = new cv::Mat(cv::imdecode(tempArray, CV_LOAD_IMAGE_COLOR));
	if (sourceImage->empty())
	{
		delete sourceImage;
		sourceImage = nullptr;
		return false;
	}
	//if ((sourceImage->cols & (sourceImage->cols - 1)) == 0 && (sourceImage->rows & (sourceImage->rows - 1)) == 0)
	//{
	//	if (sourceImage->cols > 1024 || sourceImage->rows > 1024)
	//	{
	//		//Scale texture automatically?
	//	}
	//}
	//else
	//{
	//	//Check if hardware suports NPOT
	//	if (!OpenglExt::getSingleton().isSupported("GL_ARB_texture_non_power_of_two"))
	//	{
	//		//Fail here until we have an NPOT to POT converter
	//		wxMessageBox("Hardware doesnt support non power of two textures!", "Error");
	//		return false;
	//	}
	//	//Resize

	//}
	targetXRes = sourceImage->cols; targetYRes = sourceImage->rows;
	sourceX = sourceImage->cols; sourceY = sourceImage->rows;
	isOpen = true;
	//cv::flip(*sourceImage, *sourceImage, 0);	//Flip for opengl's upsidedownness
	generateTexture(sourceImage, sourceTexture);
	m_fileName = "fromzip";
	return true;
}

void BodySplitterTexture::adjustColoursAndRes(unsigned int newColours, int x, int y)
{
	if (k_meansColourSet != newColours || (x != targetXRes) || (y != targetYRes))
	{
		targetXRes = x; targetYRes = y;
		adjustColour(newColours);
	}
}

void BodySplitterTexture::adjustColour(uint16_t colours)
{
	std::cerr << "Starting adjustColour\n";
	if (isCoaster())
		colours = std::min(colours, (uint16_t)GET_DOUBLE("maxCoasterColours"));
	k_meansColourSet = colours;
	isSimple = true;
	colourCount = colours;
	if (simplifiedImage)
		delete simplifiedImage;
	simplifiedImage = new cv::Mat(targetXRes, targetYRes, sourceImage->type());
	cv::resize(*sourceImage, *simplifiedImage, cv::Size(targetXRes, targetYRes),0,0, cv::INTER_NEAREST);
	//simplifiedImage = new cv::Mat(*sourceImage);
	colourSet.clear();
	//K-means: http://qtandopencv.blogspot.ca/2013/10/opencv-and-color-quantization-01-kmeans.html
	if(GET_BOOL("useKMeans") || isCoaster())
	{
		cv::Mat samples(simplifiedImage->total(), 3, CV_32F);
		auto samples_ptr = samples.ptr<float>(0);
		for (int row = 0; row != simplifiedImage->rows; ++row) {
			auto src_begin = simplifiedImage->ptr<uchar>(row);
			auto src_end = src_begin + simplifiedImage->cols * simplifiedImage->channels();
			//auto samples_ptr = samples.ptr<float>(row * src.cols);
			while (src_begin != src_end) {
				samples_ptr[0] = src_begin[0];
				samples_ptr[1] = src_begin[1];
				samples_ptr[2] = src_begin[2];
				samples_ptr += 3; src_begin += 3;
			}
		}
		int clusterCount = colours;
		cv::Mat labels;
		int attempts = 10;
		if (colours > 9)
			attempts = 1;
		cv::Mat centers;
		cv::kmeans(samples, clusterCount, labels,
			cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS,
				10, 0.01),
			attempts, cv::KMEANS_PP_CENTERS, centers);

		//step 3 : map the centers to the output
		cv::Mat new_image(simplifiedImage->size(), simplifiedImage->type());
		for (int row = 0; row != simplifiedImage->rows; ++row) {
			auto new_image_begin = new_image.ptr<uchar>(row);
			auto new_image_end = new_image_begin + new_image.cols * 3;
			auto labels_ptr = labels.ptr<int>(row * simplifiedImage->cols);

			while (new_image_begin != new_image_end) {
				int const cluster_idx = *labels_ptr;
				auto centers_ptr = centers.ptr<float>(cluster_idx);
				new_image_begin[0] = centers_ptr[0];
				new_image_begin[1] = centers_ptr[1];
				new_image_begin[2] = centers_ptr[2];
				new_image_begin += 3; ++labels_ptr;
			}
		}
		*simplifiedImage = new_image;
	}
	if (GET_BOOL("useKMeans")) // If we're using kmeans we can end colour counting once we reach colours
		populateColourVector(colours);
	else
		populateColourVector(-1);
	needsRegenSimplified = true;
	
	breakDownImage();
	std::cerr << "Finished adjustColour\n";
}

void BodySplitterTexture::resizeSimplifiedImage(unsigned int x, unsigned int y)
{
	if (simplifiedImage)
	{
		cv::Mat temp(*simplifiedImage);
		delete simplifiedImage;
		simplifiedImage = new cv::Mat(x, y, temp.type());
		cv::resize(temp, *simplifiedImage, cv::Size(x, y), 0, 0, cv::INTER_NEAREST);
	}
	else
	{
		simplifiedImage = new cv::Mat(x, y, sourceImage->type());
		cv::resize(*sourceImage, *simplifiedImage, cv::Size(x, y), 0, 0, cv::INTER_NEAREST);
	}
	generateTexture(simplifiedImage, simplifiedTexture);
	targetXRes = x;
	targetYRes = y;
	breakDownImage();
}

void BodySplitterTexture::populateColourVector(int haltAt)
{
	colourSet.clear();
	cv::Mat *imageToUse = sourceImage;
	if (simplifiedImage)
		imageToUse = simplifiedImage;
	for (int x = 0; x < imageToUse->cols; x++)
	{
		for (int y = 0; y < imageToUse->rows; y++)
		{
			colourSet.insert(Colour((*imageToUse).at<cv::Vec3b>(y, x)).getRGBA());
			if (haltAt >= 0 && colourSet.size() == haltAt)
				return;
		}
	}
}

std::unordered_set<uint32_t> BodySplitterTexture::getColours() 
{
	if (colourSet.empty())
		populateColourVector(100);
	return colourSet;// std::unordered_set<uint32_t>(colourSet.begin(), colourSet.end());
}

void BodySplitterTexture::makeSimplified(unsigned int colours, unsigned int res)
{
	if (colours == 0 && res == 0)
	{
		if (simplifiedImage)
			delete simplifiedImage;
		simplifiedImage = new cv::Mat(*sourceImage);
	}
	else
		adjustColour(colours);
	//breakDownImage();
}

void BodySplitterTexture::colorReduce(cv::Mat& image, int div)
{
	int nl = image.rows;                    // number of lines
	int nc = image.cols * image.channels(); // number of elements per line

	for (int j = 0; j < nl; j++)
	{
		// get the address of row j
		uchar* data = image.ptr<uchar>(j);

		for (int i = 0; i < nc; i++)
		{
			// process each pixel
			data[i] = data[i] / div * div + div / 2;
		}
	}
}

BodySplitterTexture::~BodySplitterTexture()
{
	if (sourceImage)
		delete sourceImage;
	if (simplifiedImage)
		delete simplifiedImage;
#ifndef CONSOLE
	if (simplifiedTexture)
	{
		glDeleteTextures(1, &simplifiedTexture);
	}
	if(sourceTexture)
		glDeleteTextures(1, &sourceTexture);
#endif
}

void BodySplitterTexture::bindTexture(bool simplified)
{
#ifndef CONSOLE
	if (simplified)
	{
		if (needsRegenSimplified)
		{
			generateTexture(simplifiedImage, simplifiedTexture);
			needsRegenSimplified = false;
		}
		glBindTexture(GL_TEXTURE_2D, simplifiedTexture);
	}
	else
		glBindTexture(GL_TEXTURE_2D, sourceTexture);
#endif
}

std::size_t hashRGB(const cv::Vec3b &in)
{
	std::size_t hashValue = in[0] | in[1] << 8 | in[2] << 16;
	return hashValue;
}

////Updated by CG on March 30th 2020
//This is an areas that caused color bleeding
//because of gaps in the CV generated contour, which could only be seen if the drawContour width was 1 and using paint.net to super zoom in,
//these gaps allowed the perimeter to pass thru without an intersection being detected in splitPerim with LineCrossesContour
//another problem was that the contour was so rough that the splits were also rough which looked like minor bleeding
//the solution to both these problems was to dilate the contour and then findContour again.
//iterating 10 times made no difference
//increasing the nImageMaskDilation from 5 to 10 made no difference
//applying erosion before findContour also made little difference to the final contours smoothness or watertightness
//the drawGrid result is used directly in following steps so they random colors cannot be used anymore
//1 iteration and nImageMaskDilation of 5 is recommended
void BodySplitterTexture::breakDownImage()
{
	cv::Mat src_gray;
	Canny(*simplifiedImage, src_gray, 0.01, 0.02);

	//SEL Feb 2020   add logging to take a look at the processed images
	if (Settings::getSingleton().getBoolValue("logBreakDownImages"))
	{
		imwrite("SimplifiedImage.jpg", *simplifiedImage);
		imwrite("CannyGray.jpg", src_gray);
	}

	cvcontours.clear();
	cv::findContours(src_gray, cvcontours, cvhierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));


	if (Settings::getSingleton().getBoolValue("logBreakDownImages"))
	{
		imwrite("1st_findContour.jpg", src_gray);
	}

	cv::Mat drawing = cv::Mat::zeros(sourceImage->size(), CV_8UC1);
	cv::RNG rng(12345);
	for (uint i = 0; i< cvcontours.size(); i++)
	{
		//cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		cv::Scalar color = cv::Scalar(255, 255, 255); //added by CG on March 30th 2020
		cv::drawContours(drawing, cvcontours, i, color, 1, 8, cvhierarchy, 0, cv::Point());
	}

	if (Settings::getSingleton().getBoolValue("logBreakDownImages"))
	{
		imwrite("1st_drawContour.jpg", drawing);
	}


	//insert dilation and erosion
	/*cv:THRESHOLD(backproj, thresh, nBackProjectThreshold, 255, THRESH_BINARY);

	imwrite("After_Dilation.jpg", drawing);

	*/
	int an, n;
	an = 5;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(an * 2 + 1, an * 2 + 1), cv::Point(an, an));
	cv::Mat After_Dilation;
	cv::dilate(drawing, After_Dilation, element);
	
	if (Settings::getSingleton().getBoolValue("logBreakDownImages"))
	{
		imwrite("After_Dilation.jpg", After_Dilation);
	}

	cv::Mat After_Erosion;
	erode(After_Dilation, After_Erosion, element);

	if (Settings::getSingleton().getBoolValue("logBreakDownImages"))
	{
		imwrite("After_Erosion.jpg", After_Erosion);
	}


	cvcontours.clear(); //added by CG on March 30th 2020
	cv::findContours(After_Erosion, cvcontours, cvhierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0)); //added by CG on March 30th 2020

	if (Settings::getSingleton().getBoolValue("logBreakDownImages"))
	{
		imwrite("After_2nd_findContours.jpg", After_Erosion);
	}

	for (uint i = 0; i< cvcontours.size(); i++)
	{
		//cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		cv::Scalar color = cv::Scalar(255, 255, 255); //added by CG on March 30th 2020
		cv::drawContours(After_Erosion, cvcontours, i, color, 1, 8, cvhierarchy, 0, cv::Point());
	}


	if (Settings::getSingleton().getBoolValue("logBreakDownImages"))
	{
		imwrite("After_2nd_drawContours.jpg", After_Erosion);
	}


	//SEL Mar 30 2020 add logging of contours to a text file
	if (Settings::getSingleton().getBoolValue("logTextureContours"))
	{
		FILE *fp;
		fp = fopen("TextureContours.txt", "wt");
		if (!fp)
			return;

		int nContour, nLine;

		fprintf(fp, "Texture Contours for %d Total Contours\n", cvcontours.size());
		for (nContour = 0; nContour < cvcontours.size(); nContour++)
		{
			fprintf(fp, "Contour %d\n", nContour);
			for (nLine = 0; nLine < cvcontours[nContour].size(); nLine++)
			{
				fprintf(fp, "%d,%d,%d\n", nLine, cvcontours[nContour][nLine].x, cvcontours[nContour][nLine].y);
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
}

std::vector<Colour> BodySplitterTexture::pixelColours(double u1, double v1, double u2, double v2) const noexcept
{
  std::vector<Colour> output;
  double x1 = u1*(double)simplifiedImage->cols;
  double x2 = u2*(double)simplifiedImage->cols;
  double y1 = v1*(double)simplifiedImage->rows;
  double y2 = v2*(double)simplifiedImage->rows;
  const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
  if(steep)
  {
    std::swap(x1, y1);
    std::swap(x2, y2);
  }

  if(x1 > x2)
  {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }

  const float dx = x2 - x1;
  const float dy = fabs(y2 - y1);

  float error = dx / 2.0f;
  const int ystep = (y1 < y2) ? 1 : -1;
  int y = (int)y1;

  const int maxX = (int)x2;
  for(int x=(int)x1; x<maxX; x++)
  {
    if(steep)
    {
      assert(x>=0 && y>=0);
      output.push_back(getColour(y%simplifiedImage->cols,x%simplifiedImage->rows));
    }
    else
    {
      assert(x>=0 && y>=0);
      output.push_back(getColour(x%simplifiedImage->cols,y%simplifiedImage->rows));
// 	grid[x][y].liesUpon = line.get();
//         SetPixel(x,y, color);
    }

    error -= dy;
    if(error < 0)
    {
	y += ystep;
	error += dx;
    }
  }
  return output;
  /*
  
	std::vector<Colour> output;
	bool isReversed = false;
	//If line is vertical is special case
	if (u1 == u2)
	{
		unsigned int distance = (v2 - v1)*(double)getYSize();
		int x1 = u1*getXSize();
		int y1 = v1*getYSize();
		for (unsigned int i = 0; i < distance; i++)
		{
			//Easy peasy, y increments by 1 pixel
			output.push_back(getColour(x1, y1 + i));
		}
	}
	else
	{
		if (u2 < u1)
		{
			std::swap(u2, u1);
			std::swap(v2, v1);
			isReversed = true;
		}
		//Bresenham line algorithm
		double deltaX = u2 - u1;
		double deltaY = v2 - v1;
		double error = -1.0;
		double deltaErr = fabs(deltaY / deltaX);//Safe since divide by zero is covered in the first case
		int y = v1*getYSize();
		int x1 = u1*getXSize();
		int x2 = u2*getXSize();
		for (int x = x1; x < x2 - 1; x++)
		{
			output.push_back(getColour(x, y));
			error = error + deltaErr;
			if (error >= 0)
			{
				y += 1;
				error -= 1;
			}
		}
	}
	if (isReversed)
		std::reverse(output.begin(), output.end());
	return output;
	*/
}

void BodySplitterTexture::AverageBlock(cv::Mat* block)
{
	//Find average
	unsigned int r = 0;
	unsigned int g = 0;
	unsigned int b = 0;
	unsigned int count = 0;
	for (int x = 0; x < block->cols; x++)
	{
		for (int y = 0; y < block->rows; y++)
		{
			if (block->at<cv::Vec3b>(y,x)[0] > 0 ||
				block->at<cv::Vec3b>(y,x)[1] > 0 ||
				block->at<cv::Vec3b>(y,x)[2] > 0)
			{
				b += block->at<cv::Vec3b>(y,x)[0];
				g += block->at<cv::Vec3b>(y,x)[1];
				r += block->at<cv::Vec3b>(y,x)[2];
				count++;
			}
		}
	}
	r = (float)r / (float)count;
	g = (float)g / (float)count;
	b = (float)b / (float)count;

	for (int x = 0; x < block->cols; x++)
	{
		for (int y = 0; y < block->rows; y++)
		{
			if (block->at<cv::Vec3b>(y,x)[0] > 0 ||
				block->at<cv::Vec3b>(y,x)[1] > 0 ||
				block->at<cv::Vec3b>(y,x)[2] > 0)
			{
				block->at<cv::Vec3b>(y,x)[0] = b;
				block->at<cv::Vec3b>(y,x)[1] = g;
				block->at<cv::Vec3b>(y,x)[2] = r;
			}
		}
	}
}
 
//This function is slow
// SEL added all the Comments and I hope they are correct
//
//  LineCrossesContour
//
// This method will take the inputLine perimeter and split it if-and-where it crosses the image contour lines (colour segmentation contours)
// The intersections are then sorted to be ordered along the length of the input line
// the contour lines have been created using OpenCV methods for segmentation of the image colours of the simplifiedd texture image.
//
// Returns true if intersections are found
// Outputs split lines in the Line vector splitResult
//
bool BodySplitterTexture::LineCrossesContour(const Line &inputLine, std::vector<Line> &splitResult) const noexcept
{


	/* SEL  let's set u and v for debug purposes
	cv::Point pt1(inputLine.pt1.u*simplifiedImage->cols, inputLine.pt1.v*simplifiedImage->rows);
	cv::Point pt2(inputLine.pt2.u*simplifiedImage->cols, inputLine.pt2.v*simplifiedImage->rows);   */

	double u, v;

	// Map the perimeter input line u,v points 1 and 2 into image space
	u = inputLine.pt1.u*((double)simplifiedImage->cols);
	v = inputLine.pt1.v*((double)simplifiedImage->rows);
	cv::Point pt1(u,v);   

	if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting"))
	{
		std::cout << "Pt1 world coordinate (" << inputLine.pt1.x << ", " << inputLine.pt1.y << ", " << inputLine.pt1.z << ") uv space (" << inputLine.pt1.u << ", " << inputLine.pt1.v; 
		std::cout << ")  image space (u,v) (" << pt1.x << ", " << pt1.y << ")\n";
	}//SEL debug

	u = inputLine.pt2.u*((double)simplifiedImage->cols);
	v = inputLine.pt2.v*((double)simplifiedImage->rows);
	cv::Point pt2(u, v);
	if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting"))
	{
		std::cout << "Pt2 world coordinate (" << inputLine.pt2.x << ", " << inputLine.pt2.y << ", " << inputLine.pt2.z << ")  uv space (" << inputLine.pt2.u << ", " << inputLine.pt2.v;
		std::cout << ")  image space (u,v) (" << pt2.x << ", " << pt2.y << ")\n";
	}//SEL debug

	// if pt1 equals pt2 then this is a zero length line so we can return false as no intersection is possible
	if (pt1 == pt2)  
	{
		if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting"))
		{
			std::cout << "Pt1 = Pt2 so return\n";
		}//SEL debug
		return false;
	}


	// Hold on there cowboy!  This is not the place to do the following check.  I think we have to add a check a level above when we have the entire perimeter split
	//  Let's remove small blocks of colour at that point if they do not have enough coverage to produce a viable island with the current nozzle size, keeping in mind that 
	//  islands are only created horizontaly or verticaly if a pixel is cover in colourGrid.   
#if 0
	double dNozzleD, dLen;
	//SEL  lets make sure the line is at least 3 times longer than the nozzle before we split it
	// my thinking here is that if you can't put down a single perimeter filament for each colour, then why bother


	dNozzleD = Settings::GetSingleton().getDoubleValue("nozzleD") * 3.;
	dLen = inputLine.length();
	if (dLen < dNozzleD)
	{
		if (gl_nSEL && Settings::getSingleton().getBoolValue("logLineSplitting"))
		{
			std::cout << "This Line is too short to bother splitting " << dLen << " mm\n";
		}//SEL debug
		return false;
	}
#endif



	int intersectionCount = 0;
	double outIntersectionX = -1;				 // initialize outIntersection point to -1 to indicate no intersection found
	double outIntersectionY = -1;
	//SEL removed - double lastIntersectionX = inputLine.pt1.u;  // this isn't currently used anywhere so I am removing it
	//SEL removed - double lastIntersectionY = inputLine.pt1.v;
	
	std::vector<std::pair<double, double>> intersectionPts;  //this is a vector to hold the candidate intersection points
	
	//
	// loop through all image contours testing the lines between consectutive points.   
	// Test if the given perimeter line intersects any of the image contour lines
	//
	for (uint j = 0; j < cvcontours.size(); j++)  //For all the contours in the image (j contours)
	{
		int x1 = cvcontours[j].front().x;  //set x1,y1 to the very first point in this contour
		int y1 = cvcontours[j].front().y;
		for (uint k = 1; k < cvcontours[j].size(); k++) // for all remaining points in this contour (k points)
		{

			int nextX = cvcontours[j][k].x;	//set nextX,Y to subsequent point along contour.  This will create a line (x1,y1),(nextX,nextY)
			int nextY = cvcontours[j][k].y;

			// Test if the input line in image space intersects the contour[j] line through image countor points indexed at (k-1) and (k)
			if (BodySplitter::lineIntersection(pt1.x, pt1.y, pt2.x, pt2.y,
				x1, y1, nextX, nextY,
				outIntersectionX, outIntersectionY) == 1)
			{
				//intersection is found
				intersectionCount++;
				outIntersectionX /= simplifiedImage->cols;  // convert intersection point back into uv space
				outIntersectionY /= simplifiedImage->rows;
				intersectionPts.push_back(std::pair<double, double>(outIntersectionX, outIntersectionY));  // add this intersection point to the vector

				//SEL removed - lastIntersectionX = outIntersectionX;  // record this point as the lastIntersection ??? why ???  it doesn't seem to be used anywhere
				//SEL removed - lastIntersectionY = outIntersectionY;
				if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting"))
				{
					std::cout << "Intersection " << intersectionCount << " Found at contour " << j << " Line # " << k - 1 << " uv coordinate (" << outIntersectionX << ", " << outIntersectionY << ")\n";
				}//SEL debug


			}// if intersection found

			x1 = nextX; y1 = nextY;  // now advance x1,y1 to the next point along the contour to create a new line
		}// for all points in this contour
	}// for all contours

	if (intersectionPts.size() == 0)
	{
		if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting"))
		{
			std::cout << "No Intersections Found. ";
		}// SEL debug
		return false;
	}

	//Arrange intersection points along line...
	// 
	//  We must have the split lines form a continous path from the starting point to the end point without ever reversing or overlapping.
	//  This is done by sorting the intersection points either ascending or descending along the u or v axis, whichever axis is traversed
	//  the most.
	//
	//  Let's first determine which axis the line mostly follows, u or v
	//  Then determine if we are ascending or descending
	//
	int bSortByU;
	int bAscending;
	
	bSortByU = (fabs(inputLine.pt2.u - inputLine.pt1.u) > fabs(inputLine.pt2.v - inputLine.pt1.v));
	if (bSortByU)
	{
		bAscending = (inputLine.pt2.u > inputLine.pt1.u);
	}
	else
	{
		bAscending = (inputLine.pt2.v > inputLine.pt1.v);
	}

	//
	// Now we are ready to sort the intersection points
	// yee haw!
	//
	// I will use the same loop for sorting by u or v and up or down
	//
	bool isSorted = false;
	while (!isSorted)
	{
		isSorted = true;
		for (uint i = 0; i < intersectionPts.size() - 1; i++)
		{
			if(    ( bSortByU &&  bAscending && (intersectionPts[i].first  > intersectionPts[i + 1].first)) 
				|| ( bSortByU && !bAscending && (intersectionPts[i].first  < intersectionPts[i + 1].first))
				|| (!bSortByU &&  bAscending && (intersectionPts[i].second > intersectionPts[i + 1].second))
				|| (!bSortByU && !bAscending && (intersectionPts[i].second < intersectionPts[i + 1].second)))
			{
				isSorted = false;
				std::swap(intersectionPts[i], intersectionPts[i + 1]);
			}
		}//for
	}//while

	//let's go through the points and eliminate identical ones here.  These will cause 0 length lines that have to be eliminated later 
	//if we don't.  Why do we have identical points anyway.  Maybe I will figure this out when I investigate the K Means contour segmentation thingy
	int nIdenticalPoints = 0;
	if (intersectionPts.size() > 1)
	{
		for (uint i = 0; i < intersectionPts.size() - 1; i++)
		{
			//!!!SEL  I should change the magic tolerance from 0.0001 to a define 
			if ((pow(intersectionPts[i].first - intersectionPts[i + 1].first, 2) +
				pow(intersectionPts[i].second - intersectionPts[i + 1].second, 2))
				         < (0.0001*0.0001))  // If I simply square the tolerance then I can avoid the sqrt operation in the distance check
			{
				intersectionPts.erase(intersectionPts.begin() + i + 1);
				nIdenticalPoints++;
			}
		}//for
	}//if 2 or more points

	//
	//  This would be a good place to log the result
	//
	if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting"))
	{
		std::cout << nIdenticalPoints << " Identical points removed from intersecton list\n";
		std::cout << "Intersections Found " << intersectionPts.size() << (bSortByU ? " sorted by U " : " sorted by V ") << (bAscending ? " in ascending order\n" : "in descending order\n");
		for (uint i = 0; i < intersectionPts.size(); i++)
		{
			std::cout << "Point " << i << " = (" << intersectionPts[i].first << ", " << intersectionPts[i].second << ")\n";
		}// SEL debug
	}


	// The following implementation that Robert made inherently uses a floating point comparison to see it pt1.u = pt2.u.  This can have unintended consequences
	// due to rounding errors and numerical precision.   If the line is parallel to the u axis it may not detect that and the points will not be arranged properly 
	// along the line
#if 0
	//Arrange intersection points along line...
	// I bet we should 
	// find the largest delta between pt1 and pt2 in the u or the v coordinates and then sort by either the u or the v accordingly

	//if the perimeter input line pt2 u coordinate is larger than pt1 u coordinate, then sort all the intersections in ascending u order
	if (inputLine.pt2.u > inputLine.pt1.u)
	{
		//U coords ascending
		bool isSorted = false;
		while (!isSorted)
		{
			isSorted = true;
			for (uint i = 0; i < intersectionPts.size() - 1; i++)
			{
				if (intersectionPts[i].first > intersectionPts[i + 1].first)
				{
					isSorted = false;
					std::swap(intersectionPts[i], intersectionPts[i + 1]);
				}
			}
		}
	}
	// else if the perimeter input line pt2 u coordinate is smaller than pt1 u coordinate, then sort all the intersections in descending u order
	else if (inputLine.pt2.u < inputLine.pt1.u)
	{
		//U coords descending
		bool isSorted = false;
		while (!isSorted)
		{
			isSorted = true;
			for (uint i = 0; i < intersectionPts.size() - 1; i++)
			{
				if (intersectionPts[i].first < intersectionPts[i + 1].first)
				{
					isSorted = false;
					std::swap(intersectionPts[i], intersectionPts[i + 1]);
				}
			}
		}
	}
	// else the pt1.u must equal pt2.u so lets test the v  coordinates
	// should we use a threshold here because doing an equal on a floating point value is dangerous
	// I bet rounding errors might be the cause of the sorting bug.
	else if (inputLine.pt2.v < inputLine.pt1.v)
	{
		//V coords ascending
		bool isSorted = false;
		while (!isSorted)
		{
			isSorted = true;
			for (uint i = 0; i < intersectionPts.size() - 1; i++)
			{
				if (intersectionPts[i].second < intersectionPts[i + 1].second)
				{
					isSorted = false;
					std::swap(intersectionPts[i], intersectionPts[i + 1]);
				}
			}
		}
	}
	else if (inputLine.pt2.v > inputLine.pt1.v)
	{
		//V coords descending
		bool isSorted = false;
		while (!isSorted)
		{
			isSorted = true;
			for (uint i = 0; i < intersectionPts.size() - 1; i++)
			{
				if (intersectionPts[i].second > intersectionPts[i + 1].second)
				{
					isSorted = false;
					std::swap(intersectionPts[i], intersectionPts[i + 1]);
				}
			}
		}
	}
#endif


	// now we will filter all the intersection points and remove any that are not on the actual input line
	// why would this occur?
	// we will create colinearLines from the original pt1 through all the sorted intersection points and add them to the split line vector
	if (intersectionCount > 0)
	{

		double firstU = inputLine.pt1.u;
		double firstV = inputLine.pt1.v;
	
		for (std::size_t i = 0; i < intersectionPts.size(); i++)
		{
			//Check if coord is actually on the line
			if (sqrt((inputLine.pt2.u - inputLine.pt1.u)*(inputLine.pt2.u - inputLine.pt1.u) +
				(inputLine.pt2.v - inputLine.pt1.v)*(inputLine.pt2.v - inputLine.pt1.v)) <
				sqrt((intersectionPts[i].first - inputLine.pt1.u)*(intersectionPts[i].first - inputLine.pt1.u) +
				(intersectionPts[i].second - inputLine.pt1.v)*(intersectionPts[i].second - inputLine.pt1.v)))
			{
				continue;
			}
			Line newLine = inputLine.getColinearLine(firstU, firstV, intersectionPts[i].first, intersectionPts[i].second);
			splitResult.push_back(newLine);
			firstU = intersectionPts[i].first; firstV = intersectionPts[i].second;
		}

		splitResult.push_back(inputLine.getColinearLine(firstU, firstV, inputLine.pt2.u, inputLine.pt2.v));
	}
	return intersectionCount>0;
}

Colour BodySplitterTexture::getColour(double u, double v) const noexcept
{
	int xPos = u*simplifiedImage->cols;
	int yPos = v*simplifiedImage->rows;

	xPos = std::min(xPos, simplifiedImage->cols);
	yPos = std::min(yPos, simplifiedImage->rows);

	Colour newColour((*simplifiedImage).at<cv::Vec3b>(yPos, xPos));
	return newColour;
}

Colour BodySplitterTexture::getColour(int x, int y) const noexcept
{
	int xPos = std::min(x, simplifiedImage->cols);
	int yPos = std::min(y, simplifiedImage->rows);
	xPos = std::max(0, xPos);
	yPos = std::max(0, yPos);
	Colour newColour((*simplifiedImage).at<cv::Vec3b>(yPos, xPos));

	return newColour;
}

unsigned char* BodySplitterTexture::getRawImageData()
{
	if (simplifiedImage)
		return simplifiedImage->data;
	else
		return sourceImage->data;
}

unsigned int BodySplitterTexture::getXSize() const noexcept
{
	if (simplifiedImage)
		return simplifiedImage->cols;
	else
		return sourceImage->cols;
}

unsigned int BodySplitterTexture::getYSize() const noexcept
{
	if (simplifiedImage)
		return simplifiedImage->rows;
	else
		return sourceImage->rows;
}

unsigned int BodySplitterTexture::getTargetXSize() const noexcept
{
	return targetXRes;
}

unsigned int BodySplitterTexture::getTargetYSize() const noexcept
{
	return targetYRes;
}

#ifdef COASTERTEXT
void CoasterPrintTexture::breakDownImage(double width, double height)
{
	//This function is garbage
	polys.clear();
	std::unordered_map<size_t, std::pair<cv::Vec3b,cv::Mat>> colourBreakDown;
	for (int x = 0; x < simplifiedImage->cols; x++)
	{
		for (int y = 0; y < simplifiedImage->rows; y++)
		{
			if (colourBreakDown.count(hashRGB(simplifiedImage->at<cv::Vec3b>(y, x))) == 0)
			{
				std::pair<cv::Vec3b, cv::Mat> newBlock;
				newBlock.first = simplifiedImage->at<cv::Vec3b>(y, x);
				newBlock.second = cv::Mat::zeros(simplifiedImage->size(), simplifiedImage->type());
				colourBreakDown.insert(std::pair<std::size_t, std::pair<cv::Vec3b, cv::Mat>>(hashRGB(simplifiedImage->at<cv::Vec3b>(y, x)),
					newBlock));
				//colourBreakDown[hashRGB(simplifiedImage->at<cv::Vec3b>(y, x))] = newBlock;
			}
			colourBreakDown[hashRGB(simplifiedImage->at<cv::Vec3b>(y, x))].second.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
			//colourBreakDown[hashRGB(simplifiedImage->at<cv::Vec3b>(y, x))].first = simplifiedImage->at<cv::Vec3b>(y, x);
		}
	}
	std::vector<Perimeter> islands;
	for (auto it = colourBreakDown.begin(); it != colourBreakDown.end(); ++it)
	{
		cv::Mat src_gray;
		cv::cvtColor(it->second.second, src_gray, CV_BGR2GRAY);
		//std::vector<std::vector<cv::Point> > contours;
		//std::vector<cv::Vec4i> hierarchy;
		//Canny(src_gray, src_gray, 20, 20 * 2, 3);
		//cv::edgePreservingFilter(*simplifiedImage, *simplifiedImage, cv::RECURS_FILTER, 50, 0.25);
		
		std::vector<std::vector<cv::Point> > morecvcontours;
		std::vector<cv::Vec4i> morecvhierarchy;
		findContours(src_gray, morecvcontours, morecvhierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
		std::vector<Perimeter> nextIslandSet;
		getIslands(nextIslandSet,morecvcontours, morecvhierarchy,0,true,width,height);
		for (std::size_t i = 0; i < nextIslandSet.size(); i++)
			nextIslandSet[i].setColour(Colour(it->second.first));
		islands.insert(islands.end(), nextIslandSet.begin(), nextIslandSet.end());
		//cvcontours.insert(cvcontours.end(), morecvcontours.begin(), morecvcontours.end());

	}
	//Perimeter::generateExRSPolygons(polys, islands);
	arrangePolys(islands);
	cv::Mat drawing = cv::Mat::zeros(sourceImage->size(), CV_8UC3);
	cv::RNG rng(12345);
	for (uint i = 0; i< cvcontours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, cvcontours, i, color, 2, 8, cvhierarchy, 0, cv::Point());

	}
	generateTexture(&drawing, contourTexture);
}
#endif

void CoasterPrintTexture::getIslands(std::vector<Perimeter> &islands,const std::vector<std::vector<cv::Point> > &contours, const std::vector<cv::Vec4i> &hierarchy, int index, bool odd,
	double widthX, double heightY)
{
	//double widthX = Settings::GetSingleton().getDoubleValue("coasterSize");
	//double heightY = Settings::GetSingleton().getDoubleValue("coasterSize");
	//Odd even ordering
	if (odd)
	{
		//Find all siblings and make into perimeters
		if(contours[index].size() >= 3)
			islands.push_back(Perimeter(contours[index],getXSize(),getYSize(),widthX,heightY));
		int nextSibling = hierarchy[index][0];
		//while (nextSibling >= 0)
		if(nextSibling >= 0)
			getIslands(islands, contours, hierarchy, nextSibling, odd,widthX,heightY);
	}
	else
	{
		//do nothing????
		int nextSibling = hierarchy[index][0];
		//while (nextSibling >= 0)
		if (nextSibling >= 0)
			getIslands(islands, contours, hierarchy, nextSibling, odd, widthX, heightY);
	}
	int nextChild = hierarchy[index][2];
	while (nextChild >= 0)
	{
		getIslands(islands, contours, hierarchy, nextChild, !odd, widthX, heightY);
		nextChild = hierarchy[nextChild][2];
	}
}

void CoasterPrintTexture::arrangePolys(std::vector<Perimeter> &islands)
{
	for (std::size_t i = 0; i < islands.size(); i++)
	{
		std::shared_ptr<ExRSPolygon> newPoly = std::make_shared<ExRSPolygon>(iSlicable::ToolType::FULL_COLOUR);
		newPoly->setContour(islands[i]);
		for (std::size_t j = 0; j < islands.size(); j++)
		{
			if (j == i)
				continue;
			if (islands[j].isWithin(islands[i]))
				newPoly->addHole(islands[j]);
		}
		auto insetNozzleWidth = newPoly->straightSkeletonColours(false); // This function will simply reset the polygon size if the perim is all solid colour
		polys.insert(polys.end(), insetNozzleWidth.begin(), insetNozzleWidth.end());
		//polys.push_back(newPoly);
	}
}
/*
void CoasterPrintTexture::adjustColour(uint16_t colours)
{
	colourCount = colours;
	if (simplifiedImage)
		delete simplifiedImage;
	simplifiedImage = new cv::Mat(*sourceImage);

	//K-means: http://qtandopencv.blogspot.ca/2013/10/opencv-and-color-quantization-01-kmeans.html
	if(GET_BOOL("useKMeans"))
	{
		cv::Mat samples(sourceImage->total(), 3, CV_32F);
		auto samples_ptr = samples.ptr<float>(0);
		for (int row = 0; row != sourceImage->rows; ++row) {
			auto src_begin = sourceImage->ptr<uchar>(row);
			auto src_end = src_begin + sourceImage->cols * sourceImage->channels();
			//auto samples_ptr = samples.ptr<float>(row * src.cols);
			while (src_begin != src_end) {
				samples_ptr[0] = src_begin[0];
				samples_ptr[1] = src_begin[1];
				samples_ptr[2] = src_begin[2];
				samples_ptr += 3; src_begin += 3;
			}
		}
		int clusterCount = colours;
		cv::Mat labels;
		int attempts = GET_DOUBLE("kMeansAttemps");
		cv::Mat centers;
		cv::kmeans(samples, clusterCount, labels,
			cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS,
				10, 0.01),
			attempts, cv::KMEANS_PP_CENTERS, centers);

		//step 3 : map the centers to the output
		cv::Mat new_image(sourceImage->size(), sourceImage->type());
		for (int row = 0; row != sourceImage->rows; ++row) {
			auto new_image_begin = new_image.ptr<uchar>(row);
			auto new_image_end = new_image_begin + new_image.cols * 3;
			auto labels_ptr = labels.ptr<int>(row * sourceImage->cols);

			while (new_image_begin != new_image_end) {
				int const cluster_idx = *labels_ptr;
				auto centers_ptr = centers.ptr<float>(cluster_idx);
				new_image_begin[0] = centers_ptr[0];
				new_image_begin[1] = centers_ptr[1];
				new_image_begin[2] = centers_ptr[2];
				new_image_begin += 3; ++labels_ptr;
			}
		}

		*simplifiedImage = new_image;
	}
	if (GET_BOOL("useKMeans"))
		populateColourVector(colours);
	else
		populateColourVector(-1);
	generateTexture(simplifiedImage, simplifiedTexture);
}
*/
/*
void CoasterPrintTexture::breakDownImage()
{
	cvcontours.clear();
	std::unordered_map<std::size_t,cv::Mat> colourMat;
	for (int x = 0; x < simplifiedImage->cols; x++)
	{
		for (int y = 0; y < simplifiedImage->rows; y++)
		{
			std::size_t hash = hashRGB(simplifiedImage->at<cv::Vec3b>(y, x));
			if (colourMat.count(hash) == 0)
			{
				colourMat[hash].create(simplifiedImage->size(), CV_8UC1);
				colourMat[hash] = cv::Scalar(0);
			}
			colourMat[hash].at<uchar>(y,x) = 255;
		}
	}
	for (auto it : colourMat)
	{
		std::vector<std::vector<cv::Point> > contourLoop;
		std::vector<cv::Vec4i> hierarchyLoop;
		cv::Mat bordered = cv::Mat::zeros(cv::Size(it.second.rows +4, it.second.cols + 4), it.second.type());
		cv::copyMakeBorder(it.second, bordered, 2, 2, 2, 2, cv::BORDER_CONSTANT, cv::Scalar());
		cv::findContours(bordered, contourLoop, hierarchyLoop, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
		for (int i = 0; i < contourLoop.size(); i++)
		{
			for (int j = 0; j < contourLoop[i].size(); j++)
			{
				contourLoop[i][j].x -= 2;
				contourLoop[i][j].y -= 2;
			}
		}
		cvcontours.insert(cvcontours.end(), contourLoop.begin(), contourLoop.end());
	}
}
#endif
*/