#include <cmath>
#include "VEC3F.h"
#include "VEC3I.h"
#include <iostream>

#ifdef _WSL_
#include <GL/freeglut.h>
#include <GL/glu.h>
#elif _WIN32_
#include <GL/glut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <arpa/inet.h> 
#endif

using namespace std;

// the resolution of the OpenGL window
int xScreenRes = 800;
int yScreenRes = 800;

// Text for the title bar of the window
string windowLabel("Draw Shape");

// mouse tracking variables
int xMouse         = -1;
int yMouse         = -1;
int mouseButton    = -1;
int mouseState     = -1;
int mouseModifiers = -1;

// current grid cell the mouse is pointing at
int xField = -1;
int yField = -1;

const float incAngle = 0.7;
float angle = 0.0;

int n = 0;

// forward declare the caching function here so that we can
// put it at the bottom of the file
void runOnce();

// forward declare the timestepping function here so that we can
// put it at the bottom of the file
void runEverytime();

///////////////////////////////////////////////////////////////////////
// TODO: YOU WILL NEED TO MAKE A NEW FUNCTION FOR EACH SHAPE YOU WANT
// TO DRAW
///////////////////////////////////////////////////////////////////////
void drawCube(bool drawLines)
{
  // cube vertices
  vector<VEC3F> vertices;
  vertices.push_back(VEC3F(-1.0, -1.0,  1.0));
  vertices.push_back(VEC3F( 1.0, -1.0,  1.0));
  vertices.push_back(VEC3F(-1.0,  1.0,  1.0));
  vertices.push_back(VEC3F( 1.0,  1.0,  1.0));
  vertices.push_back(VEC3F(-1.0, -1.0, -1.0));
  vertices.push_back(VEC3F( 1.0, -1.0, -1.0));
  vertices.push_back(VEC3F(-1.0,  1.0, -1.0));
  vertices.push_back(VEC3F( 1.0,  1.0, -1.0));

  /* 
  
  Never seen the vector idiom in C++ before? 
  This is the same as setting declaring an array and setting each element:

  VEC3F vertices[8];
  vertices[0] = VEC3F(-1.0, -1.0,  1.0);
  vertices[1] = VEC3F( 1.0, -1.0,  1.0);
  vertices[2] = VEC3F(-1.0,  1.0,  1.0);
  vertices[3] = VEC3F( 1.0,  1.0,  1.0);
  vertices[4] = VEC3F(-1.0, -1.0, -1.0);
  vertices[5] = VEC3F( 1.0, -1.0, -1.0);
  vertices[6] = VEC3F(-1.0,  1.0, -1.0);
  vertices[7] = VEC3F( 1.0,  1.0, -1.0);

  Except that you don't have to know the size of the array in advance.
  Every time you push a new element to the back, it will resize the array.
  All those resizes might seem a little wasteful, but the library does some
  work behind the scenes to make things fairly efficient.

  */

  // cube faces
  vector<VEC3I> faces; 

  // front face
  faces.push_back(VEC3I(0, 1, 2));
  faces.push_back(VEC3I(2, 1, 3));
  // back face
  faces.push_back(VEC3I(5, 4, 7));
  faces.push_back(VEC3I(7, 4, 6));
  // left face
  faces.push_back(VEC3I(4, 0, 6));
  faces.push_back(VEC3I(6, 0, 2));
  // right face
  faces.push_back(VEC3I(1, 5, 3));
  faces.push_back(VEC3I(3, 5, 7));
  // top face
  faces.push_back(VEC3I(2, 3, 6));
  faces.push_back(VEC3I(6, 3, 7));
  // bottom face
  faces.push_back(VEC3I(4, 5, 0));
  faces.push_back(VEC3I(0, 5, 1));

  if (drawLines)
  {
    glLineWidth(3.0);
    for (unsigned int x = 0; x < faces.size(); x++)
    {
      // GL_LINE_LOOP needs to be told that each group of
      // three vertices forms a loop
      glBegin(GL_LINE_LOOP);
      for (int y = 0; y < 3; y++)
      {
        const int index = faces[x][y];
        const VEC3F& vertex = vertices[index];
        glVertex3f(vertex[0], vertex[1], vertex[2]);
      }
      glEnd();
    }
  }
  else
  {
    glBegin(GL_TRIANGLES);

    // GL_TRIANGLES already knows to group every 
    // three vertices into a triangle
    for (unsigned int x = 0; x < faces.size(); x++)
      for (int y = 0; y < 3; y++)
      {
        const int index = faces[x][y];
        const VEC3F& vertex = vertices[index];
        glVertex3f(vertex[0], vertex[1], vertex[2]);
      }
    glEnd();
  }
}


