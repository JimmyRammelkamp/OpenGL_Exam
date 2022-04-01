#include <iostream>
#include "GL/glew.h"
#include "GL/3dgl.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// WizardAI
#include "WizardAI.h"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

// 3D Models
C3dglTerrain terrain, river, road;
C3dglModel lamp, bulb, bridge;
CWizardAI wizard;		// change C3dglModel class to CWizardAI to address Task 10
//C3dglModel wizard;

// Texture Ids
GLuint idTexNone;	// white (neutral)
GLuint idTexBridge;
GLuint idTexBridgeNormal;
GLuint idTexRiver;
GLuint idTexGrass;
GLuint idTexRoad;
GLuint idTexRoadNormal;

// GLSL Objects (Shader Program)
C3dglProgram Program;

// camera position (for first person type camera navigation)
mat4 matrixView;			// The View Matrix
float angleTilt = 15.f;		// Tilt Angle
vec3 cam(0);				// Camera movement values

bool init()
{
	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!

	// Initialise Shaders
	C3dglShader VertexShader;
	C3dglShader FragmentShader;

	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/basic.vert.shader")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/basic.frag.shader")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!Program.Create()) return false;
	if (!Program.Attach(VertexShader)) return false;
	if (!Program.Attach(FragmentShader)) return false;
	if (!Program.Link()) return false;
	if (!Program.Use(true)) return false;

	// glut additional setup
	glutSetVertexAttribCoord3(Program.GetAttribLocation("aVertex"));
	glutSetVertexAttribNormal(Program.GetAttribLocation("aNormal"));

	
	// load your 3D models here!
	if (!terrain.loadHeightmap("models\\heightmap.bmp", 10)) return false;
	if (!river.loadHeightmap("models\\rivermap.bmp", 10)) return false;
	if (!road.loadHeightmap("models\\roadmap.bmp", 10)) return false;

	if (!lamp.load("models\\victorianlamp.obj")) return false;
	if (!bulb.load("models\\light_bulb.obj")) return false;

	// the bridge and the wizard are already loading - you need to render them!
	if (!bridge.load("models\\bridge.obj")) return false;
	if (!wizard.load("models\\Yelling.dae")) return false;
	wizard.loadMaterials("models\\textures");
	wizard.loadAnimations();


	// Textures
	C3dglBitmap bm;
	
	// TO DO: load all needed textures, we only provide the white idTexNone

	// none (simple-white) texture
	glGenTextures(1, &idTexNone);
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	BYTE bytes[] = { 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &bytes);

	bm.Load("models/bridge.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	
	glGenTextures(1, &idTexBridge);
	glBindTexture(GL_TEXTURE_2D, idTexBridge);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/bridge_normalmap.png", GL_RGBA);
	if (!bm.GetBits()) return false;

	glGenTextures(1, &idTexBridgeNormal);
	glBindTexture(GL_TEXTURE_2D, idTexBridgeNormal);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/cobbled.png", GL_RGBA);
	if (!bm.GetBits()) return false;

	glGenTextures(1, &idTexRoad);
	glBindTexture(GL_TEXTURE_2D, idTexRoad);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/cobbled_normalmap.png", GL_RGBA);
	if (!bm.GetBits()) return false;

	glGenTextures(1, &idTexRoadNormal);
	glBindTexture(GL_TEXTURE_2D, idTexRoadNormal);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/grass.png", GL_RGBA);
	if (!bm.GetBits()) return false;

	glGenTextures(1, &idTexGrass);
	glBindTexture(GL_TEXTURE_2D, idTexGrass);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/water.png", GL_RGBA);
	if (!bm.GetBits()) return false;

	glGenTextures(1, &idTexRiver);
	glBindTexture(GL_TEXTURE_2D, idTexRiver);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());

	Program.SendUniform("texture0", 0);
	Program.SendUniform("textureNormal", 1);

	// setup ambient/emissive light & material
	Program.SendUniform("lightAmbient.color", 0.05, 0.05, 0.05);
	Program.SendUniform("lightEmissive.color", 0.0, 0.0, 0.0);
	Program.SendUniform("materialAmbient", 1.0, 1.0, 1.0);		// full power

	// setup directional light
	Program.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
	Program.SendUniform("lightDir.diffuse", 0.2, 0.2, 0.2);

	
	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));
	matrixView *= lookAt(
		vec3(1.9, 2.0, 3.0),
		vec3(2.5, 2.0, 0.0),
		vec3(0.0, 1.0, 0.0));

	// setup the screen background colour
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // blue sky background

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << endl;

	return true;
}

