#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <freeglut.h>
#include "gltools.h"
#include "sceneRender.h"
#include "sceneGenerator.h"


using namespace std;


sceneRender renderer("OSMFiles/Pannilani.osm", "HeightMapFiles/N45E009.hgt");


/*Functions definitions for GLUT function calls */
void Keypressed(unsigned char key, int x, int y) { renderer.Keypressed(key, x, y); }
void Keyup(unsigned char key, int x, int y){ renderer.Keyup(key, x, y); }
void MouseControl(int MouseX, int MouseY){ renderer.MouseControl(MouseX, MouseY); }
void Display(){ renderer.display(); }
void Reshape(int w, int h){ renderer.reshape(w, h); }
void MouseWheel(int button, int dir, int x, int y) { renderer.MouseWheel(button, dir, x, y);}
void Idle(){ renderer.idlefunc(); }

int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //set up the double buffering
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 50);
	glutCreateWindow("PolimiOSM");
	glewInit();

	glutKeyboardFunc(Keypressed);
	glutKeyboardUpFunc(Keyup);
	glutPassiveMotionFunc(MouseControl);
	glutMouseWheelFunc(MouseWheel);
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutReshapeFunc(Reshape);

	//glutSetCursor(GLUT_CURSOR_NONE);
	renderer.initScene();


	glutMainLoop();
	return 0;

}