void drawOct(bool drawLines)
{
  // octahedron vertices

  float s = 1.414;

  vector<VEC3F> vertices;
  vertices.push_back(VEC3F(s, 0.0, 0.0));
  vertices.push_back(VEC3F(0.0, 0.0, s));
  vertices.push_back(VEC3F(-s, 0.0, 0.0));
  vertices.push_back(VEC3F(0.0, 0.0, -s));
  vertices.push_back(VEC3F(0.0, s, 0.0));
  vertices.push_back(VEC3F(0.0, -s, 0.0));

  // octahedron faces
  vector<VEC3I> faces; 

  // front
  faces.push_back(VEC3I(0, 1, 4));
  faces.push_back(VEC3I(0, 1, 5));

  // left
  faces.push_back(VEC3I(1, 2, 4));
  faces.push_back(VEC3I(1, 2, 5)); 

  // back
  faces.push_back(VEC3I(2, 3, 4));
  faces.push_back(VEC3I(2, 3, 5));

  // right
  faces.push_back(VEC3I(3, 0, 4));
  faces.push_back(VEC3I(3, 0, 5));

  // int n = 2;
  float r = 1.414;
  vector<VEC3I> faces1;
  vector<VEC3I> faces_itr[n+1];
  faces_itr[0] = faces;
  int n_verts;


  for (int itr = 0; itr<n; itr++)
  {
    int n_faces = faces_itr[itr].size();
    for (unsigned int x = 0; x < n_faces; x++)
        {
          int index_1 = faces_itr[itr][x][0];
          int index_2 = faces_itr[itr][x][1];
          int index_3 = faces_itr[itr][x][2];


          VEC3F vertex_1 = vertices[index_1];
          VEC3F vertex_2 = vertices[index_2];
          VEC3F vertex_3 = vertices[index_3];

          float x1 = 0.5*float(vertex_1[0]+vertex_2[0]);
          float y1 = 0.5*float(vertex_1[1]+vertex_2[1]);
          float z1 = 0.5*float(vertex_1[2]+vertex_2[2]);
          float r1 = sqrt((x1*x1)+(y1*y1)+(z1*z1));
          x1 = r*x1/r1; 
          y1 = r*y1/r1; 
          z1 = r*z1/r1;

          float x2 = 0.5*float(vertex_2[0]+vertex_3[0]);
          float y2 = 0.5*float(vertex_2[1]+vertex_3[1]);
          float z2 = 0.5*float(vertex_2[2]+vertex_3[2]);
          float r2 = sqrt((x2*x2)+(y2*y2)+(z2*z2));
          x2 = r*x2/r2; 
          y2 = r*y2/r2; 
          z2 = r*z2/r2;

          float x3 = 0.5*float(vertex_3[0]+vertex_1[0]);
          float y3 = 0.5*float(vertex_3[1]+vertex_1[1]);
          float z3 = 0.5*float(vertex_3[2]+vertex_1[2]);
          float r3 = sqrt((x3*x3)+(y3*y3)+(z3*z3));
          x3 = r*x3/r3; 
          y3 = r*y3/r3; 
          z3 = r*z3/r3;

          vertices.push_back(VEC3F(x1, y1, z1));
          vertices.push_back(VEC3F(x2, y2, z2));
          vertices.push_back(VEC3F(x3, y3, z3));

          n_verts = vertices.size();

          faces_itr[itr+1].push_back(VEC3I(n_verts-1,n_verts-2,n_verts-3));
          faces_itr[itr+1].push_back(VEC3I(index_1,n_verts-3,n_verts-1));
          faces_itr[itr+1].push_back(VEC3I(index_2,n_verts-2,n_verts-3));
          faces_itr[itr+1].push_back(VEC3I(index_3,n_verts-1,n_verts-2));
          
        }
      
  }

  if (drawLines)
  {
    glLineWidth(3.0);
    for (unsigned int x = 0; x < faces_itr[n].size(); x++)
    {
      // GL_LINE_LOOP needs to be told that each group of
      // three vertices forms a loop
      glBegin(GL_LINE_LOOP);
      for (int y = 0; y < 3; y++)
      {
        const int index = faces_itr[n][x][y];
        const VEC3F& vertex = vertices[index];
        glVertex3f(vertex[0], vertex[1], vertex[2]);
      }
      glEnd();
    }
  }
  else
  {
    glBegin(GL_TRIANGLES);

    // GL_TRIANGLES already knows to group every 
    // three vertices into a triangle
    for (unsigned int x = 0; x < faces_itr[n].size(); x++)
    {
      for (int y = 0; y < 3; y++)
      {
        const int index = faces_itr[n][x][y];
        const VEC3F& vertex = vertices[index];
        glVertex3f(vertex[0], vertex[1], vertex[2]);
      }
    }
      glEnd();
  }
}


