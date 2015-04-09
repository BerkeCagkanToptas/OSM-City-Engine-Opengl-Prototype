#include "configParser.h"
#include <iostream>
#import <msxml3.dll>

using namespace MSXML2;
using namespace std;


//Function to throw an exception when creating COM object: 
inline void TESTHR(HRESULT _hr)
{
	if FAILED(_hr)
		throw(_hr);
}

std::string ConvertWCSToMBS(const wchar_t* pstr, long wslen)
{
	int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

	std::string dblstr(len, '\0');
	len = ::WideCharToMultiByte(CP_ACP, 0 /* no flags */,
		pstr, wslen /* not necessary NULL-terminated */,
		&dblstr[0], len,
		NULL, NULL /* no default char */);
	return dblstr;
}

std::string ConvertBSTRToMBS(BSTR bstr)
{
	int wslen = ::SysStringLen(bstr);
	return ConvertWCSToMBS((wchar_t*)bstr, wslen);
}


configParser::configParser()
{

}

configParser::Nodetype configParser::getNodeEnumeration(BSTR inString)
{
	if (0 == wcscmp(inString, L"Highway"))
		return configParser::Nodetype::enumHighway;
	else if (0 == wcscmp(inString, L"River"))
		return configParser::Nodetype::enumRiver;
	else if (0 == wcscmp(inString, L"Railway"))
		return configParser::Nodetype::enumRailway;
	else if (0 == wcscmp(inString, L"Building"))
		return configParser::Nodetype::enumBuilding;
	else if (0 == wcscmp(inString, L"Area"))
		return configParser::Nodetype::enumArea;
	else
		return configParser::Nodetype::undefined;
}


