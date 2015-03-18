#ifndef _GLTOOLS_H_
#define _GLTOOLS_H_

#include <freeglut.h>
#include <time.h>
#include <vector>
#include <algorithm>

using namespace std;

class Triple
{
public:

	float x;
	float y;
	float z;
	Triple()
	{}
	Triple(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}
	void Change(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}

	void normalize()
	{
		float abs = sqrt(x * x + y * y + z * z);
		x = x / abs;
		y = y / abs;
		z = z / abs;
	}

	float Length()
	{
		return sqrt(x*x + y*y + z*z);
	}

	Triple Triple::operator*(const GLfloat& other)
	{
		Triple vector;

		vector.x = x * other;
		vector.y = y * other;
		vector.z = z * other;
		return vector;
	}

	Triple Triple::operator*(const Triple&  other)
	{
			Triple vector;
			vector.x = (y*other.z) - (other.y*z);
			vector.y = -(x*other.z) + (other.x*z);
			vector.z = (x*other.y) - (y*other.x);
			return vector;
	}

	Triple Triple::operator-(const Triple&  other)
	{
		Triple vector;
		vector.x = x - other.x;
		vector.y = y - other.y;
		vector.z = z - other.z;
		return vector;
	}

	Triple Triple::operator+(const Triple&  other)
	{
		Triple vector;
		vector.x = x + other.x;
		vector.y = y + other.y;
		vector.z = z + other.z;
		return vector;
	}



};

class Tuple
{
public : 

	Tuple()
	{}

	Tuple(float a, float b)
	{
		x = a;
		y = b;
	}

	void set(float a, float b)
	{
		x = a;
		y = b;
	}

	Tuple Tuple::operator+(const Tuple&  other)
	{
		Tuple tup;
		tup.x = x + other.x;
		tup.y = y + other.y;
		return tup;
	}

	Tuple Tuple::operator-(const Tuple& other)
	{
		return Tuple(x - other.x, y - other.y);
	}

	Tuple Tuple::operator*(const float coeff)
	{
		return Tuple(x*coeff, y*coeff);
	}

	float x;
	float y;
};

class fps
{
public:


	fps(float count);
	fps();

	void start();
	bool isEndofFrame();

	void drawFPS();
	void calculateFPS();

	//  printf prints to file. printw prints to window
	void printw(float x, float y, float z, char* format, ...);

private:
	int frameCount = 0;
	float fpsValue;
	float frameTime;
	clock_t curTime;
	int currentTime = 0, previousTime = 0;
	GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_24;
};

class Motion
{

public:

	Triple lookatPosition, cameraPosition;	
	bool isjumping;
	int windowHeight;
	int windowWidth;
	bool debug;

	Motion();
	void jump();
	void KeyActions();
	void keypressed(unsigned char key, int x, int y);
	void keyup(unsigned char key, int x, int y);
	void mouseControl(int MouseX, int MouseY);
	void mouseControlOld(int MouseX, int MouseY);
	void mouseWheel(int button, int dir, int x, int y);

private:
	GLfloat t;
	GLfloat speed;
	GLfloat deltaX, deltaY;

	bool allKeys[256];
	GLfloat GLabs(GLfloat x, GLfloat y);
};

class BmpLoader
{
public:
	unsigned char* bmpRead(const char* filename, int* width, int* height);
	int LoadGLTexturesBMP(const char* filename);
};

class Geometry
{
public:

	bool getLineIntersection(Tuple *intersection, Tuple p0, Tuple p1, Tuple p2, Tuple p3)
	{
		double A1, B1, C1, A2, B2, C2;

		A1 = p1.y - p0.y;
		B1 = p0.x - p1.x;
		C1 = A1 * p0.x + B1 * p0.y;

		A2 = p3.y - p2.y;
		B2 = p2.x - p3.x;
		C2 = A2 * p2.x + B2 * p2.y;

		double delta = A1*B2 - A2*B1;

		if (abs(delta) < 0.001)
		{
			return false;
		}

		float x = (B2*C1 - B1*C2) / delta;
		float y = (A1*C2 - A2*C1) / delta;

		if (min(p0.x, p1.x) <= x && max(p0.x, p1.x) >= x && min(p0.y, p1.y) <= y && max(p0.y, p1.y) >= y &&			
			min(p2.x, p3.x) <= x && max(p2.x, p3.x) >= x && min(p2.y, p3.y) <= y && max(p2.y, p3.y) >= y)
		{
			intersection->x = x;
			intersection->y = y;
			return true;
		}
		else
			return false;
	

	}

	bool getInfiniteLineIntersection(Tuple *intersection, Tuple p0, Tuple p1, Tuple p2, Tuple p3)
	{
		double A1, B1, C1, A2, B2, C2;

		A1 = p1.y - p0.y;
		B1 = p0.x - p1.x;
		C1 = A1 * p0.x + B1 * p0.y;

		A2 = p3.y - p2.y;
		B2 = p2.x - p3.x;
		C2 = A2 * p2.x + B2 * p2.y;

		double delta = A1*B2 - A2*B1;

		if (abs(delta) < 0.001)
		{
			return false;
		}

		float x = (B2*C1 - B1*C2) / delta;
		float y = (A1*C2 - A2*C1) / delta;

		intersection->x = x;
		intersection->y = y;
		return true;
	}

};


class Shader
{
public:
	Shader();
	Shader(const char *vsFile, const char *fsFile);
	~Shader();

	void init(const char *vsFile, const char *fsFile);

	void bind();
	void unbind();

	unsigned int id();

private:
	unsigned int shader_id;
	unsigned int shader_vp;
	unsigned int shader_fp;
};

#endif 