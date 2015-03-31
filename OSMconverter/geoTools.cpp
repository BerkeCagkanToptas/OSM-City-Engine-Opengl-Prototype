#include <vector>
#include <iostream>
#include <math.h>
#include <curl/curl.h>
#include <fstream>
#include "geoTools.h"
#include <string>


using namespace std;

coordinate MercatorProjection::LatLontoMeters(double lat, double lon)
{
	double meterx, meterz;

	meterz = lon * originShift / 180.0f;
	meterx = log(tan((90 + lat) * PI / 360.0)) / (PI / 180);
	meterx = meterx * originShift / 180.0f;

	return coordinate{ meterx, meterz };
}

coordinate MercatorProjection::meterstoLatLon(double meterx, double meterz)
{
	coordinate coord;

	double lat, lon;

	lon = (meterz / originShift) * 180.0;
	lat = (meterx / originShift) * 180.0;
	lat = 180 / PI * (2 * atan(exp(lat * PI / 180.0)) - PI / 2.0);

	coord.meterx = lat;
	coord.meterz = lon;
	return coord;  // (lat , Lon)
}

Tuple MercatorProjection::metertoPixel(double meterx, double meterz, int zoom)
{

	double res = Resolution(zoom);
	double pixelX, pixelY;

	pixelY = (meterx + originShift) / res;
	pixelX = (meterz + originShift) / res;

	return Tuple(pixelX, pixelY);

}

coordinate MercatorProjection::pixeltoMeter(double pixelX, double pixelY, int zoom)
{
	double res = Resolution(zoom);
	double meterX = (pixelY * res) - originShift;
	double meterZ = (pixelX * res) - originShift;

	coordinate coord;
	coord.meterx = meterX;
	coord.meterz = meterZ;
	return coord;
}

