// --------------------------------------------------------------------------------
/*
    camera.cpp

  Copyright (c) 2009-2015  Thomas Mueller, Frank Grave


   This file is part of the GeodesicViewer.

   The GeodesicViewer is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   The GeodesicViewer is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the GeodesicViewer.  If not, see <http://www.gnu.org/licenses/>.
*/
// -------------------------------------------------------------------------------

#include "camera.h"

/*! Constructor of the simple OpenGL camera
 */
Camera::Camera() {
    setStandardParams();
}

/*! Destructor
 */
Camera::~Camera() {
}

// *********************************** public methods ******************************

/*!  Set standard parameters.
 */
void
Camera::setStandardParams() {
    mPos = m4d::vec3(0.0, 0.0, 10.0);
    mDir = m4d::vec3(0.0, 0.0, -1.0);
    mVup = m4d::vec3(0.0, 1.0,  0.0);

    mRight = mDir ^ mVup;
    mPOI = m4d::vec3(0.0, 0.0, 0.0);

    mZnear = 0.5;
    mZfar  = 100000.0;

    mWidth  = 100;
    mHeight = 100;

    mAspect = 1.0;
    mFovY   = 90.0;

    mEyeSep = 0.1;
    mStereoGlasses = enum_stereo_red_cyan;
}

/*! Set position of the camera.
 * \param pos : eye position
 */
void
Camera::setEyePos(m4d::vec3 pos) {
    mPos = pos;
}

m4d::vec3 Camera::getEyePos() {
    return mPos;
}

/*! Get camera position.
 * \param pos : pointer to position (4 floats: x,y,z,w)
 */
void
Camera::getEyePos(float* pos) {
    pos[0] = (float)mPos[0];
    pos[1] = (float)mPos[1];
    pos[2] = (float)mPos[2];
    pos[3] = 1.0f;
}

/*! Set direction of camera view.
 *  \param dir : camera viewing direction
 */
void
Camera::setDir(m4d::vec3 dir) {
    mDir = dir;
    mRight = (mDir ^ mVup).getNormalized();
}

m4d::vec3 Camera::getDir() {
    return mDir;
}

/*! Get direction of camera view.
 * \param dir : pointer to direction (3 floats).
 */
void
Camera::getDir(float* dir) {
    dir[0] = (float)mDir[0];
    dir[1] = (float)mDir[1];
    dir[2] = (float)mDir[2];
}

/*! Set point of interest.
 *  \param center : point of interest
 */
void
Camera::setPOI(m4d::vec3 center) {
    mPOI = center;
    mDir = mPOI - mPos;

    if (!mDir.isZero()) {
        mPOI = center;
        mDir.normalize();
        mRight = (mDir ^ mVup).getNormalized();
    }
}

m4d::vec3 Camera::getPOI() {
    return mPOI;
}

/*! Get point of interest.
 * \param poi : pointer to point of interest (4 floats)
 */
void
Camera::getPOI(float* poi) {
    poi[0] = (float)mPOI[0];
    poi[1] = (float)mPOI[1];
    poi[2] = (float)mPOI[2];
    poi[3] = 1.0f;
}

/*! Set up-vector of camera.
 *  \param vup : vertical up-vector of the camera
 */
void
Camera::setVup(m4d::vec3 vup) {
    mVup = vup;
    mRight = (mDir ^ mVup).getNormalized();
}

m4d::vec3 Camera::getVup() {
    return mVup;
}

/*! Get up-vector of camera.
 *  \param vup : pointer to vertical up-vector of the camera (4 floats)
 */
void
Camera::getVup(float* vup) {
    vup[0] = (float)mVup[0];
    vup[1] = (float)mVup[1];
    vup[2] = (float)mVup[2];
    vup[3] = 0.0f;
}

m4d::vec3 Camera::getRight() {
    return mRight;
}

/*! Get right-vector of camera.
 *  \param right : pointer to right-vector (4 floats)
 */
void
Camera::getRight(float* right) {
    right[0] = (float)mRight[0];
    right[1] = (float)mRight[1];
    right[2] = (float)mRight[2];
    right[3] = 0.0f;
}

/*! Set field-of-view in y-direction.
 *  \param fovy : field of view in y-direction
 */
void
Camera::setFovY(double fovy) {
    mFovY = fovy;
}

double Camera::getFovY() {
    return mFovY;
}

double Camera::getAspect() {
    return mAspect;
}

/*!  Set intrinsic parameters.
 *  \param fovy : field of view in y-direction
 *  \param nearval : near-z-distance
 *  \param farval  : far-z-distance
 */
