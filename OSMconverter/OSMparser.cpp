#include <iostream>
#import <msxml3.dll>
#include "OSMparser.h"
#include "geoTools.h"
#include "gltools.h"
#include <math.h>

using namespace MSXML2;
using namespace std;


//Function to throw an exception when creating COM object: 
inline void OSMparser::TESTHR(HRESULT _hr)
{
	if FAILED(_hr)
		throw(_hr);
}

Node OSMparser::findnode(vector<Node> *nodelist,int listSize, BSTR id)
{
	//Node toret;
	bool flag = false;
	for (int i = 0; i < listSize; i++)
	{
		if (0 == wcscmp(id, (*nodelist)[i].id))
		{
			return (*nodelist)[i];
		}
	}
	if (flag == false)
		cout << "Couldn't find the node" << endl;
	return Node();
}

Way OSMparser::findWay(vector<Way> *waylist, int listSize, BSTR id)
{
	bool flag = false;
	for (int i = 0; i < listSize; i++)
	{
		if (0 == wcscmp(id, (*waylist)[i].id))
		{
			return (*waylist)[i];
		}
	}
	if (flag == false)
		cout << "Couldn't find the Way" << endl;
	Way notExist;
	notExist.id = L"NOTEXISTS";
	return notExist;



}

wayType OSMparser::assignWayType(Way* way)
{

	 for (int i = 0; i < (*way).tags.size(); i++)
	 {
		 if (0 == wcscmp((*way).tags[i].k, L"area"))
		 {
			 (*way).isArea = true;
			 continue;
		 }

		 if (0 == wcscmp((*way).tags[i].k, L"building"))
		 {
			 return wayType::building;
		 }

		 if (0 == wcscmp((*way).tags[i].k, L"historic"))
		 {
			 if (0 == wcscmp((*way).tags[i].v, L"monument"))
				 (*way).historic = historic_monument;
			 else if (0 == wcscmp((*way).tags[i].v, L"citywalls"))
				 (*way).historic = historic_cityWalls;
			 else if (0 == wcscmp((*way).tags[i].v, L"city_gate"))
				 (*way).historic = historic_cityGate;

			return wayType::historic;
		 }

		 if (0 == wcscmp((*way).tags[i].k, L"barrier"))
		 {
			 if (0 == wcscmp((*way).tags[i].v, L"wall"))
				 (*way).barrier = barrier_wall;
			 else if (0 == wcscmp((*way).tags[i].v, L"fence"))
				 (*way).barrier = barrier_fence;
			 else if (0 == wcscmp((*way).tags[i].v, L"city_wall"))
				 (*way).barrier = barrier_cityWall;	

			 continue;
		 }



		 if (0 == wcscmp((*way).tags[i].k, L"highway"))
		 {
			 if (0 == wcscmp((*way).tags[i].v, L"residential"))
				 return wayType::highwayResidual;
			 if (0 == wcscmp((*way).tags[i].v, L"unclassified"))
				 return wayType::highwayUnclassified;
			 if (0 == wcscmp((*way).tags[i].v, L"service"))
				 return wayType::highwayService;
			 if (0 == wcscmp((*way).tags[i].v, L"primary"))
				 return wayType::highwayPrimary;
			 if (0 == wcscmp((*way).tags[i].v, L"secondary"))
				 return wayType::highwaySecondary;
			 if (0 == wcscmp((*way).tags[i].v, L"tertiary"))
				 return wayType::highwayTertiary;
			 if (0 == wcscmp((*way).tags[i].v, L"pedestrian"))
				 return wayType::highwayPedestrian;
			 if (0 == wcscmp((*way).tags[i].v, L"footway"))
				 return wayType::highwayFootway;
			 if (0 == wcscmp((*way).tags[i].v, L"path"))
				 return wayType::highwayPath;
		 }



		 if (0 == wcscmp((*way).tags[i].k, L"railway"))
		 {
			 return wayType::railway;
		 }

		 if (0 == wcscmp((*way).tags[i].k, L"waterway") && 0 == wcscmp((*way).tags[i].v, L"river"))
		 {
			 for (int j = 0; j < (*way).tags.size(); j++)
			 {
				 if (0 == wcscmp((*way).tags[j].k, L"tunnel")) //It is underground river so we don't draw it
					 return wayType::unknown;
			 }
			 return wayType::river;
		 }

		 if (0 == wcscmp((*way).tags[i].k, L"landuse"))
		 {
			 if (0 == wcscmp((*way).tags[i].v, L"forest"))
				return wayType::landuseForest;
			 if (0 == wcscmp((*way).tags[i].v, L"grass"))
				 return wayType::landuseGrass;
			 if (0 == wcscmp((*way).tags[i].v, L"meadow"))
				 return wayType::landuseMeadow;
		 }

		 if (0 == wcscmp((*way).tags[i].k, L"amenity"))
		 {
			 for (int j = 0; j < (*way).tags.size(); j++)
			 {
				 if (0 == wcscmp((*way).tags[j].k, L"building"))
				 {
					 return wayType::building;
				 }
			 }
			
			 if (0 == wcscmp((*way).tags[i].v, L"parking"))
				 return wayType::amenityParking;
			 if (0 == wcscmp((*way).tags[i].v, L"motorcycle_parking"))
				 return wayType::amenityMotorCycleParking;

			 return wayType::amenity;			 
		 }

		 if (0 == wcscmp((*way).tags[i].k, L"leisure"))
			 return wayType::leisure;

	 }

	//Default case if there is no tag assume the closed shape is building type
	 //if ((*way).nodes.front().id == (*way).nodes.back().id && (*way).tags.size() == 0)
		// return wayType::building;
	 //else
		 return wayType::unknown;

}

