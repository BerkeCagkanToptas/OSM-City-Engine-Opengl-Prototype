#include "sceneGenerator.h"
#include "gltools.h"
#include <iostream>

using namespace std;


void sceneGenerator::init(char* XMLfile, char* GEOfile)
{
	cout << "reading GeoData ..." << endl;
	terrainLoader.loadheightmap(GEOfile);
	cout << "reading OSM file ..." << endl;
	parser.parseOSM(XMLfile);
	cout << "Succesfully read" << endl;

	cout << "number of building is :" << parser.buildingList.size() + buildingListNEW.size() << endl;
	cout << "number of highway  is :" << parser.highWayList.size() << endl;

	generateTerrain();
	updateBuildingInfo(); //should be in front of updatebuildingheights()
	updateBuildingHeights();
	updateWayHeights();

	generateBuildingList();

	fillTreeTable();
	
	
	generate3DRoads();
	

	//MercatorProjection proj;
	//cout.precision(12);
	//coordinate coord = proj.LatLontoMeters(9.30, 20.15);
	//cout << "originShift:" << proj.originShift << endl;
	//cout << "Bir enlem boyu :" << proj.originShift / 180.0f << endl;
	//cout << "Input:  9.30,20.15" << endl;
	//cout <<"Meter:" << coord.meterx << " " << coord.meterz << endl;
	//coordinate coord2 = proj.meterstoLatLon(coord.meterx, coord.meterz);
	//cout << "Lat/Lon:" << coord2.meterx << " " << coord2.meterz << endl;
	//cout << "--------------" << endl;
	//coord = proj.LatLontoMeters(45, 9);	
	//cout << "Input:  45,9" << endl;
	//cout << "Meter:" << coord.meterx << " " << coord.meterz << endl;
	//coord2 = proj.meterstoLatLon(coord.meterx, coord.meterz);
	//cout << "Lat/Lon:" << coord2.meterx << " " << coord2.meterz << endl;


}

void sceneGenerator::updateBuildingHeights()
{
	
	for (int i = 0; i < (int)parser.buildingList.size(); i++)
	{
		double maxHeight = 0;

		for (int j = 0; j < (int)parser.buildingList[i].nodes.size(); j++)
		{
			parser.buildingList[i].nodes[j].height = terrainLoader.getTerrainHeight(parser.buildingList[i].nodes[j].lat, parser.buildingList[i].nodes[j].lon);
			if (parser.buildingList[i].nodes[j].height > maxHeight)
				maxHeight = parser.buildingList[i].nodes[j].height;
		}
		parser.buildingList[i].BuildingTopHeight = maxHeight + parser.buildingList[i].BuildingHeight;
	}
}

void sceneGenerator::updateWayHeights()
{
	for (int i = 0; i < (int)parser.wayList.size(); i++)
	{
		for (int j = 0; j < (int)parser.wayList[i].nodes.size(); j++)
		{
			parser.wayList[i].nodes[j].height = terrainLoader.getTerrainHeight(parser.wayList[i].nodes[j].lat, parser.wayList[i].nodes[j].lon);
		}
	}
}

void sceneGenerator::generateTerrain()
{
	MercatorProjection proj;

	//For bottom :
	double ratioX = (terrainLoader.heightMapInfo.coordmax.meterx - parser.bbox.meterBottom) / terrainLoader.heightMapInfo.height;
	int BottomIndex = (int)ceil(1200 * ratioX);

	double shiftX = (1200 * ratioX - BottomIndex) * terrainLoader.heightMapInfo.sizeX;

	//For Left :
	double ratioZ = (parser.bbox.meterLeft - terrainLoader.heightMapInfo.coordmin.meterz) / terrainLoader.heightMapInfo.width;
	int LeftIndex = (int)floor(1200*ratioZ);

	double shiftZ = (LeftIndex - 1200*ratioZ) * terrainLoader.heightMapInfo.sizeZ;

	//For top :
	ratioX = (terrainLoader.heightMapInfo.coordmax.meterx - parser.bbox.meterTop) / terrainLoader.heightMapInfo.height;
	int TopIndex = (int)floor(1200 * ratioX);

	//For Right:
	ratioZ = (parser.bbox.meterRight - terrainLoader.heightMapInfo.coordmin.meterz) / terrainLoader.heightMapInfo.width;
	int RightIndex = (int)ceil(1200 * ratioZ);

	terrain.bottom = BottomIndex;
	terrain.left = LeftIndex;
	terrain.top = TopIndex;
	terrain.right = RightIndex;
	terrain.shiftx = shiftX;
	terrain.shiftz = shiftZ;

	terrain.BBox.meterBottom = shiftX;
	terrain.BBox.meterLeft = shiftZ;
	terrain.BBox.meterTop = terrain.BBox.meterBottom + (BottomIndex - TopIndex) * terrainLoader.heightMapInfo.sizeX;
	terrain.BBox.meterRight = terrain.BBox.meterLeft + (RightIndex - LeftIndex) * terrainLoader.heightMapInfo.sizeZ;

}

