#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <freeglut.h>
#include "gltools.h"
#include "sceneRender.h"
#include "sceneGenerator.h"


using namespace std;

//renderer = sceneRender(namestr, "HeightMapFiles/N45E009.hgt");
sceneRender *renderer; 

/*Functions definitions for GLUT function calls */
void Keypressed(unsigned char key, int x, int y) { renderer->Keypressed(key, x, y); }
void Keyup(unsigned char key, int x, int y){ renderer->Keyup(key, x, y); }
void MouseControl(int MouseX, int MouseY){ renderer->MouseControl(MouseX, MouseY); }
void Display(){ renderer->display(); }
void Reshape(int w, int h){ renderer->reshape(w, h); }
void MouseWheel(int button, int dir, int x, int y) { renderer->MouseWheel(button, dir, x, y);}
void Idle(){ renderer->idlefunc(); }

int main(int argc, char **argv)
{
	cout << "Please enter a number.Supported Maps:" << endl;
	cout << "1. Via Pannilani" << endl;
	cout << "2. Como City Center" << endl;
	cout << "3. Como Brunate" << endl;
	int choice;
	cin >> choice;
	char* namestr;

	switch (choice)
	{
	case 1:
		namestr = "OSMFiles/Pannilani.osm";
		break;
	case 2:
		namestr = "OSMFiles/comoGeneral.osm";
		break;
	case 3:
		namestr = "OSMFiles/Brunate.osm";
		break;
	default:	
		namestr = "OSMFiles/comoGeneral.osm";
	}
	renderer = new sceneRender(namestr, "HeightMapFiles/N45E009.hgt");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //set up the double buffering
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 50);
	glutCreateWindow("Polimi OSM City Engine");
	glewInit();

	glutKeyboardFunc(Keypressed);
	glutKeyboardUpFunc(Keyup);
	glutPassiveMotionFunc(MouseControl);
	glutMouseWheelFunc(MouseWheel);
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutReshapeFunc(Reshape);


	//glutSetCursor(GLUT_CURSOR_NONE);
	renderer->initScene();

	glutMainLoop();
	return 0;

}