OSMparser::enumNode OSMparser::getNodeEnumeration(BSTR inString)
{

	if (0 == wcscmp(inString,L"node"))
		return OSMparser::eNode;
	else if (0 == wcscmp(inString,L"way"))
		return OSMparser::eWay;
	else if (0 == wcscmp(inString, L"relation"))
		return OSMparser::eRelation;
	else if (0 == wcscmp(inString, L"bounds"))
		return OSMparser::eBbox;
	else
		return OSMparser::undefined;
}

void OSMparser::readNodeType(MSXML2::IXMLDOMNode* node)
{
	MSXML2::IXMLDOMNodeList *childList = NULL;//node list containing the child nodes
	MSXML2::IXMLDOMNamedNodeMapPtr DOMNamedNodeMapPtr;//iteration through the collection of attribute nodes
	MSXML2::IXMLDOMNode *pIAttrNode = NULL;//pointer to attribute node
	MSXML2::IXMLDOMNode *tagnode = NULL;
	HRESULT hResult;		 
	BSTR bstrAttrName, bstrAttrText; //Variables to store attribute's name and text

	Node newnode; //node to add our list
	MercatorProjection mercatorCalculator; // To calculate lat-lon to meter 

	
	//GET THE ATTRIBUTES:
	int j = 0;//loop-index variable
	long length;// number of attributes in the collection

	DOMNamedNodeMapPtr = node->attributes;
	hResult = DOMNamedNodeMapPtr->get_length(&length);

	if (SUCCEEDED(hResult))
	{
		//Loop through the number of attributes:
		for (j = 0; j < length; j++)
		{
									
			DOMNamedNodeMapPtr->get_item(j, &pIAttrNode); //get attribute	
			pIAttrNode->get_nodeName(&bstrAttrName); //get attribute name
			pIAttrNode->get_text(&bstrAttrText); //get attribute value

			if (0 == wcscmp(bstrAttrName, L"id"))
				newnode.id = bstrAttrText;
			if (0 == wcscmp(bstrAttrName, L"lat"))
				newnode.lat = wcstod(bstrAttrText, NULL);
			if (0 == wcscmp(bstrAttrName, L"lon"))
				newnode.lon =  wcstod(bstrAttrText,NULL);

		}


		coordinate coord = mercatorCalculator.LatLontoMeters(newnode.lat, newnode.lon);
		newnode.meterx = (coord.meterx - bbox.meterBottom);
		newnode.meterz = (coord.meterz - bbox.meterLeft);

	}

	//GET TAGS
	node->get_childNodes(&childList); // Get the child nodes into childList
	hResult = childList->get_length(&length);

	if (SUCCEEDED(hResult))
	{
		for (j = 0; j < length; j++)
		{
			Tag newtag;
			childList->get_item(j, &tagnode);
			DOMNamedNodeMapPtr = tagnode->attributes;
			DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get k attribute	
			pIAttrNode->get_text(&bstrAttrText); //get k value
			newtag.k = bstrAttrText;
			
			DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get v attribute	
			pIAttrNode->get_text(&bstrAttrText); //get v value
			newtag.v = bstrAttrText;

			newnode.tags.push_back(newtag);
		}


	}
	nodeList.push_back(newnode);
}