void sceneGenerator::generateBuildingList()
{
	int buildingID = 1;
	for (int i = 0; i < (int)parser.relationList.size(); i++)
	{
		bool isbuilding = false;
		NEWbuilding newBuilding;

		for (int j = 0; j < (int)parser.relationList[i].tags.size(); j++)
		{
			if (0 == wcscmp(parser.relationList[i].tags[j].k, L"building"))
			{
				newBuilding.id = buildingID++;
				newBuilding.TextureID = rand() % 3 + 1;
				newBuilding.BuildingHeight = Buildingheight;
				isbuilding = true;

				for (int k = 0; k < (int)parser.relationList[i].tags.size(); k++)
				{
					newBuilding.tags.push_back(parser.relationList[i].tags[k]);
					if (0 == wcscmp(parser.relationList[i].tags[k].k, L"building:levels"))
						newBuilding.BuildingHeight = _wtoi(parser.relationList[i].tags[k].v) * (Buildingheight / 4);
				}
			

				break;
			}
		}

		if (isbuilding == false)
			continue;

		bool duplicateFlag = false;

		for (int j = 0; j < (int)parser.relationList[i].members.size(); j++)
		{
			if (0 == wcscmp(parser.relationList[i].members[j].role, L"outer"))
			{
				Way wall = parser.findWay(&parser.wayList, parser.wayList.size(), parser.relationList[i].members[j].ref);
				if (0 == wcscmp(wall.id, L"NOTEXISTS"))
				{
					duplicateFlag = true;
					break; // Duplicate of BuildingList
				}
				newBuilding.outerWall = wall;

			}
			if (0 == wcscmp(parser.relationList[i].members[j].role, L"inner"))
			{
				newBuilding.innerWalls.push_back(parser.findWay(&parser.wayList, parser.wayList.size(), parser.relationList[i].members[j].ref));
			}
		}

		if (!duplicateFlag)
			buildingListNEW.push_back(newBuilding);
	}



}

void sceneGenerator::updateBuildingInfo()
{
	//Building List come from WAY
	for (int i = 0; i < (int)parser.buildingList.size(); i++)
	{
		for (int j = 0; j < (int)parser.buildingList[i].tags.size(); j++)
		{
			//IF BUILDING IS A KIOSK TYPE
			if (0 == wcscmp(parser.buildingList[i].tags[j].k, L"shop") && 0 == wcscmp(parser.buildingList[i].tags[j].v, L"kiosk"))
			{
				parser.buildingList[i].BuildingTopHeight = 0.25 * Buildingheight;
				parser.buildingList[i].TextureID = 4;
				break;
			}

			// IF BUILDING IS A TOWER
			for (int j = 0; j < (int)parser.buildingList[i].tags.size(); j++)
			{
				if (0 == wcscmp(parser.buildingList[i].tags[j].k, L"man_made") && 0 == wcscmp(parser.buildingList[i].tags[j].v, L"tower"))
				{
					parser.buildingList[i].BuildingHeight = 1.4 * Buildingheight;
					parser.buildingList[i].TextureID = 5;
					break;
				}
			}
		}

	}

	//Building List come from RELATION
	for (int i = 0; i < (int)buildingListNEW.size(); i++)
	{
		// IF BUILDING IS A TOWER
		for (int j = 0; j < (int)buildingListNEW[i].tags.size(); j++)
		{
			if (0 == wcscmp(buildingListNEW[i].tags[j].k, L"man_made") && 0 == wcscmp(buildingListNEW[i].tags[j].v, L"tower"))
			{
				buildingListNEW[i].BuildingHeight = Buildingheight * 1.3;
				buildingListNEW[i].TextureID = 5;
				break;
			}
		}
	}



}

void sceneGenerator::fillTreeTable()
{
	for (int i = 0; i < (int)parser.nodeList.size(); i++)
	{
		for (int j = 0; j < (int)parser.nodeList[i].tags.size(); j++)
		{
			if (0 == wcscmp(parser.nodeList[i].tags[j].k, L"natural") && 0 == wcscmp(parser.nodeList[i].tags[j].v, L"tree"))
				Trees.push_back(parser.nodeList[i]);
		}
	}
}


//HELPERS FOR correctWay Connection Function
double findDistance(Node *node1, Node *node2)
{
	return sqrt(pow(node1->meterx - node2->meterx, 2) + pow(node1->meterz - node2->meterz, 2));
}
double findDistance(Tuple d1, Tuple d2)
{
	return sqrt(pow(d1.x - d2.x, 2) + pow(d1.y - d2.y, 2));
}

