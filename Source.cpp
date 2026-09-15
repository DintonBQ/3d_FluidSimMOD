#include "Header.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#ifdef WINDOWS_OS
	RGBQUAD* im;//BGRA
	HDC hdc;
	HDC MemDC;
	bool dragging = false;//для переміщення вікна
	POINT MousePnt;//для переміщення вікна
	HWND my_window;//FluidSimMOD
#endif
double Zoom = 121;
//double din_Pi = 3.141592653589793;//замінено бібліотечним
double From = 1500;//відстань від камери до екрану(впливає на кут огляду(FOV))
double* DistanceToPixels=nullptr;
RECTANGLE ScreenRect{400, 400}; //rect-прямокутник
RECTANGLE TtextureRect{400, 400};
BYTE* Ttexture;
std::vector<Point_3d> Vertices; //вершини
std::vector<Plane> Planes; //площини
bool drawBackground = false;
bool drawPlanes = false;
bool drawSphere = true;
bool texture = true;
bool drawLight = true;
bool demoAreas = false;
bool drawFluid = false;//FluidSimMOD
bool drawFrame = true;//рамка області малювання

/*double din_abs(double a) { //замінено бібліотечним
	if(a < 0) return -a;
	else return a;
}*/

int channels;
void Ttexture_Load(const char* file){
cout<<"Ttexture_Load(): file="<<file<<endl;
Ttexture=stbi_load(file,&TtextureRect.right,&TtextureRect.bottom,&channels,4);//stb_image.h
//cout<<"Ttexture="<<(void*)Ttexture<<endl;
}
BYTE* Ttexture_GetPixelAddress(int x, int y){
//cout<<"Ttexture_GetPixelAddress(): Ttexture="<<(void*)Ttexture<<endl;
//cout<<"POS x="<<x<<" y="<<y<<" width="<<TtextureRect.right<<" height="<<TtextureRect.bottom<<endl;
if(x>=0&&x<TtextureRect.right&&y>=0&&y<TtextureRect.bottom) return Ttexture+GetIndex(x, y,TtextureRect.right) * channels;

//cout<<"ERROR POS"<<endl;
return nullptr;
}

#ifdef WINDOWS_OS
RGBQUAD* GetAddr(RGBQUAD* arr, int x, int y) {//BGRA
	return GetIndex(x, y,ScreenRect.right) + arr;
}
#endif

#ifdef WINDOWS_OS
#define KEY_SPACE        (int)' '
#define KEY_NUMPAD_PLUS  VK_ADD
#define KEY_NUMPAD_MINUS VK_SUBTRACT
#define KEY_CTRL         17
#define KEY_ESC          27

#define KEY_1 (int)'1'
#define KEY_2 (int)'2'
#define KEY_3 (int)'3'
#define KEY_4 (int)'4'
#define KEY_5 (int)'5'
#define KEY_6 (int)'6'
#define KEY_7 (int)'7'
#define KEY_8 (int)'8'
#define KEY_9 (int)'9'
#define KEY_0 (int)'0'

#define KEY_Q (int)'Q'
#define KEY_W (int)'W'
#define KEY_E (int)'E'

#define KEY_A (int)'A'
#define KEY_S (int)'S'
#define KEY_D (int)'D'
#define KEY_F (int)'F'

#define KEY_Z (int)'Z'
#define KEY_X (int)'X'
#define KEY_C (int)'C'
#define KEY_V (int)'V'
#define KEY_B (int)'B'
#define KEY_N (int)'N'
#define KEY_G (int)'G'//FluidSimMOD
#define KEY_M (int)'M'
#endif


#ifdef LINUX_OS
#define KEY_SPACE SDL_SCANCODE_SPACE
#define KEY_NUMPAD_PLUS SDL_SCANCODE_KP_PLUS
#define KEY_NUMPAD_MINUS SDL_SCANCODE_KP_MINUS
//#define KEY_CTRL SDL_SCANCODE_LCTRL //unused
#define KEY_ESC SDL_SCANCODE_ESCAPE
#define KEY_1 SDL_SCANCODE_1
#define KEY_2 SDL_SCANCODE_2
#define KEY_3 SDL_SCANCODE_3
#define KEY_4 SDL_SCANCODE_4
#define KEY_5 SDL_SCANCODE_5
#define KEY_6 SDL_SCANCODE_6
#define KEY_7 SDL_SCANCODE_7
#define KEY_8 SDL_SCANCODE_8
#define KEY_9 SDL_SCANCODE_9
#define KEY_0 SDL_SCANCODE_0
#define KEY_Q SDL_SCANCODE_Q
#define KEY_W SDL_SCANCODE_W
#define KEY_E SDL_SCANCODE_E