void OSMparser::readWayType(MSXML2::IXMLDOMNode* node)
{

	MSXML2::IXMLDOMNodeList *childList = NULL;//node list containing the child nodes
	MSXML2::IXMLDOMNamedNodeMapPtr DOMNamedNodeMapPtr;//iteration through the collection of attribute nodes
	MSXML2::IXMLDOMNode *pIAttrNode = NULL;//pointer to attribute node
	MSXML2::IXMLDOMNode *tagnode = NULL;
	HRESULT hResult;
	BSTR bstrAttrName, bstrAttrText; //Variables to store attribute's name and text
	BSTR bstrNodeName;

	Way newWay;
	Building building;
	HighWay highway;

	//GET THE ATTRIBUTES:
	int j = 0;//loop-index variable
	long length;// number of attributes in the collection

	DOMNamedNodeMapPtr = node->attributes;
	hResult = DOMNamedNodeMapPtr->get_length(&length);

	if (SUCCEEDED(hResult))
	{
		//Loop through the number of attributes:
		for (j = 0; j < length; j++)
		{

			DOMNamedNodeMapPtr->get_item(j, &pIAttrNode); //get attribute	
			pIAttrNode->get_nodeName(&bstrAttrName); //get attribute name
			pIAttrNode->get_text(&bstrAttrText); //get attribute value
			if (0 == wcscmp(bstrAttrName, L"id"))
				newWay.id = bstrAttrText;
		}
	}

	//GET TAGS AND NDS
	node->get_childNodes(&childList); // Get the child nodes into childList
	hResult = childList->get_length(&length);

	if (SUCCEEDED(hResult))
	{
		for (j = 0; j < length; j++)
		{
			
			childList->get_item(j, &tagnode);
			tagnode->get_nodeName(&bstrNodeName);

			if (0 == wcscmp(bstrNodeName, L"nd"))
			{
				Node newnd;
				DOMNamedNodeMapPtr = tagnode->attributes;
				DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute	
				pIAttrNode->get_text(&bstrAttrText); //get ref value

				newnd = findnode(&nodeList, nodeList.size(), bstrAttrText);
				newWay.nodes.push_back(newnd);
			}

			if (0 == wcscmp(bstrNodeName, L"tag"))
			{
				Tag newtag;
				DOMNamedNodeMapPtr = tagnode->attributes;
				DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get k attribute	
				pIAttrNode->get_text(&bstrAttrText); //get k value
				newtag.k = bstrAttrText;

				DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get v attribute	
				pIAttrNode->get_text(&bstrAttrText); //get v value
				newtag.v = bstrAttrText;

				newWay.tags.push_back(newtag);
			}

		}


	}


	newWay.type = assignWayType(&newWay);
	switch (newWay.type)
	{

	case wayType::building:
		building.id = newWay.id;
		for (int i = 0; i < newWay.nodes.size(); i++)
		{
			building.nodes.push_back(newWay.nodes[i]);
		}
		for (int i = 0; i < newWay.tags.size(); i++)
			building.tags.push_back(newWay.tags[i]);
		building.BuildingHeight = Buildingheight;

		building.TextureID = rand() % 3 + 1;
		buildingList.push_back(building);
		break;
//*----------------------------------------------------------------------------	
	case wayType::highwayPedestrian:
	//case wayType::highwayFootway:
	case wayType::highwayPrimary:
	case wayType::highwayResidual:
	case wayType::highwaySecondary:
	case wayType::highwayService:
	case wayType::highwayTertiary:
	case wayType::highwayUnclassified:
	case wayType::highwayPath:
	case wayType::river:
	case wayType::railway:
		highway.type = newWay.type;
		switch (highway.type)
		{

		case wayType::highwayResidual:
			highway.size = HighwayResidualSize;
			break;
		case wayType::highwayUnclassified:
			highway.size = HighwayUnclassifiedSize;
			break;
		case wayType::highwayService:
			highway.size = HighwayServiceSize;
			break;
		case wayType::highwayPrimary:
			highway.size = HighwayPrimarySize;
			break;
		case wayType::highwaySecondary:
			highway.size = HighwaySecondarySize;
			break;
		case wayType::highwayTertiary:
			highway.size = HighwayTertiarySize;
			break;
		case wayType::highwayPedestrian:
			highway.size = HighwayPavementSize;
			break;
		case wayType::highwayFootway:
			highway.size = HighwayFootwaySize;
			break;
		case wayType::highwayPath:
			highway.size = HighwayPathSize;
			break;
		case wayType::railway:
			highway.size = RailwaySize;
			break;
		case wayType::river:
			highway.size = RiverSize;
			break;
		}

		highway.id = newWay.id;	
		for (int i = 0; i < newWay.nodes.size(); i++)
			highway.nodes.push_back(newWay.nodes[i]);
		for (int i = 0; i < newWay.tags.size(); i++)
			highway.tags.push_back(newWay.tags[i]);
		highWayList.push_back(highway);
		wayList.push_back(newWay);
		break;
	default :
		wayList.push_back(newWay);
	}






}

