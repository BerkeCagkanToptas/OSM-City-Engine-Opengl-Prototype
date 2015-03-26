#pragma once
//to map image filenames to textureIds
#include <string.h>
#include <map>

#include <gl\glew.h>
#include <IL\il.h>

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/PostProcess.h"
#include "assimp/Scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"



// Information to render each assimp node
struct MyMesh
{

	GLuint vao;
	GLuint texIndex;
	GLuint uniformBlockIndex;
	int numFaces;
};

struct MyMaterial{

	float diffuse[4];
	float ambient[4];
	float specular[4];
	float emissive[4];
	float shininess;
	int texCount;
};



class ObjectLoader
{
public:
	ObjectLoader();

	bool ImportObject(const std::string folderPath, const std::string fileName);
	GLuint generateObjectVBO();

	void drawObject();


private:
	// the global Assimp scene object
	const aiScene* scene = NULL;
	std::string objectfolderPath;


	// images / texture
	std::map<std::string, GLuint> textureIdMap;	// map image filenames to textureIds
	GLuint*		textureIds;							// pointer to texture Array

	// Create an instance of the Importer class
	Assimp::Importer importer;
	void recursive_render(const struct aiScene *sc, const struct aiNode* nd, float scale);
	void apply_material(const aiMaterial *mtl);

	// Can't send color down as a pointer to aiColor4D because AI colors are ABGR.
	void Color4f(const aiColor4D *color)
	{
		glColor4f(color->r, color->g, color->b, color->a);
	}
	void set_float4(float f[4], float a, float b, float c, float d)
	{
		f[0] = a;
		f[1] = b;
		f[2] = c;
		f[3] = d;
	}
	void color4_to_float4(const aiColor4D *c, float f[4])
	{
		f[0] = c->r;
		f[1] = c->g;
		f[2] = c->b;
		f[3] = c->a;
	}
	int LoadGLTextures(const aiScene* scene);
};