void
Camera::setIntrinsic(double fovy, double nearval, double farval) {
    mFovY  = fovy;
    mZnear = nearval;
    mZfar  = farval;
}

/*! Get intrinsic parameters.
 *  \param fovy : reference to field of view in y-direction
 *  \param nearval : reference to near-z-distance
 *  \param farval  : reference to far-z-distance
 */
void
Camera::getIntrinsic(double &fovy, double &nearval, double &farval) {
    fovy	  = mFovY;
    nearval = mZnear;
    farval  = mZfar;
}

/*! Set camera resolution and adjust aspect ratio.
 *  \param width : camera resolution in x-direction
 *  \param height : camera resolution in y-direction
 */
void
Camera::setSizeAndAspect(int width, int  height) {
    mWidth  = width;
    mHeight = height;
    mAspect = double(width) / double(height);
}

/*! Get camera resolution.
 *  \param width : reference to camera resolution in x-direction
 *  \param height : reference to camera resolution in y-direction
 */
void
Camera::getSize(int &width, int &height) {
    width  = mWidth;
    height = mHeight;
}

/*! Rotate camera around vup with fixed point of interest.
 *  \param angle : angle of rotation
 */
void
Camera::fixRotAroundVup(double angle) {
    Quaternion dir(0, mDir);
    Quaternion right(0, mRight);
    Quaternion eye(0, mPos);

    Quaternion rho;
    rho.setRot(-angle, mVup);
    dir   = dir | rho;
    right = right | rho;
    eye   = eye | rho;

    mPos   = eye.getVector();
    mDir   = dir.getVector();
    mRight = right.getVector();
}

/*! Rotate camera around right with fixed point of interest.
 *  \param angle : angle of rotation
 */
void
Camera::fixRotAroundRight(double angle) {
    Quaternion dir(0, mDir);
    Quaternion vup(0, mVup);
    Quaternion eye(0, mPos);

    Quaternion rho;
    rho.setRot(-angle, mRight);
    dir = dir | rho;
    vup = vup | rho;
    eye = eye | rho;

    mPos = eye.getVector();
    mDir = dir.getVector();
    mVup = vup.getVector();
}

/*! Rotate camera around dir with fixed point of interest.
 *  \param angle : angle of rotation
 */
void
Camera::fixRotAroundDir(double angle) {
    Quaternion vup(0, mVup);
    Quaternion right(0, mRight);
    Quaternion eye(0, mPos);

    Quaternion rho;
    rho.setRot(-angle, mDir);
    vup   = vup | rho;
    right = right | rho;
    eye   = eye | rho;

    mPos = eye.getVector();
    mVup = vup.getVector();
    mRight = right.getVector();
}

/*! Rotate camera around x axis;
 *  \param angle : angle of rotation
 */
void
Camera::fixRotAroundX(double angle) {
    m4d::vec3 x(1.0, 0.0, 0.0);
    Quaternion vup(0, mVup);
    Quaternion right(0, mRight);
    Quaternion eye(0, mPos);
    Quaternion dir(0, mDir);

    Quaternion rho;
    rho.setRot(-angle, x);
    vup   = vup | rho;
    right = right | rho;
    eye   = eye | rho;
    dir   = dir | rho;

    mPos = eye.getVector();
    mVup = vup.getVector();
    mRight = right.getVector();
    mDir = dir.getVector();
}

/*! Rotate camera around y axis;
 *  \param angle : angle of rotation
 */
void
Camera::fixRotAroundY(double angle) {
    m4d::vec3 y(0.0, 1.0, 0.0);
    Quaternion vup(0, mVup);
    Quaternion right(0, mRight);
    Quaternion eye(0, mPos);
    Quaternion dir(0, mDir);

    Quaternion rho;
    rho.setRot(-angle, y);
    vup   = vup | rho;
    right = right | rho;
    eye   = eye | rho;
    dir   = dir | rho;

    mPos = eye.getVector();
    mVup = vup.getVector();
    mRight = right.getVector();
    mDir = dir.getVector();
}

/*! Rotate camera around z axis;
 *  \param angle : angle of rotation
 */
void
Camera::fixRotAroundZ(double angle) {
    m4d::vec3 z(0.0, 0.0, 1.0);
    Quaternion vup(0, mVup);
    Quaternion right(0, mRight);
    Quaternion eye(0, mPos);
    Quaternion dir(0, mDir);

    Quaternion rho;
    rho.setRot(-angle, z);
    vup   = vup | rho;
    right = right | rho;
    eye   = eye | rho;
    dir   = dir | rho;

    mPos = eye.getVector();
    mVup = vup.getVector();
    mRight = right.getVector();
    mDir = dir.getVector();
}

