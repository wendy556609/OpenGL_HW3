#include "header/Angel.h"
//#include "Common/TypeDefine.h"
//#include "Common/Flat.h"
#include "Common/Camera.h"
#include "Common/Sprite.h"
//#include "Common/Cube.h"
//#include "Common/Sphere.h"
//#include "Common/Sprite.h"
//#include "Common/ModelPool.h"
#include "Common/Room.h"

#define SPACE_KEY 32
#define SCREEN_SIZE 800
#define HALF_SIZE SCREEN_SIZE /2
#define VP_HALFWIDTH  20.0f
#define VP_HALFHEIGHT 20.0f


// For Model View and Projection Matrix
mat4 g_mxModelView(1.0f);
mat4 g_mxProjection;

// For View Point
GLfloat g_fRadius = GRID_SIZE * 2;
GLfloat g_fTheta = 60.0f*DegreesToRadians;
GLfloat g_fPhi = 45.0f*DegreesToRadians;

vec4    g_vUp(0.0, 1.0, 0.0, 0.0);

point4 eye;
point4 at;
point4 front;
point4 right;

Room *room;

//----------------------------------------------------------------------------
// Part 2 : for single light source
bool g_bAutoRotating = false;
float g_fElapsedTime = 0;
float g_fLightRadius = 6;
float g_fLightTheta = 0;

float g_fLightR = 0.5f;
float g_fLightG = 0.5f;
float g_fLightB = 0.5f;

