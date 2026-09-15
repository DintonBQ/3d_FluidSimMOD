#include "Header.h"
double vectorLength3d(Point_3d vector) {
    return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}
double vectorLength2d(Point_2d vector) {
    return sqrt(vector.x * vector.x + vector.y * vector.y);
}
double dotProduct(const Point_3d& A, const Point_3d& B) {
    return A.x * B.x + A.y * B.y + A.z * B.z;
}
double LineLength(Point_3d A, Point_3d B) {//довжина лінії в 3D
	B.x -= A.x;
	B.y -= A.y;
	B.z -= A.z;
	return vectorLength3d(B);
}
double rad2gradus(double radians) {
    return radians * 180.0 / M_PI;
}
double gradus2rad(double gradus) {
	return (gradus * M_PI / 180.0);
}
Point_3d normalizeVector(const Point_3d& vector, double& length) {
    length = vectorLength3d(vector);
    if (length == 0.0) {
        return { 0.0, 0.0, 0.0 };
    }
    return { vector.x / length, vector.y / length, vector.z / length };
}
Point_3d normalizeVector(const Point_3d& vector) {
    double length;
    return normalizeVector(vector, length);
}
Point_3d IIdToIIIdRayCast(double ScreenX, double ScreenY, double Distance) {//FluidSimMOD
	Point_3d Eye{ 0, 0, -From };
	Point_3d Cursor{ ScreenX, ScreenY, 0 };
	Point_3d dir = normalizeVector(Point_3d{
		Cursor.x - Eye.x,
		Cursor.y - Eye.y,
		Cursor.z - Eye.z
	});

	return Point_3d{
		Eye.x + dir.x * Distance,
		Eye.y + dir.y * Distance,
		Eye.z + dir.z * Distance
	};
}
Point_3d vectorXvector(const Point_3d& A, const Point_3d& B) {
    Point_3d result;
    result.x = A.y * B.z - A.z * B.y;
    result.y = A.z * B.x - A.x * B.z;
    result.z = A.x * B.y - A.y * B.x;
    return result;
}
Point_3d calculateTriangleNormal(const Point_3d& A, const Point_3d& B, const Point_3d& C) {
    //обчислення векторів AB і AC
    Point_3d AB = { B.x - A.x, B.y - A.y, B.z - A.z };
    Point_3d AC = { C.x - A.x, C.y - A.y, C.z - A.z };

    return normalizeVector(vectorXvector(AB, AC));//помножити вектори і нормалізувати
}
double angleBetweenVectors(Point_3d vec1, Point_3d vec2) {
    double scalarDobutok = dotProduct(vec1, vec2);//скалярний добуток двох векторів
    double cosAngle = scalarDobutok / (vectorLength3d(vec1) * vectorLength3d(vec2));//косинуса кута між векторами
    return acos(cosAngle);//кут у радіанах
}
int GetIndex(int x, int y,int width) {
	return (y * width + x);
}
double HeronsFormula(double a, double b, double c) {
	double p = (a + b + c) / 2;
	return sqrt(p * (p - a) * (p - b) * (p - c));
}
void ScalePoint(Point_3d& A, double Zoom) {
	A.x *= Zoom;
	A.y *= Zoom;
	A.z *= Zoom;
}
void ScalePlane(Plane& plane, double Zoom) {
	ScalePoint(plane.A, Zoom);
	ScalePoint(plane.B, Zoom);
	ScalePoint(plane.C, Zoom);
}
void din_povorot_tochku_navkolo_tochku(double& x, double& y, double angle, double centr_x, double centr_y) { //Pout_y Pout_x куди результат записати //рахує проти годинникової стрілки
	x -= centr_x;
	y -= centr_y;

	double R = vectorLength2d(Point_2d{ x, y });
	if(R) {
		double asinYR = asin(y / R);
		if(x >= 0) {
			//1,4 четверть
			x = R * cos(asinYR + angle);
			y = R * sin(asinYR + angle);
		} else {
			//2,3 четверть
			x = -R * cos(asinYR - angle); //на 2,3 четвертях мінус перед R завжди //другий знак: + проти годинникової, - за годинниковою стрілкою(на 1,4) | - проти годинникової, + за годинниковою(на 2,3)
			y = R * sin(asinYR - angle);
		}
	} else { //точка крутиться навколо себе
		x = 0;
		y = 0;
	}

	x += centr_x;
	y += centr_y;
}
void IIIdToIId(Point_3d A3d, Point_2d& A2d) { //проекція
	A2d.x = (From * A3d.x) / (From + A3d.z);
	A2d.y = (From * A3d.y) / (From + A3d.z);
}
void Rotation3D(Point_3d& A, double rad_XZ_around_angle, double rad_XY_angle, double rad_ZY_angle, double rad_XZ_angle, Point_3d Navkolo) {
	din_povorot_tochku_navkolo_tochku(A.x, A.z, rad_XZ_around_angle, Navkolo.x, Navkolo.z); //XZ_навколо своєї осі
	din_povorot_tochku_navkolo_tochku(A.x, A.y, rad_XY_angle, Navkolo.x, Navkolo.y);//XY
	din_povorot_tochku_navkolo_tochku(A.z, A.y, rad_ZY_angle, Navkolo.z, Navkolo.y);//ZY
	din_povorot_tochku_navkolo_tochku(A.x, A.z, rad_XZ_angle, Navkolo.x, Navkolo.z);//XZ
}
void RotatePlane(Plane& plane, double rad_XZ_around_angle, double rad_XY_angle, double rad_ZY_angle, double rad_XZ_angle, Point_3d Navkolo) {
	Rotation3D(plane.A, rad_XZ_around_angle, rad_XY_angle, rad_ZY_angle, rad_XZ_angle, Navkolo);
	Rotation3D(plane.B, rad_XZ_around_angle, rad_XY_angle, rad_ZY_angle, rad_XZ_angle, Navkolo);
	Rotation3D(plane.C, rad_XZ_around_angle, rad_XY_angle, rad_ZY_angle, rad_XZ_angle, Navkolo);
}