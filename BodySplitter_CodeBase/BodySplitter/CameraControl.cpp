#include "CameraControl.h"
#include "Settings.h"
#include <glm/geometric.hpp>
#include <glm/matrix.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


TrackBall::TrackBall(float r, int sw, int sh) : radius(r),screenWidth(sw),screenHeight(sh)
{
	halfScreenWidth = (double)sw/2.0;
	halfScreenHeight = (double)sh/2.0;
	radius = std::min(sw,sh);
}

TrackBall::~TrackBall()
{

}

glm::vec3 TrackBall::getVector(int x, int y) const
{
  //std::cout << "X:"<<x<<" Y:" <<y <<"\n";
	float mx = x - halfScreenWidth;
	float my = halfScreenHeight - y;    // OpenGL uses bottom to up orientation
	return getVectorWithArc(mx, my);
}

glm::vec3 TrackBall::getVectorWithArc(float x, float y) const
{
	float arc = sqrtf(x*x + y*y);   // legnth between cursor and screen center
	float a = arc / radius;         // arc = r * a
	float b = atan2f(y, x);         // angle on x-y plane
	float x2 = radius * sinf(a);    // x rotated by "a" on x-z plane

	glm::vec3 vec;
	vec.x = x2 * cosf(b);
	vec.y = x2 * sinf(b);
	vec.z = radius * cosf(a);

	return vec;
}

bool intersectPlane(const glm::vec3 &n, const glm::vec3 &p0, const glm::vec3 &l0, const glm::vec3 &l, float &t)
{
  // assuming vectors are all normalized
 
  float denom =  glm::dot(n,l);
  if (denom > 1e-6) {
    glm::vec3 p0l0 = p0 - l0;
    t =  glm::dot(p0l0, n) / denom;
    return (t >= 0);
  }

  return false;
} 

bool intersectXYPlane(const glm::vec3 &l0, const glm::vec3 &l, float &t, glm::vec3 &xy)
{
  //l0 is the camera position
  //Solve l0 +l*t.z = 0;
  if(l.z == 0)
    return false;
  
  t = -l0.z/l.z;
  xy.x = l0.x+l.x*t;
  xy.y = l0.y+l.y*t;
  xy.z = 0;
  return true;
}

void CameraControl::mouseToWorld(int x, int y, double& outX, double& outY)
{
#ifndef CONSOLE
    //Find a vector for pt1, and pt2
GLdouble posX, posY, posZ;
  glGetIntegerv( GL_VIEWPORT, viewport );
  
 /* gluUnProject(x1, y1, 0, modelview,
    projection, viewport, &posX,&posY,&posZ);
    */
//  std::cout << "CameraPos: ("<<cameraPos.x<<","<<cameraPos.y<<","<<cameraPos.z<<")\n";
  glm::vec3 l0(cameraPos);
  gluUnProject(x, y, 1, modelview,
    projection, viewport, &posX,&posY,&posZ);
  glm::vec3 l(posX,posY,posZ);
  l -= l0;
  glm::vec3 xyIntersect1;
  float t;
  if(intersectXYPlane(l0,l,t,xyIntersect1))
  {
  outX = xyIntersect1.x;
  outY = xyIntersect1.y;
  }
  else
  {
    outX =0; outY = 0;
  }
#endif
}

CameraControl::CameraControl()
{
	this->resetCamera();
}

CameraControl::~CameraControl()
{

}

void CameraControl::zoom(double delta)
{
  m_zoom-=delta;
//   std::cout << "ZOOM:"<<m_zoom;
  std::min(0.0,m_zoom);
}


void CameraControl::rotate(double horizontal, double vertical)
{
	horizontal = -horizontal;
	vertical = -vertical;
	yaw -= (horizontal)*TRACKBALL_SIZE;
	//$self->_sphi($self->_sphi + ($pos->x - $orig->x) * TRACKBALLSIZE);
	pitch += vertical*TRACKBALL_SIZE;
	pitch = std::min(179.5,pitch);
	pitch = std::max(pitch,0.5);
}