LightSource g_Light1[LightCount] = {
	{
		0,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(0.95f, 0.95f, 0.95f, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(0.0f, 15.0f, 15.0f, 1.0f),   // position
		point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
		vec3(0.0f, 0.0f, 0.0f),			  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
		1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
		45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		0	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1	,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
		0	,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
		0	,	// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
		1
	},
	{
		1,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(g_fLightR, 0, 0, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(25.0f, 10.0f, 25.0f, 1.0f),   // position
		point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
		vec3(25.0f, 0.0f, 25.0f),			  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
		1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
		45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		0.707f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1	,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
		0	,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
		0	,	// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
		1
	},
	{
		1,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(0, g_fLightG, 0, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(-25.0f, 10.0f, 25.0f, 1.0f),   // position
		point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
		vec3(-25.0f, 0.0f, 25.0f),			  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
		1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
		45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		0.707f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1	,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
		0	,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
		0	,	// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
		1
	},
	{
		1,
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // ambient 
		color4(0, 0, g_fLightB, 1.0f), // diffuse
		color4(g_fLightR, g_fLightG, g_fLightB, 1.0f), // specular
		point4(0.0f, 10.0f, -30.0f, 1.0f),   // position
		point4(0.0f, 0.0f, 0.0f, 1.0f),   // halfVector
		vec3(0.0f, 0.0f, -30.0f),			  //spotTarget
		vec3(0.0f, 0.0f, 0.0f),			  //spotDirection
		1.0f	,	// spotExponent(parameter e); cos^(e)(phi) 
		45.0f,	// spotCutoff;	// (range: [0.0, 90.0], 180.0)  spot 的照明範圍
		0.707f	,	// spotCosCutoff; // (range: [1.0,0.0],-1.0), 照明方向與被照明點之間的角度取 cos 後, cut off 的值
		1	,	// constantAttenuation	(a + bd + cd^2)^-1 中的 a, d 為光源到被照明點的距離
		0	,	// linearAttenuation	    (a + bd + cd^2)^-1 中的 b
		0	,	// quadraticAttenuation (a + bd + cd^2)^-1 中的 c
		1
	}
};

//Buttom
Sprite *g_p2DBtn[4];
mat4  g_2DView;
mat4  g_2DProj;
//CWireSphere *_pLight;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// 函式的原型宣告
extern void IdleProcess();
void SetBtn();

void init(void)
{

	// 產生所需之 Model View 與 Projection Matrix

	eye = point4(0.0f, 10.0f, -45.0f, 1.0f);
	at = point4(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);
	auto camera = Camera::create();
	camera->updateViewLookAt(eye, at);
	camera->updatePerspective(60.0, (GLfloat)SCREEN_SIZE / (GLfloat)SCREEN_SIZE, 1.0, 1000.0);

	room = new Room;

	for (int i = 0; i < LightCount; i++)
	{
		g_Light1[i].UpdateDirection();
	}

	SetBtn();

	// 因為本範例不會動到 Projection Matrix 所以在這裡設定一次即可
	// 就不寫在 OnFrameMove 中每次都 Check
	bool bPDirty;
	
	mat4 mpx = camera->getProjectionMatrix(bPDirty);

	room->SetProjectionMatrix(mpx);	
}

void SetBtn() {
	mat4 mxT;
	vec4 vColor = vec4(0, 0, 0, 1.0f);
	mat4 mxS = Scale(0.1f, 0.1f, 1.0f);
	g_p2DBtn[0] = new Sprite;
	vColor.x = 1; vColor.y = 1; vColor.z = 1; g_p2DBtn[0]->SetDefaultColor(vColor);
	mxT = Translate(0.90f, -0.90f, 0);
	g_p2DBtn[0]->SetBtnTRSMatrix(mxT*mxS);
	g_p2DBtn[0]->SetViewMatrix(g_2DView);
	g_p2DBtn[0]->SetViewMatrix(g_2DProj);

	g_p2DBtn[1] = new Sprite;
	vColor.x = 1; vColor.y = 0; vColor.z = 0; g_p2DBtn[1]->SetDefaultColor(vColor);
	mxT = Translate(0.75f, -0.90f, 0);
	g_p2DBtn[1]->SetBtnTRSMatrix(mxT*mxS);
	g_p2DBtn[1]->SetViewMatrix(g_2DView);
	g_p2DBtn[1]->SetViewMatrix(g_2DProj);

	g_p2DBtn[2] = new Sprite;
	vColor.x = 0; vColor.y = 1; vColor.z = 0; g_p2DBtn[2]->SetDefaultColor(vColor);
	mxT = Translate(0.60f, -0.90f, 0);
	g_p2DBtn[2]->SetBtnTRSMatrix(mxT*mxS);
	g_p2DBtn[2]->SetViewMatrix(g_2DView);
	g_p2DBtn[2]->SetViewMatrix(g_2DProj);

	g_p2DBtn[3] = new Sprite;
	vColor.x = 0; vColor.y = 0; vColor.z = 1; g_p2DBtn[3]->SetDefaultColor(vColor);
	mxT = Translate(0.45f, -0.90f, 0);
	g_p2DBtn[3]->SetBtnTRSMatrix(mxT*mxS);
	g_p2DBtn[3]->SetViewMatrix(g_2DView);
	g_p2DBtn[3]->SetViewMatrix(g_2DProj);
}

//----------------------------------------------------------------------------
void GL_Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window

	room->Draw();

	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i < 4; i++) g_p2DBtn[i]->Draw();
	glEnable(GL_DEPTH_TEST);

	glutSwapBuffers();	// 交換 Frame Buffer
}

//----------------------------------------------------------------------------
// Part 2 : for single light source
void UpdateLightPosition(float dt)
{
	mat4 mxT;
	// 每秒繞 Y 軸轉 90 度
	g_fElapsedTime += dt;
	g_fLightTheta = g_fElapsedTime*(float)M_PI_2;
	if (g_fLightTheta >= (float)M_PI*2.0f) {
		g_fLightTheta -= (float)M_PI*2.0f;
		g_fElapsedTime -= 4.0f;
	}
	g_Light1[0].position.x = 15.0f * sinf(g_fLightTheta);
	g_Light1[0].position.z = 15.0f * cosf(g_fLightTheta);
	mxT = Translate(g_Light1[0].position);
}
//----------------------------------------------------------------------------

void onFrameMove(float delta)
{
	mat4 mvx;	// view matrix & projection matrix
	bool bVDirty;	// view 與 projection matrix 是否需要更新給物件
	auto camera = Camera::getInstance();
	mvx = camera->getViewMatrix(bVDirty);
	if (bVDirty) {
		room->SetViewMatrix(mvx);
	}

	if (g_bAutoRotating) { // Part 2 : 重新計算 Light 的位置
		UpdateLightPosition(delta);
	}
	// 如果需要重新計算時，在這邊計算每一個物件的顏色

	room->Update(g_Light1, delta);

	GL_Display();
}

//----------------------------------------------------------------------------

void Win_Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case  SPACE_KEY:

		break;
		//----------------------------------------------------------------------------
		// Part 2 : for single light source
	case 65: // A key
	case 97: // a key

		break;
	case 82: // R key
		if (g_fLightR <= 0.95f) g_fLightR += 0.05f;
		g_Light1[0].diffuse.x = g_fLightR;
		break;
	case 114: // r key
		if (g_fLightR >= 0.05f) g_fLightR -= 0.05f;
		g_Light1[0].diffuse.x = g_fLightR;
		break;
	case 71: // G key
		if (g_fLightG <= 0.95f) g_fLightG += 0.05f;
		g_Light1[0].diffuse.y = g_fLightG;
		break;
	case 103: // g key
		if (g_fLightG >= 0.05f) g_fLightG -= 0.05f;
		g_Light1[0].diffuse.y = g_fLightG;
		break;
	case 66: // B key
		if (g_fLightB <= 0.95f) g_fLightB += 0.05f;
		g_Light1[0].diffuse.z = g_fLightB;
		break;
	case 98: // b key
		if (g_fLightB >= 0.05f) g_fLightB -= 0.05f;
		g_Light1[0].diffuse.z = g_fLightB;
		break;
		//----------------------------------------------------------------------------
	case 033:
		glutIdleFunc(NULL);
		delete room;
		for (int i = 0; i < 4; i++) delete g_p2DBtn[i];
		Camera::getInstance()->destroyInstance();
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------
void Win_Mouse(int button, int state, int x, int y) {
	vec2 pt;
	switch (button) {
	case GLUT_LEFT_BUTTON:   // 目前按下的是滑鼠左鍵
		if (state == GLUT_DOWN) {
			pt.x = 2.0f*(float)x / SCREEN_SIZE - 1.0f;
			pt.y = 2.0f*(float)(SCREEN_SIZE - y) / SCREEN_SIZE - 1.0f;
			if (g_p2DBtn[0]->OnTouches(pt)) {
				if (g_p2DBtn[0]->getButtonStatus())g_bAutoRotating = true;
				else g_bAutoRotating = false;
			}
			if (g_p2DBtn[1]->OnTouches(pt)) {
				if (g_p2DBtn[1]->getButtonStatus())g_Light1[1].isLighting = false;
				else g_Light1[1].isLighting = true;
			}
			if (g_p2DBtn[2]->OnTouches(pt)) {
				if (g_p2DBtn[2]->getButtonStatus())g_Light1[2].isLighting = false;
				else g_Light1[2].isLighting = true;
			}
			if (g_p2DBtn[3]->OnTouches(pt)) {
				if (g_p2DBtn[3]->getButtonStatus())g_Light1[3].isLighting = false;
				else g_Light1[3].isLighting = true;
			}
		}
		break;
	case GLUT_MIDDLE_BUTTON:  // 目前按下的是滑鼠中鍵 ，換成 Y 軸
		if (state == GLUT_UP) {
			eye += g_vUp* 3.0f;
			if (eye.y > (GRID_SIZE / 2 - 5.0f))eye.y = (GRID_SIZE / 2 - 5.0f);
		}; 		  //if ( state == GLUT_DOWN ) ; 
		break;
	case GLUT_RIGHT_BUTTON:   // 目前按下的是滑鼠右鍵
		if (state == GLUT_UP) {
			eye -= g_vUp* 3.0f;
			if (eye.y < 5.0f)eye.y = 5.0f;
		}; 					  //if ( state == GLUT_DOWN ) ;
		break;
	default:
		break;
	}
	auto camera = Camera::getInstance();
	camera->updateViewPosition(eye);
}
//----------------------------------------------------------------------------
void Win_SpecialKeyboard(int key, int x, int y) {
	front = normalize(at - eye);
	right = normalize(cross(front, g_vUp));
	switch (key) {
	case GLUT_KEY_UP:
		eye += point4(front.x, 0.0f, front.z, 0.0f)* 3.0f;
		break;
	case GLUT_KEY_DOWN:
		eye -= point4(front.x, 0.0f, front.z, 0.0f)* 3.0f;
		break;
	case GLUT_KEY_LEFT:		// 目前按下的是向左方向鍵
		eye -= point4(right.x, 0.0f, right.z, 0.0f)* 3.0f;
		break;
	case GLUT_KEY_RIGHT:	// 目前按下的是向右方向鍵
		eye += point4(right.x, 0.0f, right.z, 0.0f)* 3.0f;
		break;
	default:
		break;
	}
	if (eye.x > 45.0f)eye.x = 45.0f;
	if (eye.x < -45.0f)eye.x = -45.0f;
	if (eye.z > 45.0f)eye.z = 45.0f;
	if (eye.z < -45.0f)eye.z = -45.0f;
	at += point4(front.x, 0.0f, front.z, 0.0f);
	auto camera = Camera::getInstance();
	camera->updateViewLookAt(eye, at);
}

//----------------------------------------------------------------------------
// The passive motion callback for a window is called when the mouse moves within the window while no mouse buttons are pressed.
void Win_PassiveMotion(int x, int y) {

	g_fPhi = (float)-M_PI*(x - HALF_SIZE) / (HALF_SIZE); // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
	g_fTheta = (float)M_PI*(float)y / SCREEN_SIZE;
	at = point4(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);

	auto camera = Camera::getInstance();
	camera->updateLookAt(at);
}

// The motion callback for a window is called when the mouse moves within the window while one or more mouse buttons are pressed.
void Win_MouseMotion(int x, int y) {
	g_fPhi = (float)-M_PI*(x - HALF_SIZE) / (HALF_SIZE);  // 轉換成 g_fPhi 介於 -PI 到 PI 之間 (-180 ~ 180 之間)
	g_fTheta = (float)M_PI*(float)y / SCREEN_SIZE;
	at = point4(g_fRadius*sin(g_fTheta)*sin(g_fPhi), g_fRadius*cos(g_fTheta), g_fRadius*sin(g_fTheta)*cos(g_fPhi), 1.0f);

	auto camera = Camera::getInstance();
	camera->updateLookAt(at);
}
//----------------------------------------------------------------------------
void GL_Reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glClearColor(0.0, 0.0, 0.0, 1.0); // black background
	glEnable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------------------

int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_SIZE, SCREEN_SIZE);

	// If you use freeglut the two lines of code can be added to your application 
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutCreateWindow("Show Room");

	// The glewExperimental global switch can be turned on by setting it to GL_TRUE before calling glewInit(), 
	// which ensures that all extensions with valid entry points will be exposed.
	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutMouseFunc(Win_Mouse);
	glutMotionFunc(Win_MouseMotion);
	glutPassiveMotionFunc(Win_PassiveMotion);
	glutKeyboardFunc(Win_Keyboard);	// 處理 ASCI 按鍵如 A、a、ESC 鍵...等等
	glutSpecialFunc(Win_SpecialKeyboard);	// 處理 NON-ASCI 按鍵如 F1、Home、方向鍵...等等
	glutDisplayFunc(GL_Display);
	glutReshapeFunc(GL_Reshape);
	glutIdleFunc(IdleProcess);

	glutMainLoop();
	return 0;
}