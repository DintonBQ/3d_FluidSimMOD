#include "Header.h"
float scatter = 0.0001;
void zShift(Point_3d& A, Point_3d& B, Point_3d& C, Point_3d& M) {
    if (fabs(B.z - A.z) <= scatter &&
        fabs(B.z - C.z) <= scatter &&
        fabs(C.z - A.z) <= scatter) {
        B.z = 0; C.z = 0;  A.z = 0; M.z = 0;
    }
}
void rotatePointToMatchNormal(Point_3d& A, const Point_3d& massCenter, const Point_3d& rotationAxis, double angle) {
    double cosA = cos(angle);
    double sinA = sin(angle);
    double OMC = 1 - cosA;//OneMinusCos
    double ux = rotationAxis.x;
    double uy = rotationAxis.y;
    double uz = rotationAxis.z;

    double Vec[3] = {A.x-massCenter.x,A.y-massCenter.y,A.z-massCenter.z};
    double RotationMatrix[3][3] = {
    {cosA + OMC * ux * ux     , OMC * ux * uy - sinA * uz, OMC * ux * uz + sinA * uy},
    {OMC * ux * uy + sinA * uz, cosA + OMC * uy * uy     , OMC * uy * uz - sinA * ux},
    {OMC * ux * uz - sinA * uy, OMC * uy * uz + sinA * ux, cosA + OMC * uz * uz     }
    };
    
    //множення матриць
    double result[3]{};//заповнити нулями
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result[i] += RotationMatrix[i][j] * Vec[j];
        }
    }
    A = {result[0] + massCenter.x, result[1] + massCenter.y, result[2] + massCenter.z};
}

void rotateTriangleToMatchNormal(Point_3d& _A, Point_3d& _B, Point_3d& _C, const Point_3d& needNormal, Point_3d& M) {
    Point_3d triangleNormal = calculateTriangleNormal(_A, _B, _C);
    double angle = angleBetweenVectors(needNormal, triangleNormal);

    //вісь, навколо якої повертати
    Point_3d rotationAxis = calculateTriangleNormal(Point_3d{ 0,0,0 }, triangleNormal, needNormal);

    Point_3d massCenter = {(_A.x + _B.x + _C.x) / 3, (_A.y + _B.y + _C.y) / 3, (_A.z + _B.z + _C.z) / 3};
    
    rotatePointToMatchNormal(_A, massCenter, rotationAxis, angle);
    rotatePointToMatchNormal(_B, massCenter, rotationAxis, angle);
    rotatePointToMatchNormal(_C, massCenter, rotationAxis, angle);

    rotatePointToMatchNormal(M, massCenter, rotationAxis, angle);

    //cout << "__ROTATION__\n";
}
int mainTexturization(Point_3d A, Point_3d B, Point_3d C, Point_3d& _M) {
    //vvv INPUT vvv
    /*Point_3d A = { 1, 4, 1 };
    Point_3d B = { 2, 1, 1 };
    Point_3d C = { 1, 1, 2 };*/
    /*Point_3d A = { 5, 1, 7 };
    Point_3d B = { 2, 9, 4 };
    Point_3d C = { 8, 3, 6 };*/

    Point_3d normalXY = Point_3d{ 0.0, 0.0, 1.0 };
    //^^^ INPUT ^^^
    //cout << fixed;

    //vvv SAVE OLD VALUES vvv
    double trueAB = LineLength(A, B);
    double trueBC = LineLength(B, C);
    double trueAC = LineLength(A, C);
    Point_3d prevNormal = calculateTriangleNormal(A, B, C);
    Point_3d oldA = A, oldB = B, oldC = C;
    //^^^ SAVE OLD VALUES ^^^

    //vvv ROTATION vvv
    rotateTriangleToMatchNormal(A, B, C, normalXY, _M); zShift(A, B, C, _M);
    //^^^ ROTATION ^^^
//test:
//TRIANGLE NOT CHANGED nop
//TRIANGLE NOT CHANGED 2 rotations
//TRIANGLE MOVED WITHOUT ROTATION
//onXY: true

    //vvv NEW VALUES vvv
    double AB = LineLength(A, B);
    double BC = LineLength(B, C);
    double AC = LineLength(A, C);
    Point_3d newNormal = calculateTriangleNormal(A, B, C);
    //^^^ NEW VALUES ^^^

    //vvv ПЕРЕВІРКИ РОБОТИ АЛГОРИТМУ vvv
if(false){
    bool oldPos = fabs(A.x - oldA.x) <= scatter &&
        fabs(A.y - oldA.y) <= scatter &&
        fabs(A.z - oldA.z) <= scatter &&
        fabs(B.x - oldB.x) <= scatter &&
        fabs(B.y - oldB.y) <= scatter &&
        fabs(B.z - oldB.z) <= scatter &&
        fabs(C.x - oldC.x) <= scatter &&
        fabs(C.y - oldC.y) <= scatter &&
        fabs(C.z - oldC.z) <= scatter;
    if (oldPos) {
        cout << "TRIANGLE NOT CHANGED" << endl;
    } else {
        bool formSaved = fabs(AB - trueAB) <= scatter &&
            fabs(BC - trueBC) <= scatter &&
            fabs(AC - trueAC) <= scatter;
        if (formSaved) {
            bool equalXYnormal = fabs(newNormal.x - normalXY.x) <= scatter &&
                fabs(newNormal.y - normalXY.y) <= scatter &&
                fabs(newNormal.z - normalXY.z) <= scatter;
            if (equalXYnormal) {
                bool onXY = A.z == 0 && B.z == 0 && C.z == 0;
                cout << (onXY ? "onXY: true" : "equalXYnormal: true\nonXY: false") << endl;
            }
            else {
                bool equalOldNormal = fabs(newNormal.x - prevNormal.x) <= scatter &&
                    fabs(newNormal.y - prevNormal.y) <= scatter &&
                    fabs(newNormal.z - prevNormal.z) <= scatter;
                cout << "equalOldNormal: " << (equalOldNormal ? "TRIANGLE MOVED WITHOUT ROTATION" : "false") << endl;
            }
        }
        else {
            cout << "CODE_ERROR formNotSaved" << endl;
            cout << "dAB " << fabs(AB - trueAB) << endl;
            cout << "dBC " << fabs(BC - trueBC) << endl;
            cout << "dAC " << fabs(AC - trueAC) << endl;

        }
    }
    cout << "A: (" << A.x << ", " << A.y << ", " << A.z << ")\n";
    cout << "B: (" << B.x << ", " << B.y << ", " << B.z << ")\n";
    cout << "C: (" << C.x << ", " << C.y << ", " << C.z << ")\n";
}
    //^^^ ПЕРЕВІРКИ РОБОТИ АЛГОРИТМУ ^^^

    //cout << defaultfloat;
    return 0;
}