/*! Get spherical eye position.
 * \param theta : reference to theta.
 * \param phi : reference to phi.
 * \param dist : reference to dist.
 */
void
Camera::getSphericalEyePos(double &theta, double &phi, double &dist) {
    m4d::vec3 dp = mPos - mPOI;
    dist = dp.getNorm();

    theta = atan2(sqrt(dp.x(0) * dp.x(0) + dp.x(1) * dp.x(1)), dp.x(2));
    phi   = atan2(dp.x(1), dp.x(0));
}

/*! Move camera along sphere.
 * \param theta : angle theta.
 * \param phi : angle phi.
 * \param dist : radius of sphere.
 */
void
Camera::moveOnSphere(double theta, double phi, double dist) {
    if (dist <= 0.0) {
        return;
    }

    double eps = 1e-2;
    if (theta < eps) {
        theta = eps;
    }
    if (theta > M_PI - eps) {
        theta = M_PI - eps;
    }

    mPos = mPOI + m4d::vec3(dist * sin(theta) * cos(phi), dist * sin(theta) * sin(phi), dist * cos(theta));
    mDir = mPOI - mPos;
    mDir.normalize();

    m4d::vec3 z(0.0, 0.0, 1.0);
    mRight = mDir ^ z;
    mRight.normalize();
    mVup = mRight ^ mDir;
}

/*! Call gluLookAt method with internal parameters
 */
void
Camera::lookAt() {
    m4d::vec3 c = mPos + mDir;
    gluLookAt(mPos.x(0), mPos.x(1), mPos.x(2), mPOI.x(0), mPOI.x(1), mPOI.x(2), mVup.x(0), mVup.x(1), mVup.x(2));
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

/*! Call gluLookAt method with internal parameters
 */
void
Camera::lookAtCenter() {
    m4d::vec3 c = mPos + mDir;
    m4d::vec3 normedPos = mPos.getNormalized();
    float fac = 37.0;

    gluLookAt(normedPos.x(0)*fac, normedPos.x(1)*fac, normedPos.x(2)*fac, 0.0, 0.0, 0.0, mVup.x(0), mVup.x(1), mVup.x(2));
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

/*! Call gluLookAt method with internal parameters
 */
void
Camera::lookAtModelView() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    lookAt();
}

/*! Call gluLookAt method with internal parameters
 */
void
Camera::lookAtCenterModelView() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    lookAtCenter();
}

/*! Set stereo parameters.
 *  \param sep : eye separation
 *  \param type : type of stereo glasses
 */
void
Camera::setStereoParams(double sep, enum_stereo_glasses type) {
    mEyeSep = sep;
    mStereoGlasses = type;
}

/*! Get stereo parameters.
 *  \param sep : reference to eye separation
 *  \param type : reference to type of stereo glasses
 */
void
Camera::getStereoParams(double &sep, enum_stereo_glasses &type) {
    sep = mEyeSep;
    type = mStereoGlasses;
}

/*! Set stereo type.
 * \param type : type of stereo projection.
 */
void
Camera::setStereoType(enum_stereo_type type) {
    mStereoType = type;
}

/*! Get stereo type.
 * \param type : reference to type of stereo projection.
 */
void
Camera::getStereoType(enum_stereo_type &type) {
    type = mStereoType;
}

/*! Set look-at position for left eye.
 */
void
Camera::lookAtMV_Left() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    switch (mStereoGlasses) {
        case enum_stereo_red_blue:
        case enum_stereo_red_green:
        case enum_stereo_red_cyan:
            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
            break;
        case enum_stereo_blue_red:
            glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
            break;
        case enum_stereo_green_red:
            glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
            break;
        case enum_stereo_cyan_red:
            glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
            break;
    }

    m4d::vec3 eye   = mPos - mRight * 0.5 * mEyeSep;
    m4d::vec3 focus = mPOI;
    switch (mStereoType) {
        default:
        case enum_stereo_off_axis: {
            double focallength = (mPos - mPOI).getNorm();
            double wd2  = mZnear * tan(mFovY * DEG_TO_RAD * 0.5);
            double ndfl = mZnear / focallength;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            double left  = - mAspect * wd2 - 0.5 * mEyeSep * ndfl;
            double right =   mAspect * wd2 - 0.5 * mEyeSep * ndfl;
            double top    =   wd2;
            double bottom = - wd2;
            glFrustum(left, right, bottom, top, mZnear, mZfar);

            focus = focus - mRight * 0.5 * mEyeSep + mDir;
            break;
        }
        case enum_stereo_parallel: {
            focus = focus - mRight * 0.5 * mEyeSep;
            break;
        }
        case enum_stereo_toe_in:
            break;
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x(0), eye.x(1), eye.x(2),  focus.x(0), focus.x(1), focus.x(2),  mVup.x(0), mVup.x(1), mVup.x(2));
}