void CameraControl::resetCamera()
{
	m_zoom = -2000;
	pitch = 135.0;
	yaw = 225.0;

	cameraTarget.x = SETTINGS_DOUBLE("cameraOriginX");
	cameraTarget.y = SETTINGS_DOUBLE("cameraOriginY");
	cameraTarget.z = -SETTINGS_DOUBLE("cameraOriginZ");

	cameraPos.x = cameraTarget.x + cos(yaw)*m_zoom;
	cameraPos.y = cameraTarget.y + sin(yaw)*m_zoom;
	cameraPos.z = SETTINGS_DOUBLE("cameraZ");

	upVector.x = 0;
	upVector.y = 0;
	upVector.z = 1;
}

void CameraControl::look()
{
#ifndef CONSOLE
#ifdef TURNTABLE
  glm::vec3 tempTarget;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	yaw = fmod(yaw,360.0);
	pitch = fmod(pitch,360.0);
	tempTarget.x = (float)m_zoom*(float)sin(AI_DEG_TO_RAD(yaw))*sin(AI_DEG_TO_RAD(-pitch));
	tempTarget.y = (float)m_zoom*(float)cos(AI_DEG_TO_RAD(yaw))*sin(AI_DEG_TO_RAD(-pitch));
	tempTarget.z = (float)m_zoom*(float)cos(AI_DEG_TO_RAD(pitch));
	//glTranslatef(tempTarget.x+cameraTarget.x, tempTarget.y+cameraTarget.y, tempTarget.z+cameraTarget.z );
	cameraPos = tempTarget+cameraTarget;
	
	gluLookAt(cameraPos.x,cameraPos.y,cameraPos.z,
	  cameraTarget.x,cameraTarget.y,cameraTarget.z,
	   0,0,1
	);
#else
	glPushMatrix();

	// tramsform camera
	glm::mat4 tempmat;
	//tempmat = glm::translate(tempmat,glm::vec3(cameraPos.x,cameraPos.y,0));
	
	tempmat *= glm::toMat4(cameraQuat);
	
	tempmat = glm::transpose(tempmat);
	tempmat = glm::translate(tempmat,glm::vec3(cameraTarget.x,cameraTarget.y,cameraTarget.z));
	//tempmat = glm::translate(tempmat, glm::vec3(0, 0, (float)-m_zoom));
	//tempmat = glm::translate(tempmat, glm::vec3(0, 0, -10.0f));
	glMultMatrixf(&tempmat[0][0]);
	//gluPerspective(m_zoom, 1, 1, 1000);
	return;
#endif
#endif
}

glm::quat CameraControl::getQuat() const
{
	return cameraQuat;
}

void CameraControl::setQuat(const glm::quat &q)
{
	cameraQuat = q;
}



