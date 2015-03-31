#pragma once
#include "OSMparser.h"
#include "gltools.h"
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
	
	MercatorProjection()
	{
		tileSize = 256;
		initialResolution = 2.0 * (double)PI * 6378137.0 / (double)tileSize;
	}

	//"Converts given lat/lon in WGS84 Datum to XY in Spherical Mercator EPSG:900913"
	coordinate LatLontoMeters(double lat, double lon);
	//"Converts XY point from Spherical Mercator EPSG:900913 to lat/lon in WGS84 Datum" 
	coordinate meterstoLatLon(double meterx, double meterz);

	//"Converts EPSG:900913 to pyramid pixel coordinates in given zoom level"
	Tuple metertoPixel(double meterx, double meterz, int zoom);

	//Converts pixel coordinates in given zoom level of pyramid to EPSG:900913 
	coordinate pixeltoMeter(double pixelX, double pixelZ, int zoom);

	//"Returns a tile covering region in given pixel coordinates"
	Tuple PixelToTile(double pixelX, double pixelZ);

private:
	const double originShift = PI * 6378137.0;
	int tileSize;
	double initialResolution;

	//"Resolution (meters/pixel) for given zoom level (measured at Equator)"
	double Resolution(int zoom)
	{
		return initialResolution / (double)pow(2, zoom);
	}


};



class AerialImageLoader
{

public :
	AerialImageLoader();

	//Saves aerial images to the BingAerial Folder
	bool downloadAndSaveImage(std::string url, std::string fileName);

	//generate tile URL from bounding box
	std::string getTileURL(BBox bbox, int pixelsizeX, int pixelsizeY);

	//generate tile URL from centerpoint and specified zoom level (19)
	std::string getTileURL2(coordinate centercoord, int pixelX, int pixelY);

	//download necessary aerials according to OSM bbox
	void downloadAerials(Terrain *terrain, HeightmapInfo *heightmapinfo);

private:
	std::string apiKEY;
	std::string cacheFolder;

};


class TerrainLoader
{

public:

	short heightmap[1201][1201];

	//Hold information about height map loaded
	HeightmapInfo heightMapInfo;

	//load height map to the memory given
	void loadheightmap(char* filename);

	//Get the height of terrain given lat/lon value
	double getTerrainHeight(double lat, double lon);

	//Get the height of terrain given meter coordinate
	double getTerrainHeight2(double meterx, double meterz);
private:

	//Convert heightmap entries from BigEndian to LittleEndian for windows
	short convertLittleEndian(short num);

	//interpolate triangle for obtain inner height value in triangle
	double getTerrainTriangleHeight(double meterx, double meterz);

};