Tuple MercatorProjection::PixelToTile(double pixelX, double pixelY)
{
	int Tilex = (int)(ceil(pixelX / (double)tileSize)) -1;
	int Tiley = (int)(ceil(pixelY / (double)tileSize)) -1;

	return Tuple(Tilex, Tiley);
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

AerialImageLoader::AerialImageLoader()
{

	apiKEY = "Ajk_KcXsfD1lOygMwZcc1m3-1nwh_qugU1rQQuDyWGVfJ3yjld16xJrdXm7kUJqM";
	cacheFolder = "BingAerial/";


}

std::string AerialImageLoader::getTileURL(BBox bbox, int pixelX, int pixelY)
{
	string server = "http://dev.virtualearth.net/REST/v1/Imagery/Map/Aerial?";
	string mapSize = to_string(pixelX) + "," + to_string(pixelY);
	string mapArea = std::to_string(bbox.bottom) + "," + std::to_string(bbox.left) + "," + std::to_string(bbox.top) + "," + std::to_string(bbox.right);
	string URL = server + "maparea=" + mapArea + "&ms=" + mapSize + "&key=" + apiKEY;

	return URL;
}

string AerialImageLoader::getTileURL2(coordinate centercoord, int pixelX, int pixelY)
{
	string server = "http://dev.virtualearth.net/REST/v1/Imagery/Map/Aerial/";
	string centerpoint = to_string(centercoord.meterx) + "," + to_string(centercoord.meterz);
	string zoomLevel = "19";
	string mapSize = to_string(pixelX) + "," + to_string(pixelY);
	string URL = server + centerpoint + "/" + zoomLevel + "?mapsize=" + mapSize + "&key=" + apiKEY;

	return URL;
}

void AerialImageLoader::downloadAerials(Terrain *terrain, HeightmapInfo *heightmapinfo)
{

	double delta = 1.0 / 1200.0;
	MercatorProjection proj;

	for (int i = terrain->top+1; i <= terrain->bottom; i += 2)
	{
		for (int j = terrain->left+1; j <= terrain->right; j += 2)
		{
			coordinate centerCoordlatlon;
			centerCoordlatlon.meterx = (double)heightmapinfo->Lat + 1.0 - delta * i;
			centerCoordlatlon.meterz = (double)heightmapinfo->Lon + delta * j;

			coordinate bottomleftCoordlatlon;
			bottomleftCoordlatlon.meterx = (double)heightmapinfo->Lat + 1.0 - delta * (i + 1);
			bottomleftCoordlatlon.meterz = (double)heightmapinfo->Lon + delta * (j - 1);
			coordinate toprightCoordlatlon;
			toprightCoordlatlon.meterx = heightmapinfo->Lat + 1 - delta * (i - 1);
			toprightCoordlatlon.meterz = heightmapinfo->Lon + delta * (j + 1);

			coordinate bottomleftmeter = proj.LatLontoMeters(bottomleftCoordlatlon.meterx, bottomleftCoordlatlon.meterz);
			Tuple bottomleftpixel = proj.metertoPixel(bottomleftmeter.meterx, bottomleftmeter.meterz,19);
			coordinate toprightmeter = proj.LatLontoMeters(toprightCoordlatlon.meterx, toprightCoordlatlon.meterz);
			Tuple toprightpixel = proj.metertoPixel(toprightmeter.meterx, toprightmeter.meterz, 19);

			int pixelX = (int)abs(bottomleftpixel.x - toprightpixel.x);
			int pixelY = (int)abs(bottomleftpixel.y - toprightpixel.y);


			string url = getTileURL2(centerCoordlatlon, pixelX, pixelY);
			string fileName = "Bing" + to_string(heightmapinfo->Lat) + "_" + to_string(heightmapinfo->Lon) + "H" + to_string(j-1) + "_" + to_string(i-1) + ".jpg";
			downloadAndSaveImage(url, fileName);

		}
	}

}

bool AerialImageLoader::downloadAndSaveImage(std::string url, std::string FileNametoSave)
{
	//check if file already exists
	std::string outputPath = cacheFolder + FileNametoSave;

	std::ifstream fin(outputPath.c_str());
	if (!fin.fail())
	{
		fin.close();
		cout << "Aerial image is exists on the cache" << endl;
		return true;
	}

	CURL *curl;
	FILE *fp;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) 
	{
		try
		{
			fp = fopen(outputPath.c_str(), "wb");
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			res = curl_easy_perform(curl);
			/* always cleanup */
			curl_easy_cleanup(curl);
			fclose(fp);
		}
		catch (exception e)
		{
			cout << "Problem occured getting Image from Server" << endl;
			return false;
		}
	}


	return true;
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

		MercatorProjection proj;
		coordinate c = proj.meterstoLatLon(meterx, meterz);

		double ratioX = heightMapInfo.Lat + 1.0 - c.meterx;
		double ratioZ = c.meterz - heightMapInfo.Lon;

		//WRONG CALCULATION
		double ratioXwro = (heightMapInfo.coordmax.meterx - meterx) / heightMapInfo.height;
		double ratioZwro = (meterz - heightMapInfo.coordmin.meterz) / heightMapInfo.width;
	
		double Xindexwro = 1200.0 * ratioXwro;
		double Zindexwro = 1200.0 * ratioZwro;

		double Xindex = (1200.0 * ratioX);
		double Zindex = (1200.0 * ratioZ);
		double delta = 1.0 / 1200.0;



		////Upper Triangle
		//if ((Xindex - floor(Xindex)) + (Zindex - floor(Zindex)) < 1.0f)
		//{
		//	int n = heightmap[(int)floor(Xindex)][(int)ceil(Zindex)];
		//	int m = heightmap[(int)floor(Xindex)][(int)floor(Zindex)];
		//	int k = heightmap[(int)ceil(Xindex)][(int)floor(Zindex)];
		//	double interpolationUp = m + (n - m) * (Zindex - floor(Zindex));
		//	double interpolationHypothenus = k + (n - k) * (Zindex - floor(Zindex));
		//
		//	//double Xtop = heightMapInfo.coordmax.meterx - floor(Xindex) * heightMapInfo.sizeX;
		//	//double a = Xtop - meterx;
		//	//double aplusb = heightMapInfo.sizeX * (ceil(Zindex) - Zindex);
		//	
		//	double Xtop = heightMapInfo.Lat + 1 - floor(Xindex) * delta;
		//	double a = Xtop - c.meterx;
		//	double aplusb = delta * (ceil(Zindex) - Zindex);
		//	
		//	finalvalue = interpolationUp + (interpolationHypothenus - interpolationUp) * (a / aplusb);
		//}

		////Lower Triangle
		//else if ((Xindex - floor(Xindex)) + (Zindex - floor(Zindex)) >= 1.0f)
		//{
		//	int n = heightmap[(int)ceil(Xindex)][(int)ceil(Zindex)];
		//	int m = heightmap[(int)ceil(Xindex)][(int)floor(Zindex)];
		//	int k = heightmap[(int)floor(Xindex)][(int)ceil(Zindex)];
		//	double interpolationDown = m + (n - m) * (Zindex - floor(Zindex));
		//	double interpolationHypothenus = m + (k - m) * (Zindex - floor(Zindex));
		//		
		//	//double Xbottom = heightMapInfo.coordmax.meterx - ceil(Xindex) * heightMapInfo.sizeX;
		//	//double a = meterx - Xbottom;
		//	//double aplusb = heightMapInfo.sizeX * (Zindex - floor(Zindex));

		//	double Xbottom = heightMapInfo.Lat + 1 - ceil(Xindex) * delta;
		//	double a = c.meterx - Xbottom;
		//	double aplusb = delta * (Zindex - floor(Zindex));

		//	finalvalue = interpolationDown + (interpolationHypothenus - interpolationDown) * (a / aplusb);
		//}

		//[ALTERNATE METHOD]

	


		//Upper Triangle
		if ((Xindex - floor(Xindex)) + (Zindex - floor(Zindex)) < 1.0f)
		{
			int n = heightmap[(int)floor(Xindex)][(int)ceil(Zindex)];
			int m = heightmap[(int)floor(Xindex)][(int)floor(Zindex)];
			int k = heightmap[(int)ceil(Xindex)][(int)floor(Zindex)];
			double interpolationUp = m + (n - m) * (Zindex - floor(Zindex));
			double interpolationHypothenus = k + (n - k) * (Zindex - floor(Zindex));
		
			double a = Xindex - floor(Xindex);
			double aplusb = (ceil(Zindex) - Zindex);
			finalvalue = interpolationUp + (interpolationHypothenus - interpolationUp) * (a / aplusb);

		}
		
		//Lower Triangle
		else if ((Xindex - floor(Xindex)) + (Zindex - floor(Zindex)) >= 1.0f)
		{

			int n = heightmap[(int)ceil(Xindex)][(int)ceil(Zindex)];
			int m = heightmap[(int)ceil(Xindex)][(int)floor(Zindex)];
			int k = heightmap[(int)floor(Xindex)][(int)ceil(Zindex)];
			double interpolationDown = m + (n - m) * (Zindex - floor(Zindex));
			double interpolationHypothenus = m + (k - m) * (Zindex - floor(Zindex));
				
			double a = ceil(Xindex) - Xindex;
			double aplusb = Zindex - floor(Zindex);
			finalvalue = interpolationDown + (interpolationHypothenus - interpolationDown) * (a / aplusb);
		
			if ((Xindex - floor(Xindex)) + (Zindex - floor(Zindex)) > 0.98f && (Xindex - floor(Xindex)) + (Zindex - floor(Zindex)) < 1.02f)
				return interpolationHypothenus;

		}
			

		return finalvalue;
}