// Helper function used to display player coordinates (X-Z) on-screen
void displayCoords(float x, float z)
{
	char buf[100];
	snprintf(buf, sizeof(buf), "(%1.1f, %1.1f)", x, z);
	Program.SendUniform("Text", 1);
	glWindowPos2i(10, 10);  // move in 10 pixels from the left and bottom edges
	for (char *p = buf; *p; p++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
	Program.SendUniform("Text", 0);
}

void done()
{
}

void renderScene(mat4& matrixView, float time)
{
	mat4 m;

	Program.SendUniform("lightPoint.position", 5.8f, 5.0f, -1.0f);
	Program.SendUniform("lightPoint.diffuse", 0.2, 0.2, 0.2);

	// setup diffuse material - neutral white
	Program.SendUniform("materialDiffuse", 1.0, 1.0, 1.0);	// grassy green

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexGrass);
	// render the terrain
	m = translate(matrixView, vec3(0, 0, 0));
	terrain.render(m);


	Program.SendUniform("time", time);
	Program.SendUniform("speedX", 2.0f);
	Program.SendUniform("speedY", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexRiver);
	// render the river
	m = translate(matrixView, vec3(0, 0, 0));
	river.render(m);

	Program.SendUniform("time", 0);
	Program.SendUniform("speedX", 0);
	Program.SendUniform("speedY", 0);

	Program.SendUniform("useNormalMap", true);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexRoad);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, idTexRoadNormal);
	// render the road
	m = matrixView;
	m = translate(m, vec3(5.0f, 0.01f, 0.0f));
	road.render(m);
	Program.SendUniform("useNormalMap", false);

	// render the lamp
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	Program.SendUniform("materialDiffuse", 0.1f, 0.1f, 0.2f);
	m = matrixView;
	m = translate(m, vec3(5.8f, 4.0f, -1.0f));
	m = scale(m, vec3(0.3f, 0.3f, 0.3f));
	lamp.render(m);

	// render the light bulb
	m = matrixView;
	m = translate(m, vec3(5.8f, 5.05f, -1.0f));
	Program.SendUniform("matrixModelView", m);
	glutSolidSphere(0.08, 32, 32);
	Program.SendUniform("materialDiffuse", 0.0, 0.0, 0.0);
	m = translate(m, vec3(0, -0.06f, 0));
	m = scale(m, vec3(0.035f, 0.035f, 0.035f));
	Program.SendUniform("lightEmissive.color", 1.0, 1.0, 0.8);
	bulb.render(m);
	Program.SendUniform("lightEmissive.color", 0.0, 0.0, 0.0);

	Program.SendUniform("useNormalMap", true);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexBridge);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, idTexBridgeNormal);
	Program.SendUniform("materialDiffuse", 1.0, 1.0, 1.0);
	m = matrixView;
	//m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = translate(m, vec3(4.5f, 1.7f, -5.5f));
	m = scale(m, vec3(0.3f, 0.3f, 0.3f));
	bridge.render(m);
	Program.SendUniform("useNormalMap", false);


	mat4 inv = inverse(matrixView);
	vec3 pos = vec3(inv[3].x, inv[3].y, inv[3].z);
	float distance = length(pos - vec3(4.7f, 3.3f, -5.5f));

	// calculate and send bone transforms
	std::vector<float> transforms;
	
	if (distance < 5.0f)
		wizard.ChangeState(CWizardAI::YOU_SHALL_NOT_PASS, time);
	if (distance > 9.0f)
		wizard.ChangeState(CWizardAI::IDLE, time);
	
	wizard.getAnimData(0, time, transforms);
	Program.SendUniformMatrixv("bones", (float*)&transforms[0], transforms.size() / 16);

	m = matrixView;
	//m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = translate(m, vec3(4.7f, 3.3f, -5.5f));
	m = scale(m, vec3(1.0f, 1.0f, 1.0f));
	wizard.render(m);


	// Display debug information: your current coordinates (x, z)
	// These coordinates are available as inv[3].x, inv[3].z
	
	//displayCoords(inv[3].x, inv[3].z);
	displayCoords(distance, distance);
}

