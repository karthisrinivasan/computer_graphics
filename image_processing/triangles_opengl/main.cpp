#include <cmath>
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
int xScreenRes = 850;
int yScreenRes = 850;

// Text for the title bar of the window
string windowLabel("OpenGL Window");

///////////////////////////////////////////////////////////////////////
// draw a single purple triangle
///////////////////////////////////////////////////////////////////////
void drawPurpleTriangle()
{
    glColor4f(0.0, 0.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
      glVertex3f(0.5, -0.5, 0.0);
      glVertex3f(-0.5, -0.5, 0.0);
      glVertex3f(0.0, 0.5, 0.0);
    glEnd();
}

void drawTent(int n)
{

    float pi = 3.141;
    float theta = (2*pi)/n;

    glBegin(GL_TRIANGLES);
        for (int i = 0; i<n ; i=i+2)
        {
            glColor4f(0.0, 0.0, 1.0, 1.0);
            glVertex3f(0.5*cos(i*theta), 0.5*sin(i*theta), 0.0);
            glVertex3f(0.5*cos(theta*(i+1)), 0.5*sin(theta*(i+1)), 0.0);
            glVertex3f(0.0, 0.0, 0.0);
        }
    glEnd();

    glBegin(GL_TRIANGLES);
        for (int i = 0; i<n ; i=i+2)
        {
            glColor4f(1.0, 0.0, 0.0, 1.0);
            glVertex3f(0.5*cos(theta*(i+1)), 0.5*sin(theta*(i+1)), 0.0);
            glVertex3f(0.5*cos(theta*(i+2)), 0.5*sin(theta*(i+2)), 0.0);
            glVertex3f(0.0, 0.0, 0.0);
        }
    glEnd();

}

void drawRamp(int n)
{

    float pi = 3.141;
    float theta = (2*pi)/n;

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_BLEND); 

    glBegin(GL_TRIANGLES);
        for (int i = 0; i<n ; i++)
        {
            glColor4f(1.0, 0.0, 0.0, 0.5);
            glVertex3f(0.5*cos(theta*(i)), 0.5*sin(theta*(i)), 0.0);
            glColor4f(0.0, 1.0, 0.0, 0.5);
            glVertex3f(0.5*cos(theta*(i+1)), 0.5*sin(theta*(i+1)), 0.0);
            glColor4f(0.0, 0.0, 1.0, 0.5);
            glVertex3f(0.0, 0.0, 0.0);
        }
    glEnd();

}

///////////////////////////////////////////////////////////////////////
// GL and GLUT callbacks
///////////////////////////////////////////////////////////////////////
void glutDisplay()
{
    // Make ensuing transforms affect the projection matrix
    glMatrixMode(GL_PROJECTION);
    
    // set the projection matrix to an orthographic view
    glLoadIdentity();
    float halfZoom = 0.5;
    
    glOrtho(-halfZoom, halfZoom, -halfZoom, halfZoom, -10, 10);
    
    // set the matrix mode back to modelview
    glMatrixMode(GL_MODELVIEW);
    
    // set the lookat transform
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 1.0,  // eye
              0.0, 0.0, 0.0,  // center 
              0, 1, 0);   // up
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw a purple triangle
    // drawPurpleTriangle();   
    // drawTent(30);   
    drawRamp(36);   

    glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void printCommands()
{
    cout << "=============================================================== " << endl;
    cout << " Assignment 1 for CPSC 478/578" << endl;
    cout << "=============================================================== " << endl;
    cout << " q           - quit" << endl;
}

///////////////////////////////////////////////////////////////////////
// Map the keyboard keys to something here
///////////////////////////////////////////////////////////////////////
void glutKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'Q':
        case 'q':
            exit(0);
            break;
        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void glutIdle()
{
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
    glClearColor(0.1, 0.1, 0.1, 0);
    
    // register all the callbacks
    glutDisplayFunc(&glutDisplay);
    glutIdleFunc(&glutIdle);
    glutKeyboardFunc(&glutKeyboard);
    
    // enter the infinite GL loop
    glutMainLoop();
    
    // Control flow will never reach here
    return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    // initialize GLUT and GL
    glutInit(&argc, argv);
    
    // open the GL window
    glvuWindow();
    return 1;
}
