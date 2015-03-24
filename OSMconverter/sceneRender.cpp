#include <gl\glew.h>
#include <freeglut.h>
#include <iostream>
#include "gltools.h"
#include "sceneRender.h"
#include "ObjectLoader.h"

fps fpstool;
GLuint roofdraw;
GLuint roofdraw2;
GLuint amenitydraw;
GLuint areadraw;

GLdouble vertices[100000][6];               // arrary to store newly created vertices (x,y,z,r,g,b) by combine callback
int vertexIndex = 0;                    // array index for above array incremented inside combine callback
int ind = 0;
GLdouble vertex[100000][3];

GLuint *highwayDraw;
GLdouble **highwayVertex;
int highwayVertexIndex = 0;


GLuint **highwayVBO;
GLuint **buildingVBO;
GLuint terrainVBO[2];

ObjectLoader treeObj;


// CALLBACK functions for GLU_TESS ////////////////////////////////////////////
// NOTE: must be declared with CALLBACK directive
void CALLBACK tessBeginCB(GLenum which);
void CALLBACK tessEndCB();
void CALLBACK tessErrorCB(GLenum errorCode);
void CALLBACK tessVertexCB(const GLvoid *data);
void CALLBACK tessCombineCB(const GLdouble newVertex[3], const GLdouble *neighborVertex[4],
	const GLfloat neighborWeight[4], GLdouble **outData);



GLuint sceneRender::tessellateRoof(vector<Building> *buildingList)
{

	GLuint id = glGenLists(1);  // create a display list
	if (!id) return id;          // failed to create a list, return 0

	GLUtesselator *tess = gluNewTess(); // create a tessellator
	if (!tess) return 0;         // failed to create tessellation object, return 0

	// register callback functions
	gluTessCallback(tess, GLU_TESS_BEGIN, (void(__stdcall*)(void))tessBeginCB);
	gluTessCallback(tess, GLU_TESS_END, (void(__stdcall*)(void))tessEndCB);
	gluTessCallback(tess, GLU_TESS_ERROR, (void(__stdcall*)(void))tessErrorCB);
	gluTessCallback(tess, GLU_TESS_VERTEX, (void(__stdcall*)())tessVertexCB);
	gluTessCallback(tess, GLU_TESS_COMBINE, (void(__stdcall*)())tessCombineCB);

	glColor3f(0.5f, 0.0f, 0.0f);
	glNewList(id, GL_COMPILE);
	gluTessBeginPolygon(tess, 0);

	for (int i = 0; i < (*buildingList).size(); i++)
	{
			gluTessBeginContour(tess);
			for (int j = 0; j < (*buildingList)[i].nodes.size(); j++)
			{				
				vertex[ind][0] = (*buildingList)[i].nodes[j].meterx;
				vertex[ind][1] = (*buildingList)[i].BuildingTopHeight;
				vertex[ind][2] = (*buildingList)[i].nodes[j].meterz;
				gluTessVertex(tess, vertex[ind], vertex[ind]);
				ind++;
			}
			gluTessEndContour(tess);
	}

	gluTessEndPolygon(tess);
	glEndList();

	gluDeleteTess(tess);
	return id;      // return handle ID of a display list
}
GLuint sceneRender::tessellateRoof2()
{
	GLuint id = glGenLists(1);  // create a display list
	if (!id) return id;          // failed to create a list, return 0

	GLUtesselator *tess = gluNewTess(); // create a tessellator
	if (!tess) return 0;         // failed to create tessellation object, return 0

	// register callback functions
	gluTessCallback(tess, GLU_TESS_BEGIN, (void(__stdcall*)(void))tessBeginCB);
	gluTessCallback(tess, GLU_TESS_END, (void(__stdcall*)(void))tessEndCB);
	gluTessCallback(tess, GLU_TESS_ERROR, (void(__stdcall*)(void))tessErrorCB);
	gluTessCallback(tess, GLU_TESS_VERTEX, (void(__stdcall*)())tessVertexCB);
	gluTessCallback(tess, GLU_TESS_COMBINE, (void(__stdcall*)())tessCombineCB);

	glColor3f(0.5f, 0.0f, 0.0f);
	glNewList(id, GL_COMPILE);
	gluTessBeginPolygon(tess, 0);


	for (int i = 0; i < scene.buildingListNEW.size(); i++)
	{
		gluTessBeginContour(tess);
		for (int j = 0; j < scene.buildingListNEW[i].outerWall.nodes.size(); j++)
		{
			vertex[ind][0] = scene.buildingListNEW[i].outerWall.nodes[j].meterx;
			vertex[ind][1] = scene.buildingListNEW[i].BuildingTopHeight;
			vertex[ind][2] = scene.buildingListNEW[i].outerWall.nodes[j].meterz;
			gluTessVertex(tess, vertex[ind], vertex[ind]);
			ind++;
		}
		gluTessEndContour(tess);

		for (int k = 0; k < scene.buildingListNEW[i].innerWalls.size(); k++)
		{
			gluTessBeginContour(tess);
			for (int j = 0; j < scene.buildingListNEW[i].innerWalls[k].nodes.size(); j++)
			{
				vertex[ind][0] = scene.buildingListNEW[i].innerWalls[k].nodes[j].meterx;
				vertex[ind][1] = scene.buildingListNEW[i].BuildingTopHeight;
				vertex[ind][2] = scene.buildingListNEW[i].innerWalls[k].nodes[j].meterz;
				gluTessVertex(tess, vertex[ind], vertex[ind]);
				ind++;
			}
			gluTessEndContour(tess);

		}
	}

	gluTessEndPolygon(tess);
	glEndList();

	gluDeleteTess(tess);
	return id;      // return handle ID of a display list

}
GLuint sceneRender::tessellateAmenity(vector<Way> *wayList)
{
	GLuint id = glGenLists(1);  // create a display list
	if (!id) return id;          // failed to create a list, return 0

	GLUtesselator *tess = gluNewTess(); // create a tessellator
	if (!tess) return 0;         // failed to create tessellation object, return 0

	// register callback functions
	gluTessCallback(tess, GLU_TESS_BEGIN, (void(__stdcall*)(void))tessBeginCB);
	gluTessCallback(tess, GLU_TESS_END, (void(__stdcall*)(void))tessEndCB);
	gluTessCallback(tess, GLU_TESS_ERROR, (void(__stdcall*)(void))tessErrorCB);
	gluTessCallback(tess, GLU_TESS_VERTEX, (void(__stdcall*)())tessVertexCB);

	glNewList(id, GL_COMPILE);
	gluTessBeginPolygon(tess, 0);

	for (int i = 0; i < (*wayList).size(); i++)
	{
		switch ((*wayList)[i].type)
		{
		case wayType::amenityParking:
		case wayType::amenityMotorCycleParking:
			gluTessBeginContour(tess);
			for (int j = 0; j < (*wayList)[i].nodes.size(); j++)
			{
				vertex[ind][0] = (*wayList)[i].nodes[j].meterx;
				vertex[ind][1] = 0.3f + scene.terrainLoader.getTerrainHeight((*wayList)[i].nodes[j].lat, (*wayList)[i].nodes[j].lon);
				vertex[ind][2] = (*wayList)[i].nodes[j].meterz;
				gluTessVertex(tess, vertex[ind], vertex[ind]);
				ind++;
			}
			gluTessEndContour(tess);
			break;
		default:
			continue;
		}
	}


	gluTessEndPolygon(tess);
	glEndList();

	gluDeleteTess(tess);
	return id;      // return handle ID of a display list


}
GLuint sceneRender::tessellateArea(vector<Way> *wayList)
{
	GLuint id = glGenLists(1);  // create a display list
	if (!id) return id;          // failed to create a list, return 0

	GLUtesselator *tess = gluNewTess(); // create a tessellator
	if (!tess) return 0;         // failed to create tessellation object, return 0

	// register callback functions
	gluTessCallback(tess, GLU_TESS_BEGIN, (void(__stdcall*)(void))tessBeginCB);
	gluTessCallback(tess, GLU_TESS_END, (void(__stdcall*)(void))tessEndCB);
	gluTessCallback(tess, GLU_TESS_ERROR, (void(__stdcall*)(void))tessErrorCB);
	gluTessCallback(tess, GLU_TESS_VERTEX, (void(__stdcall*)())tessVertexCB);

	glNewList(id, GL_COMPILE);
	gluTessBeginPolygon(tess, 0);

	for (int i = 0; i < (*wayList).size(); i++)
	{
		if ((*wayList)[i].isArea)
		{
			gluTessBeginContour(tess);
			for (int j = 0; j < (*wayList)[i].nodes.size(); j++)
			{
				vertex[ind][0] = (*wayList)[i].nodes[j].meterx;
				vertex[ind][1] = 0.3f + scene.terrainLoader.getTerrainHeight((*wayList)[i].nodes[j].lat, (*wayList)[i].nodes[j].lon);
				vertex[ind][2] = (*wayList)[i].nodes[j].meterz;
				gluTessVertex(tess, vertex[ind], vertex[ind]);
				ind++;
			}
			gluTessEndContour(tess);
		}
	}

	gluTessEndPolygon(tess);
	glEndList();

	gluDeleteTess(tess);
	return id;      // return handle ID of a display list




}