void configParser::readAreaNode(MSXML2::IXMLDOMNode* node)
{
	MSXML2::IXMLDOMNodeList *childList = NULL;//node list containing the child nodes
	MSXML2::IXMLDOMNamedNodeMapPtr DOMNamedNodeMapPtr;//iteration through the collection of attribute nodes
	MSXML2::IXMLDOMNode *pIAttrNode = NULL;//pointer to attribute node
	MSXML2::IXMLDOMNode *tagnode = NULL;

	HRESULT hResult;
	BSTR bstrAttrName, bstrAttrText; //Variables to store attribute's name and text
	BSTR bstrNodeName;
	long length;


	node->get_childNodes(&childList); // Get the child nodes into childList
	hResult = childList->get_length(&length);


	if (SUCCEEDED(hResult))
	{
		childList->get_item(0, &tagnode);
		tagnode->get_nodeName(&bstrNodeName);

		if (0 == wcscmp(bstrNodeName, L"DefaultColor"))
		{
			DOMNamedNodeMapPtr = tagnode->attributes;
			hResult = DOMNamedNodeMapPtr->get_length(&length);
			config.AreaColor = Triple();
			for (int k = 0; k < length; k++)
			{
				DOMNamedNodeMapPtr->get_item(k, &pIAttrNode); //get attribute	
				pIAttrNode->get_nodeName(&bstrAttrName); //get attribute name
				pIAttrNode->get_text(&bstrAttrText); //get attribute value

				if (0 == wcscmp(bstrAttrName, L"red"))
					config.AreaColor.x = wcstod(bstrAttrText, NULL);
				if (0 == wcscmp(bstrAttrName, L"green"))
					config.AreaColor.y = wcstod(bstrAttrText, NULL);
				if (0 == wcscmp(bstrAttrName, L"blue"))
					config.AreaColor.z = wcstod(bstrAttrText, NULL);				
			}
		}
	}

}
void configParser::readRiverNode(MSXML2::IXMLDOMNode* node)
{
	MSXML2::IXMLDOMNamedNodeMapPtr DOMNamedNodeMapPtr;//iteration through the collection of attribute nodes
	MSXML2::IXMLDOMNode *pIAttrNode = NULL;//pointer to attribute node
	HRESULT hResult;
	BSTR bstrAttrName, bstrAttrText; //Variables to store attribute's name and text

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
			if (0 == wcscmp(bstrAttrName, L"size"))
				config.river.size = wcstod(bstrAttrText, NULL);
			if (0 == wcscmp(bstrAttrName, L"texture"))
				config.river.texturepath = ConvertBSTRToMBS(bstrAttrText);
		}
	}
}
void configParser::readRailwayNode(MSXML2::IXMLDOMNode* node)
{
	MSXML2::IXMLDOMNamedNodeMapPtr DOMNamedNodeMapPtr;//iteration through the collection of attribute nodes
	MSXML2::IXMLDOMNode *pIAttrNode = NULL;//pointer to attribute node
	HRESULT hResult;
	BSTR bstrAttrName, bstrAttrText; //Variables to store attribute's name and text

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
			if (0 == wcscmp(bstrAttrName, L"size"))
				config.railway.size = wcstod(bstrAttrText, NULL);
			if (0 == wcscmp(bstrAttrName, L"texture"))
				config.railway.texturepath = ConvertBSTRToMBS(bstrAttrText);
		}
	}
}
void configParser::readHighwayNode(MSXML2::IXMLDOMNode* node)
{
	MSXML2::IXMLDOMNodeList *childList = NULL;//node list containing the child nodes
	MSXML2::IXMLDOMNamedNodeMapPtr DOMNamedNodeMapPtr;//iteration through the collection of attribute nodes
	MSXML2::IXMLDOMNode *pIAttrNode = NULL;//pointer to attribute node
	MSXML2::IXMLDOMNode *tagnode = NULL;
	long length;
	HRESULT hResult;
	BSTR bstrAttrName, bstrAttrText; //Variables to store attribute's name and text
	BSTR bstrNodeName;

	node->get_childNodes(&childList); // Get the child nodes into childList
	hResult = childList->get_length(&length);

	for (int k = 0; k < length; k++)
	{
		childList->get_item(k, &tagnode);
		tagnode->get_nodeName(&bstrNodeName);

		if (0 == wcscmp(bstrNodeName, L"HighwayResidual"))
		{
			DOMNamedNodeMapPtr = tagnode->attributes;
			DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute
			pIAttrNode->get_text(&bstrAttrText);
			config.residual.size = wcstod(bstrAttrText,NULL);

			DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
			pIAttrNode->get_text(&bstrAttrText);
			config.residual.texturepath = ConvertBSTRToMBS(bstrAttrText);
			continue;
		}
		else if (0 == wcscmp(bstrNodeName, L"HighwayUnclassified"))
		{
			DOMNamedNodeMapPtr = tagnode->attributes;
			DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute
			pIAttrNode->get_text(&bstrAttrText);
			config.unclassified.size = wcstod(bstrAttrText,NULL);

			DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
			pIAttrNode->get_text(&bstrAttrText);
			config.unclassified.texturepath = ConvertBSTRToMBS(bstrAttrText);
			continue;
		}
		else if (0 == wcscmp(bstrNodeName, L"HighwayPrimary"))
		{
			DOMNamedNodeMapPtr = tagnode->attributes;
			DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute
			pIAttrNode->get_text(&bstrAttrText);
			config.primary.size = wcstod(bstrAttrText,NULL);

			DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
			pIAttrNode->get_text(&bstrAttrText);
			config.primary.texturepath = ConvertBSTRToMBS(bstrAttrText);
			continue;
		}
		else if (0 == wcscmp(bstrNodeName, L"HighwaySecondary"))
		{
			DOMNamedNodeMapPtr = tagnode->attributes;
			DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute
			pIAttrNode->get_text(&bstrAttrText);
			config.secondary.size = wcstod(bstrAttrText,NULL);

			DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
			pIAttrNode->get_text(&bstrAttrText);
			config.secondary.texturepath = ConvertBSTRToMBS(bstrAttrText);
			continue;
		}
		else if (0 == wcscmp(bstrNodeName, L"HighwayTertiary"))
		{
			DOMNamedNodeMapPtr = tagnode->attributes;
			DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute
			pIAttrNode->get_text(&bstrAttrText);
			config.tertiary.size = wcstod(bstrAttrText, NULL);

			DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
			pIAttrNode->get_text(&bstrAttrText);
			config.tertiary.texturepath = ConvertBSTRToMBS(bstrAttrText);
			continue;
		}
		else if (0 == wcscmp(bstrNodeName, L"HighwayService"))
		{
			DOMNamedNodeMapPtr = tagnode->attributes;
			DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute
			pIAttrNode->get_text(&bstrAttrText);
			config.service.size = wcstod(bstrAttrText, NULL);

			DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
			pIAttrNode->get_text(&bstrAttrText);
			config.service.texturepath = ConvertBSTRToMBS(bstrAttrText);
			continue;
		}
		else if (0 == wcscmp(bstrNodeName, L"HighwayFootway"))
		{
			DOMNamedNodeMapPtr = tagnode->attributes;
			DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute
			pIAttrNode->get_text(&bstrAttrText);
			config.footway.size = wcstod(bstrAttrText, NULL);

			DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
			pIAttrNode->get_text(&bstrAttrText);
			config.footway.texturepath = ConvertBSTRToMBS(bstrAttrText);
			continue;
		}
		else if (0 == wcscmp(bstrNodeName, L"HighwayPath"))
		{
			DOMNamedNodeMapPtr = tagnode->attributes;
			DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute
			pIAttrNode->get_text(&bstrAttrText);
			config.path.size = wcstod(bstrAttrText, NULL);

			DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
			pIAttrNode->get_text(&bstrAttrText);
			config.path.texturepath = ConvertBSTRToMBS(bstrAttrText);
			continue;
		}
		else if (0 == wcscmp(bstrNodeName, L"HighwayPavement"))
		{
			DOMNamedNodeMapPtr = tagnode->attributes;
			DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute
			pIAttrNode->get_text(&bstrAttrText);
			config.pavament.size = wcstod(bstrAttrText, NULL);

			DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
			pIAttrNode->get_text(&bstrAttrText);
			config.pavament.texturepath = ConvertBSTRToMBS(bstrAttrText);
			continue;
		}
	}



}
void configParser::readBuildingNode(MSXML2::IXMLDOMNode* node)
{
	MSXML2::IXMLDOMNodeList *childList = NULL;//node list containing the child nodes
	MSXML2::IXMLDOMNamedNodeMapPtr DOMNamedNodeMapPtr;//iteration through the collection of attribute nodes
	MSXML2::IXMLDOMNode *pIAttrNode = NULL;//pointer to attribute node
	MSXML2::IXMLDOMNode *tagnode = NULL;
	long length;
	HRESULT hResult;
	BSTR bstrAttrName, bstrAttrText; //Variables to store attribute's name and text
	BSTR bstrNodeName;

	node->get_childNodes(&childList); // Get the child nodes into childList
	hResult = childList->get_length(&length);

	if (SUCCEEDED(hResult))
	{
		for (int k = 0; k < length; k++)
		{
			childList->get_item(k, &tagnode);
			tagnode->get_nodeName(&bstrNodeName);

			if (0 == wcscmp(bstrNodeName, L"FloorInfo"))
			{
				DOMNamedNodeMapPtr = tagnode->attributes;
				DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute	
				pIAttrNode->get_text(&bstrAttrText); //get ref value
				config.building.floornumber = _wtoi(bstrAttrText);

				DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
				pIAttrNode->get_text(&bstrAttrText); //get ref value
				config.building.floorheight = wcstod(bstrAttrText, NULL);
				continue;
			}

			else if (0 == wcscmp(bstrNodeName, L"DefaultColor"))
			{
				DOMNamedNodeMapPtr = tagnode->attributes;

				DOMNamedNodeMapPtr->get_item(0, &pIAttrNode); //get ref attribute	
				pIAttrNode->get_text(&bstrAttrText); //get ref value
				config.building.defaultcolor.x = wcstod(bstrAttrText, NULL);

				DOMNamedNodeMapPtr->get_item(1, &pIAttrNode); //get ref attribute	
				pIAttrNode->get_text(&bstrAttrText); //get ref value
				config.building.defaultcolor.y = wcstod(bstrAttrText, NULL);

				DOMNamedNodeMapPtr->get_item(2, &pIAttrNode); //get ref attribute	
				pIAttrNode->get_text(&bstrAttrText); //get ref value
				config.building.defaultcolor.z = wcstod(bstrAttrText, NULL);
				continue;
			}

			else if (0 == wcscmp(bstrNodeName, L"DefaultTextures"))
			{
				MSXML2::IXMLDOMNodeList *textureList;
				MSXML2::IXMLDOMNode *textureNode = NULL;
				MSXML2::IXMLDOMNamedNodeMapPtr textureAttributes;

				long textureCount;

				tagnode->get_childNodes(&textureList); // Get the child nodes into textureList
				hResult = textureList->get_length(&textureCount);

				if (SUCCEEDED(hResult))
				{
					for (int x = 0; x < textureCount; x++)
					{
						textureList->get_item(x, &textureNode);
						textureAttributes = textureNode->attributes;
						BuildingTexture bt;

						for (int y = 0; y < textureAttributes->Getlength(); y++)
						{
							textureAttributes->get_item(y, &pIAttrNode); //get ref attribute	
							pIAttrNode->get_nodeName(&bstrAttrName);
							pIAttrNode->get_text(&bstrAttrText); //get ref value

							if (0 == wcscmp(bstrAttrName, L"type"))
								bt.type = ConvertBSTRToMBS(bstrAttrText);
							else if (0 == wcscmp(bstrAttrName, L"path"))
								bt.path = ConvertBSTRToMBS(bstrAttrText);
							else if (0 == wcscmp(bstrAttrName, L"width"))
								bt.width = wcstod(bstrAttrText, NULL);
						}
						config.building.textures.push_back(bt);
					}

				}
			}

		}



	}




}

