#include "gltools.h"
#include "sceneGenerator.h"



class sceneRender
{


public:
		 sceneRender(char* xmlFile, char* geoFile);
	void initScene();
	void display();
	void idlefunc();
	void reshape(int w, int h);
	void Keypressed(unsigned char key, int x, int y) { mot.keypressed(key, x, y);}
	void Keyup(unsigned char key, int x, int y){ mot.keyup(key, x, y); }
	void MouseControl(int MouseX, int MouseY){ mot.mouseControlOld(MouseX, MouseY); }
	void MouseWheel(int button, int dir, int x, int y){ mot.mouseWheel(button, dir, x, y); }
	void drawScene();

private:

	void drawBuildings();
	void drawBuildings2();
	void drawHighways();
	void drawWays();
	void drawTrees();
	void drawTerrain();

	void initSkybox();
	void drawSkybox();
	GLuint tessellateRoof(vector<Building> *buildingList);
	GLuint tessellateRoof2();
	GLuint tessellateAmenity(vector<Way> *wayList);
	GLuint tessellateArea(vector<Way> *wayList);

	GLuint tessellateHighway(HighWay *way, GLuint texture, float layerheight);
	void generateHighwayList();


	void Draw3DWay(HighWay *way, GLuint texture, float layerheight); //LayerHeight : How high it should be compared to other road types for intersection
	void drawHighways2();


	Triple cameraPos;
	Triple lookatPos;


	sceneGenerator scene;
	BmpLoader bmploader;
	GLuint Skybox[6];
	GLuint roadUnclassifiedTexture;
	GLuint roadResidualTexture;
	GLuint roadServiceTexture;
	GLuint roadPrimaryTexture;
	GLuint roadSecondaryTexture;
	GLuint roadTertiaryTexture;
	GLuint roadPavementTexture;
	GLuint roadPathTexture;
	GLuint railwayTexture;
	GLuint riverTexture;

	GLuint buildingTexture;
	GLuint buildingTexture2;
	GLuint buildingTexture3;
	GLuint kioskTexture;

	GLuint wallTexture;
	GLuint roadTestTexture;

	Motion mot;


};