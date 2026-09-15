#include "Header.h"
void LightPos(Point_3d Light) { //положення свiтла
	Point_2d Light2d;
	IIIdToIId(Light, Light2d);
	for(int i=-3;i<=3;i++)
		for(int j=-3;j<=3;j++)
			din_SetPixel(Light2d.x+i, Light2d.y+j, 255,255,255, 255);
}
std::vector<Point_3d> findIntersection2(Point_3d L1, Point_3d L2, Point_3d sc, double r) {
    std::vector<Point_3d> intersections;
    Point_3d S = { L2.x - L1.x, L2.y - L1.y, L2.z - L1.z };
    Point_3d L1pos_relativeToSphereCenter{ L1.x - sc.x, L1.y - sc.y, L1.z - sc.z };
    double a = dotProduct(S, S);
    double b = 2 * dotProduct(L1pos_relativeToSphereCenter, S);
    double c = dotProduct(L1pos_relativeToSphereCenter, L1pos_relativeToSphereCenter) - r * r;
    double D = b*b-4*a*c;
    if (D < 0) return intersections;
    double sqrtD = sqrt(D);
    double t1 = (-b+sqrtD) / (2*a);
    double t2 = (-b-sqrtD) / (2*a);
    Point_3d intersection1 = {t1*S.x+L1.x, t1*S.y+L1.y, t1*S.z+L1.z};
    intersections.push_back(intersection1);
    if(D > 0) {
        Point_3d intersection2 = {t2*S.x+L1.x, t2*S.y+L1.y, t2*S.z+L1.z};
        intersections.push_back(intersection2);
    }
    return intersections;
}
void Sphere3D(int ScreenX, int ScreenY, const Point_3d& sc, double r, Point_3d Light) {
	Point_3d L1 = { (double)ScreenX,(double)ScreenY,0 };
	Point_3d L2 = { 0,0,-From };

	std::vector<Point_3d> intersections = findIntersection2(L1, L2, sc, r);

	if(intersections.size() > 0){
		for(Point_3d SpherePoint: intersections){
			//дублюється
int x=(int)ScreenX + (ScreenRect.right/2);
int y=(int)ScreenY + (ScreenRect.bottom/2);
if(!(x>=0&&x<ScreenRect.right && y>=0&&y<ScreenRect.bottom))continue;
			int index = GetIndex(x, y,ScreenRect.right);
			double DistanceToPixel = LineLength(Point_3d{0,0,-From}, SpherePoint);
if(index<0||index>=ScreenRect.right * ScreenRect.bottom){
	cout<<"ERROR INDEX: "<<index<<endl;
	//cout<<"GetIndex("<<x<<","<<y<<","<<ScreenRect.right<<")"<<endl;
	continue;
}
			if(DistanceToPixels[index] == UNDEFINED || DistanceToPixel < DistanceToPixels[index]) {
				DistanceToPixels[index] = DistanceToPixel;
				int lightBrightness;
				int minimalBright=10;
				int maximalBright=255;
				if(drawLight){ //обчислення свiтла
					int k = LineLength(Light, SpherePoint);
					int lightDistance = 220;
					if(k > lightDistance) lightBrightness = minimalBright; else lightBrightness = (1-k/(float)lightDistance)*(maximalBright-minimalBright)+minimalBright; // iнша шкала
				} else lightBrightness=maximalBright;

				din_SetPixel(ScreenX, ScreenY, lightBrightness, lightBrightness, 0, 255);
			}
		}
	}
}
void Sphere(const Point_3d& center, double r, const Point_3d& Light) {
	Point_2d center2d; IIIdToIId(center, center2d);
	Point_2d r2d; IIIdToIId(Point_3d{ center.x + r, center.y, center.z }, r2d);

	int m = fabs(r2d.x - center2d.x) + 1; //радіус проекції сфери+добавка //r+1
	for(int ScreenY = center2d.y - m; ScreenY <= center2d.y + m; ScreenY++) { //оптимізація рендерингу сфери
		for(int ScreenX = center2d.x - m; ScreenX <= center2d.x + m; ScreenX++) {
			//можна розпаралелити
			//if(1||ScreenX%2==0)din_SetPixel(ScreenX, ScreenY, 0, 255, 0, 255);//область оптимізації
			Sphere3D(ScreenX, ScreenY, center, r, Light);
		}
	}
}