#define KEY_A SDL_SCANCODE_A
#define KEY_S SDL_SCANCODE_S
#define KEY_D SDL_SCANCODE_D
#define KEY_F SDL_SCANCODE_F

#define KEY_Z SDL_SCANCODE_Z
#define KEY_X SDL_SCANCODE_X
#define KEY_C SDL_SCANCODE_C
#define KEY_V SDL_SCANCODE_V
#define KEY_B SDL_SCANCODE_B
#define KEY_N SDL_SCANCODE_N
#define KEY_G SDL_SCANCODE_G//FluidSimMOD
#define KEY_M SDL_SCANCODE_M
#endif

#define KEY_drawBackground KEY_Z
#define KEY_texture KEY_X
#define KEY_drawLight KEY_C
#define KEY_demoAreas KEY_V
#define KEY_drawPlanes KEY_B
#define KEY_drawSphere KEY_N
#define KEY_drawFluid KEY_G//FluidSimMOD
#define KEY_drawFrame KEY_M

#define KEY_FromPlus KEY_Q
#define KEY_FromMinus KEY_W
#define KEY_ZoomPlus KEY_NUMPAD_PLUS
#define KEY_ZoomMinus KEY_NUMPAD_MINUS

#define KEY_CUT KEY_SPACE

#define KEY_tex1 KEY_A
#define KEY_tex2 KEY_S
#define KEY_tex3 KEY_D
#define KEY_tex4 KEY_F



bool din_isKeyPressed(int key) {
	#ifdef WINDOWS_OS
		return (GetAsyncKeyState(key) >> 15) & 1;
	#endif
	#ifdef LINUX_OS
		SDL_PumpEvents();//примусово оновити події, чинить деякі зависання клавіш на лінукс
		auto state = SDL_GetKeyboardState(NULL);
		return state[key];
	#endif
}

void FPS() { //помістити в цикл малювання кадра
	int ScreenFPS; //оце на вивiд
	static int dinFPS = 0;
	static int Seconds = 0;
#ifdef WINDOWS_OS
	SYSTEMTIME st;
	dinFPS++; GetLocalTime(&st);
	if (Seconds != st.wSecond) {
		Seconds = st.wSecond;
		ScreenFPS = dinFPS;
		cout << "FPS:" << ScreenFPS << endl;
		dinFPS = 0;
	}
#endif
#ifdef LINUX_OS
	struct timeval tv;
	dinFPS++; gettimeofday(&tv, NULL);
	if(Seconds != tv.tv_sec) {
		Seconds = tv.tv_sec;
		ScreenFPS = dinFPS;
		printf("FPS: %d\n", ScreenFPS);
		dinFPS = 0;
	}
#endif
}

#ifdef LINUX_OS
	SDL_Renderer* renderer;
#endif

void din_SetPixel(int x, int y, BYTE R, BYTE G, BYTE B, BYTE Alpha/* = 255 в прототипі*/, bool shiftToCenter/* = true в прототипі*/) {
	if(shiftToCenter) {
		x += ScreenRect.right / 2;
		y += ScreenRect.bottom / 2;
	}

	if(x >= 0/*2*/ && x < ScreenRect.right && y >= 0/*2*/ && y < ScreenRect.bottom) {
#ifdef WINDOWS_OS
		RGBQUAD* tmp = GetAddr(im, x, y);
		tmp->rgbBlue = B;
		tmp->rgbGreen = G;
		tmp->rgbRed = R;
		tmp->rgbReserved = Alpha;
#endif
#ifdef LINUX_OS
		SDL_SetRenderDrawColor(renderer, R, G, B, Alpha);
		SDL_RenderDrawPoint(renderer, x, y);
#endif
	}
}

bool isPointInPlane(Point_3d M, Plane plane) { //тест чи 3D точка в 3D трикутнику
	//ABC-трикутник
	//M-тестуєма точка
	Point_3d A = plane.A; Point_3d B = plane.B; Point_3d C = plane.C;
	double AB = LineLength(A, B);
	double BC = LineLength(B, C);
	double AC = LineLength(A, C);
	double MB = LineLength(M, B);
	double MA = LineLength(M, A);
	double MC = LineLength(M, C);
	double all = HeronsFormula(AB, BC, AC);
	double okremo1 = HeronsFormula(AB, MB, MA);
	double okremo2 = HeronsFormula(MB, BC, MC);
	double okremo3 = HeronsFormula(MA, MC, AC);
	double okremo = okremo1 + okremo2 + okremo3;

	//маленький розкид
	static double koef = 1;
	if(fabs(all - okremo) <= koef) return true;
	else return false;
}