///////////////////////////////////////////////////////////////////////
// GL and GLUT callbacks
///////////////////////////////////////////////////////////////////////
void glutDisplay()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // set up the perspective volume
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, 1.0, 0.1, 100.0);

  // setup the lookat transform
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 2.0, 6.0,
            0.0, 0.0, 0.0,
            0, 1, 0);

  glEnable(GL_DEPTH_TEST);

  glPushMatrix();
    // make the shape spin
    glRotatef(angle, 0.0, 1.0, 0.0);

    // draw the faces of the cube
    glColor4f(0.82, 0.82, 0.82, 1.0); // set face color to gray

    // TODO: REPLACE THIS WITH THE CALL TO YOUR NEW SHAPE
    // drawCube(false);
    drawOct(false);
    
    // draw the outlines of the triangles in the cube
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0, 1.0);
    glColor4f(0.0, 0.0, 0.0, 1.0); // set line color to black
    
    // TODO: REPLACE THIS WITH THE CALL TO YOUR NEW SHAPE
    // drawCube(true);
    drawOct(true);
    glDisable(GL_POLYGON_OFFSET_FILL);    
  glPopMatrix();

  glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void printCommands()
{
  cout << " q - quit" << endl;
}

///////////////////////////////////////////////////////////////////////
// Map the arrow keys to something here
///////////////////////////////////////////////////////////////////////
void glutSpecial(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_LEFT:
      break;
    case GLUT_KEY_RIGHT:
      break;
    case GLUT_KEY_UP:
      break;
    case GLUT_KEY_DOWN:
      break;
    default:
      break;
  }
}

///////////////////////////////////////////////////////////////////////
// Map the keyboard keys to something here
///////////////////////////////////////////////////////////////////////
void glutKeyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case '?':
      printCommands();
      break;
    case 'p':
      n += 1;
      break;
    case 'q':
    case 'Q':
      exit(0);
      break;
    case ' ':
      runEverytime();
      break;
    default:
      break;
  }
}

///////////////////////////////////////////////////////////////////////
// animate and display new result
///////////////////////////////////////////////////////////////////////
void glutIdle()
{
  runEverytime();
  glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////////
// open the GLVU window
//////////////////////////////////////////////////////////////////////////////
int glvuWindow()
{
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE| GLUT_RGBA);
  glutInitWindowSize(xScreenRes, yScreenRes); 
  glutInitWindowPosition(10, 10);
  glutCreateWindow(windowLabel.c_str());

  // set the viewport resolution (w x h)
  glViewport(0, 0, (GLsizei) xScreenRes, (GLsizei) yScreenRes);

  // set the background color to gray
  glClearColor(0.309, 0.505, 0.74, 1.0);

  // register all the callbacks
  glutDisplayFunc(&glutDisplay);
  glutIdleFunc(&glutIdle);
  glutKeyboardFunc(&glutKeyboard);
  glutSpecialFunc(&glutSpecial);

  // enter the infinite GL loop
  glutMainLoop();

  // Control flow will never reach here
  return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  runOnce();

  // initialize GLUT and GL
  glutInit(&argc, argv);

  // open the GL window
  glvuWindow();
  return 1;
}

///////////////////////////////////////////////////////////////////////
// This function is called every frame -- do something interesting
// here.
///////////////////////////////////////////////////////////////////////
void runEverytime()
{
  angle += incAngle;
}

///////////////////////////////////////////////////////////////////////
// This is called once at the beginning so you can precache
// something here
///////////////////////////////////////////////////////////////////////
void runOnce()
{
}

