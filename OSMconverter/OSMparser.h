#if !defined(AFX_STDAFX_H__750FCCDF_A2FA_4F11_A272_44E4718EA66C__INCLUDED_)
#define AFX_STDAFX_H__750FCCDF_A2FA_4F11_A272_44E4718EA66C__INCLUDED_

#import <msxml3.dll>
#include <vector>
#include "gltools.h"
#include <iostream>

using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum wayType {
	building,
	river,
	railway,
	highwayResidual,
	highwayUnclassified,
	highwayPrimary,
	highwaySecondary,
	highwayTertiary,
	highwayService,
	highwayPedestrian,
	highwayFootway,
	highwayPath,
	landuseForest,
	landuseGrass,
	landuseMeadow,
	leisure,
	historic,
	amenity,
	amenityParking,
	amenitySocialFacility,
	amenityMotorCycleParking,
	amenityFireStation,
	unknown
};


enum barrierType
{
	barrier_fence,
	barrier_wall,
	barrier_cityWall,
	barrier_none
};
enum historicType
{
historic_cityGate,
historic_cityWalls,
historic_monument,
historic_none
};




struct nd
{
	BSTR ref;
};

struct Tag
{
	BSTR k;
	BSTR v;
};

struct Node
{
	BSTR id;
	double lat;
	double lon;
	double meterx;
	double meterz;
	double height;
	vector<Tag> tags;
};

struct Member
{
	BSTR type;
	BSTR ref;
	BSTR role;
};

struct Relation
{
	BSTR id;
	vector<Member> members;
	vector<Tag> tags;

};

struct Way
{
	BSTR id;
	wayType type;
	bool isArea = false;
	barrierType barrier = barrier_none;
	historicType historic = historic_none;
	vector<Node> nodes;
	vector<Tag> tags;
};

struct Building
{
	BSTR id;
	int floorNumber;
	double floorHeight;
	double BuildingTopHeight;
	GLuint TextureID;
	vector<Node> nodes;
	vector<Tag> tags;
};

struct Building2
{
	int id;
	double floorHeight;
	int floorNumber;
	double BuildingTopHeight; // To Equalize Top surface of building
	int TextureID;
	Way outerWall;
	vector<Way> innerWalls;
	vector<Tag> tags;
};


struct HighWay
{
	BSTR id;
	GLuint textureID;
	string texturePath;
	wayType type;
	vector<Node> nodes;
	vector<Tag> tags;
	float size;

	vector<Triple> leftSideVertexes;
	vector<Triple> rightSideVertexes;

	vector<Tuple> leftSideTexCoords;
	vector<Tuple> rightSideTexCoords;

	vector<Triple> leftIntersections;
	vector<Triple> rightIntersections;

};

struct BBox
{
	double left, bottom, top, right;
	double meterLeft, meterBottom, meterTop, meterRight;

};

class OSMparser
{

public:
	
	struct BBox bbox;
	vector<Node> nodeList;
	vector<Way> wayList;
	vector<Relation> relationList;
	vector<Building> buildingList;
	vector<Building2> buildingList2;
	vector<HighWay> highWayList; //Rivers + railways included

	void parseOSM(char *szFileName);
	Node findnode(vector<Node> *nodelist, int listSize, BSTR id);
	Way findWay(vector<Way> *waylist, int listSize, BSTR id);

private:

	enum enumNode
	{
		eNode,
		eWay,
		eRelation,
		eBbox,
		undefined
	};
	inline void TESTHR(HRESULT _hr);
	OSMparser::enumNode getNodeEnumeration(BSTR inString);
	void readNodeType(MSXML2::IXMLDOMNode* node);
	void readWayType(MSXML2::IXMLDOMNode* node);
	void readRelationType(MSXML2::IXMLDOMNode* node);
	void readBBoxType(MSXML2::IXMLDOMNode* node);
	wayType assignWayType(Way *way);

	
	
};

#endif // !defined(AFX_STDAFX_H__750FCCDF_A2FA_4F11_A272_44E4718EA66C__INCLUDED_)