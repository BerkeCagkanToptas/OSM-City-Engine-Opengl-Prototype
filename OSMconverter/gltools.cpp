#include <iostream>
#include <stdlib.h>
#include <GL\glew.h>
#include <math.h>
#include <Windows.h>
#include "gltools.h"
#include <freeglut.h>
#include <time.h>



#define piover180 0.0174532925f


//--------------------<MOTION>---------------------------------------------------------
Motion::Motion()
{
	for (int i = 0; i < 256; i++)
		allKeys[i] = false;

	speed = 1.0f;
	t = 0.03f;

	GLint m_viewport[4];
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	windowHeight = m_viewport[3];
	windowWidth = m_viewport[2];
	deltaX = 0;
	deltaY = 0;

	debug = false;
}
GLfloat Motion::GLabs(GLfloat x, GLfloat y)
{
	return sqrt(x*x + y*y);
}
void Motion::jump()
{

	if (isjumping == false)
		return;

	GLfloat jumpheight = 3.0f + 0.03f * ((50 * t) - (5 * t * t));
	t += 0.03f;

	if (jumpheight <= 3.0f)
	{
		t = 0.05f;
		jumpheight = 3.0f;
		isjumping = false;
	}

	cameraPosition.y = jumpheight;
	lookatPosition.y = jumpheight;
}
void Motion::keypressed(unsigned char key, int x, int y)
{
	if (allKeys[(int)' '] == true)
		return;

	allKeys[key] = true;
}
void Motion::keyup(unsigned char key, int x, int y)
{
	allKeys[key] = false;
}
void Motion::KeyActions()
{
	GLfloat tmx = lookatPosition.x - cameraPosition.x;
	GLfloat tmz = lookatPosition.z - cameraPosition.z;
	GLfloat abs = GLabs(tmx, tmz);


	jump();


	if (allKeys[(int)'w'])
	{
		cameraPosition.x += (tmx / abs)*speed;
		lookatPosition.x += (tmx / abs)*speed;
		cameraPosition.z += (tmz / abs)*speed;
		lookatPosition.z += (tmz / abs)*speed;
	}

	if (allKeys[(int)'a'])
	{
		cameraPosition.x += (tmz / abs)*speed;
		lookatPosition.x += (tmz / abs)*speed;
		cameraPosition.z -= (tmx / abs)*speed;
		lookatPosition.z -= (tmx / abs)*speed;
	}

	if (allKeys[(int)'s'])
	{
		cameraPosition.x -= (tmx / abs)*speed;
		lookatPosition.x -= (tmx / abs)*speed;
		cameraPosition.z -= (tmz / abs)*speed;
		lookatPosition.z -= (tmz / abs)*speed;
	}


	if (allKeys[(int)'d'])
	{
		cameraPosition.x -= (tmz / abs)*speed;
		lookatPosition.x -= (tmz / abs)*speed;
		cameraPosition.z += (tmx / abs)*speed;
		lookatPosition.z += (tmx / abs)*speed;
	}

	if (allKeys[(int) 't'])
	{
		cameraPosition.y += speed;
		lookatPosition.y += speed;
	}
	if (allKeys[(int) 'y'])
	{
		cameraPosition.y -= speed;
		lookatPosition.y -= speed;
	}

	if (allKeys[(int) 'p'])	
		debug = true;
	
	if (allKeys[(int) 'o'])
		debug = false;
		

	if (allKeys[27])
		exit(1);
}
void Motion::mouseControl(int MouseX, int MouseY)
{

	deltaX += (MouseX - windowWidth / 2);
	deltaY += (MouseY - windowHeight / 2);

	if (deltaY >= 300)
		deltaY = 300;
	if (deltaY <= -300)
		deltaY = -300;

	GLfloat yxangle = (-75.0f / (windowHeight / 2))*deltaY;    // enable to turn 75 degree
	GLfloat zxangle = (-180.0f / (windowWidth / 2))*deltaX;

	lookatPosition.x = cameraPosition.x + 10 * cos(yxangle*piover180) * sin(zxangle * piover180);
	lookatPosition.z = cameraPosition.z + 10 * cos(yxangle*piover180) * cos(zxangle * piover180);
	lookatPosition.y = cameraPosition.y + 10 * sin(yxangle*piover180);

	//cout << "LookatPosition: " << lookatPosition.x << " " << lookatPosition.y << " " << lookatPosition.z << endl;
	//cout << "DeltaX : " << deltaX << " " << "DeltaY: " << deltaY << endl;
}
void Motion::mouseControlOld(int MouseX, int MouseY)
{
	GLfloat zxangle = (360.0 / 400.0)*(MouseX - 400);
	GLfloat yxangle = (120.0 / 400.0)*(MouseY - 300);

	lookatPosition.y = cameraPosition.y + -1 * 100 * tan(yxangle * piover180);
	lookatPosition.x = cameraPosition.x + 100 * sin(zxangle * piover180);
	lookatPosition.z = cameraPosition.z + -1 * 100 * cos(zxangle * piover180);

	glutPostRedisplay();

	//cout << "x:" << cameraPosition.x << "  y:" << cameraPosition.y << " z:" << cameraPosition.z << endl;

}
void Motion::mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		cameraPosition.y -= 10*speed;
		lookatPosition.y -= 10 * speed;
		speed -= 0.1;
	}
	else
	{
		cameraPosition.y += 10*speed;
		lookatPosition.y += 10 * speed;
		speed += 0.1;
	}
	glutPostRedisplay();
}
//----------------- </MOTION>--------------------------------------------------------