void getPoint_IntersectionLinePlane(double ScreenX, double ScreenY, Point_3d* pM, Plane plane) { //точка перетину прямої з площиною
	//ДЛЯ ПРЯМОЇ

	Point_3d L1 = { ScreenX, ScreenY, 0 };
	Point_3d L2 = { 0, 0, -From };

	//напрямний вектор
	Point_3d S = { L1.x - L2.x, L1.y - L2.y, L1.z - L2.z };//переміщуємо пряму, щоб точка L2 була в початку координат
	//==============================
	//ДЛЯ ПЛОЩИНИ

	Point_3d A = plane.A; Point_3d B = plane.B; Point_3d C = plane.C;

	Point_3d PlaneNormalVector = calculateTriangleNormal(A, B, C);
	double OnlyNumberKoef = -dotProduct(A /*будь-яка точка на площині, можна використати і B або C*/, PlaneNormalVector); //це те, наскільки площина здвинута у напрямку нормалі, але з протилежним знаком
	//==============================
	double t = -(dotProduct(L1, PlaneNormalVector) + OnlyNumberKoef) / dotProduct(S, PlaneNormalVector);
	*pM = { t * S.x + L1.x, t * S.y + L1.y, t * S.z + L1.z }; //M=XYZ
}

void MyTriangleRasterizationOutput(float ScreenX, float ScreenY, Plane T) {
	//рисувати тільки коли піксель в межах екрану
	if(!(ScreenX >= -ScreenRect.right / 2 && ScreenX < ScreenRect.right / 2 && ScreenY >= -ScreenRect.bottom / 2 && ScreenY < ScreenRect.bottom / 2)) return;
	Point_3d M;
	getPoint_IntersectionLinePlane(ScreenX, ScreenY, &M, T);
	if(M.z <= 0) {
		if(din_isKeyPressed(KEY_CUT)) return; //не показувати все, що ближче віртуального екрану
	}

	if(isPointInPlane(M, T)) {
		int index = GetIndex((int)ScreenX + (ScreenRect.right/2), (int)ScreenY + (ScreenRect.bottom/2),ScreenRect.right);
		double DistanceToPixel = LineLength(Point_3d{0,0,-From}, M);
		if(DistanceToPixels[index] == UNDEFINED || DistanceToPixel < DistanceToPixels[index]) {
			DistanceToPixels[index] = DistanceToPixel;
			if(texture) {
				Point_3d _M = M;
				mainTexturization(T.A, T.B, T.C, _M);
				auto kx = _M.x + 200;//половина ширини зображення
				auto ky = _M.y + 200;
			#ifdef WINDOWS_OS
				BYTE* ptr = Ttexture_GetPixelAddress(kx, TtextureRect.bottom - 1 - ky);
			#endif
			#ifdef LINUX_OS
				BYTE* ptr = Ttexture_GetPixelAddress(kx, ky);
			#endif
				if(ptr)din_SetPixel(ScreenX, ScreenY, ptr[0], ptr[1], ptr[2], 255);
			} else {
				din_SetPixel(ScreenX, ScreenY, T.color.r, T.color.g, T.color.b, 255);
			}
		}
	}
}

void DrawPlane(Plane T) {
	Point_2d A2, B2, C2;
	IIIdToIId(T.A, A2);
	IIIdToIId(T.B, B2);
	IIIdToIId(T.C, C2);

	//оптимізація(рендер тільки у області)
	int Min_x, Min_y, Max_x, Max_y;
	Min_x = A2.x;
	Max_x = A2.x;
	Min_y = A2.y;
	Max_y = A2.y;

	if(B2.x < Min_x) { Min_x = B2.x; }
	if(B2.y < Min_y) { Min_y = B2.y; }
	if(B2.x > Max_x) { Max_x = B2.x; }
	if(B2.y > Max_y) { Max_y = B2.y; }

	if(C2.x < Min_x) { Min_x = C2.x; }
	if(C2.y < Min_y) { Min_y = C2.y; }
	if(C2.x > Max_x) { Max_x = C2.x; }
	if(C2.y > Max_y) { Max_y = C2.y; }

	if(Min_x <= Max_x && Min_y <= Max_y) { //перевірка на переповнення
		for(int ScreenY = Min_y; ScreenY <= Max_y; ScreenY++) {
			for(int ScreenX = Min_x; ScreenX <= Max_x; ScreenX++) {
				//можна розпаралелити
				MyTriangleRasterizationOutput(ScreenX, ScreenY, T);
			}
		}
	}
}

