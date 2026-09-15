#ifndef HEADER_H
#define HEADER_H

#define _CRT_SECURE_NO_WARNINGS//щоб можна використовувати старі функції c++
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <time.h>
#define _USE_MATH_DEFINES //M_PI
#include <cmath>

#ifdef _WIN32
#define WINDOWS_OS
#endif

#ifdef __linux__
#define LINUX_OS
#endif

#ifdef WINDOWS_OS
#include <windows.h>
//#include <atlimage.h> //обгортки для HDC//замінено на кросплатформенну
extern HWND my_window; //FluidSimMOD
#endif

#ifdef LINUX_OS
#include <SDL2/SDL.h>
#include <sys/time.h> //FPS gettimeofday
#include <stdlib.h>
#endif

using namespace std;

#define BYTE unsigned char
#define INF (double)1000000
#define UNDEFINED (double)0

struct Point_2d {
	double x = 0, y = 0;
};
struct Point_3d
{
	double x = 0, y = 0, z = 0;
};
struct COLOR {
	BYTE r = 0, g = 0, b = 0;
};
struct Plane {//площина, трикутник
	Point_3d A, B, C;
	COLOR color;
};
struct RECTANGLE {
    int right;
    int bottom;
};

extern double From;
extern RECTANGLE ScreenRect;
extern double* DistanceToPixels;
extern bool drawLight;
//extern double din_Pi;//замінено бібліотечним
int mainTexturization(Point_3d A, Point_3d B, Point_3d C, Point_3d& _M);
void din_SetPixel(int x, int y, BYTE R, BYTE G, BYTE B, BYTE Alpha = 255, bool shiftToCenter = true);

//SPHERE MOD PROTOTYPES:
void LightPos(Point_3d Light);
void Sphere3D(int ScreenX, int ScreenY, const Point_3d& sc, double r, Point_3d Light);
void Sphere(const Point_3d& center, double r, const Point_3d& Light);

//SMALL INDEPENDENT FUNCTIONS PROTOTYPES:
double vectorLength3d(Point_3d vector);
double vectorLength2d(Point_2d vector);
double dotProduct(const Point_3d& A, const Point_3d& B);
double LineLength(Point_3d A, Point_3d B);
double rad2gradus(double radians);
double gradus2rad(double gradus);
Point_3d normalizeVector(const Point_3d& vector, double& length);
Point_3d normalizeVector(const Point_3d& vector);
Point_3d vectorXvector(const Point_3d& A, const Point_3d& B);
Point_3d calculateTriangleNormal(const Point_3d& A, const Point_3d& B, const Point_3d& C);
double angleBetweenVectors(Point_3d vec1, Point_3d vec2);
int GetIndex(int x, int y,int width);
double HeronsFormula(double a, double b, double c);
void ScalePoint(Point_3d& A, double Zoom);
void ScalePlane(Plane& plane, double Zoom);
void din_povorot_tochku_navkolo_tochku(double& x, double& y, double angle, double centr_x, double centr_y);
void IIIdToIId(Point_3d A3d, Point_2d& A2d);
void Rotation3D(Point_3d& A, double rad_XZ_around_angle, double rad_XY_angle, double rad_ZY_angle, double rad_XZ_angle, Point_3d Navkolo);
void RotatePlane(Plane& plane, double rad_XZ_around_angle, double rad_XY_angle, double rad_ZY_angle, double rad_XZ_angle, Point_3d Navkolo);
Point_3d IIdToIIIdRayCast(double ScreenX, double ScreenY, double Distance);//FluidSimMOD

//FLUID SIMULATION PROTOTYPES://FluidSimMOD
extern Point_3d LightPosition;
extern double FromEyeToLight;
void DrawFluid();
#endif