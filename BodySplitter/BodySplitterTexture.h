#pragma once
/* holds the texture for meshes and coasters */
#include <opencv2/core/core.hpp>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <unordered_set>

class ExRSPolygon;
class Perimeter;
class Line;
class Colour;
typedef unsigned int GLuint;
class BodySplitterTexture
{
protected:
	uint16_t k_meansColourSet;
	uint16_t colourCount;
	struct ColourBlock {
		cv::Mat colorMap;// = cv::Mat::zeros(output.size(), output.type());
		cv::Vec3b baseColour;// = output.at<cv::Vec3b>(0, 0);
		unsigned int pixelCount;
		std::vector<std::vector<cv::Point> > contours;
		std::vector<cv::Vec4i> hierarchy;
	};
	unsigned int targetXRes, targetYRes;
	std::string m_fileName;

	std::unordered_set<uint32_t> colourSet;
	std::vector<ColourBlock> colours;
	std::vector<ColourBlock> outputBlocks;

	std::vector<std::vector<cv::Point> > cvcontours;
	std::vector<cv::Vec4i> cvhierarchy;

	//std::vector<Contour> subPixContours;
	bool needsRegenSimplified = false;	//Simplified texture requires regenerating
	bool isBase2 = true;
	int sourceX, sourceY;
	int openGLX, openGLY;
	cv::Mat* sourceImage;
	cv::Mat* simplifiedImage;
	cv::Mat* contours;
	GLuint sourceTexture;
	GLuint simplifiedTexture;
	GLuint contourTexture;
	bool isOpen;
	bool isSimple = false;
	void generateTexture(const cv::Mat* source, GLuint &index);
	virtual void breakDownImage();
	void AverageBlock(cv::Mat* block);
	void colorReduce(cv::Mat& image, int div);
	//Fills the colour vector, optionally stops at haltAt
	void populateColourVector(int haltAt=-1);
	virtual bool isCoaster() {
		return false;
	};
public:
	BodySplitterTexture(const std::string &fullFilePath);
	BodySplitterTexture();
	const std::string &getFileName() const {
		return m_fileName;
	}
	void bindTexture(bool simplified);
	void makeSimplified(unsigned int colours, unsigned int res);
	virtual void adjustColour(uint16_t newColours);
	virtual ~BodySplitterTexture();
	bool openImage(const std::vector<unsigned char> &textureBuffer);
	bool openImage(const std::string &fullPath);
	bool isGood() const {
		return isOpen;
	}
	bool isSimplified() const {
		return isSimple;
	}
	void removeSimplified();
	bool LineCrossesContour(const Line &inputLine, std::vector<Line> &splitResult) const noexcept;
	Colour getColour(double u, double v) const noexcept;
	Colour getColour(int x, int y) const noexcept;
	const std::vector<std::vector<cv::Point> >& getContours() { return cvcontours; }
	const std::vector<cv::Vec4i> & getContourHierarchy() { return cvhierarchy; }
	unsigned int getXSize() const noexcept;
	unsigned int getYSize() const noexcept;
	unsigned int getSourceXSize() const noexcept {
		return sourceX;
	}
	unsigned int getSourceYSize() const noexcept {
		return sourceY;
	}
	unsigned int getTargetXSize() const noexcept;
	unsigned int getTargetYSize() const noexcept;

	std::vector<Colour> pixelColours(double u1, double v1, double u2, double v2) const noexcept;

	//Get the total list of colours present in the texture
	std::unordered_set<uint32_t> getColours();

	void adjustColoursAndRes(unsigned int newColours, int x, int y);

	void resizeSimplifiedImage(unsigned int x, unsigned int y);

	unsigned char* getRawImageData();

	std::vector<uchar> exportImage(const std::string &format);
};
#define COASTERTEXT
class CoasterPrintTexture : public BodySplitterTexture
{
protected:
	std::list<std::shared_ptr<ExRSPolygon>> polys;
	void arrangePolys(std::vector<Perimeter> &islands);
	void getIslands(std::vector<Perimeter> &islands,const std::vector<std::vector<cv::Point> > &contours, const std::vector<cv::Vec4i> &hierarchy,int index, bool odd, double widthX, double heightY);
	virtual bool isCoaster() {
		return true;
	};
public:
	//void adjustColour(uint16_t colours);
	CoasterPrintTexture(const std::string &fullFilePath) : BodySplitterTexture(fullFilePath) {};
	CoasterPrintTexture() {

	};
	virtual ~CoasterPrintTexture() {};
	const std::list<std::shared_ptr<ExRSPolygon>>& getPolys() {
		return polys;
	}
#ifdef COASTERTEXT
	void breakDownImage(double width, double height);
#endif

	
};