/*! Set look-at position for right eye.
 */
void
Camera::lookAtMV_Right() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    switch (mStereoGlasses) {
        case enum_stereo_red_blue:
            glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
            break;
        case enum_stereo_red_green:
            glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
            break;
        case enum_stereo_red_cyan:
            glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
            break;
        case enum_stereo_blue_red:
        case enum_stereo_green_red:
        case enum_stereo_cyan_red:
            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
            break;
    }

    m4d::vec3 eye   = mPos + mRight * 0.5 * mEyeSep;
    m4d::vec3 focus = mPOI;
    switch (mStereoType) {
        default:
        case enum_stereo_off_axis: {
            double focallength = (mPos - mPOI).getNorm();
            double wd2  = mZnear * tan(mFovY * DEG_TO_RAD * 0.5);
            double ndfl = mZnear / focallength;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            double left  = - mAspect * wd2 + 0.5 * mEyeSep * ndfl;
            double right =   mAspect * wd2 + 0.5 * mEyeSep * ndfl;
            double top    =   wd2;
            double bottom = - wd2;
            glFrustum(left, right, bottom, top, mZnear, mZfar);

            focus = focus + mRight * 0.5 * mEyeSep + mDir;
            break;
        }
        case enum_stereo_parallel: {
            focus = focus + mRight * 0.5 * mEyeSep;
            break;
        }
        case enum_stereo_toe_in:
            break;
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x(0), eye.x(1), eye.x(2),  focus.x(0), focus.x(1), focus.x(2),  mVup.x(0), mVup.x(1), mVup.x(2));
}

/*! Call gluPerspective method with internal parameters.
 */
void
Camera::perspective() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, mWidth, mHeight);
    gluPerspective(mFovY, mAspect, mZnear, mZfar);
}

/*! Call gluPerspective method with internal parameters.
 */
void
Camera::perspectiveAxes() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, mWidth, mHeight);
    gluPerspective(5.0, 1.0, mZnear, mZfar);
}

/*! Call gluOrtho2D method with internal parameters.
 */
void
Camera::orthographic() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, mWidth, mHeight);

    double size = 4.0;
    double bottom = -size * tan(0.5 * mFovY * DEG_TO_RAD);
    double top    = size * tan(0.5 * mFovY * DEG_TO_RAD);
    double left   = -size * tan(0.5 * mFovY * DEG_TO_RAD) * mAspect;
    double right  = size * tan(0.5 * mFovY * DEG_TO_RAD) * mAspect;
    glOrtho(left, right, bottom, top, mZnear, mZfar);
}

/*! Call glViewport method with internal parameters;
 */
void
Camera::viewport() {
    glViewport(0, 0, GLsizei(mWidth), GLsizei(mHeight));
}

/*! Print camera parameters.
 *  \param ptr : file pointer
 */
void
Camera::print(FILE* ptr) {
    fprintf(ptr, "eye    :  %6.3f %6.3f %6.3f\n", mPos.x(0), mPos.x(1), mPos.x(2));
    fprintf(ptr, "dir    :  %6.3f %6.3f %6.3f\n", mDir.x(0), mDir.x(1), mDir.x(2));
    fprintf(ptr, "vup    :  %6.3f %6.3f %6.3f\n", mVup.x(0), mVup.x(1), mVup.x(2));
    fprintf(ptr, "poi    :  %6.3f %6.3f %6.3f\n", mPOI.x(0), mPOI.x(1), mPOI.x(2));
    fprintf(ptr, "right  :  %6.3f %6.3f %6.3f\n", mRight.x(0), mRight.x(1), mRight.x(2));
    fprintf(ptr, "size   :  %4i %4i\n", mWidth, mHeight);
    fprintf(ptr, "aspect :  %6.3f\n", mAspect);
    fprintf(ptr, "fovY   :  %6.3f\n", mFovY);
    fprintf(ptr, "near   :  %8.2f\n", mZnear);
    fprintf(ptr, "far    :  %8.2f\n", mZfar);
}