void render()
{
	// this global variable controls the animation
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the View Matrix (camera)
	mat4 m = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));// switch tilt off
	m = translate(m, cam);												// animate camera motion (controlled by WASD keys)
	m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));			// switch tilt on
	matrixView = m * matrixView;

	// move the camera up following the profile of terrain (Y coordinate of the terrain)
	float terrainY = -terrain.getInterpolatedHeight(inverse(matrixView)[3][0], inverse(matrixView)[3][2]);
	matrixView = translate(matrixView, vec3(0, terrainY, 0));

	// setup View Matrix
	Program.SendUniform("matrixView", matrixView);

	// render the scene objects
	renderScene(matrixView, time);

	// the camera must be moved down by terrainY to avoid unwanted effects
	matrixView = translate(matrixView, vec3(0, -terrainY, 0));

	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	Program.SendUniform("matrixProjection", perspective(radians(60.f), ratio, 0.02f, 1000.f));
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': cam.z = std::max(cam.z * 1.05f, 0.01f); break;
	case 's': cam.z = std::min(cam.z * 1.05f, -0.01f); break;
	case 'a': cam.x = std::max(cam.x * 1.05f, 0.01f); break;
	case 'd': cam.x = std::min(cam.x * 1.05f, -0.01f); break;
	case 'e': cam.y = std::max(cam.y * 1.05f, 0.01f); break;
	case 'q': cam.y = std::min(cam.y * 1.05f, -0.01f); break;
	}
	// speed limit
	cam.x = std::max(-0.15f, std::min(0.15f, cam.x));
	cam.y = std::max(-0.15f, std::min(0.15f, cam.y));
	cam.z = std::max(-0.15f, std::min(0.15f, cam.z));
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': cam.z = 0; break;
	case 'a':
	case 'd': cam.x = 0; break;
	case 'q':
	case 'e': cam.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
bool bJustClicked = false;
void onMouse(int button, int state, int x, int y)
{
	bJustClicked = (state == GLUT_DOWN);
	glutSetCursor(bJustClicked ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
}

// handle mouse move
void onMotion(int x, int y)
{
	if (bJustClicked)
		bJustClicked = false;
	else
	{
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// find delta (change to) pan & tilt
		float deltaPan = 0.25f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
		float deltaTilt = 0.25f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// View = Tilt * DeltaPan * Tilt^-1 * DeltaTilt * View;
		angleTilt += deltaTilt;
		mat4 m = mat4(1.f);
		m = rotate(m, radians(angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaPan), vec3(0.f, 1.f, 0.f));
		m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaTilt), vec3(1.f, 0.f, 0.f));
		matrixView = m * matrixView;
	}
}

int main(int argc, char **argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("CI5520 3D Graphics Programming");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "GLEW Error: " << glewGetErrorString(err) << endl;
		return 0;
	}
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	// register callbacks
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);

	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "Version: " << glGetString(GL_VERSION) << endl;

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{
		cerr << "Application failed to initialise" << endl;
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	done();

	return 1;
}