//void sceneGenerator::correctWayConnections()
//{
//	Geometry geo;
//	double threshold = 0.01;
//	Tuple intersect;
//
//	for (int i = 0; i < (int)parser.highWayList.size(); i++)
//	{
//		HighWay *selectedWay = &parser.highWayList[i];
//
//
//		for (int j = i + 1; j < (int)parser.highWayList.size(); j++)
//		{
//			//FOR STARTING POINT OF SELECTED WAY
//			for (int k = 0; k < (int)parser.highWayList[j].nodes.size() - 1; k++)
//			{
//				double dist = findDistance(&selectedWay->nodes[0], &parser.highWayList[j].nodes[k]) + findDistance(&selectedWay->nodes[0], &parser.highWayList[j].nodes[k + 1]);
//				double realdist = findDistance(&parser.highWayList[j].nodes[k], &parser.highWayList[j].nodes[k + 1]);
//
//				//FOUND CONNECTION
//				if (abs(realdist - dist) < threshold)
//				{
//					//CHECK FROM WHICH SIDE ROADS ARE CONNECTED
//					Tuple p1(selectedWay->nodes[1].meterx, selectedWay->nodes[1].meterz);
//					Tuple p2(selectedWay->nodes[0].meterx, selectedWay->nodes[0].meterz);
//					double d1 = findDistance(p1, p2); //Length of Branch from center
//					Tuple p3(parser.highWayList[j].nodes3D[4 * k + 2], parser.highWayList[j].nodes3D[4 * k + 3]);
//					Tuple p4(parser.highWayList[j].nodes3D[4 * k + 6], parser.highWayList[j].nodes3D[4 * k + 7]);
//					intersect = geo.getLineIntersection(p1, p2, p3, p4);
//					double d2 = findDistance(p1, intersect); //Length of Branch from right side
//
//					if (d1 > d2)
//					{
//						//Intersection is from right side
//
//						//CORRECT LEFT POINT COORDINATE
//						p1.set(selectedWay->nodes3D[0], selectedWay->nodes3D[1]);
//						p2.set(selectedWay->nodes3D[4], selectedWay->nodes3D[5]);
//						p3.set(parser.highWayList[j].nodes3D[4 * k + 2], parser.highWayList[j].nodes3D[4 * k + 3]);
//						p4.set(parser.highWayList[j].nodes3D[4 * k + 6], parser.highWayList[j].nodes3D[4 * k + 7]);		
//						intersect = geo.getLineIntersection(p1, p2, p3, p4);
//						if (intersect.x == -1 && intersect.y == -1)
//						{
//							if (k == (parser.highWayList[j].nodes.size() - 1))
//							{
//								break;
//								//TODO: Handle this situation
//							}
//							p3.set(parser.highWayList[j].nodes3D[4 * k + 10], parser.highWayList[j].nodes3D[4 * k + 11]);
//							intersect = geo.getLineIntersection(p1, p2, p3, p4);
//						}
//						selectedWay->nodes3D[0] = intersect.x;
//						selectedWay->nodes3D[1] = intersect.y;
//
//						//CORRECT RIGHT POINT COORDINATE	
//						p1.set(selectedWay->nodes3D[2], selectedWay->nodes3D[3]);
//						p2.set(selectedWay->nodes3D[6], selectedWay->nodes3D[7]);
//						p3.set(parser.highWayList[j].nodes3D[4 * k + 2], parser.highWayList[j].nodes3D[4 * k + 3]);
//						p4.set(parser.highWayList[j].nodes3D[4 * k + 6], parser.highWayList[j].nodes3D[4 * k + 7]);
//						intersect = geo.getLineIntersection(p1, p2, p3, p4);
//						if (intersect.x == -1 && intersect.y == -1)
//						{
//							if (k == (parser.highWayList[j].nodes.size() - 1))
//							{
//								break;
//								//TODO: Handle this situation
//							}
//							p3.set(parser.highWayList[j].nodes3D[4 * k + 10], parser.highWayList[j].nodes3D[4 * k + 11]);
//							intersect = geo.getLineIntersection(p1, p2, p3, p4);
//						}
//						selectedWay->nodes3D[2] = intersect.x;
//						selectedWay->nodes3D[3] = intersect.y;
//						break;
//					}
//					else
//					{
//						//Intersection is from left side
//
//						//CORRECT LEFT POINT COORDINATE
//						p1.set(selectedWay->nodes3D[0], selectedWay->nodes3D[1]);
//						p2.set(selectedWay->nodes3D[4], selectedWay->nodes3D[5]);
//						p3.set(parser.highWayList[j].nodes3D[4 * k], parser.highWayList[j].nodes3D[4 * k + 1]);
//						p4.set(parser.highWayList[j].nodes3D[4 * k + 4], parser.highWayList[j].nodes3D[4 * k + 5]);
//						intersect = geo.getLineIntersection(p1, p2, p3, p4);
//						if (intersect.x == -1 && intersect.y == -1)
//						{
//							if (k == 0)
//							{
//								break;
//								//TODO : HANDLE THIS SITUATION
//							}
//								
//							p4.set(parser.highWayList[j].nodes3D[4 * k -4], parser.highWayList[j].nodes3D[4 * k -3]);
//							intersect = geo.getLineIntersection(p1, p2, p3, p4);
//						}
//						selectedWay->nodes3D[0] = intersect.x;
//						selectedWay->nodes3D[1] = intersect.y;
//
//
//						//CORRECT RIGHT POINT COORDINATE
//						p1.set(selectedWay->nodes3D[2], selectedWay->nodes3D[3]);
//						p2.set(selectedWay->nodes3D[6], selectedWay->nodes3D[7]);
//						p3.set(parser.highWayList[j].nodes3D[4 * k], parser.highWayList[j].nodes3D[4 * k + 1]);
//						p4.set(parser.highWayList[j].nodes3D[4 * k + 4], parser.highWayList[j].nodes3D[4 * k + 5]);
//						intersect = geo.getLineIntersection(p1, p2, p3, p4);
//						if (intersect.x == -1 && intersect.y == -1)
//						{
//							if (k == 0)
//							{
//								break;
//								//TODO : HANDLE THIS SITUATION
//							}
//							p4.set(parser.highWayList[j].nodes3D[4 * k -4], parser.highWayList[j].nodes3D[4 * k -3]);
//							intersect = geo.getLineIntersection(p1, p2, p3, p4);
//						}
//						selectedWay->nodes3D[2] = intersect.x;
//						selectedWay->nodes3D[3] = intersect.y;
//						break;
//					}
//
//
//				}
//				else continue;
//
//			}
//
//			//FOR ENDING POINT OF SELECTED WAY
//
//
//		}
//
//
//
//
//	}
//
//
//}