void world_unload3DObj() {
    Vertices.clear();
    Planes.clear();
}
std::string filename_for_reload="";
void/*bool*/ world_load3DObj(const std::string filename) {
	filename_for_reload=filename;
	world_unload3DObj();
	std::ifstream file; file.open(filename);

	if(!file) { cout << filename << " not found" << endl; return; }
	cout << "start loading " << filename << endl;

	while(!file.eof()) {
		std::string str;
		std::getline(file, str);
		if(str == "") {
			//cout << "[empty]" << endl;
			continue;
		} else if(str == "\r") {
			//cout << "[\\r]" << endl;
			continue;
		} else if(str == "\n") {
			//cout << "[\\n]" << endl;
			continue;
		}

		std::string type = str.substr(0, 2);
		str = str.substr(2, str.size());

		if(type == "v ") {
			//ТОЧКИ
			std::stringstream str_stream(str);
			float x, y, z;
			str_stream >> x >> y >> z;
			Vertices.push_back(Point_3d{ x, y, z });
		} else if (type == "f ") {
			//ПЛОЩИНИ
			int indexes[4];
			int ti = 0;
			int vi = 0;
			int verticesCount = 1;
			const char* buffer = str.c_str();
			for (const char* i = buffer; *i!='\0'; i++) if(*i==' ')verticesCount++;
			if (verticesCount == 3||verticesCount == 4) {// трикутні та чотирикутні полігони
				if (verticesCount == 4) {
					sscanf(buffer, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
					&indexes[0], &ti, &vi,
					&indexes[1], &ti, &vi,
					&indexes[2], &ti, &vi,
					&indexes[3], &ti, &vi);
				} else
					sscanf(buffer, "%d/%d/%d %d/%d/%d %d/%d/%d",
					&indexes[0], &ti, &vi,
					&indexes[1], &ti, &vi,
					&indexes[2], &ti, &vi);				
				Point_3d Tm[4];//2 трикутника
				for(int i = 0; i < verticesCount; i++) Tm[i] = Vertices[indexes[i] - 1];
				COLOR planeColor=COLOR{ (BYTE)(rand()%256), (BYTE)(rand()%256), (BYTE)(rand()%256) };
				Plane plane1 = { Tm[0], Tm[1], Tm[2], planeColor};
				Planes.push_back(plane1);
				if (verticesCount == 4) {
					Plane plane2 = { Tm[0], Tm[3], Tm[2], planeColor};
					Planes.push_back(plane2);
				}
			}
		}
	}
#ifdef WINDOWS_OS
	cout << filename << " loaded" << endl;
#endif
}
void world_reload3DObj() {
	if(!filename_for_reload.empty()) {
		world_load3DObj(filename_for_reload);
	}
}

