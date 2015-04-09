#pragma once

#include "OSMparser.h"
#include "geoTools.h"
#include "configParser.h"



class sceneGenerator
{

public:
	void init(char* xmlFile, char* geoFile);
	
	OSMparser parser;
	configParser configparser;
	TerrainLoader terrainLoader;
	Terrain terrain;
	

	vector<Node> Trees;
	

	void generateHighwayBuffers(HighWay *way, float *vertexBuffer, float *textureBuffer);
	void generateBuildingBuffers(Building *building, float *vertexBuffer, float *textureBuffer);

private:


	void applyConfigurations();

	void updateBuildingHeights();
	void updateWayHeights();
	void generateTerrain();

	void generateBuildingList2(); //generate Buildings by looking relationList
	void updateBuildingInfo();
	void correctWayConnections();
	void generateTreeTable();

	void generate3DRoads();
	void generate3Dway(HighWay *way);


	void addIntersection1(HighWay *way);
	void addIntersection2(HighWay *way);
	void addIntersection3(HighWay *way);
	void divideLongWaySegment(HighWay *way);
	void generateTexCoords(HighWay *way);
	void generateInitial3Dway(HighWay *way);
};