//----------------------<FPS>-----------------------------------------------------------
fps::fps(float interval)
{
	frameTime = interval;
}
fps::fps()
{}
void fps::start()
{
	curTime = clock();
}
bool fps::isEndofFrame()
{
	clock_t t = clock();
	float elapsed = ((float)(t - curTime) / 1000.0f);
	if (elapsed >= frameTime)
		return true;
	else
		return false;

}
void fps::drawFPS()
{
	//  Load the identity matrix so that FPS string being drawn
	//  won't get animates
	glLoadIdentity();

	//  Print the FPS to the window
	glColor3f(1.0f, 0.3f, 0.3f);
	printw(-1.4f,-1.4f, -2.0f, "FPS: %4.2f", fpsValue);
	glColor4f(1, 1, 1, 1);
	//cout << "FPS: " << fpsValue << endl;
}
void fps::calculateFPS()
{
	//  Increase frame count
	frameCount++;

	//  Get the number of milliseconds since glutInit called
	//  (or first call to glutGet(GLUT ELAPSED TIME)).
	currentTime = glutGet(GLUT_ELAPSED_TIME);

	//  Calculate time passed
	int timeInterval = currentTime - previousTime;

	if (timeInterval > 1000)
	{
		//  calculate the number of frames per second
		fpsValue = frameCount / (timeInterval / 1000.0f);

		//  Set time
		previousTime = currentTime;

		//  Reset frame count
		frameCount = 0;
	}
}
//-------------------------------------------------------------------------
//  Draws a string at the specified coordinates.
//-------------------------------------------------------------------------
void fps::printw(float x, float y, float z, char* format, ...)
{
	va_list args;	//  Variable argument list
	int len;		//	String length
	int i;			//  Iterator
	char * text;	//	Text

	//  Initialize a variable argument list
	va_start(args, format);

	//  Return the number of characters in the string referenced the list of arguments.
	//  _vscprintf doesn't count terminating '\0' (that's why +1)
	len = _vscprintf(format, args) + 1;

	//  Allocate memory for a string of the specified size
	text = (char *)malloc(len * sizeof(char));

	//  Write formatted output using a pointer to the list of arguments
	vsprintf_s(text, len, format, args);

	//  End using variable argument list 
	va_end(args);

	//  Specify the raster position for pixel operations.
	glRasterPos3f(x, y,z);

	//  Draw the characters one by one
	for (i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(font_style, text[i]);

	//  Free the allocated memory for the string
	free(text);
}
//----------------------</FPS>---------------------------------------------------------





//----------------- <BMPLOADER>------------------------------------------------------
unsigned char* BmpLoader::bmpRead(const char* filename, int* width, int* height)
{
	unsigned char* image;

	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int w, h;
	unsigned int imageSize;   // = width*height*3

	FILE *file;
	fopen_s(&file, filename, "rb");

	//FILE * file = fopen(filename, "rb");
	if (!file)
	{
		perror(filename);
		return NULL;
	}

	if (fread(header, 1, 54, file) != 54)
	{
		// If not 54 bytes read : problem
		fprintf(stderr, "Not a correct BMP file1\n");
		return NULL;
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		cout << header[0] << "-----" << header[1] << endl;
		cout << filename << endl;
		cout << "Not a correct BMP file2\n";
		return NULL;
	}


	dataPos = *(int*)&(header[0x0A]);
	w = *(int*)&(header[0x12]);
	h = *(int*)&(header[0x16]);
	imageSize = *(int*)&(header[0x22]);


	if (imageSize == 0)    imageSize = w*h * 3;
	if (dataPos == 0)      dataPos = 54;


	image = new unsigned char[imageSize];
	fread(image, 1, imageSize, file);
	fclose(file);

	*width = w;
	*height = h;

	return image;
}
int BmpLoader::LoadGLTexturesBMP(const char* filename)
{
	GLuint texture;
	unsigned char * data;
	FILE * file;


	int sizex, sizey;

	data = bmpRead(filename, &sizex, &sizey);

	glGenTextures(1, &texture); //generate the texture with the loaded data
	glBindTexture(GL_TEXTURE_2D, texture); //bind the texture to it's array
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //set texture environment parameters

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Adjusted for repeat texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sizex, sizey, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	free(data); //free the texture

	return texture; //return whether it was successfull
}
//----------------- </BMPLOADER>------------------------------------------------------



//----------------- <SHADER>----------------------------------------------------------
static char* textFileRead(const char *fileName)
{
	char* text = NULL;

	if (fileName != NULL) {

		FILE *file;
		fopen_s(&file, fileName, "rb");

		//FILE *file = fopen(fileName, "rt");


		if (file != NULL) {
			fseek(file, 0, SEEK_END);
			int count = ftell(file);
			rewind(file);

			if (count > 0) {
				text = (char*)malloc(sizeof(char)* (count + 1));
				count = fread(text, sizeof(char), count, file);
				text[count] = '\0';
			}
			fclose(file);
		}
	}
	return text;
}
static void validateShader(GLuint shader, const char* file = 0)
{
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;

	glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
	if (length > 0) {
		cerr << "Shader " << shader << " (" << (file ? file : "") << ") compile error: " << buffer << endl;
	}
}
static void validateProgram(GLuint program)
{
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;

	memset(buffer, 0, BUFFER_SIZE);
	glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
	if (length > 0)
		cerr << "Program " << program << " link error: " << buffer << endl;

	glValidateProgram(program);
	GLint status;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
		cerr << "Error validating shader " << program << endl;
}
Shader::Shader()
{

}
Shader::Shader(const char *vsFile, const char *fsFile)
{
	init(vsFile, fsFile);
}
void Shader::init(const char *vsFile, const char *fsFile)
{
	shader_vp = glCreateShader(GL_VERTEX_SHADER);
	shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vsText = textFileRead(vsFile);
	const char* fsText = textFileRead(fsFile);

	if (vsText == NULL || fsText == NULL)
	{
		cerr << "Either vertex shader or fragment shader file not found." << endl;
		return;
	}

	glShaderSource(shader_vp, 1, &vsText, 0);
	glShaderSource(shader_fp, 1, &fsText, 0);

	glCompileShader(shader_vp);
	validateShader(shader_vp, vsFile);
	glCompileShader(shader_fp);
	validateShader(shader_fp, fsFile);

	shader_id = glCreateProgram();
	glAttachShader(shader_id, shader_fp);
	glAttachShader(shader_id, shader_vp);
	glLinkProgram(shader_id);
	validateProgram(shader_id);
}
Shader::~Shader()
{
	glDetachShader(shader_id, shader_fp);
	glDetachShader(shader_id, shader_vp);

	glDeleteShader(shader_fp);
	glDeleteShader(shader_vp);
	glDeleteProgram(shader_id);
}
unsigned int Shader::id()
{
	return shader_id;
}
void Shader::bind()
{
	glUseProgram(shader_id);
}
void Shader::unbind()
{
	glUseProgram(0);
}
//------------------</SHADER>----------------------------------------------------------

