#include <vector>
#include <iostream>
#include <math.h>
#include <fstream>
#include "geoTools.h"

using namespace std;

coordinate MercatorProjection::LatLontoMeters(double lat, double lon)
{
	double meterx, meterz;

	meterz = lon * originShift / 180.0f;
	meterx = log(tan((90 + lat) * PI / 360.0)) / (PI / 180);
	meterx = meterx * originShift / 180.0f;

	return coordinate{ meterx, meterz };
}

coordinate MercatorProjection::meterstoLatLon(double mx, double my)
{
	coordinate coord;

	double lat, lon;

	lon = (my / originShift) * 180.0;
	lat = (mx / originShift) * 180.0;
	lat = 180 / PI * (2 * atan(exp(lat * PI / 180.0)) - PI / 2.0);

	coord.meterx = lat;
	coord.meterz = lon;
	return coord;  // (lat , Lon)
}

void TerrainLoader::loadheightmap(char* filename)
{
	
	//Reading filename and get the Coordinates
	int startindex = 0;
	for (int i = 0;; i++)
	{
		if (filename[i] == '/' || filename[i] == '\\')
			startindex = i+1;
		if (filename[i] == '.')
			break;
	}

	int Lat = 10 * (filename[startindex+1] - 48)  + (filename[startindex+2] - 48);
	int Lon = 100 *(filename[startindex+4] - 48)  + 10* (filename[startindex+5] - 48) + (filename[startindex+6]-48);


	MercatorProjection proj;
	heightMapInfo.Lat = Lat;
	heightMapInfo.Lon = Lon;
	heightMapInfo.coordmin = proj.LatLontoMeters(Lat, Lon);
	heightMapInfo.coordmax = proj.LatLontoMeters(Lat + 1, Lon + 1);

	heightMapInfo.sizeX = (heightMapInfo.coordmax.meterx - heightMapInfo.coordmin.meterx) / 1200;
	heightMapInfo.sizeZ = (heightMapInfo.coordmax.meterz - heightMapInfo.coordmin.meterz) / 1200;

	heightMapInfo.height = (heightMapInfo.coordmax.meterx - heightMapInfo.coordmin.meterx);
	heightMapInfo.width = (heightMapInfo.coordmax.meterz - heightMapInfo.coordmin.meterz);


	ifstream inputfile(filename, ios::in | ios::binary);
	int end;

	if (inputfile.is_open())
	{
		inputfile.seekg(0, ios::end);
		end = inputfile.tellg();
		inputfile.seekg(0);
		for (int i = 0; i < 1201; i++)
		{
			for (int j = 0; j < 1201; j++)
			{
				short tmp;
				inputfile.read((char*)&tmp, sizeof(short));

				heightmap[i][j] = convertLittleEndian(tmp);
				if (heightmap[i][j] < -1000)
					heightmap[i][j] = heightmap[i][j-1];
				
			}
		}
		if (inputfile.tellg() == (streampos)end)
			cout << "finished reading Terrain Map" << endl;

		inputfile.close();
	}
	else
		cout << "Height map Could not Opened !!" << endl;
}

short TerrainLoader::convertLittleEndian(short num)
{
	short ret;
	unsigned char *dst = (unsigned char *)&ret;
	unsigned char *src = (unsigned char *)&num;

	dst[0] = src[1];
	dst[1] = src[0];

	return ret;

}

double TerrainLoader::getTerrainHeight(double lat, double lon) // BiLinear Interpolation is Applied
{
	MercatorProjection proj;
	coordinate coord = proj.LatLontoMeters(lat,lon);
	return getTerrainTriangleHeight(coord.meterx, coord.meterz);
}

double TerrainLoader::getTerrainHeight2(double meterx, double meterz)
{
	return getTerrainTriangleHeight(meterx, meterz);
}

