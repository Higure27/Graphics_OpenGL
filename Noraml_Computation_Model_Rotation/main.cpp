#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

unsigned int g_windowWidth = 800;
unsigned int g_windowHeight = 600;
char* g_windowName = "OpenGL-Basics";

GLFWwindow* g_window;

// model data
std::vector<float> g_meshVertices;
std::vector<float> g_meshNormals;
std::vector<unsigned int> g_meshIndices;

GLfloat g_modelViewMatrix[16];

void computeNormals()
{
	g_meshNormals.resize(g_meshVertices.size());
	float edge1[3];
	float edge2[3];
	float point1[3];
	float point2[3];
	float point3[3];
	float normx, normy, normz;
	
	for (int i = 0; i < g_meshIndices.size() / 3; i++)
	{
		//get the indices of the triangle face
		int a = g_meshIndices[3 * i];
		int b = g_meshIndices[3 * i + 1];
		int c = g_meshIndices[3 * i + 2];

		//get the x,y,z of the first index
		point1[0] = g_meshVertices[a * 3];
		point1[1] = g_meshVertices[(a * 3) + 1];
		point1[2] = g_meshVertices[(a * 3) + 2];

		//get the x,y,z of the second index
		point2[0] = g_meshVertices[b * 3];
		point2[1] = g_meshVertices[(b * 3) + 1];
		point2[2] = g_meshVertices[(b * 3) + 2];

		//get the x,y,z of the third index
		point3[0] = g_meshVertices[c * 3];
		point3[1] = g_meshVertices[(c * 3) + 1];
		point3[2] = g_meshVertices[(c * 3) + 2];

		//find the x,y,z of the edge between index 1 and 2
		edge1[0] = point2[0] - point1[0];
		edge1[1] = point2[1] - point1[1];
		edge1[2] = point2[2] - point1[2];

		//find the x,y,z of the edge between index 1 and 3
		edge2[0] = point3[0] - point1[0];
		edge2[1] = point3[1] - point1[1];
		edge2[2] = point3[2] - point1[2];

		//Finding the cross product of the 2 egdes to get an orthogonal vector
		//ref: http://tutorial.math.lamar.edu/Classes/CalcII/CrossProduct.aspx
		normx = edge1[1] * edge2[2] - edge1[2] * edge2[1];
		normy = edge1[2] * edge2[0] - edge1[0] * edge2[2];
		normz = edge1[0] * edge2[1] - edge1[1] * edge2[0];

		//finding the magnitude needed to divide by to find the normalized vector 
		float magnitude = sqrt(normx*normx + normy*normy + normz*normz);

		//pushing the normalized vector into the meshNormal list, using a as a common index   
		g_meshNormals[a * 3] = normx/magnitude;
		g_meshNormals[(a * 3) + 1] = normy/magnitude;
		g_meshNormals[(a * 3) + 2] = normz/magnitude;

	}

}

void loadObj(std::string p_path)
{
	std::ifstream nfile;
	nfile.open(p_path);
	std::string s;

	while (nfile >> s)
	{
		if (s.compare("v") == 0)
		{
			float x, y, z;
			nfile >> x >> y >> z;
			g_meshVertices.push_back(x);
			g_meshVertices.push_back(y);
			g_meshVertices.push_back(z);
		}		
		else if (s.compare("f") == 0)
		{
			std::string sa, sb, sc;
			unsigned int a, b, c;
			nfile >> sa >> sb >> sc;

			a = std::stoi(sa);
			b = std::stoi(sb);
			c = std::stoi(sc);

			g_meshIndices.push_back(a - 1);
			g_meshIndices.push_back(b - 1);
			g_meshIndices.push_back(c - 1);
		}
		else
		{
			std::getline(nfile, s);
		}
	}

	computeNormals();

	std::cout << p_path << " loaded. Vertices: " << g_meshVertices.size() / 3 << " Triangles: " << g_meshIndices.size() / 3 << std::endl;
}

double getTime()
{
	return glfwGetTime();
}

void glfwErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << std::endl;
	exit(1);
}

void glfwKeyCallback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
{
	if (p_key == GLFW_KEY_ESCAPE && p_action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(g_window, GL_TRUE);
	}
}

void initWindow()
{
	// initialize GLFW
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
	{
		std::cerr << "GLFW Error: Could not initialize GLFW library" << std::endl;
		exit(1);
	}

	g_window = glfwCreateWindow(g_windowWidth, g_windowHeight, g_windowName, NULL, NULL);
	if (!g_window)
	{
		glfwTerminate();
		std::cerr << "GLFW Error: Could not initialize window" << std::endl;
		exit(1);
	}

	// callbacks
	glfwSetKeyCallback(g_window, glfwKeyCallback);

	// Make the window's context current
	glfwMakeContextCurrent(g_window);

	// turn on VSYNC
	glfwSwapInterval(1);
}

void initGL()
{
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat) g_windowWidth / (GLfloat)g_windowHeight, 0.1f, 10.0f);
}

void clearModelViewMatrix()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			g_modelViewMatrix[4 * i + j] = 0.0f;
		}
	}
}

void updateModelViewMatrix()
{	
	clearModelViewMatrix();

	//Using rotation matrix 
	//ref: https://en.wikipedia.org/wiki/Rotation_matrix
	g_modelViewMatrix[0] = cos(getTime());
	g_modelViewMatrix[2] = -sin(getTime());
	g_modelViewMatrix[5] = 1.0f;
	g_modelViewMatrix[8] = sin(getTime());
	g_modelViewMatrix[10] = cos(getTime());

	g_modelViewMatrix[14] = -5.0f;
	g_modelViewMatrix[15] = 1.0f;
}

void setModelViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	updateModelViewMatrix();
	glLoadMatrixf(g_modelViewMatrix);
}

void render()
{
	setModelViewMatrix();

	glBegin(GL_TRIANGLES);

	for (size_t f = 0; f < g_meshIndices.size(); ++f)
	{
		const float scale = 0.1f;
		const unsigned int idx = g_meshIndices[f];
		const float x = scale * g_meshVertices[3 * idx + 0];
		const float y = scale * g_meshVertices[3 * idx + 1];
		const float z = scale * g_meshVertices[3 * idx + 2];

		const float nx = g_meshNormals[3 * idx + 0];
		const float ny = g_meshNormals[3 * idx + 1];
		const float nz = g_meshNormals[3 * idx + 2];
				
		glNormal3f(nx, ny, nz);
		glVertex3f(x, y, z);
	}

	glEnd();
	
}

void renderLoop()
{
	while (!glfwWindowShouldClose(g_window))
	{
		// Clear buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render();

		// Swap front and back buffers
		glfwSwapBuffers(g_window);

		// Poll for and process events
		glfwPollEvents();
	}
}

int main()
{
	initWindow();
	initGL();
	loadObj("data/teapot.obj");
	renderLoop();
}
