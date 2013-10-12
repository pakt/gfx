/* sphere, Copyright (c) 2002 Paul Bourke <pbourke@swin.edu.au>
 * Utility function to create a unit sphere in GL.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 *
 *  8-Oct-98: dek          Released initial version of "glplanet"
 * 21-Mar-01: jwz@jwz.org  Broke sphere routine out into its own file.
 * 28-Feb-02: jwz@jwz.org  New implementation from Paul Bourke:
 *                         http://astronomy.swin.edu.au/~pbourke/opengl/sphere/
 */

#include <math.h>
#include <stdlib.h>
#include "glee/GLee.h"


typedef struct { GLfloat x, y, z; } XYZ;

void
unit_sphere (int stacks, int slices, int wire_p)
{
  int i,j;
  double theta1, theta2, theta3;
  XYZ e, p;
  XYZ la = { 0, 0, 0 }, lb = { 0, 0, 0 };
  XYZ c = {0, 0, 0};  /* center */
  double r = 1.0;     /* radius */
  int stacks2 = stacks * 2;

  if (r < 0)
    r = -r;
  if (slices < 0)
    slices = -slices;

  if (slices < 4 || stacks < 2 || r <= 0)
    {
      glBegin (GL_POINTS);
      glVertex3f (c.x, c.y, c.z);
      glEnd();
      return;
    }

  for (j = 0; j < stacks; j++)
    {
      theta1 = j       * (M_PI+M_PI) / stacks2 - M_PI_2;
      theta2 = (j + 1) * (M_PI+M_PI) / stacks2 - M_PI_2;

      glBegin (wire_p ? GL_LINE_LOOP : GL_TRIANGLE_STRIP);
      for (i = 0; i <= slices; i++)
        {
          theta3 = i * (M_PI+M_PI) / slices;

          if (wire_p && i != 0)
            {
              glVertex3f (lb.x, lb.y, lb.z);
              glVertex3f (la.x, la.y, la.z);
            }

          e.x = cos (theta2) * cos(theta3);
          e.y = sin (theta2);
          e.z = cos (theta2) * sin(theta3);
          p.x = c.x + r * e.x;
          p.y = c.y + r * e.y;
          p.z = c.z + r * e.z;

          glNormal3f (e.x, e.y, e.z);
          glTexCoord2f (i       / (double)slices,
                        2*(j+1) / (double)stacks2);
          glVertex3f (p.x, p.y, p.z);
          if (wire_p) la = p;

          e.x = cos(theta1) * cos(theta3);
          e.y = sin(theta1);
          e.z = cos(theta1) * sin(theta3);
          p.x = c.x + r * e.x;
          p.y = c.y + r * e.y;
          p.z = c.z + r * e.z;

          glNormal3f (e.x, e.y, e.z);
          glTexCoord2f (i   / (double)slices,
                        2*j / (double)stacks2);
          glVertex3f (p.x, p.y, p.z);
          if (wire_p) lb = p;
        }
      glEnd();
    }
}

void unit_cube(){
	glColor3f(0.4f,0.4f,0.4f); //grey
	glBegin(GL_QUADS);						
    //top
		//glColor3f(0.0f,1.0f,0.0f);			// Set The Color To Blue
    glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f, 1.0f);			// Bottom Left Of The Quad (Top)
		glVertex3f( 1.0f, 1.0f, 1.0f);			// Bottom Right Of The Quad (Top)

    //bottom
		//glColor3f(1.0f,0.5f,0.0f);			// Set The Color To Orange
    glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f( 1.0f,-1.0f, 1.0f);			// Top Right Of The Quad (Bottom)
		glVertex3f(-1.0f,-1.0f, 1.0f);			// Top Left Of The Quad (Bottom)
		glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Bottom)
		glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Bottom)

    //front
		//glColor3f(1.0f,0.0f,0.0f);			// Set The Color To Red
    glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f( 1.0f, 1.0f, 1.0f);			// Top Right Of The Quad (Front)
		glVertex3f(-1.0f, 1.0f, 1.0f);			// Top Left Of The Quad (Front)
		glVertex3f(-1.0f,-1.0f, 1.0f);			// Bottom Left Of The Quad (Front)
		glVertex3f( 1.0f,-1.0f, 1.0f);			// Bottom Right Of The Quad (Front)

    //back
		//glColor3f(1.0f,1.0f,0.0f);			// Set The Color To Yellow
    glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Back)
		glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Back)
		glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Back)
		glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Back)

    //left
		//glColor3f(0.0f,0.0f,1.0f);			// Set The Color To Blue
    glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);			// Top Right Of The Quad (Left)
		glVertex3f(-1.0f, 1.0f,-1.0f);			// Top Left Of The Quad (Left)
		glVertex3f(-1.0f,-1.0f,-1.0f);			// Bottom Left Of The Quad (Left)
		glVertex3f(-1.0f,-1.0f, 1.0f);			// Bottom Right Of The Quad (Left)

    //right
		//glColor3f(1.0f,0.0f,1.0f);			// Set The Color To Violet
    glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f( 1.0f, 1.0f,-1.0f);			// Top Right Of The Quad (Right)
		glVertex3f( 1.0f, 1.0f, 1.0f);			// Top Left Of The Quad (Right)
		glVertex3f( 1.0f,-1.0f, 1.0f);			// Bottom Left Of The Quad (Right)
		glVertex3f( 1.0f,-1.0f,-1.0f);			// Bottom Right Of The Quad (Right)
	glEnd();						
}

void genUnitObjects(GLuint *cubeList, GLuint *solidSphereList, 
                    GLuint *wireSphereList){
  const int N=8;

  GLuint index = glGenLists(3);

  glNewList(index, GL_COMPILE);
  unit_cube();
  glEndList();

  glNewList(index+1, GL_COMPILE);
  unit_sphere(N, N, 0);
  glEndList();

  glNewList(index+2, GL_COMPILE);
  unit_sphere(N, N, 1);
  glEndList();

  *cubeList = index;
  *solidSphereList = index+1;
  *wireSphereList = index+2;

}