void OSMparser::readRelationType(MSXML2::IXMLDOMNode* node)
{
	MSXML2::IXMLDOMNodeList *childList = NULL;//node list containing the child nodes
	MSXML2::IXMLDOMNamedNodeMapPtr DOMNamedNodeMapPtr;//iteration through the collection of attribute nodes
	MSXML2::IXMLDOMNode *pIAttrNode = NULL;//pointer to attribute node
	MSXML2::IXMLDOMNode *tagnode = NULL;
	HRESULT hResult;
	BSTR bstrAttrName, bstrAttrText; //Variables to store attribute's name and text
	BSTR bstrNodeName;

	Relation newRelation; //relation to add our list

	//GET THE ATTRIBUTES:
	long length;// number of attributes in the collection

	DOMNamedNodeMapPtr = node->attributes;
	hResult = DOMNamedNodeMapPtr->get_length(&length);

	if (SUCCEEDED(hResult))
	{
		//Loop through the number of attributes:
		for (int j = 0; j < length; j++)
		{

			DOMNamedNodeMapPtr->get_item(j, &pIAttrNode); //get attribute	
			pIAttrNode->get_nodeName(&bstrAttrName); //get attribute name
			pIAttrNode->get_text(&bstrAttrText); //get attribute value
			if (0 == wcscmp(bstrAttrName, L"id"))
			{
				newRelation.id = bstrAttrText;
				break;
			}
		}
	}

	//GET TAGS
	node->get_childNodes(&childList); // Get the child nodes into childList
	hResult = childList->get_length(&length);

	if (SUCCEEDED(hResult))
	{
		for (int j = 0; j < length; j++)
		{

			childList->get_item(j, &tagnode);
			tagnode->get_nodeName(&bstrNodeName);

			if (0 == wcscmp(bstrNodeName, L"member"))
			{
				Member newMember;
				DOMNamedNodeMapPtr = tagnode->attributes;
				DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get type attribute	
				pIAttrNode->get_text(&bstrAttrText); //get type value
				newMember.type = bstrAttrText;
				DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
				pIAttrNode->get_text(&bstrAttrText); //get ref value
				newMember.ref = bstrAttrText;
				DOMNamedNodeMapPtr->get_item(2, &pIAttrNode); //get role attribute	
				pIAttrNode->get_text(&bstrAttrText); //get role value
				newMember.role = bstrAttrText;
				
				newRelation.members.push_back(newMember);
			}

			if (0 == wcscmp(bstrNodeName, L"tag"))
			{
				Tag newtag;
				DOMNamedNodeMapPtr = tagnode->attributes;
				DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get k attribute	
				pIAttrNode->get_text(&bstrAttrText); //get k value
				newtag.k = bstrAttrText;

				DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get v attribute	
				pIAttrNode->get_text(&bstrAttrText); //get v value
				newtag.v = bstrAttrText;

				newRelation.tags.push_back(newtag);
			}

		}


	}

	relationList.push_back(newRelation);



}