sceneRender::sceneRender()
{
}
sceneRender::sceneRender(char* XMLfile, char* GEOfile)
{	
	scene.init(XMLfile, GEOfile);
}

void sceneRender::initSkybox()
{
	Skybox[0] = bmploader.LoadGLTexturesBMP("Textures/Skybox/Front.bmp");
	Skybox[1] = bmploader.LoadGLTexturesBMP("Textures/Skybox/Back.bmp");
	Skybox[2] = bmploader.LoadGLTexturesBMP("Textures/Skybox/Left.bmp");
	Skybox[3] = bmploader.LoadGLTexturesBMP("Textures/Skybox/Right.bmp");
	Skybox[4] = bmploader.LoadGLTexturesBMP("Textures/Skybox/Top.bmp");
	Skybox[5] = bmploader.LoadGLTexturesBMP("Textures/Skybox/Bottom.bmp");
}
void sceneRender::drawSkybox()
{

	
	int initHeight = 200.0f;
	double height = abs(scene.terrain.BBox.meterTop - scene.terrain.BBox.meterBottom)*1.5;
	double width = abs(scene.terrain.BBox.meterRight - scene.terrain.BBox.meterLeft)*1.5;
	int size = (abs(width) + abs(height)) / 2;
	glPushMatrix();
	glTranslatef(-0.25*abs(height), 0, -0.25*abs(width));
	//FRONT FACE
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Skybox[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(height, size+initHeight, width);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(height, -size+initHeight, width);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0, -size + initHeight, width);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0, size + initHeight, width);
	glEnd();

	//BACK FACE
	glBindTexture(GL_TEXTURE_2D, Skybox[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0, size + initHeight, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0, -size + initHeight, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(height, -size + initHeight, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(height, size + initHeight, 0);
	glEnd();

	//LEFT FACE	
	glBindTexture(GL_TEXTURE_2D, Skybox[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0, size + initHeight, width);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0, -size + initHeight, width);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0, -size + initHeight, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0, size + initHeight, 0);

	glEnd();

	//RIGHT FACE	
	glBindTexture(GL_TEXTURE_2D, Skybox[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(height, size + initHeight, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(height, -size + initHeight, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(height, -size + initHeight, width);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(height, size + initHeight, width);

	glEnd();


	//TOP FACE		
	glBindTexture(GL_TEXTURE_2D, Skybox[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0, size + initHeight, 0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(height, size + initHeight, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(height, size + initHeight, width);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0, size + initHeight, width);
	glEnd();

	//BACK FACE		
	glBindTexture(GL_TEXTURE_2D, Skybox[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0, -size + initHeight, 0);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(height, -size + initHeight, 0);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(height, -size + initHeight, width);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0, -size + initHeight, width);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	//glLoadIdentity();


}
void sceneRender::initScene(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculation
	glDepthRange(0.0f, 1.0f);
	

	glClearColor(0, 0, 0, 0);                   // background color
	glClearStencil(0);                          // clear stencil buffer
	glClearDepth(1.0f);                         // 0 is near, 1 is far


	glEnable(GL_LIGHT0);    // Uses default lighting parameters
	glLightModeli(GL_LIGHT_MODEL_AMBIENT, GL_TRUE);

	GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[] = { 100.0f, 800.0f, 100.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);


	mot.cameraPosition.Change(100.0, 600.0, 4.0);
	mot.lookatPosition.Change(100.0, 600.0, -6.0);

	initSkybox();
	roadUnclassifiedTexture = bmploader.LoadGLTexturesBMP("Textures/road.bmp");
	roadResidualTexture = bmploader.LoadGLTexturesBMP("Textures/road4.bmp");
	roadServiceTexture = bmploader.LoadGLTexturesBMP("Textures/road2.bmp");
	roadPrimaryTexture = bmploader.LoadGLTexturesBMP("Textures/road3.bmp");
	roadSecondaryTexture = bmploader.LoadGLTexturesBMP("Textures/road3.bmp");
	roadTertiaryTexture = bmploader.LoadGLTexturesBMP("Textures/road.bmp");
	roadPavementTexture = bmploader.LoadGLTexturesBMP("Textures/roadPavement.bmp");
	railwayTexture = bmploader.LoadGLTexturesBMP("Textures/railway.bmp");
	riverTexture = bmploader.LoadGLTexturesBMP("Textures/river.bmp");
	roadPathTexture = bmploader.LoadGLTexturesBMP("Textures/roadPath.bmp");


	buildingTexture = bmploader.LoadGLTexturesBMP("Textures/buildingTexture.bmp");
	buildingTexture2 = bmploader.LoadGLTexturesBMP("Textures/buildingTexture2.bmp");
	buildingTexture3 = bmploader.LoadGLTexturesBMP("Textures/buildingTexture3.bmp");
	kioskTexture = bmploader.LoadGLTexturesBMP("Textures/kioskTexture.bmp");
	wallTexture = bmploader.LoadGLTexturesBMP("Textures/stoneWall.bmp");

	treeObj.ImportObject("C:\\Users\\user\\Documents\\Visual Studio 2013\\Projects\\OSMconverter\\OSMconverter\\ObjFiles\\f_tree1\\", "obj__tree1.obj");


	roofdraw = tessellateRoof(&scene.parser.buildingList);
	roofdraw2 = tessellateRoof2();
	amenitydraw = tessellateAmenity(&scene.parser.wayList);
	areadraw = tessellateArea(&scene.parser.wayList);
	


	generateHighwayVBO();
	generateBuildingVBO();

	//shader.init("shader.vert", "shader.frag");

}

void sceneRender::reshape(int w, int h)
{
	//GLint m_viewport[4];
	//glGetIntegerv(GL_VIEWPORT, m_viewport);
	//mot.windowHeight = m_viewport[3];
	//mot.windowWidth = m_viewport[2];
	mot.windowHeight = h;
	mot.windowWidth = w;
	
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80, 1, 1, 1000000);
}



//DEPRECATED - IMMEDIATE DRAWS
void sceneRender::drawHighways()
{
	for (int i = 0; i < scene.parser.highWayList.size(); i++)
	{
		switch (scene.parser.highWayList[i].type)
		{
		case wayType::railway:
			glColor3f(1.0f, 1.0f, 1.0f);
			Draw3DWay(&scene.parser.highWayList[i], railwayTexture, 0.05);
			continue;

		case wayType::highwayResidual:
			glColor3f(0.8f, 0.8f, 0.8f);
			Draw3DWay(&scene.parser.highWayList[i], roadResidualTexture, 0.01);
			continue;

		case wayType::highwayUnclassified:
			glColor3f(0.8f, 0.8f, 0.8f);
			Draw3DWay(&scene.parser.highWayList[i], roadUnclassifiedTexture, 0.015);
			continue;

		case wayType::highwayService:
			glColor3f(1.0f, 1.0f, 1.0f);
			Draw3DWay(&scene.parser.highWayList[i], roadServiceTexture, 0.009);
			continue;

		case wayType::highwayPrimary:
			glColor3f(1.0f, 1.0f, 1.0f);
			Draw3DWay(&scene.parser.highWayList[i], roadPrimaryTexture, 0.03);
			continue;

		case wayType::highwaySecondary:
			glColor3f(1.0f, 1.0f, 1.0f);
			Draw3DWay(&scene.parser.highWayList[i], roadSecondaryTexture, 0.025);
			continue;

		case wayType::highwayTertiary:
			glColor3f(0.8f, 0.8f, 0.8f);
			Draw3DWay(&scene.parser.highWayList[i], roadTertiaryTexture, 0.035);
			continue;

		case wayType::highwayPedestrian:
			glColor3f(0.9f, 0.8f, 0.8f);
			Draw3DWay(&scene.parser.highWayList[i], roadPavementTexture, 0.005);
			continue;

		case wayType::highwayPath:
			continue;
			glColor3f(1.0f, 1.0f, 1.0f);
			Draw3DWay(&scene.parser.highWayList[i], roadPathTexture, 0.005);
			continue;

		case wayType::river:
			glColor3f(0.7f, 0.7f, 0.7f);
			Draw3DWay(&scene.parser.highWayList[i], riverTexture, 0.005);
			continue;
		}
	}

}
void sceneRender::drawBuildings()
{
	glEnable(GL_TEXTURE_2D);
	for (int i = 0; i < scene.parser.buildingList.size(); i++)
	{

		switch (scene.parser.buildingList[i].TextureID)
		{
		case 1:
			glColor3f(1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, buildingTexture);
			break;
		case 2:
			glColor3f(0.5f, 0.5f, 0.5f);
			glBindTexture(GL_TEXTURE_2D, buildingTexture2);
			break;
		case 3:
			glColor3f(0.5f, 0.5f, 0.5f);
			glBindTexture(GL_TEXTURE_2D, buildingTexture3);
			break;
		case 4:
			glColor3f(1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, kioskTexture);
			break;
		case 5:
			glColor3f(0.5f, 0.5f, 0.5f);
			glBindTexture(GL_TEXTURE_2D, wallTexture);
			break;
		}
		glBegin(GL_QUADS);
		for (int j = 0, texcoord = 0; j < scene.parser.buildingList[i].nodes.size() - 1; j++)
		{
			int a1 = scene.parser.buildingList[i].nodes[j + 1].meterx - scene.parser.buildingList[i].nodes[j].meterx;
			int a2 = scene.parser.buildingList[i].nodes[j + 1].meterz - scene.parser.buildingList[i].nodes[j].meterz;
			texcoord = (sqrt(a1*a1 + a2*a2) / 8.0f);

			//glTexCoord2f(0.0f, 1.0f);
			//glVertex3f(scene.parser.buildingList[i].nodes[j].meterx, scene.parser.buildingList[i].nodes[j].height + scene.parser.buildingList[i].BuildingHeight, (scene.parser.buildingList[i].nodes[j].meterz));
			//glTexCoord2f(0.0f, 0.0f);
			//glVertex3f(scene.parser.buildingList[i].nodes[j].meterx, scene.parser.buildingList[i].nodes[j].height, scene.parser.buildingList[i].nodes[j].meterz);
			//glTexCoord2f(texcoord, 0.0f);
			//glVertex3f(scene.parser.buildingList[i].nodes[j + 1].meterx, scene.parser.buildingList[i].nodes[j+1].height, scene.parser.buildingList[i].nodes[j + 1].meterz);
			//glTexCoord2f(texcoord, 1.0f);
			//glVertex3f(scene.parser.buildingList[i].nodes[j + 1].meterx, scene.parser.buildingList[i].nodes[j+1].height + scene.parser.buildingList[i].BuildingHeight, (scene.parser.buildingList[i].nodes[j + 1].meterz));

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(scene.parser.buildingList[i].nodes[j].meterx, scene.parser.buildingList[i].BuildingTopHeight, (scene.parser.buildingList[i].nodes[j].meterz));
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(scene.parser.buildingList[i].nodes[j].meterx, scene.parser.buildingList[i].nodes[j].height, scene.parser.buildingList[i].nodes[j].meterz);
			glTexCoord2f(texcoord, 0.0f);
			glVertex3f(scene.parser.buildingList[i].nodes[j + 1].meterx, scene.parser.buildingList[i].nodes[j + 1].height, scene.parser.buildingList[i].nodes[j + 1].meterz);
			glTexCoord2f(texcoord, 1.0f);
			glVertex3f(scene.parser.buildingList[i].nodes[j + 1].meterx, scene.parser.buildingList[i].BuildingTopHeight, (scene.parser.buildingList[i].nodes[j + 1].meterz));


		}
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}
void sceneRender::drawBuildings2()
{

	for (int i = 0; i < (int)scene.buildingListNEW.size(); i++)
	{
		glEnable(GL_TEXTURE_2D);
		//OUTER WAY
		switch (scene.buildingListNEW[i].TextureID)
		{
		case 1:
			glColor3f(1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, buildingTexture);
			break;
		case 2:
			glColor3f(0.5f, 0.5f, 0.5f);
			glBindTexture(GL_TEXTURE_2D, buildingTexture2);
			break;
		case 3:
			glColor3f(0.5f, 0.5f, 0.5f);
			glBindTexture(GL_TEXTURE_2D, buildingTexture3);
			break;
		case 4:
			glColor3f(1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, kioskTexture);
			break;
		case 5:
			glColor3f(0.5f, 0.5f, 0.5f);
			glBindTexture(GL_TEXTURE_2D, wallTexture);
			break;
		}
		glBegin(GL_QUADS);
		for (int j = 0, texcoord = 0; j < scene.buildingListNEW[i].outerWall.nodes.size() - 1; j++)
		{
			int a1 = scene.buildingListNEW[i].outerWall.nodes[j + 1].meterx - scene.buildingListNEW[i].outerWall.nodes[j].meterx;
			int a2 = scene.buildingListNEW[i].outerWall.nodes[j + 1].meterz - scene.buildingListNEW[i].outerWall.nodes[j].meterz;
			texcoord = (sqrt(a1*a1 + a2*a2) / 8.0f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(scene.buildingListNEW[i].outerWall.nodes[j].meterx, scene.buildingListNEW[i].BuildingTopHeight, scene.buildingListNEW[i].outerWall.nodes[j].meterz);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(scene.buildingListNEW[i].outerWall.nodes[j].meterx, scene.buildingListNEW[i].outerWall.nodes[j].height, scene.buildingListNEW[i].outerWall.nodes[j].meterz);
			glTexCoord2f(texcoord, 0.0f);
			glVertex3f(scene.buildingListNEW[i].outerWall.nodes[j + 1].meterx, scene.buildingListNEW[i].outerWall.nodes[j + 1].height, scene.buildingListNEW[i].outerWall.nodes[j + 1].meterz);
			glTexCoord2f(texcoord, 1.0f);
			glVertex3f(scene.buildingListNEW[i].outerWall.nodes[j + 1].meterx, scene.buildingListNEW[i].BuildingTopHeight, scene.buildingListNEW[i].outerWall.nodes[j + 1].meterz);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);

		//INNER WAYS
		glColor3f(0.3f, 0.3f, 0.3f);
		for (int k = 0; k < (int)scene.buildingListNEW[i].innerWalls.size(); k++)
		{
			glBegin(GL_QUADS);
			for (int j = 0; j < (int)scene.buildingListNEW[i].innerWalls[k].nodes.size() - 1; j++)
			{

				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(scene.buildingListNEW[i].innerWalls[k].nodes[j].meterx, scene.buildingListNEW[i].innerWalls[k].nodes[j].height, scene.buildingListNEW[i].innerWalls[k].nodes[j].meterz);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(scene.buildingListNEW[i].innerWalls[k].nodes[j + 1].meterx, scene.buildingListNEW[i].innerWalls[k].nodes[j + 1].height, scene.buildingListNEW[i].innerWalls[k].nodes[j + 1].meterz);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(scene.buildingListNEW[i].innerWalls[k].nodes[j + 1].meterx, scene.buildingListNEW[i].BuildingTopHeight, scene.buildingListNEW[i].innerWalls[k].nodes[j + 1].meterz);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(scene.buildingListNEW[i].innerWalls[k].nodes[j].meterx, scene.buildingListNEW[i].BuildingTopHeight, scene.buildingListNEW[i].innerWalls[k].nodes[j].meterz);
			}
			glEnd();
		}



	}




}
void sceneRender::drawTerrain()
{
	GLfloat color;
	glPushMatrix();
	glTranslatef(scene.terrain.shiftx, 0, scene.terrain.shiftz);
	glBegin(GL_TRIANGLES);
	for (int i = scene.terrain.bottom, x = 0; i > scene.terrain.top; i--, x++)
	{
		for (int j = scene.terrain.left, z = 0; j < scene.terrain.right; j++, z++)
		{
			short h1 = scene.terrainLoader.heightmap[i][j];
			short h2 = scene.terrainLoader.heightmap[i][j + 1];
			short h3 = scene.terrainLoader.heightmap[i - 1][j + 1];
			short h4 = scene.terrainLoader.heightmap[i - 1][j];

			//color hash function
			//color = (h1 + h2 + h3 + h4) / 8000.0f + 0.1f;
			color = 0.18f;
			glColor3f(color, 2 * color, color);

			glVertex3d(x*scene.terrainLoader.heightMapInfo.sizeX, h1, z*scene.terrainLoader.heightMapInfo.sizeZ);
			glVertex3d((x)*scene.terrainLoader.heightMapInfo.sizeX, h2, (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ);
			glVertex3d((x + 1)*scene.terrainLoader.heightMapInfo.sizeX, h3, (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ);
			glVertex3d((x + 1)*scene.terrainLoader.heightMapInfo.sizeX, h3, (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ);
			glVertex3d((x + 1)*scene.terrainLoader.heightMapInfo.sizeX, h4, z*scene.terrainLoader.heightMapInfo.sizeZ);
			glVertex3d(x*scene.terrainLoader.heightMapInfo.sizeX, h1, z*scene.terrainLoader.heightMapInfo.sizeZ);
		}
	}
	glEnd();


	//DEBUG MODE
	if (mot.debug)
	{
		glBegin(GL_LINES);

		for (int i = scene.terrain.bottom, x = 0; i > scene.terrain.top; i--, x++)
		{
			for (int j = scene.terrain.left, z = 0; j < scene.terrain.right; j++, z++)
			{
				short h1 = scene.terrainLoader.heightmap[i][j];
				short h2 = scene.terrainLoader.heightmap[i][j + 1];
				short h3 = scene.terrainLoader.heightmap[i - 1][j + 1];
				short h4 = scene.terrainLoader.heightmap[i - 1][j];

				glColor3f(1.0f, 0, 0);
				glVertex3d(x*scene.terrainLoader.heightMapInfo.sizeX, h1, z*scene.terrainLoader.heightMapInfo.sizeZ);
				glVertex3d((x)*scene.terrainLoader.heightMapInfo.sizeX, h2, (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ);

				glVertex3d((x)*scene.terrainLoader.heightMapInfo.sizeX, h2, (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ);
				glVertex3d((x + 1)*scene.terrainLoader.heightMapInfo.sizeX, h3, (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ);

				glVertex3d(x*scene.terrainLoader.heightMapInfo.sizeX, h1, z*scene.terrainLoader.heightMapInfo.sizeZ);
				glVertex3d((x + 1)*scene.terrainLoader.heightMapInfo.sizeX, h3, (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ);

			}
		}
		glEnd();
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	glPopMatrix();

}
void sceneRender::Draw3DWay(HighWay *way, GLuint texture, float layerheight)
{

	glEnable(GL_TEXTURE_2D);
	layerheight += 0.3f;
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < way->leftSideVertexes.size(); i++)
	{
	glTexCoord2f(way->leftSideTexCoords[i].x, way->leftSideTexCoords[i].y);
	glVertex3f(way->leftSideVertexes[i].x, way->leftSideVertexes[i].y + layerheight, way->leftSideVertexes[i].z);

	glTexCoord2f(way->rightSideTexCoords[i].x, way->rightSideTexCoords[i].y);
	glVertex3f(way->rightSideVertexes[i].x, way->rightSideVertexes[i].y + layerheight, way->rightSideVertexes[i].z);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);


	glColor3f(1.0f, 1.0f, 1.0f);

}



//CURRENT METHODS - DRAWING WITH VBO
void sceneRender::generateHighwayVBO()
{
	float *textureBuffer;
	float *vertexBuffer;
	float *normalBuffer;

	highwayVBO = new GLuint*[scene.parser.highWayList.size()];
	for (int i = 0; i < scene.parser.highWayList.size(); i++)
	{
		int vertexBufferSize = scene.parser.highWayList[i].leftSideVertexes.size() * 6;
		int textureBufferSize = scene.parser.highWayList[i].leftSideVertexes.size() * 4;

		vertexBuffer = new float[vertexBufferSize];
		textureBuffer = new float[textureBufferSize];
		scene.generateHighwayBuffers(&scene.parser.highWayList[i],vertexBuffer, textureBuffer);

		highwayVBO[i] = new GLuint[2];
		glGenBuffers(1, &highwayVBO[i][0]); 
		glBindBuffer(GL_ARRAY_BUFFER, highwayVBO[i][0]); // Bind our vertex Buffer 
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexBufferSize, vertexBuffer, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &highwayVBO[i][1]);
		glBindBuffer(GL_ARRAY_BUFFER, highwayVBO[i][1]); // Bind our texture Buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * textureBufferSize, textureBuffer, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		delete vertexBuffer;
		delete textureBuffer;
	}
}
void sceneRender::drawHighwayVBO()
{

	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	for (int i = 0; i < scene.parser.highWayList.size(); i++)
	{
		switch (scene.parser.highWayList[i].type)
		{
		case wayType::railway:
			glColor3f(1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, railwayTexture);
			break;

		case wayType::river:
			glColor3f(0.7f, 0.7f, 0.7f);
			glBindTexture(GL_TEXTURE_2D, riverTexture);
			break;

		case wayType::highwayResidual:
			glColor3f(0.8f, 0.8f, 0.8f);
			glBindTexture(GL_TEXTURE_2D, roadResidualTexture);
			break;

		case wayType::highwayUnclassified:
			glColor3f(0.8f, 0.8f, 0.8f);
			glBindTexture(GL_TEXTURE_2D, roadUnclassifiedTexture);
			break;

		case wayType::highwayService:
			glColor3f(1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, roadServiceTexture);
			break;

		case wayType::highwayPrimary:
			glColor3f(1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, roadPrimaryTexture);
			break;

		case wayType::highwaySecondary:
			glColor3f(1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, roadSecondaryTexture);
			break;

		case wayType::highwayTertiary:
			glColor3f(0.8f, 0.8f, 0.8f);
			glBindTexture(GL_TEXTURE_2D, roadTertiaryTexture);
			break;

		case wayType::highwayPedestrian:
			glColor3f(0.9f, 0.8f, 0.8f);
			glBindTexture(GL_TEXTURE_2D, roadPavementTexture);
			break;

		case wayType::highwayPath:
			continue;
			glColor3f(1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, roadPathTexture);
			break;
		}


		glBindBuffer(GL_ARRAY_BUFFER, highwayVBO[i][0]);
		glVertexPointer(3, GL_FLOAT, 0, 0l);

		glBindBuffer(GL_ARRAY_BUFFER, highwayVBO[i][1]);
		glTexCoordPointer(2, GL_FLOAT, 0, 0l);



		//cout << scene.parser.highWayList[i].leftSideVertexes.size() << endl;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, scene.parser.highWayList[i].leftSideVertexes.size()*2);
		


	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);


}

void sceneRender::generateBuildingVBO()
{
	float *textureBuffer;
	float *vertexBuffer;

	buildingVBO = new GLuint*[scene.parser.buildingList.size()];
	for (int i = 0; i < scene.parser.buildingList.size(); i++)
	{
		int vertexBufferSize = scene.parser.buildingList[i].nodes.size() * 4 * 3;
		int textureBufferSize = scene.parser.buildingList[i].nodes.size() * 4 * 2;

		vertexBuffer = new float[vertexBufferSize];
		textureBuffer = new float[textureBufferSize];
		scene.generateBuildingBuffers(&scene.parser.buildingList[i], vertexBuffer, textureBuffer);

		buildingVBO[i] = new GLuint[2];
		glGenBuffers(1, &buildingVBO[i][0]);
		glBindBuffer(GL_ARRAY_BUFFER, buildingVBO[i][0]); // Bind our vertex Buffer 
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* vertexBufferSize, vertexBuffer, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &buildingVBO[i][1]);
		glBindBuffer(GL_ARRAY_BUFFER, buildingVBO[i][1]); // Bind our texture Buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* textureBufferSize, textureBuffer, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		delete vertexBuffer;
		delete textureBuffer;
	}

}
void sceneRender::drawBuildingVBO()
{
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);


	for (int i = 0; i < scene.parser.buildingList.size(); i++)
	{

		switch (scene.parser.buildingList[i].TextureID)
		{
		case 1:
			glColor3f(1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, buildingTexture);
			break;
		case 2:
			glColor3f(0.5f, 0.5f, 0.5f);
			glBindTexture(GL_TEXTURE_2D, buildingTexture2);
			break;
		case 3:
			glColor3f(0.5f, 0.5f, 0.5f);
			glBindTexture(GL_TEXTURE_2D, buildingTexture3);
			break;
		case 4:
			glColor3f(1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, kioskTexture);
			break;
		case 5:
			glColor3f(0.5f, 0.5f, 0.5f);
			glBindTexture(GL_TEXTURE_2D, wallTexture);
			break;
		}


		glBindBuffer(GL_ARRAY_BUFFER, buildingVBO[i][0]);
		glVertexPointer(3, GL_FLOAT, 0, 0l);

		glBindBuffer(GL_ARRAY_BUFFER, buildingVBO[i][1]);
		glTexCoordPointer(2, GL_FLOAT, 0, 0l);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, scene.parser.buildingList[i].nodes.size() * 2);
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
}

//Terrain VBO is misisng !!
void sceneRender::generateTerrainVBO()
{
	float *textureBuffer; //NOT EXISTS FOR NOW
	float *vertexBuffer;

	int vertexBufferSize = (1 + scene.terrain.right - scene.terrain.left) * (1 + scene.terrain.bottom - scene.terrain.top) * 3;
	vertexBuffer = new float[vertexBufferSize];
	int itr = 0;

	glTranslatef(scene.terrain.shiftx, 0, scene.terrain.shiftz);
	glBegin(GL_TRIANGLES);
	for (int i = scene.terrain.bottom, x = 0; i > scene.terrain.top; i--, x++)
	{
		for (int j = scene.terrain.left, z = 0; j < scene.terrain.right; j++, z++)
		{
			short h1 = scene.terrainLoader.heightmap[i][j];
			short h2 = scene.terrainLoader.heightmap[i][j + 1];
			short h3 = scene.terrainLoader.heightmap[i - 1][j + 1];
			short h4 = scene.terrainLoader.heightmap[i - 1][j];

			vertexBuffer[itr++] = x*scene.terrainLoader.heightMapInfo.sizeX;
			vertexBuffer[itr++] = h1;
			vertexBuffer[itr++] = z*scene.terrainLoader.heightMapInfo.sizeZ;

			vertexBuffer[itr++] = x*scene.terrainLoader.heightMapInfo.sizeX;
			vertexBuffer[itr++] = h2;
			vertexBuffer[itr++] = (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ;

			vertexBuffer[itr++] = (x + 1)*scene.terrainLoader.heightMapInfo.sizeX;
			vertexBuffer[itr++] = h3;
			vertexBuffer[itr++] = (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ;

			//TODO: Devamini getir bunun ..


			glVertex3d(x*scene.terrainLoader.heightMapInfo.sizeX, h1, z*scene.terrainLoader.heightMapInfo.sizeZ);
			glVertex3d((x)*scene.terrainLoader.heightMapInfo.sizeX, h2, (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ);
			glVertex3d((x + 1)*scene.terrainLoader.heightMapInfo.sizeX, h3, (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ);
			glVertex3d((x + 1)*scene.terrainLoader.heightMapInfo.sizeX, h3, (z + 1)*scene.terrainLoader.heightMapInfo.sizeZ);
			glVertex3d((x + 1)*scene.terrainLoader.heightMapInfo.sizeX, h4, z*scene.terrainLoader.heightMapInfo.sizeZ);
			glVertex3d(x*scene.terrainLoader.heightMapInfo.sizeX, h1, z*scene.terrainLoader.heightMapInfo.sizeZ);
		}
	}
	glEnd();

}
void sceneRender::drawTerrainVBO()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0l);

	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO[0]);
	glTexCoordPointer(2, GL_FLOAT, 0, 0l);

	//glDrawArrays(GL_TRIANGLE, 0,100);
	//TODO: Bunu cizerken indexin kullanmak gerekicek

}


void sceneRender::drawDebugMode()
{
	for (int m = 0; m < scene.parser.highWayList.size(); m++)
	{
		HighWay *way = &scene.parser.highWayList[m];

		glColor3f(0.1f, 0.1f, 0.1f);
		for (int i = 0; i < way->leftIntersections.size(); i++)
		{
			glPushMatrix();
			glTranslatef(way->leftIntersections[i].x, way->leftIntersections[i].y, way->leftIntersections[i].z);
			glutSolidSphere(1.0f, 8, 8);
			glPopMatrix();
		}


		glColor3f(0.1f, 0.1f, 0.1f);
		for (int i = 0; i < way->rightIntersections.size(); i++)
		{
			glPushMatrix();
			glTranslatef(way->rightIntersections[i].x, way->rightIntersections[i].y, way->rightIntersections[i].z);
			glutSolidSphere(1.0f, 8, 8);
			glPopMatrix();
		}

		glColor3f(0.5f, 0.1f, 0.1f);
		for (int i = 0; i < way->rightSideVertexes.size(); i++)
		{
			glPushMatrix();
			glTranslatef(way->rightSideVertexes[i].x, way->rightSideVertexes[i].y, way->rightSideVertexes[i].z);
			glutSolidSphere(0.8f, 10, 10);
			glPopMatrix();
		}

		glColor3f(0.1f, 0.1f, 0.6f);
		for (int i = 0; i < way->leftSideVertexes.size(); i++)
		{
			glPushMatrix();
			glTranslatef(way->leftSideVertexes[i].x, way->leftSideVertexes[i].y, way->leftSideVertexes[i].z);
			glutSolidSphere(0.8f, 10, 10);
			glPopMatrix();
		}
	}
}



void sceneRender::drawWays()
{

	for (int i = 0; i < scene.parser.wayList.size(); i++)
	{
		switch (scene.parser.wayList[i].type)
		{

		case wayType::leisure:
			glColor3f(0.3f, 0.3f, 0.2f);
			glBegin(GL_TRIANGLE_FAN);
			break;

		case wayType::landuseForest:
			glColor3f(0.1f, 0.3f, 0.1f);
			glBegin(GL_TRIANGLE_FAN);
			break;

		case wayType::landuseGrass:
			glColor3f(0.2f, 0.35f, 0.2f);
			glBegin(GL_TRIANGLE_FAN);
			break;

		case wayType::landuseMeadow:
			glColor3f(0.2f, 0.35f, 0.2f);
			glBegin(GL_TRIANGLE_FAN);
			break;

		case wayType::historic:
			glColor3f(0.5f, 0.5f, 0.5f);
			if (scene.parser.wayList[i].historic == historic_cityWalls)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, wallTexture);
				glBegin(GL_QUADS);
				for (int j = 0, texcoord = 0; j < scene.parser.wayList[i].nodes.size() - 1; j++)
				{
					int a1 = scene.parser.wayList[i].nodes[j + 1].meterx - scene.parser.wayList[i].nodes[j].meterx;
					int a2 = scene.parser.wayList[i].nodes[j + 1].meterz - scene.parser.wayList[i].nodes[j].meterz;
					texcoord = (sqrt(a1*a1 + a2*a2) / 20.0f);

					glTexCoord2f(0.0f, 1.0f);
					glVertex3f(scene.parser.wayList[i].nodes[j].meterx, scene.parser.wayList[i].nodes[j].height + Buildingheight, (scene.parser.wayList[i].nodes[j].meterz));
					glTexCoord2f(0.0f, 0.0f);
					glVertex3f(scene.parser.wayList[i].nodes[j].meterx, scene.parser.wayList[i].nodes[j].height, scene.parser.wayList[i].nodes[j].meterz);
					glTexCoord2f(texcoord, 0.0f);
					glVertex3f(scene.parser.wayList[i].nodes[j + 1].meterx, scene.parser.wayList[i].nodes[j + 1].height, scene.parser.wayList[i].nodes[j + 1].meterz);
					glTexCoord2f(texcoord, 1.0f);
					glVertex3f(scene.parser.wayList[i].nodes[j + 1].meterx, scene.parser.wayList[i].nodes[j + 1].height + Buildingheight, (scene.parser.wayList[i].nodes[j + 1].meterz));
				}
				glEnd();
				glDisable(GL_TEXTURE_2D);
			}

			if (scene.parser.wayList[i].historic == historic_cityGate)
			{
				continue;
			}
			break;

		case wayType::unknown:  //DEFAULT CASE
			continue;
			glColor3f(0.3f, 0.3f, 0.3f);
			glBegin(GL_LINE_STRIP);
			break;
		default:
			continue;

		}


		glLineWidth(4.0f);
		for (int j = 0; j < scene.parser.wayList[i].nodes.size(); j++)
		{
			glVertex3f(scene.parser.wayList[i].nodes[j].meterx, scene.terrainLoader.getTerrainHeight(scene.parser.wayList[i].nodes[j].lat, scene.parser.wayList[i].nodes[j].lon), (scene.parser.wayList[i].nodes[j].meterz));
		}
		glEnd();
	}



}
void sceneRender::drawTrees()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	for (int i = 0; i < scene.Trees.size(); i++)
	{
		glPushMatrix();
		glTranslatef(scene.Trees[i].meterx, scene.Trees[i].height, scene.Trees[i].meterz);
		treeObj.drawObject();
		glPopMatrix();
	}
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
}



void sceneRender::drawScene()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	drawWays();


	if (mot.debug)
		drawDebugMode();
	else
	{
		drawBuildings2();
		drawBuildingVBO();
		glColor3f(0.45, 0.3, 0.3);
		glCallList(roofdraw);
		glCallList(roofdraw2);
	}

	drawHighwayVBO();
	drawTrees();


	glColor3f(0.2, 0.2, 0.2);
	glCallList(amenitydraw);
	glColor3f(0.45f, 0.4, 0.4f);
	glCallList(areadraw);
	glColor3f(1, 1, 1);


}
void sceneRender::display()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.5, 0.5, 0.9, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glLoadIdentity();
	gluLookAt(mot.cameraPosition.x, mot.cameraPosition.y, mot.cameraPosition.z, mot.lookatPosition.x, mot.lookatPosition.y, mot.lookatPosition.z, 0.0, 1.0, 0.0);
	glScalef(2.0f, 2.0f, 2.0f);
	drawTerrain();
	drawScene();
	drawSkybox();

	glPopMatrix();

	fpstool.drawFPS();

	//glutWarpPointer(mot.windowWidth / 2, mot.windowHeight / 2);

	glutSwapBuffers();
}
void sceneRender::idlefunc()
{	
	mot.KeyActions();
	fpstool.calculateFPS();

	glutPostRedisplay();	
}




//=============================================================================
// CALLBACKS
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// GLU_TESS CALLBACKS
///////////////////////////////////////////////////////////////////////////////
void CALLBACK tessBeginCB(GLenum which)
{
	glBegin(which);

}
void CALLBACK tessEndCB()
{
	glEnd();

}
void CALLBACK tessVertexCB(const GLvoid *data)
{
	// cast back to double type
	const GLdouble *ptr = (const GLdouble*)data;

	glVertex3dv(ptr);
}
///////////////////////////////////////////////////////////////////////////////
// Combine callback is used to create a new vertex where edges intersect.
// In this function, copy the vertex data into local array and compute the
// color of the vertex. And send it back to tessellator, so tessellator pass it
// to vertex callback function.
//
// newVertex: the intersect point which tessellator creates for us
// neighborVertex[4]: 4 neighbor vertices to cause intersection (given from 3rd param of gluTessVertex()
// neighborWeight[4]: 4 interpolation weights of 4 neighbor vertices
// outData: the vertex data to return to tessellator
///////////////////////////////////////////////////////////////////////////////
void CALLBACK tessCombineCB(const GLdouble newVertex[3], const GLdouble *neighborVertex[4],
	const GLfloat neighborWeight[4], GLdouble **outData)
{
	//// copy new intersect vertex to local array
	//// Because newVertex is temporal and cannot be hold by tessellator until next
	//// vertex callback called, it must be copied to the safe place in the app.
	//// Once gluTessEndPolygon() called, then you can safly deallocate the array.
	vertices[vertexIndex][0] = newVertex[0];
	vertices[vertexIndex][1] = newVertex[1];
	vertices[vertexIndex][2] = newVertex[2];

	// return output data (vertex coords and others)
	*outData = vertices[vertexIndex];   // assign the address of new intersect vertex

	++vertexIndex;  // increase index for next vertex
}

void CALLBACK tessCombineCBHighway(const GLdouble newVertex[3], const GLdouble *neighborVertex[4],
	const GLfloat neighborWeight[4], GLdouble **outData)
{
	//// copy new intersect vertex to local array
	//// Because newVertex is temporal and cannot be hold by tessellator until next
	//// vertex callback called, it must be copied to the safe place in the app.
	//// Once gluTessEndPolygon() called, then you can safly deallocate the array.
	highwayVertex[highwayVertexIndex][0] = newVertex[0];
	highwayVertex[highwayVertexIndex][1] = newVertex[1];
	highwayVertex[highwayVertexIndex][2] = newVertex[2];

	// return output data (vertex coords and others)
	*outData = highwayVertex[highwayVertexIndex];   // assign the address of new intersect vertex

	++highwayVertexIndex;  // increase index for next vertex
}

void CALLBACK tessErrorCB(GLenum errorCode)
{
	const GLubyte *errorStr;

	errorStr = gluErrorString(errorCode);
	cerr << "[ERROR]: " << errorStr << endl;
}

void CALLBACK tessVertexCBHighway(const GLvoid *data)
{
	// cast back to double type
	const GLdouble *ptr = (const GLdouble*)data;

	double a = *(ptr + 3);
	double b = *(ptr + 4);

	glTexCoord2d(a,b);
	glVertex3dv(ptr);
}