void configParser::parseConfigFile(string fileName)
{
	try
	{
		MSXML2::IXMLDOMDocumentPtr docPtr;//pointer to DOMDocument object
		MSXML2::IXMLDOMNodeListPtr NodeListPtr;//indexed access. and iteration through the collection of nodes
		MSXML2::IXMLDOMNodePtr DOMNodePtr;//pointer to the node
		MSXML2::IXMLDOMNode *pIDOMNode = NULL;//pointer to element's node

		//Variable with the name of node to find: 
		BSTR strFindText = L" ";//" " means to output every node

		//Variables to store item's name, parent, text and node type:
		BSTR bstrItemText, bstrItemNode, bstrNodeType;

		//Initialize COM Library:
		CoInitialize(NULL);

		//Create an instance of the DOMDocument object:
		docPtr.CreateInstance(__uuidof(DOMDocument30));

		// Load a document:
		_variant_t varXml(fileName.c_str());//XML file to load
		_variant_t varResult((bool)TRUE);//result 

		varResult = docPtr->load(varXml);

		if ((bool)varResult == FALSE)
		{
			printf("*** Error:failed to load config.xml file. ***\n");
			return;
		}

		//Collect all or selected nodes by tag name:
		NodeListPtr = docPtr->getElementsByTagName(strFindText);

		//Output root node:
		docPtr->documentElement->get_nodeName(&bstrItemText);


		for (int i = 0; i < (NodeListPtr->length); i++)
		{

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
					case Nodetype::enumHighway:
						readHighwayNode(pIDOMNode);
						break;

					case Nodetype::enumBuilding:
						readBuildingNode(pIDOMNode);
						break;

					case Nodetype::enumRailway:
						readRailwayNode(pIDOMNode);
						break;

					case Nodetype::enumRiver:
						readRiverNode(pIDOMNode);
						break;

					case Nodetype::enumArea:
						readAreaNode(pIDOMNode);
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