void CameraControl::pan(int x1, int y1, int x2, int y2) noexcept
{
  //Find a vector for pt1, and pt2
//   GLdouble modelview[16];                 // Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
//   glGetDoublev(GL_MODELVIEW_MATRIX, modelview);       // Retrieve The Modelview Matrix
//   GLdouble projection[16];                // Where The 16 Doubles Of The Projection Matrix Are To Be Stored
//   glGetDoublev(GL_PROJECTION_MATRIX, projection);     // Retrieve The Projection Matrix
//   GLdouble posX, posY, posZ;
//   GLint viewport[4];
//   glGetIntegerv( GL_VIEWPORT, viewport );
//  
//  cameraPos = cameraTarget;
  //cameraPos.z += 10;
#ifndef CONSOLE
  glm::vec3 newCameraTarget(cameraTarget);
#ifdef TURNTABLE
  glm::vec3 l0(cameraPos);
  GLdouble posX, posY, posZ;
  gluUnProject(x1, y1, 1, modelview,
	  projection, viewport, &posX, &posY, &posZ);

  glm::vec3 l(posX, posY, posZ);
  l -= l0;
  glm::vec3 xyIntersect1;
  float t;
  if (intersectXYPlane(l0, l, t, xyIntersect1))
  {
	  glm::vec3 xyIntersect2;
	  gluUnProject(x2, y2, 1, modelview,
		  projection, viewport, &posX, &posY, &posZ);
	  glm::vec3 l(posX, posY, posZ);
	  l -= l0;
	  if (intersectXYPlane(l0, l, t, xyIntersect2)) {
		  newCameraTarget -= (xyIntersect2 - xyIntersect1);
		  
	  }
  }


#else
  {
    //Do X movement
    glm::vec3 l0(cameraPos);
    GLdouble posX, posY, posZ;
    gluUnProject(x1, y1, 1, modelview,
      projection, viewport, &posX,&posY,&posZ);
    
    glm::vec3 l(posX,posY,posZ);
    l -= l0;
    glm::vec3 xyIntersect1;
    float t;
    if(intersectXYPlane(l0,l,t,xyIntersect1))
    {
      glm::vec3 xyIntersect2;
      gluUnProject(x2, y1, 1, modelview,
	projection, viewport, &posX,&posY,&posZ);
      glm::vec3 l(posX,posY,posZ);
      l -= l0;
      if(intersectXYPlane(l0,l,t,xyIntersect2)){
      // glm::vec3 cameraMove(xyIntersect2-xyIntersect1);
	if(x2-x1>0)
	  newCameraTarget+=getRightVec()*glm::distance(xyIntersect2,xyIntersect1);
	else
	  newCameraTarget-=getRightVec()*glm::distance(xyIntersect2,xyIntersect1);
	
      }
    }
  }
  
  {
    //Do Y movement
    glm::vec3 l0(cameraPos);
    GLdouble posX, posY, posZ;
    gluUnProject(x1, y1, 1, modelview,
      projection, viewport, &posX,&posY,&posZ);
    
    glm::vec3 l(posX,posY,posZ);
    l -= l0;
    glm::vec3 xyIntersect1;
    float t;
    if(intersectXYPlane(l0,l,t,xyIntersect1))
    {
      glm::vec3 xyIntersect2;
      gluUnProject(x1, y2, 1, modelview,
	projection, viewport, &posX,&posY,&posZ);
      glm::vec3 l(posX,posY,posZ);
      l -= l0;
      if(intersectXYPlane(l0,l,t,xyIntersect2)){
      // glm::vec3 cameraMove(xyIntersect2-xyIntersect1);
	if(y2-y1>0)	//Opengl y is backwards
	  newCameraTarget+=glm::normalize(glm::cross(getDirectionVec(),getRightVec()))*glm::distance(xyIntersect2,xyIntersect1);
	else
	  newCameraTarget-=glm::normalize(glm::cross(getDirectionVec(),getRightVec()))*glm::distance(xyIntersect2,xyIntersect1);
	
      }
    }
  }
#endif

  cameraTarget = newCameraTarget;
#endif
  return;
}

glm::vec3 CameraControl::getRightVec() const
{
 // return glm::normalize(cameraQuat*glm::vec3(1.0f,0.0f,0.0f));
  
//   std::cout << "CameraLook ("<<getDirectionVec().x<<","<<getDirectionVec().y<<","<<getDirectionVec().z<<")\n";
  glm::vec3 rightVec = glm::normalize(glm::cross(glm::vec3(0,0,1),getDirectionVec()));
//   std::cout << "RightVec ("<<rightVec.x<<","<<rightVec.y<<","<<rightVec.z<<")\n";
  return glm::normalize(glm::cross(glm::vec3(0,0,1),getDirectionVec()));
}

glm::vec3 CameraControl::getUpVec() const
{
  return glm::normalize(cameraQuat*glm::vec3(0.0f,0.0f,1.0f));
}

glm::vec3 CameraControl::getDirectionVec() const
{
 // return glm::normalize(glm::cross(getUpVec(),getRightVec()));
  glm::vec3 tempTarget;
  	tempTarget.x = (float)m_zoom*(float)sin(AI_DEG_TO_RAD(yaw))*sin(AI_DEG_TO_RAD(-pitch));
	tempTarget.y = (float)m_zoom*(float)cos(AI_DEG_TO_RAD(yaw))*sin(AI_DEG_TO_RAD(-pitch));
	tempTarget.z = (float)m_zoom*(float)cos(AI_DEG_TO_RAD(pitch));
	tempTarget*=-1.0f;
	return glm::normalize(tempTarget);
}