void Din_Draw() {
	for(int i = 0; i < ScreenRect.right * ScreenRect.bottom; i++) DistanceToPixels[i] = UNDEFINED; //ZeroMemory();
	if(din_isKeyPressed(KEY_FromPlus)) { From += 5.F; cout << "From:" << From << endl; }
	if(din_isKeyPressed(KEY_FromMinus) && From >= 45) { From -= 5.F; cout << "From:" << From << endl; }
	if(din_isKeyPressed(KEY_ZoomPlus)) { Zoom += 1; cout << "Zoom:" << Zoom << endl; }
	if(din_isKeyPressed(KEY_ZoomMinus)) { Zoom -= 1; cout << "Zoom:" << Zoom << endl; }
	//*********************************************
	static double XZ_around_angle = 0; XZ_around_angle += 1;
	if(XZ_around_angle >= 360) XZ_around_angle -= 360;//XZ_around_angle=XZ_around_angle%360 альтернативний варіант
	double rad_XZ_around_angle = gradus2rad(XZ_around_angle);
	//-----------------------
	static double XY_angle = 0; XY_angle += 4;
	if(XY_angle >= 360) XY_angle -= 360;
	double rad_XY_angle = gradus2rad(XY_angle);
	//-----------------------
	static double ZY_angle = 0; ZY_angle += 2;
	if(ZY_angle >= 360) ZY_angle -= 360;
	double rad_ZY_angle = gradus2rad(ZY_angle);
	//-----------------------
	static double XZ_angle = 0; XZ_angle += 3;
	if(XZ_angle >= 360) XZ_angle -= 360;
	double rad_XZ_angle = gradus2rad(XZ_angle);
	//*********************************************
	Point_3d Navkolo{ 0,0,0 };
	if(drawBackground) {
		for(int y = 0; y < TtextureRect.bottom; y++) {
			for(int x = 0; x < TtextureRect.right; x++) {
#ifdef WINDOWS_OS
				BYTE* ptr = Ttexture_GetPixelAddress(x, TtextureRect.bottom - 1 - y);
#endif
#ifdef LINUX_OS
				BYTE* ptr = Ttexture_GetPixelAddress(x, y);
#endif
				if(ptr)din_SetPixel(x, y, ptr[0], ptr[1], ptr[2], 255, false);
			}
		}
	}
if(drawSphere){
	Point_3d sc{ 0/*115*/,0,0 };//SphereCenter
	double r = 50;//радіус сфери
	Point_3d Light{ 115/*0*/,0,0 };
	
	static double kk = 0; kk += 6;
	if(kk >= 360) kk -= 360;
	if (demoAreas) {
		sc.x=sin(gradus2rad(kk))*50.0; // -50..50 
		Light.x=0;
	} else {
		sc.x=0; Light.x=115;
	}
	if(demoAreas){
	//-----------------------
	static double XY_angleS = 0; XY_angleS += 6;//щоб не як у тетраедра, а то синхронізується
	if(XY_angleS >= 360) XY_angleS -= 360;
	//double rad_XY_angleSphere = gradus2rad(XY_angleS);
	//-----------------------
	//Rotation3D(sc, rad_XZ_around_angle, rad_XY_angleSphere, rad_ZY_angle, rad_XZ_angle, Light);
	} else Rotation3D(Light, rad_XZ_around_angle, rad_XY_angle, rad_ZY_angle, rad_XZ_angle, sc);
	Sphere(sc, r, Light);
	if(!demoAreas)Sphere(Light, 20, Light);
	if(!drawLight)LightPos(Light);
} //endof if(drawSphere)
if(drawPlanes){
	for(Plane plane: Planes) {
		ScalePlane(plane, Zoom);
		RotatePlane(plane, rad_XZ_around_angle, rad_XY_angle, rad_ZY_angle, rad_XZ_angle, Navkolo);
		DrawPlane(plane);
	}
}
if(drawFluid){//FluidSimMOD
	double cursorScreenX = 0, cursorScreenY = 0;
	#ifdef WINDOWS_OS
		POINT mousePos; GetCursorPos(&mousePos);
		RECT windowRect; GetWindowRect(my_window, &windowRect);
		cursorScreenX = (mousePos.x - windowRect.left) - ScreenRect.right / 2.0;
		cursorScreenY = -(mousePos.y - windowRect.top) + ScreenRect.bottom / 2.0;
	#endif
	#ifdef LINUX_OS
		int mousePosX, mousePosY; SDL_GetMouseState(&mousePosX, &mousePosY);
		cursorScreenX = mousePosX - ScreenRect.right / 2.0;
		cursorScreenY = mousePosY - ScreenRect.bottom / 2.0;
	#endif
	LightPosition = IIdToIIIdRayCast(cursorScreenX, cursorScreenY, FromEyeToLight);
	DrawFluid();
}
//1 піксель рамка області малювання
	if(drawFrame){
		for(int x = 0; x < ScreenRect.right; x++) {
			din_SetPixel(x, 0, 255, 255, 255, 255, false);
			din_SetPixel(x, ScreenRect.bottom - 1, 255, 255, 255, 255, false);
		}
		for(int y = 1; y < ScreenRect.bottom - 1; y++) {
			din_SetPixel(0, y, 255, 255, 255, 255, false);
			din_SetPixel(ScreenRect.right - 1, y, 255, 255, 255, 255, false);
		}
	}
}

#ifdef WINDOWS_OS
	void normalize() {
		for(int i = 0; i < ScreenRect.right; i++) {
			for(int j = 0; j < ScreenRect.bottom; j++) {
				BYTE* ptr = (BYTE*)GetAddr(im, i, j);
				ptr[0] = ((ptr[0] * ptr[3])) / 255;
				ptr[1] = ((ptr[1] * ptr[3])) / 255;
				ptr[2] = ((ptr[2] * ptr[3])) / 255;
			}
		}
	}