void sceneGenerator::generate3DRoads()
{
	for (int i = 0; i < parser.highWayList.size(); i++)
	{
		generateInitial3Dway(&parser.highWayList[i]);
		addIntersection1(&parser.highWayList[i]);
		addIntersection2(&parser.highWayList[i]);
		addIntersection3(&parser.highWayList[i]);
		generateTexCoords(&parser.highWayList[i]);
	}
}

//19 March Tex Coordinate Generator
void sceneGenerator::generateTexCoords(HighWay *way)
{
	const double TEXSIZE = 4.0f;
	double roadPos=0.0;

	//Start Point (i = 0)
	way->leftSideTexCoords.push_back(Tuple(0.0f, 0.0f));
	//Inter Points (1 <= i <= N)
	for (int i = 0; i < way->leftSideVertexes.size()-1; i++)
	{
		Triple fark = way->leftSideVertexes[i + 1] - way->leftSideVertexes[i];
		roadPos += fark.Length();
		way->leftSideTexCoords.push_back(Tuple(0.0f, roadPos/TEXSIZE));
	}

	roadPos = 0.0;
	//Start Point (i=0)
	way->rightSideTexCoords.push_back(Tuple(1.0f, 0.0f));
	for (int i = 0; i < way->rightSideVertexes.size() - 1; i++)
	{
		Triple fark = way->rightSideVertexes[i + 1] - way->rightSideVertexes[i];
		roadPos += fark.Length();
		way->rightSideTexCoords.push_back(Tuple(1.0f, roadPos / TEXSIZE));
	}
}


