#pragma once
#include "OSMparser.h"
# define PI 3.14159265358979323846


struct coordinate
{
	double meterx;
	double meterz;
};

struct HeightmapInfo
{
	int Lat, Lon;
	coordinate coordmin, coordmax;
	double sizeX, sizeZ; //Sample size in meter
	double height;
	double width;
};

struct Terrain
{
	//Index BBOX of Terrain to Draw;
	int left, right, bottom, top;
	//Shift of Bbox to adjust from origin
	double shiftx, shiftz;

	//meter coordinates of terrain
	BBox BBox;

};


class MercatorProjection
{

	
public:
	const double originShift = PI * 6378137;
	//"Converts given lat/lon in WGS84 Datum to XY in Spherical Mercator EPSG:900913"
	coordinate LatLontoMeters(double lat, double lon);
	//"Converts XY point from Spherical Mercator EPSG:900913 to lat/lon in WGS84 Datum [NOT WORK WELL]" 
	coordinate meterstoLatLon(double meterx, double meterz);
};


class TerrainLoader
{

public:
	short heightmap[1201][1201];
	HeightmapInfo heightMapInfo;
	void loadheightmap(char* filename);

	short getTerrainTriangleHeight(double meterx, double meterz);

	short getTerrainHeight(double lat, double lon);
	short getTerrainHeight2(double meterx, double meterz);
private:
	short convertLittleEndian(short num);

};