#endif
#ifdef WINDOWS_OS
bool Input(WPARAM sc) {
#endif
#ifdef LINUX_OS
bool Input(SDL_Scancode sc) {
#endif
	static const char* STR_ENABLED = "enabled.";
	static const char* STR_DISABLED = "disabled.";
	bool exit=false;
	if (sc==KEY_1||sc==KEY_2||sc==KEY_3||sc==KEY_4){
		drawBackground=true;
		texture=true;
		drawLight=false;
		demoAreas=false;
		drawPlanes=true;
		drawSphere=false;
		Zoom=121; world_load3DObj("load.obj");
	} else if (sc==KEY_8||sc==KEY_9||sc==KEY_0){
		drawBackground=false;
		texture=false;
		drawLight=false;
		demoAreas=false;
		drawPlanes=true;
		drawSphere=false;
	}
	switch (sc) {
		case KEY_ESC: exit = true; break;
		case KEY_1: Ttexture_Load("tex1.png"); break;
		case KEY_2: Ttexture_Load("tex2.png"); break;
		case KEY_3: Ttexture_Load("tex3.png"); break;
		case KEY_4: Ttexture_Load("tex4.png"); break;
		case KEY_5:
			drawBackground=false;
			texture=false;
			drawLight=true;
			demoAreas=false;
			drawPlanes=false;
			drawSphere=true;
			break;
		case KEY_6:
			drawBackground=false;
			texture=false;
			drawLight=true;
			demoAreas=true;
			drawPlanes=true;
			drawSphere=true;
			Zoom=121; world_load3DObj("load.obj");
			break;
		case KEY_7:
			drawBackground=false;
			texture=false;
			drawLight=true;
			demoAreas=false;
			drawPlanes=true;
			drawSphere=true;
			Zoom=121; world_load3DObj("load.obj");
			break;
		case KEY_8:
			Zoom=70; world_load3DObj("0_LOWbybluk.obj");
			break;
		case KEY_9:
			Zoom=80; world_load3DObj("0_bybluk.obj");
			break;
		case KEY_0:
			Zoom=121; world_load3DObj("cube.obj");
			break;
		case KEY_E: world_reload3DObj(); break;
		case KEY_tex1: Ttexture_Load("tex1.png"); break;
		case KEY_tex2: Ttexture_Load("tex2.png"); break;
		case KEY_tex3: Ttexture_Load("tex3.png"); break;
		case KEY_tex4: Ttexture_Load("tex4.png"); break;
		case KEY_drawBackground:
			drawBackground = !drawBackground;
			cout << "drawBackground is " << (drawBackground ? STR_ENABLED : STR_DISABLED) << endl;
			break;
		case KEY_texture:
			texture = !texture;
			cout << "Texturization is " << (texture ? STR_ENABLED : STR_DISABLED) << endl;
			break;
		case KEY_drawLight:
			drawLight = !drawLight;
			cout << "drawLight is " << (drawLight ? STR_ENABLED : STR_DISABLED) << endl;
			break;
		case KEY_demoAreas:
			demoAreas = !demoAreas;
			cout << "demoAreas is " << (demoAreas ? STR_ENABLED : STR_DISABLED) << endl;
			break;
		case KEY_drawPlanes:
			drawPlanes = !drawPlanes;
			cout << "drawPlanes is " << (drawPlanes ? STR_ENABLED : STR_DISABLED) << endl;
			break;
		case KEY_drawSphere:
			drawSphere = !drawSphere;
			cout << "drawSphere is " << (drawSphere ? STR_ENABLED : STR_DISABLED) << endl;
			break;
		case KEY_drawFluid://FluidSimMOD
			drawFluid = !drawFluid;
			drawSphere = !drawFluid;
			cout << "drawFluid is " << (drawFluid ? STR_ENABLED : STR_DISABLED) << endl;
			break;
		case KEY_drawFrame:
			drawFrame = !drawFrame;
			cout << "drawFrame is " << (drawFrame ? STR_ENABLED : STR_DISABLED) << endl;
			break;
		default: break;
	}
return exit;
}

#ifdef WINDOWS_OS
	LRESULT CALLBACK WndProc(HWND my_window, UINT message, WPARAM sc, LPARAM lParam) {
		POINT pt2 = { 0, 0 }; SIZE WndSize = { ScreenRect.right, ScreenRect.bottom };
		BLENDFUNCTION blend;
		ZeroMemory(&blend, sizeof(BLENDFUNCTION));
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.SourceConstantAlpha = 0xff;//100%
		blend.AlphaFormat = AC_SRC_ALPHA;

		switch (message) {
			case WM_PAINT: {	
				for(int i = 0; i < ScreenRect.right * ScreenRect.bottom; i++) im[i].rgbReserved = 0; //все прозорим залити
				Din_Draw(); //у MemDC
				normalize();
				FPS();
				UpdateLayeredWindow(my_window, hdc, 0/*pos*/, &WndSize, MemDC, &pt2, RGB(0, 0, 0), &blend, ULW_ALPHA);
			}
			break;
			case WM_KEYDOWN:
				Input(sc);
				break;
			case WM_LBUTTONDOWN://переміщення вікна
				if(din_isKeyPressed(KEY_CTRL)) {
					GetCursorPos(&MousePnt);
					dragging = true;
					SetCapture(my_window);
				}
				break;
			case WM_MOUSEMOVE://переміщення вікна
				if(dragging && din_isKeyPressed(KEY_CTRL)) {
					POINT point;
					RECT wndrect;
					GetCursorPos(&point);
					GetWindowRect(my_window, &wndrect);

					wndrect.left = wndrect.left + (point.x - MousePnt.x);
					wndrect.top = wndrect.top + (point.y - MousePnt.y);

					SetWindowPos(my_window, NULL, wndrect.left, wndrect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
					MousePnt = point;
				}
				break;
			case WM_LBUTTONUP://переміщення вікна
				if(dragging) {
					dragging = false;
					ReleaseCapture();
				}
				break;
			case WM_MOUSEWHEEL: {//відстань до сфери світла//FluidSimMOD
				short scrollY = (short)HIWORD(sc);
				double step = 40;
				if(scrollY > 0) FromEyeToLight += step;
				else if(scrollY < 0) FromEyeToLight -= step;
				if(scrollY != 0) cout << "FromEyeToLight: " << FromEyeToLight << endl;
				break;
			}
			case WM_CLOSE:
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			default:
				InvalidateRect(my_window, 0, 1);
				return DefWindowProcW(my_window, message, sc, lParam);
		}
		return 0;
	}
#endif

#ifdef WINDOWS_OS
	ATOM MyRegisterClass(HINSTANCE hInstance, WCHAR* szWindowClass) {
		WNDCLASSEXW wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);//DEBUG
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = 0;
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);//DEBUG
		return RegisterClassExW(&wcex);
	}