double TerrainLoader::getTerrainTriangleHeight(double meterx, double meterz)
{
		double finalvalue;


		double ratioX = (heightMapInfo.coordmax.meterx - meterx) / heightMapInfo.height;
		double ratioZ = (meterz - heightMapInfo.coordmin.meterz) / heightMapInfo.width;
	
		double Xindex = (1200 * ratioX);
		double Zindex = (1200 * ratioZ);

		//Upper Triangle
		if ((Xindex - floor(Xindex)) + (Zindex - floor(Zindex)) < 1.0f)
		{
			int n = heightmap[(int)floor(Xindex)][(int)ceil(Zindex)];
			int m = heightmap[(int)floor(Xindex)][(int)floor(Zindex)];
			int k = heightmap[(int)ceil(Xindex)][(int)floor(Zindex)];
			double interpolationUp = m + (n - m) * (Zindex - floor(Zindex));
			double interpolationHypothenus = k + (n - k) * (Zindex - floor(Zindex));
		
			double Xtop = heightMapInfo.coordmax.meterx - floor(Xindex) * heightMapInfo.sizeX;
			double a = Xtop - meterx;
			double aplusb = heightMapInfo.sizeX * (ceil(Zindex) - Zindex);
			finalvalue = interpolationUp + (interpolationHypothenus - interpolationUp) * (a / aplusb);
			//return finalvalue;

		}

		//Lower Triangle
		else if ((Xindex - floor(Xindex)) + (Zindex - floor(Zindex)) >= 1.0f)
		{
			int n = heightmap[(int)ceil(Xindex)][(int)ceil(Zindex)];
			int m = heightmap[(int)ceil(Xindex)][(int)floor(Zindex)];
			int k = heightmap[(int)floor(Xindex)][(int)ceil(Zindex)];
			double interpolationDown = m + (n - m) * (Zindex - floor(Zindex));
			double interpolationHypothenus = m + (k - m) * (Zindex - floor(Zindex));
				
			double Xbottom = heightMapInfo.coordmax.meterx - ceil(Xindex) * heightMapInfo.sizeX;
			double a = meterx - Xbottom;
			double aplusb = heightMapInfo.sizeX * (Zindex - floor(Zindex));
			finalvalue = interpolationDown + (interpolationHypothenus - interpolationDown) * (a / aplusb);
			//return finalvalue;
		}

		//[ALTERNATE METHOD]
		//double finalvalue2;
		////Upper Triangle
		//if ((Xindex - floor(Xindex)) + (Zindex - floor(Zindex)) < 1.0f)
		//{
		//	int n = heightmap[(int)floor(Xindex)][(int)ceil(Zindex)];
		//	int m = heightmap[(int)floor(Xindex)][(int)floor(Zindex)];
		//	int k = heightmap[(int)ceil(Xindex)][(int)floor(Zindex)];
		//	double interpolationUp = m + (n - m) * (Zindex - floor(Zindex));
		//	double interpolationHypothenus = k + (n - k) * (Zindex - floor(Zindex));
		//
		//	double a = Xindex - floor(Xindex);
		//	double aplusb = (ceil(Zindex) - Zindex);
		//	finalvalue2 = interpolationUp + (interpolationHypothenus - interpolationUp) * (a / aplusb);
		//}
		//
		////Lower Triangle
		//else if ((Xindex - floor(Xindex)) + (Zindex - floor(Zindex)) >= 1.0f)
		//{
		//	int n = heightmap[(int)ceil(Xindex)][(int)ceil(Zindex)];
		//	int m = heightmap[(int)ceil(Xindex)][(int)floor(Zindex)];
		//	int k = heightmap[(int)floor(Xindex)][(int)ceil(Zindex)];
		//	double interpolationDown = m + (n - m) * (Zindex - floor(Zindex));
		//	double interpolationHypothenus = m + (k - m) * (Zindex - floor(Zindex));
		//		
		//	double a = ceil(Xindex) - Xindex;
		//	double aplusb = Zindex - floor(Zindex);
		//	finalvalue2 = interpolationDown + (interpolationHypothenus - interpolationDown) * (a / aplusb);
		//}

		return finalvalue;
}