void OSMparser::readBBoxType(MSXML2::IXMLDOMNode* node)
{
	MSXML2::IXMLDOMNamedNodeMapPtr DOMNamedNodeMapPtr;//iteration through the collection of attribute nodes
	MSXML2::IXMLDOMNode *pIAttrNode = NULL;//pointer to attribute node
	HRESULT hResult;
	BSTR bstrAttrName, bstrAttrText; //Variables to store attribute's name and text
	MercatorProjection mercatorCalculator; // To calculate lat-lon to meter 

	//Get the attributes:
	int j = 0;//loop-index variable
	long length;// number of attributes in the collection

	DOMNamedNodeMapPtr = node->attributes;

	hResult = DOMNamedNodeMapPtr->get_length(&length);

	if (SUCCEEDED(hResult))
	{
		//Loop through the number of attributes:
		for (j = 0; j < length; j++)
		{
			//get attribute node:							
			DOMNamedNodeMapPtr->get_item(j, &pIAttrNode);
			pIAttrNode->get_nodeName(&bstrAttrName);
			pIAttrNode->get_text(&bstrAttrText);
			if (0 == wcscmp(bstrAttrName, L"minlat"))
				bbox.bottom = wcstod(bstrAttrText, NULL);
		    if (0 == wcscmp(bstrAttrName, L"minlon"))
				bbox.left = wcstod(bstrAttrText, NULL);
			if (0 == wcscmp(bstrAttrName, L"maxlat"))
				bbox.top = wcstod(bstrAttrText, NULL);
			if (0 == wcscmp(bstrAttrName, L"maxlon"))
				bbox.right = wcstod(bstrAttrText, NULL);
		}

		coordinate coord = mercatorCalculator.LatLontoMeters(bbox.bottom, bbox.left);
		bbox.meterBottom = coord.meterx;
		bbox.meterLeft = coord.meterz;
		coord =  mercatorCalculator.LatLontoMeters(bbox.top, bbox.right);
		bbox.meterTop = coord.meterx;
		bbox.meterRight = coord.meterz;

		cout << "minlat :" << bbox.bottom << "  minlon:" << bbox.left << "  maxlat:" << bbox.top << "  maxlon:" << bbox.right << endl;

	}


}

void OSMparser::parseOSM(char *szFileName)
{

	try
	{
		//Qualify namespase explicitly to avoid Compiler Error C2872 "ambiguous symbol" during linking.
		//Now Msxml2.dll use the "MSXML2" namespace
		//(see http://support.microsoft.com/default.aspx?scid=kb;en-us;316317):
		MSXML2::IXMLDOMDocumentPtr docPtr;//pointer to DOMDocument object
		MSXML2::IXMLDOMNodeListPtr NodeListPtr;//indexed access. and iteration through the collection of nodes
		MSXML2::IXMLDOMNodePtr DOMNodePtr;//pointer to the node

		MSXML2::IXMLDOMNode *pIDOMNode = NULL;//pointer to element's node


		//Variable with the name of node to find: 
		BSTR strFindText = L" ";//" " means to output every node

		//Variables to store item's name, parent, text and node type:
		BSTR bstrItemText, bstrItemNode, bstrNodeType;


		int i = 0;//loop-index variable

		//Initialize COM Library:
		CoInitialize(NULL);

		//Create an instance of the DOMDocument object:
		docPtr.CreateInstance(__uuidof(DOMDocument30));

		// Load a document:
		_variant_t varXml(szFileName);//XML file to load
		_variant_t varResult((bool)TRUE);//result 

		varResult = docPtr->load(varXml);

		if ((bool)varResult == FALSE)
		{
			printf("*** Error:failed to load XML file. ***\n");
			return;
		}

		//Collect all or selected nodes by tag name:
		NodeListPtr = docPtr->getElementsByTagName(strFindText);

		//Output root node:
		docPtr->documentElement->get_nodeName(&bstrItemText);

		

		for (i = 0; i < (NodeListPtr->length); i++)
		{

			// RATIO OF LOADING
			if (i % 500 == 0)
				cout << "Completed :  %" << ((float)i *100)/NodeListPtr->length << endl;

			if (pIDOMNode) pIDOMNode->Release();
			NodeListPtr->get_item(i, &pIDOMNode);


			if (pIDOMNode)
			{

				pIDOMNode->get_nodeTypeString(&bstrNodeType); 
				BSTR temp = L"element";

				if (lstrcmp((LPCTSTR)bstrNodeType, (LPCTSTR)temp) == 0)
				{
					
					pIDOMNode->get_nodeName(&bstrItemNode); // Get the Element Name Here 

					switch (getNodeEnumeration(bstrItemNode))
					{
					case OSMparser::eBbox :
						readBBoxType(pIDOMNode);
						break;

					case OSMparser::eNode :
						readNodeType(pIDOMNode);
						break;

					case OSMparser::eWay :
						readWayType(pIDOMNode);
						break;

					case OSMparser::eRelation :
						readRelationType(pIDOMNode);
						break;

					default:
						;//cout << "Different TAG" << endl;

					}	

				}

			}
		}
		//Do not forget to release interfaces:
		pIDOMNode->Release();
		pIDOMNode = NULL;

	}

	catch (...)
	{
		cout << "exception occur" << endl;
	}


	CoUninitialize();

}
