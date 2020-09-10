#pragma once

#ifdef _WIN32
#include <Windows.h>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <GL/glut.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#define TURNTABLE
#define TRACKBALL_SIZE 0.8

class TrackBall
{
public:
	TrackBall(float radius, int screenWidth, int screenHeight);
	~TrackBall();

	glm::vec3 getVector(int x, int y) const;
private:
	glm::vec3 getVectorWithArc(float x, float y) const;
	float clampX(float x) const;                // -halfScreenWidth < x < halfScreenWidth
	float clampY(float y) const;                // -halfScreenHeight < y < halfScreenHeight

	float radius;
	int   screenWidth;
	int   screenHeight;
	float halfScreenWidth;
	float halfScreenHeight;
};


class CameraControl
{
private:
	glm::quat  cameraQuat;
	mutable glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	mutable glm::vec3 upVector;
	double pitch;
	double yaw;
	double m_zoom;
	double trackBallSize = 2.0;
	glm::vec3 getRightVec() const;
	glm::vec3 getDirectionVec() const;
	glm::vec3 getUpVec() const;
public:
	glm::vec3 getPos() const {
		return cameraPos;
	}
	glm::quat getQuat() const;
	void setQuat(const glm::quat &quat);
	CameraControl();
	/* x1..y2 are screen coords*/
	void pan(int x1, int y1, int x2, int y2) noexcept;
	void rotate(double horizontal, double vertical);
	void look();
	void zoom(double delta);
	void mouseToWorld(int x, int y, double &outX, double &outY);
	void resetCamera();
	~CameraControl();
	
	  GLdouble modelview[16];                 // Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
// 	  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);       // Retrieve The Modelview Matrix
	  GLdouble projection[16];                // Where The 16 Doubles Of The Projection Matrix Are To Be Stored
// 	  glGetDoublev(GL_PROJECTION_MATRIX, projection);     // Retrieve The Projection Matrix
	  
	  GLint viewport[4];
};
