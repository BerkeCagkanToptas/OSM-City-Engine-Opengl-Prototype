#pragma once

#include "OSMparser.h"
#include "geoTools.h"

struct NEWbuilding
{
	int id;
	double BuildingHeight;
	int TextureID;
	Way outerWall;
	vector<Way> innerWalls;
	vector<Tag> tags;
};

class sceneGenerator
{

public:
	void init(char* xmlFile, char* geoFile);
	
	OSMparser parser;
	TerrainLoader terrainLoader;


	Terrain terrain;
	vector<NEWbuilding> buildingListNEW;
	vector<Node> Trees;
	

	void generateHighwayBuffers(HighWay *way, float *vertexBuffer, float *textureBuffer);
	void generateBuildingBuffers(Building *building, float *vertexBuffer, float *textureBuffer);

private:
	void updateBuildingHeights();
	void updateWayHeights();
	void generateTerrain();

	void generateBuildingList(); //generate Buildings by looking relationList
	void updateBuildingInfo();
	void correctWayConnections();
	void fillTreeTable();	//RENDERING PART IS IMCOMPLETE

	void generate3DRoads();
	void generate3Dway(HighWay *way);


	void addIntersection1(HighWay *way);
	void addIntersection2(HighWay *way);
	void addIntersection3(HighWay *way);
	void divideLongWaySegment(HighWay *way);
	void generateTexCoords(HighWay *way);
	void generateInitial3Dway(HighWay *way);
};