//[ALL INTERSECTIONS ARE DETECTED AND ADDED TO BUFFER  +%95 Work Correctly]
void sceneGenerator::addIntersection1(HighWay *way)
{
	GLfloat waySize = way->size;
	Geometry geo;

	for (int i = 0; i < (int)way->leftSideVertexes.size() - 1; i++)
	{
		Tuple pointLeft1 = Tuple(way->leftSideVertexes[i].x, way->leftSideVertexes[i].z);
		Tuple pointLeft2 = Tuple(way->leftSideVertexes[i+1].x, way->leftSideVertexes[i+1].z);

		Tuple pointRight1 = Tuple(way->rightSideVertexes[i].x, way->rightSideVertexes[i].z);
		Tuple pointRight2 = Tuple(way->rightSideVertexes[i + 1].x, way->rightSideVertexes[i + 1].z);

		int cnt = 0;
		int leftiterator = i+1;
		int rightiterator = i + 1;

		for (int k = terrain.left, z = 0; k <= terrain.right; k++, z++)
		{
			//Vertical Terrain Line
			Tuple pTop(terrain.BBox.meterTop, terrain.BBox.meterLeft + z * terrainLoader.heightMapInfo.sizeZ);
			Tuple pBottom(terrain.BBox.meterBottom, terrain.BBox.meterLeft + z * terrainLoader.heightMapInfo.sizeZ);

			//LEFT SIDE INTERSECTION
			Tuple intersectleft;
			Tuple intersectright;
			bool isleft = geo.getLineIntersection(&intersectleft, pTop, pBottom, pointLeft1, pointLeft2);
			bool isright = geo.getLineIntersection(&intersectright, pTop, pBottom, pointRight1, pointRight2);

			if (isleft && isright)
			{
				if (pointLeft1.y - pointLeft2.y < 0)
				{
					way->leftSideVertexes.insert(way->leftSideVertexes.begin() + leftiterator, Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
					way->leftIntersections.push_back(Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
					leftiterator++;
				}
				else
				{
					way->leftSideVertexes.insert(way->leftSideVertexes.begin() + leftiterator, Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
					way->leftIntersections.push_back(Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
				}

				if (pointRight1.y - pointRight2.y < 0)
				{
					way->rightSideVertexes.insert(way->rightSideVertexes.begin() + rightiterator, Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
					way->rightIntersections.push_back(Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
					rightiterator++;
				}
				else
				{
					way->rightSideVertexes.insert(way->rightSideVertexes.begin() + rightiterator, Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
					way->rightIntersections.push_back(Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
				}
				cnt++;
			}
			else if (isleft)
			{
				way->leftSideVertexes.insert(way->leftSideVertexes.begin() + leftiterator, Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
				way->leftIntersections.push_back(Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));

				double ratio = (pointLeft1 - intersectleft).length()/(pointLeft1 - pointLeft2).length();
				Tuple rightPointnew = pointRight1 + (pointRight2 - pointRight1) * ratio;
				way->rightSideVertexes.insert(way->rightSideVertexes.begin() + rightiterator, Triple(rightPointnew.x, terrainLoader.getTerrainHeight2(rightPointnew.x + parser.bbox.meterBottom, rightPointnew.y + parser.bbox.meterLeft), rightPointnew.y));
				way->rightIntersections.push_back(Triple(rightPointnew.x, terrainLoader.getTerrainHeight2(rightPointnew.x + parser.bbox.meterBottom, rightPointnew.y + parser.bbox.meterLeft), rightPointnew.y));

				if (pointLeft1.y - pointLeft2.y < 0)
				{
					leftiterator++;
					rightiterator++;
				}
				cnt++;

			}
			else if (isright)
			{

				way->rightSideVertexes.insert(way->rightSideVertexes.begin() + rightiterator, Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
				way->rightIntersections.push_back(Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));

				double ratio = (pointRight1 - intersectright).length() / (pointRight1 - pointRight2).length();
				Tuple leftPointnew = pointLeft1 + (pointLeft2 - pointLeft1) * ratio;
				way->leftSideVertexes.insert(way->leftSideVertexes.begin() + leftiterator, Triple(leftPointnew.x, terrainLoader.getTerrainHeight2(leftPointnew.x + parser.bbox.meterBottom, leftPointnew.y + parser.bbox.meterLeft), leftPointnew.y));
				way->leftIntersections.push_back(Triple(leftPointnew.x, terrainLoader.getTerrainHeight2(leftPointnew.x + parser.bbox.meterBottom, leftPointnew.y + parser.bbox.meterLeft), leftPointnew.y));

				if (pointRight1.y - pointRight2.y < 0)
				{
					rightiterator++;
					leftiterator++;
				}
				cnt++;
			}


		}

		i += cnt;
	
	}
}
void sceneGenerator::addIntersection2(HighWay *way)
{
	GLfloat waySize = way->size;
	Geometry geo;

	for (int i = 0; i < (int)way->leftSideVertexes.size() - 1; i++)
	{

		//LEFT SIDE
		Tuple pointLeft1 = Tuple(way->leftSideVertexes[i].x, way->leftSideVertexes[i].z);
		Tuple pointLeft2 = Tuple(way->leftSideVertexes[i + 1].x, way->leftSideVertexes[i + 1].z);
		//RIGHT SIDE
		Tuple pointRight1 = Tuple(way->rightSideVertexes[i].x, way->rightSideVertexes[i].z);
		Tuple pointRight2 = Tuple(way->rightSideVertexes[i + 1].x, way->rightSideVertexes[i + 1].z);
		int cnt = 0;
		int leftit = i + 1;
		int rightit = i + 1;

		for (int k = terrain.bottom, t = 0; k >= terrain.top; k--, t++)
		{
			//Horizontal Line
			Tuple pLeft(terrain.BBox.meterBottom + t * terrainLoader.heightMapInfo.sizeX, terrain.BBox.meterLeft);
			Tuple pRight(terrain.BBox.meterBottom + t * terrainLoader.heightMapInfo.sizeX, terrain.BBox.meterRight);

			
			Tuple intersectleft;
			Tuple intersectright;
			bool isleft = geo.getLineIntersection(&intersectleft, pLeft, pRight, pointLeft1, pointLeft2);
			bool isright = geo.getLineIntersection(&intersectright, pLeft, pRight, pointRight1, pointRight2);
			if (isleft && isright)
			{

				if (pointLeft1.x < pointLeft1.x)
				{
					way->leftSideVertexes.insert(way->leftSideVertexes.begin() + leftit, Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
					way->leftIntersections.push_back(Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
					leftit++;
				}
				else
				{
					way->leftSideVertexes.insert(way->leftSideVertexes.begin() + leftit, Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
					way->leftIntersections.push_back(Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
				}

				if (pointRight1.x < pointRight2.x)
				{
					way->rightSideVertexes.insert(way->rightSideVertexes.begin() + rightit, Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
					way->rightIntersections.push_back(Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
					rightit++;
				}
				else
				{
					way->rightSideVertexes.insert(way->rightSideVertexes.begin() + rightit, Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
					way->rightIntersections.push_back(Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
				}
				cnt++;
			}
			else if (isleft)
			{
				way->leftSideVertexes.insert(way->leftSideVertexes.begin() + leftit, Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
				way->leftIntersections.push_back(Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));

				double ratio = (pointLeft1 - intersectleft).length() / (pointLeft1 - pointLeft2).length();
				Tuple rightPointnew = pointRight1 + (pointRight2 - pointRight1) * ratio;
				way->rightSideVertexes.insert(way->rightSideVertexes.begin() + rightit, Triple(rightPointnew.x, terrainLoader.getTerrainHeight2(rightPointnew.x + parser.bbox.meterBottom, rightPointnew.y + parser.bbox.meterLeft), rightPointnew.y));
				way->rightIntersections.push_back(Triple(rightPointnew.x, terrainLoader.getTerrainHeight2(rightPointnew.x + parser.bbox.meterBottom, rightPointnew.y + parser.bbox.meterLeft), rightPointnew.y));

				if (pointLeft1.x - pointLeft2.x < 0)
				{
					leftit++;
					rightit++;
				}
				cnt++;
			}
			else if (isright)
			{

				way->rightSideVertexes.insert(way->rightSideVertexes.begin() + rightit, Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
				way->rightIntersections.push_back(Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));

				double ratio = (pointRight1 - intersectright).length() / (pointRight1 - pointRight2).length();
				Tuple leftPointnew = pointLeft1 + (pointLeft2 - pointLeft1) * ratio;
				way->leftSideVertexes.insert(way->leftSideVertexes.begin() + leftit, Triple(leftPointnew.x, terrainLoader.getTerrainHeight2(leftPointnew.x + parser.bbox.meterBottom, leftPointnew.y + parser.bbox.meterLeft), leftPointnew.y));
				way->leftIntersections.push_back(Triple(leftPointnew.x, terrainLoader.getTerrainHeight2(leftPointnew.x + parser.bbox.meterBottom, leftPointnew.y + parser.bbox.meterLeft), leftPointnew.y));

				if (pointRight1.x - pointRight2.x < 0)
				{
					rightit++;
					leftit++;
				}
				cnt++;
			}


		}
		i += cnt;
	}
}
void sceneGenerator::addIntersection3(HighWay *way)
{
	GLfloat waySize = way->size;
	Geometry geo;

	for (int i = 0; i < (int)way->leftSideVertexes.size() - 1; i++)
	{

		//LEFT SIDE
		Tuple pointLeft1 = Tuple(way->leftSideVertexes[i].x, way->leftSideVertexes[i].z);
		Tuple pointLeft2 = Tuple(way->leftSideVertexes[i + 1].x, way->leftSideVertexes[i + 1].z);

		//RIGHT SIDE
		Tuple pointRight1 = Tuple(way->rightSideVertexes[i].x, way->rightSideVertexes[i].z);
		Tuple pointRight2 = Tuple(way->rightSideVertexes[i + 1].x, way->rightSideVertexes[i + 1].z);

		int cnt = 0;

		for (int k = terrain.left, t = 1; k <= terrain.right + (terrain.bottom - terrain.top); k++, t++)
		{
			//Diagonal Line
			Tuple pBottom;
			Tuple pTop;
			if (k < terrain.right)
				pTop = Tuple(terrain.BBox.meterTop, terrain.BBox.meterLeft + t *terrainLoader.heightMapInfo.sizeZ);
			else
				pTop = Tuple(terrain.BBox.meterTop - (t - (terrain.right - terrain.left)) * terrainLoader.heightMapInfo.sizeX, terrain.BBox.meterRight);

			if (t <= (terrain.bottom - terrain.top))
				pBottom = Tuple(terrain.BBox.meterTop - t*terrainLoader.heightMapInfo.sizeX, terrain.BBox.meterLeft);
			else
				pBottom = Tuple(terrain.BBox.meterBottom, terrain.BBox.meterLeft + (t - (terrain.bottom - terrain.top)) * terrainLoader.heightMapInfo.sizeZ);

			//LEFT SIDE INTERSECTION
			Tuple intersectleft;
			//RIGHT SIDE INTERSECTION
			Tuple intersectright;

			bool isLeft = geo.getLineIntersection(&intersectleft, pTop, pBottom, pointLeft1, pointLeft2);
			bool isRight = geo.getLineIntersection(&intersectright, pTop, pBottom, pointRight1, pointRight2);

			if (isLeft && isRight)
			{
				way->leftSideVertexes.insert(way->leftSideVertexes.begin() + i + 1, Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
				way->leftIntersections.push_back(Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
				
				way->rightSideVertexes.insert(way->rightSideVertexes.begin() + i + 1, Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
				way->rightIntersections.push_back(Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
				cnt++;
			}
			else if (isLeft)
			{
				way->leftSideVertexes.insert(way->leftSideVertexes.begin() + i + 1, Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));
				way->leftIntersections.push_back(Triple(intersectleft.x, terrainLoader.getTerrainHeight2(intersectleft.x + parser.bbox.meterBottom, intersectleft.y + parser.bbox.meterLeft), intersectleft.y));

				double ratio = (pointLeft1-intersectleft).length()/(pointLeft1 - pointLeft2).length();
				Tuple rightPointnew = pointRight1 + (pointRight2 - pointRight1) * ratio;
				way->rightSideVertexes.insert(way->rightSideVertexes.begin() + i + 1, Triple(rightPointnew.x, terrainLoader.getTerrainHeight2(rightPointnew.x + parser.bbox.meterBottom, rightPointnew.y + parser.bbox.meterLeft), rightPointnew.y));
				way->rightIntersections.push_back(Triple(rightPointnew.x, terrainLoader.getTerrainHeight2(rightPointnew.x + parser.bbox.meterBottom, rightPointnew.y + parser.bbox.meterLeft), rightPointnew.y));
				cnt++;
			}

			else if (isRight)
			{
				way->rightSideVertexes.insert(way->rightSideVertexes.begin() + i + 1, Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));
				way->rightIntersections.push_back(Triple(intersectright.x, terrainLoader.getTerrainHeight2(intersectright.x + parser.bbox.meterBottom, intersectright.y + parser.bbox.meterLeft), intersectright.y));

				double ratio = (pointRight1 - intersectright).length() / (pointRight1 - pointRight2).length();
				Tuple leftPointnew = pointLeft1 + (pointLeft2 - pointLeft1) * ratio;
				way->leftSideVertexes.insert(way->leftSideVertexes.begin() + i + 1, Triple(leftPointnew.x, terrainLoader.getTerrainHeight2(leftPointnew.x + parser.bbox.meterBottom, leftPointnew.y + parser.bbox.meterLeft), leftPointnew.y));
				way->leftIntersections.push_back(Triple(leftPointnew.x, terrainLoader.getTerrainHeight2(leftPointnew.x + parser.bbox.meterBottom, leftPointnew.y + parser.bbox.meterLeft), leftPointnew.y));
				cnt++;
			}
		}
		i += cnt;
	}
}

//17 Mart Version 3 (For Triangle Terrain)
//This will be used for generating initial vertexes
void sceneGenerator::generateInitial3Dway(HighWay *way)
{
	GLfloat waySize = way->size;
	Geometry geo;

	if (way->nodes.size() == 2)
	{
		Triple up(0, 1, 0);
		Triple forward = Triple((*way).nodes[1].meterx, 0, (*way).nodes[1].meterz) - Triple((*way).nodes[0].meterx, 0, (*way).nodes[0].meterz);
		Triple right = forward * up;
		right.normalize();
		Triple left(-right.x, -right.y, -right.z);

		//LEFT SIDE
		Tuple pointLeft1 = Tuple(way->nodes[0].meterx, way->nodes[0].meterz) + Tuple(left.x, left.z) * (waySize / 2.0f);
		Tuple pointLeft2 = Tuple(way->nodes[1].meterx, way->nodes[1].meterz) + Tuple(left.x, left.z) * (waySize / 2.0f);
		way->leftSideVertexes.push_back(Triple(pointLeft1.x, terrainLoader.getTerrainHeight2(pointLeft1.x + parser.bbox.meterBottom, pointLeft1.y + parser.bbox.meterLeft), pointLeft1.y));
		way->leftSideVertexes.push_back(Triple(pointLeft2.x, terrainLoader.getTerrainHeight2(pointLeft2.x + parser.bbox.meterBottom, pointLeft2.y + parser.bbox.meterLeft), pointLeft2.y));

		//RIGHT SIDE
		Tuple pointRight1 = Tuple(way->nodes[0].meterx, way->nodes[0].meterz) + Tuple(right.x, right.z) * (waySize / 2.0f);
		Tuple pointRight2 = Tuple(way->nodes[1].meterx, way->nodes[1].meterz) + Tuple(right.x, right.z) * (waySize / 2.0f);
		way->rightSideVertexes.push_back(Triple(pointRight1.x, terrainLoader.getTerrainHeight2(pointRight1.x + parser.bbox.meterBottom, pointRight1.y + parser.bbox.meterLeft), pointRight1.y));
		way->rightSideVertexes.push_back(Triple(pointRight2.x, terrainLoader.getTerrainHeight2(pointRight2.x + parser.bbox.meterBottom, pointRight2.y + parser.bbox.meterLeft), pointRight2.y));
	}
	else
	{
		for (int i = 0; i < way->nodes.size() - 2; i++)
		{
			Triple up(0, 1, 0);
			Triple forward1 = Triple((*way).nodes[i + 1].meterx, 0, (*way).nodes[i + 1].meterz) - Triple((*way).nodes[i].meterx, 0, (*way).nodes[i].meterz);
			Triple right1 = forward1 * up;
			right1.normalize();
			Triple left1(-right1.x, -right1.y, -right1.z);

			Triple forward2 = Triple((*way).nodes[i + 2].meterx, 0, (*way).nodes[i + 2].meterz) - Triple((*way).nodes[i + 1].meterx, 0, (*way).nodes[i + 1].meterz);
			Triple right2 = forward2 * up;
			right2.normalize();
			Triple left2(-right2.x, -right2.y, -right2.z);

			double A1, B1, C1;
			double A2, B2, C2;

			//INITIAL POINTS ARE ADDED TO NODES3D
			if (i == 0)
			{
				Tuple pointLeft1 = Tuple(way->nodes[i].meterx, way->nodes[i].meterz) + Tuple(left1.x, left1.z) * (waySize / 2.0f);
				way->leftSideVertexes.push_back(Triple(pointLeft1.x, terrainLoader.getTerrainHeight2(pointLeft1.x + parser.bbox.meterBottom, pointLeft1.y + parser.bbox.meterLeft), pointLeft1.y));

				Tuple pointRight1 = Tuple(way->nodes[i].meterx, way->nodes[i].meterz) + Tuple(right1.x, right1.z) * (waySize / 2.0f);
				way->rightSideVertexes.push_back(Triple(pointRight1.x, terrainLoader.getTerrainHeight2(pointRight1.x + parser.bbox.meterBottom, pointRight1.y + parser.bbox.meterLeft), pointRight1.y));
			}

			//1. line left
			Tuple p0 = Tuple(way->nodes[i].meterx, way->nodes[i].meterz) + Tuple(left1.x, left1.z) * (waySize / 2.0f);
			Tuple p1 = Tuple(way->nodes[i + 1].meterx, way->nodes[i + 1].meterz) + Tuple(left1.x, left1.z) * (waySize / 2.0f);

			//2. line left
			Tuple p2 = Tuple(way->nodes[i + 1].meterx, way->nodes[i + 1].meterz) + Tuple(left2.x, left2.z) * (waySize / 2.0f);
			Tuple p3 = Tuple(way->nodes[i + 2].meterx, way->nodes[i + 2].meterz) + Tuple(left2.x, left2.z) * (waySize / 2.0f);

			Tuple iL; //intersection Left
			if(!geo.getInfiniteLineIntersection(&iL, p0, p1, p2, p3))
				way->leftSideVertexes.push_back(Triple(p1.x, terrainLoader.getTerrainHeight2(p1.x + parser.bbox.meterBottom, p1.y + parser.bbox.meterLeft), p1.y));
			else
				way->leftSideVertexes.push_back(Triple(iL.x, terrainLoader.getTerrainHeight2(iL.x + parser.bbox.meterBottom, iL.y + parser.bbox.meterLeft), iL.y));
			

			//1. line right
			p0 = Tuple(way->nodes[i].meterx, way->nodes[i].meterz) + Tuple(right1.x, right1.z) * (waySize / 2.0f);
			p1 = Tuple(way->nodes[i + 1].meterx, way->nodes[i + 1].meterz) + Tuple(right1.x, right1.z) * (waySize / 2.0f);

			//2. line right
			p2 = Tuple(way->nodes[i + 1].meterx, way->nodes[i + 1].meterz) + Tuple(right2.x, right2.z) * (waySize / 2.0f);
			p3 = Tuple(way->nodes[i + 2].meterx, way->nodes[i + 2].meterz) + Tuple(right2.x, right2.z) * (waySize / 2.0f);

			Tuple iR; //intersection Left
			if (!geo.getInfiniteLineIntersection(&iR, p0, p1, p2, p3))
				way->rightSideVertexes.push_back(Triple(p1.x, terrainLoader.getTerrainHeight2(p1.x + parser.bbox.meterBottom, p1.y + parser.bbox.meterLeft), p1.y));
			else
				way->rightSideVertexes.push_back(Triple(iR.x, terrainLoader.getTerrainHeight2(iR.x + parser.bbox.meterBottom, iR.y + parser.bbox.meterLeft), iR.y));


			//ENDING POINTS ARE ADDEDD TO NODES3D
			if (i == ((*way).nodes.size() - 3))
			{
				Tuple pointLeft1 = Tuple(way->nodes[i + 2].meterx, way->nodes[i + 2].meterz) + Tuple(left2.x, left2.z) * (waySize / 2);
				way->leftSideVertexes.push_back(Triple(pointLeft1.x, terrainLoader.getTerrainHeight2(pointLeft1.x + parser.bbox.meterBottom, pointLeft1.y + parser.bbox.meterLeft), pointLeft1.y));

				Tuple pointRight1 = Tuple(way->nodes[i + 2].meterx, way->nodes[i + 2].meterz) + Tuple(right2.x, right2.z) * (waySize / 2);
				way->rightSideVertexes.push_back(Triple(pointRight1.x, terrainLoader.getTerrainHeight2(pointRight1.x + parser.bbox.meterBottom, pointRight1.y + parser.bbox.meterLeft), pointRight1.y));
				return;
			}
		}
	}



}



//[ON HOLD]
bool isIntersection(vector<Triple> *list, Triple X)
{
	for (int i = 0; i < (*list).size(); i++)
	{
		if ( ((*list)[i].x == X.x) && ((*list)[i].y == X.y)  && ((*list)[i].z == X.z))
			return true;
	}

	return false;
}
//19 March CREATE VBO FOR DRAWING
void sceneGenerator::generateVBO(HighWay *way)
{
	//We will tesselate the road by triangles

	int i = 0, j = 0;

	while (1)
	{
		way->textureBuffer.push_back(way->leftSideTexCoords[i]);
		way->vertexBuffer.push_back(way->leftSideVertexes[i]);
		way->textureBuffer.push_back(way->rightSideTexCoords[j]);
		way->vertexBuffer.push_back(way->rightSideVertexes[j]);		
		way->textureBuffer.push_back(way->rightSideTexCoords[j+1]);
		way->vertexBuffer.push_back(way->rightSideVertexes[j+1]);

		way->textureBuffer.push_back(way->rightSideTexCoords[j+1]);
		way->vertexBuffer.push_back(way->rightSideVertexes[j+1]);
		way->textureBuffer.push_back(way->leftSideTexCoords[i+1]);
		way->vertexBuffer.push_back(way->leftSideVertexes[i+1]);
		way->textureBuffer.push_back(way->leftSideTexCoords[i]);
		way->vertexBuffer.push_back(way->leftSideVertexes[i]);

		i++;
		j++;
	}


}

