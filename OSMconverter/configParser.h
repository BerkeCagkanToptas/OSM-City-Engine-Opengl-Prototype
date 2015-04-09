
#import <msxml3.dll>
#include <string>
#include "gltools.h"

using namespace std;

typedef struct _BuildingTexture
{
	string type;
	string path;
	double width;
}BuildingTexture;

typedef struct _Highwayconfig
{
	double size;
	string texturepath;
}Highwayconfig;

typedef struct _Buildingconfig
{
	int floornumber;
	double floorheight;
	Triple defaultcolor;
	vector<BuildingTexture> textures;

}Buildingconfig;



class configParser
{

public:

	struct Configurations
{
	Highwayconfig residual;
	Highwayconfig unclassified;
	Highwayconfig primary;
	Highwayconfig secondary;
	Highwayconfig tertiary;
	Highwayconfig service;
	Highwayconfig footway;
	Highwayconfig path;
	Highwayconfig pavament;

	Highwayconfig river;
	Highwayconfig railway;

	Buildingconfig building;
	Triple AreaColor;

};

	Configurations config;
	void parseConfigFile(string fileName);
	configParser();

	private:
	enum Nodetype
	{
		enumHighway,
		enumRiver,
		enumRailway,
		enumBuilding,
		enumArea,
		undefined
	};
	void readHighwayNode(MSXML2::IXMLDOMNode* node);
	void readBuildingNode(MSXML2::IXMLDOMNode* node);
	void readAreaNode(MSXML2::IXMLDOMNode* node);
	void readRailwayNode(MSXML2::IXMLDOMNode* node);
	void readRiverNode(MSXML2::IXMLDOMNode* node);
	
	Nodetype getNodeEnumeration(BSTR inString);
};

class savedconfigParser
{
public:
	savedconfigParser();
	~savedconfigParser();
};