#endif

#ifdef LINUX_OS
	void SDL_Paint() {
		FPS();
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); //все прозорим залити
		SDL_RenderClear(renderer);
		Din_Draw();
		SDL_RenderPresent(renderer); //updateWND
	}
#endif

int main(int argc, char* argv[]) {
#ifdef LINUX_OS
	cout << "LINUX VERSION" << endl;
	cout << "CONTROLS:" << endl;
#endif
#ifdef WINDOWS_OS
	cout << "WINDOWS VERSION" << endl;
	cout << "CONTROLS:" << endl;
	cout << "Ctrl+mouse drag : move window" << endl;
#endif
	cout << "1..90 demos" << endl;
	cout << "Q W : change distance from eye to screen(FOV)" << endl;
	cout << "a,s,d,f : change images" << endl;
	cout << "Z : turn on/off png drawing" << endl;
	cout << "X : turn on/off texture drawing" << endl;
	cout << "C : turn on/off pseudolight drawing" << endl;
	cout << "V : turn on/off DEMO areas" << endl;
	cout << "B : turn on/off planes(obj model) drawing" << endl;
	cout << "N : turn on/off sphere drawing" << endl;
	cout << "G : turn on/off fluid simulation" << endl;//FluidSimMOD
	cout << "M : turn on/off screen frame" << endl;
	cout << "Space : virtual cut" << endl;
	cout << "+ - : zoom" << endl;
	cout << "Esc : exit" << endl;

	srand(time(0));
	Ttexture_Load("tex1.png");
	if(argc > 1) world_load3DObj(argv[1]); else world_load3DObj("load.obj");
	DistanceToPixels = new double[ScreenRect.right * ScreenRect.bottom]; //відстані від камери до 3D точок
	for(int i = 0; i < ScreenRect.right * ScreenRect.bottom; i++) DistanceToPixels[i] = UNDEFINED;
	#ifdef WINDOWS_OS
	//GetWindowRect(GetDesktopWindow(), &ScreenRect);
	cout << ScreenRect.right << "x" << ScreenRect.bottom << endl; //отримати розмір екрану
	#endif

#ifdef WINDOWS_OS
	im = new RGBQUAD[ScreenRect.right * ScreenRect.bottom];//пікселі BlueGreenRedAlpha(BGRA)
	HINSTANCE hInstance = GetModuleHandleW(NULL); int nCmdShow = SW_SHOW;//заміна віконній програмі(щоб була активна і консоль)
	WCHAR szTitle[] = L"C++"; WCHAR szWindowClass[] = L"Window_class";
	MyRegisterClass(hInstance, szWindowClass);//зареєструвати клас вікна
	/*WS_EX_OVERLAPPEDWINDOW*//*WS_EX_CLIENTEDGE*/
//FluidSimMOD: my_window global
	my_window = CreateWindowExW(WS_EX_LAYERED/*шаристе вікно(те, що підтримує прозорість)*/ | WS_EX_TOPMOST/*над усіма вікнами*/, szWindowClass, szTitle, /*прибрати шапку щоб не було зверху заокруглень*/WS_POPUP/*WS_OVERLAPPEDWINDOW було раніше*/,
		100, 100/*розташування вікна*//*CW_USEDEFAULT, CW_USEDEFAULT ***стандартне розташування, але лише для WS_OVERLAPPEDWINDOW*/, ScreenRect.right, ScreenRect.bottom/*розміри вікна*/, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(my_window, nCmdShow); UpdateWindow(my_window);

	BITMAPINFO BMI;
	ZeroMemory(&BMI, sizeof(BITMAPINFO));
	BMI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BMI.bmiHeader.biBitCount = 32;//32 біта(з прозорістю),24 - без
	BMI.bmiHeader.biWidth = ScreenRect.right;
	BMI.bmiHeader.biHeight = ScreenRect.bottom;
	BMI.bmiHeader.biPlanes = 1;
	BMI.bmiHeader.biCompression = BI_RGB;

	hdc = GetDC(my_window);
	MemDC = CreateCompatibleDC(hdc);
	HBITMAP hbmp = CreateDIBSection(MemDC, &BMI, DIB_RGB_COLORS, (void**)&im, NULL, 0);//створити бітмап в оперативній пам'яті, а не пам'яті відеокарти
	HBITMAP h_old_bmp = (HBITMAP)SelectObject(MemDC, hbmp);
#endif
#ifdef LINUX_OS
	SDL_Window *window;
	SDL_Init(SDL_INIT_VIDEO);
	//disable fps lock for sdl2://вимкнути обмеження кадрів в секунду:
//створюємо вікно
window = SDL_CreateWindow("My Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
						  ScreenRect.right, ScreenRect.bottom, SDL_WINDOW_SHOWN);
//створюємо рендерер без VSync
renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
#endif

	//цикл малювання
	#ifdef WINDOWS_OS
	MSG msg;
	while(!din_isKeyPressed(KEY_ESC) && GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	#endif
	#ifdef LINUX_OS
	bool exit=false;
	while(!exit) {
		SDL_Event event;
		SDL_Paint();
		//SDL_Delay(5);
		SDL_PumpEvents();//примусово оновити події, чинить деякі зависання клавіш на лінукс		
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: exit = true; break;
				case SDL_KEYDOWN: {
					SDL_Scancode sc = event.key.keysym.scancode;
					exit=Input(sc);
					break;
				}
				case SDL_MOUSEWHEEL: {//відстань до сфери світла//FluidSimMOD
					double step = 40;
					if(event.wheel.y > 0) FromEyeToLight += step;
					else if(event.wheel.y < 0) FromEyeToLight -= step;
					if(event.wheel.y != 0) cout << "FromEyeToLight: " << FromEyeToLight << endl;
					break;
				}
			}
		}
	}
	#endif
	cout << "EXIT" << endl;
	//звільнення пам'яті
	#ifdef WINDOWS_OS
	DestroyWindow(my_window);
	SelectObject(MemDC, h_old_bmp);
	DeleteObject(hbmp);
	DeleteDC(MemDC);
	DeleteDC(hdc);
	//не робити delete[]im(якась функція вже видалила)
	#endif
	#ifdef LINUX_OS
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	#endif
	stbi_image_free(Ttexture);
	delete[]DistanceToPixels;
	return 0;
}