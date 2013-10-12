#include <windows.h>  
#include "glee/GLee.h"
#define GLUT_DISABLE_ATEXIT_HACK
#include <gl/glut.h> 
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <cmath>
#include "err.h"
#include "gbuffer.h"
#include "shader.h"
#include "objects.h"
#include "camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace tdogl;

#define HEIGHT 800
#define WIDTH 1200
#define SEED 0xbadc0de
#define PI (atanf(1.0)*4.0)

struct vec3 {
  float x,y,z;
};

typedef struct vec3 vec3_t;

struct Light {
  vec3_t color;
  vec3_t position;
  float radius;
};

typedef struct Light light_t;

struct Cube {
  vec3_t pos;
  vec3_t rot;
};

typedef struct Cube cube_t;

//==============================================================================
// globals
//==============================================================================
//

tdogl::Camera gCamera;

float	delta = 0.1, cdelta, light_z, gRotY;
unsigned int frameCount = 0, currentTime=0, previousTime=0;
float fps = 0;
int g_width=WIDTH, g_height=HEIGHT;

// boxes + omni lights shown on screen
#define N_CUBES 500
#define CUBE_SPHERE_RADIUS 6.0
#define N_LIGHTS 100
#define MAX_LIGHT_RADIUS 3.0

light_t gLights[N_LIGHTS];
cube_t gCubes[N_CUBES];
GLuint gQuadVertexbuffer;

GLuint gCubeList, gSolidSphereList, gWireSphereList;

//framebuffer and its textures: color, normal, position.
GBuffer *gBuf = new GBuffer();
GLuint mrtShader, deferredShader, omniShader, omniStencilShader, 
       quad_shader;
GLuint gTexDiffuse, gTexNormal, gTexPos;

void drawFPS();
void timer(int id);
//  Pointer to a font style..
//  Fonts supported by GLUT are: GLUT_BITMAP_8_BY_13, 
//  GLUT_BITMAP_9_BY_15, GLUT_BITMAP_TIMES_ROMAN_10, 
//  GLUT_BITMAP_TIMES_ROMAN_24, GLUT_BITMAP_HELVETICA_10,
//  GLUT_BITMAP_HELVETICA_12, and GLUT_BITMAP_HELVETICA_18.
GLvoid *font_style = GLUT_BITMAP_8_BY_13;

void draw_axes(float rot_x, float rot_y){
	unsigned int i, n;
	char	buf[128];

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
	glTranslatef(0, 0, -1);
	glRotatef(-rot_x, 1, 0, 0);
	glRotatef(-rot_y, 0, 1, 0);

	glColor3f(1,1,1);
	glRasterPos3f(1,0,0);
	glColor3f(1,1,0);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'x');
	glRasterPos3f(0,1,0);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'y');
	glRasterPos3f(0,0,1);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'z');
	glRasterPos3f(0,0,0);
	strcpy(buf, "O");
  n = strlen(buf);
	for(i = 0 ; i < n ; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, buf[i]);
	}

	glColor3f(1,0,0);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(1,0,0);
	glEnd();
	glColor3f(0,1,0);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(0,1,0);
	glEnd();
	glColor3f(0,0,1);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(0,0,1);
	glEnd();
  glPopMatrix();
}

void draw_sphere(vec3_t *v, float r, int is_solid){
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(v->x, v->y, v->z);
  glScalef(r, r, r);
  if(is_solid){
    //glutSolidSphere(1, n, n);
    glCallList(gSolidSphereList);
  }
  else{
    //glutWireSphere(1, n, n);
    glCallList(gWireSphereList);
  }
  glPopMatrix();
}

void InitGL ( void )     // Create Some Everyday Functions
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glEnable ( GL_COLOR_MATERIAL );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

}

void draw_cube(vec3_t *p, vec3_t *r){
	glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

	glRotatef(r->x,1.0f,0.0f,0.0f);	
	glRotatef(r->y+gRotY,0.0f,1.0f,0.0f);	
	glRotatef(r->z,0.0f,0.0f,1.0f);	
  glTranslatef(p->x, p->y, p->z);

  //glScalef(1.2, 1.2, 1.2);
  glCallList(gCubeList);

  glPopMatrix();
}

float randf(float m, float M){
  float r;
  r = (float)rand()/(float)RAND_MAX;
  return (m + r*(M-m));
}

void drawCubes(cube_t cubes[], int n){
  for (int i = 0; i < n; i++) {
    draw_cube(&cubes[i].pos, &cubes[i].rot);
  }
}

//First generate random points on the big sphere's surface, then
//move them away from the center, by multiplying their position by k>1
float rndPointOnSphere(float R, float max_r, vec3_t *pos){
  float r, k, phi, theta, z;

  r = randf(1.0, max_r);
  k = randf(1.0, 1.0+r/R);

  z = randf(-R, R);
  phi = randf(0.0, 2.0*PI);
  theta = asinf(z/R);
  pos->x = k*R*cosf(theta)*cosf(phi);
  pos->y = k*R*cosf(theta)*sinf(phi);
  pos->z = k*z;

  return r;
}

void genCubes(cube_t cubes[], int n){
  const float phi=90.0;
  int i;
  vec3_t pos, rot;

  rot.x = rot.y = rot.z = 0.0;
  for(i=0;i<n;){
    rndPointOnSphere(CUBE_SPHERE_RADIUS, 0, &pos);
    rot.x = randf(-phi, phi);
    rot.y = randf(-phi, phi);
    rot.z = randf(-phi, phi);
    cubes[i].pos = pos;
    cubes[i].rot = rot;
    i++;
  }
}

void genLights(light_t lights[], int n){
  light_t l;

  srand(SEED);
  for(int i=0;i<n;i++){
    l.radius = rndPointOnSphere(CUBE_SPHERE_RADIUS, MAX_LIGHT_RADIUS, 
                                &l.position);
    l.color.x = randf(0.0, 1.0);
    l.color.y = randf(0.0, 1.0);
    l.color.z = randf(0.0, 1.0);
    lights[i] = l;
  }
}

void setup_quad_vb(GLuint *qvb){
	// The fullscreen quad's FBO
	static const GLfloat g_quad_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	GLuint quad_vertexbuffer;
	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), 
               g_quad_vertex_buffer_data, GL_STATIC_DRAW);
  *qvb = quad_vertexbuffer;
}

void renderQuad(int x, int y, int width, int height){

  glViewport(x, y, width, height); 

  // 1st attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, gQuadVertexbuffer);
  glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  // Draw the triangles !
  // 2*3 indices starting at 0 -> 2 triangles
  glDrawArrays(GL_TRIANGLES, 0, 6); 

  glDisableVertexAttribArray(0);

  glViewport(0, 0, g_width, g_height);
}

void renderQuadTex(int x, int y, int width, int height, GLuint tex){

  // Use our shader
  shader_bind(quad_shader);
	GLuint in_tex_id = glGetUniformLocation(quad_shader, "in_tex");
  // Bind our texture in Texture Unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);
  // Set our "tex" sampler to user Texture Unit 0
  glUniform1i(in_tex_id, 0);
  
  renderQuad(x, y, width, height);

  shader_unbind();
}

void bind_tex_to_uni(GLuint shader, unsigned int tex_unit, GLuint tex, 
                    char *uniform_name){

	GLint in_tex_id = glGetUniformLocation(shader, uniform_name);
  assert(in_tex_id != -1);
  glActiveTexture(GL_TEXTURE0+tex_unit);
  glBindTexture(GL_TEXTURE_2D, tex);
  //point shader var to use texture unit "tex_unit"
  glUniform1i(in_tex_id, tex_unit);
}

void geom_pass(cube_t cubes[], light_t lights[]){
  int i;

  // Only the geometry pass updates the depth buffer
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  gBuf->bindForGeomPass();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader_bind(mrtShader);
  drawCubes(cubes, N_CUBES);
  //draw spheres where lights should be
  for(i=0;i<N_LIGHTS;i++){
    draw_sphere(&lights[i].position, 0.1, 1);
  }
  shader_unbind();

  glDepthMask(GL_FALSE); //do not update depth buffer
}

void bind_uni_to_3f(GLuint shader, char *uniform_name, float x, float y, 
                    float z){
  GLint loc = glGetUniformLocation(shader, uniform_name);
  assert(loc != -1);
  glUniform3f(loc, x, y, z);
}

void bind_uni_to_1f(GLuint shader, char *uniform_name, float r){
  GLint loc = glGetUniformLocation(shader, uniform_name);
  assert(loc != -1);
  glUniform1f(loc, r);
}

glm::vec3 worldToEye(vec3_t *v){
  GLfloat modelView[16]; 

  glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
  glm::mat4 mv = glm::make_mat4(modelView);
  glm::vec4 lightPos = glm::vec4(v->x, v->y, v->z, 1.0);
  return glm::vec3(mv * lightPos);
}

void ambientLightPass(GLuint texDiffuse, GLuint texNormal, 
                          GLuint texPos)
{
  //big radius to cover whole scene
  light_t l = {{0.2,0.2,0.2}, {6,6,6}, 20.0}; 
  vec3_t v,c;

  //we're going to draw over texFinal
  gBuf->bindForLightPass();

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  //render fullscreen quad
  shader_bind(deferredShader);
  v = l.position;
  c = l.color;
  glm::vec3 w = worldToEye(&v);
  bind_uni_to_3f(deferredShader, "light.position", w.x, w.y, w.z);
  bind_uni_to_3f(deferredShader, "light.color", c.x, c.y, c.z);
  bind_uni_to_1f(deferredShader, "light.radius", l.radius);
  bind_tex_to_uni(deferredShader, 0, texDiffuse, (char*)"texColor");
  bind_tex_to_uni(deferredShader, 1, texNormal, (char*)"texNormal");
  bind_tex_to_uni(deferredShader, 2, texPos, (char*)"texPos");
  renderQuad(0, 0, g_width, g_height);
  shader_unbind();

  glDisable(GL_BLEND);
}


void display ( void )   
{
  const int frac=4;
  light_t l;
  vec3_t v, c;
  int i;
  
  gBuf->clearTextures();

  geom_pass(gCubes, gLights);
  
  glEnable(GL_STENCIL_TEST);

  for(i=0;i<N_LIGHTS;i++){
    l = gLights[i];
    v = l.position;

    // stencil pass
    gBuf->bindForStencilPass();
    shader_bind(omniStencilShader);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClear(GL_STENCIL_BUFFER_BIT);
    // We need the stencil test to be enabled but we want it
    // to succeed always. Only the depth test matters.
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

    draw_sphere(&v, l.radius, 1);

    shader_unbind();

    // shading pass
    gBuf->bindForLightPass();
    shader_bind(omniShader);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
          
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
        
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    //renderQuadTex(0, 0, g_width, g_height, gTexDepth);

    //binding textures is done in reshape() function
    bind_tex_to_uni(omniShader, 0, gTexDiffuse, (char*)"texColor");
    bind_tex_to_uni(omniShader, 1, gTexNormal, (char*)"texNormal");
    bind_tex_to_uni(omniShader, 2, gTexPos, (char*)"texPos");

    glm::vec3 w = worldToEye(&v);
    c = l.color;
    bind_uni_to_3f(omniShader, "light.position", w.x, w.y, w.z);
    bind_uni_to_3f(omniShader, "light.color", c.x, c.y, c.z);
    bind_uni_to_1f(omniShader, "light.radius", l.radius);
    draw_sphere(&v, l.radius, 1);
    shader_unbind();
    glCullFace(GL_BACK);
		glDisable(GL_BLEND);
  }
  glDisable(GL_STENCIL_TEST);
  glDepthMask(GL_TRUE);

  // last light -- illuminate whole scene
  ambientLightPass(gTexDiffuse, gTexNormal, gTexPos);
  gBuf->bindForFinalPass();
  glBlitFramebuffer(0, 0, g_width, g_height, 0, 0, g_width, g_height, 
                    GL_COLOR_BUFFER_BIT, GL_LINEAR);

  // render debug info
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  renderQuadTex(0, 0, g_width/frac, g_height/frac, gTexDiffuse);
  renderQuadTex(g_width/frac, 0, g_width/frac, g_height/frac, gTexNormal);
  renderQuadTex(2*g_width/frac, 0, g_width/frac, g_height/frac, gTexPos);

  // Add FPS counter
  drawFPS();
  glutSwapBuffers();
  glutTimerFunc(0, timer, 0);
}

void reshape(int width, int height)   // Create The Reshape Function (the viewport)
{
  if (height==0)									
	{
		height=1;										
	}
  g_width = width;
  g_height = height;

	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();					

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();									
  gluLookAt(0, 0, 20, 0, 0, 0, 0, 1, 0);

  // setup gCamera
  gCamera.setPosition(glm::vec3(0,0,20));
  gCamera.setViewportAspectRatio((float)g_width / (float)g_height);

  gBuf->init(width, height);

  gTexDiffuse = gBuf->getTex(GBuffer::GBUFFER_TEX_DIFFUSE);
  gTexNormal = gBuf->getTex(GBuffer::GBUFFER_TEX_NORMAL);
  gTexPos = gBuf->getTex(GBuffer::GBUFFER_TEX_POSITION);

}

void keyboard ( unsigned char key, int x, int y )  // Create Keyboard Function
{
  switch ( key ) {
    case 27:        // When Escape Is Pressed...
      exit ( 0 );   // Exit The Program
      break;        // Ready For Next Case
    case 'a':
      cdelta = delta;
      delta = 0.0;
      break;
    case 'b':
      delta = cdelta;
      break;
    default:        // Now Wrap It Up
      break;
  }
}

void arrow_keys ( int a_keys, int x, int y )  // Create Special Function (required for arrow keys)
{
  switch ( a_keys ) {
    case GLUT_KEY_UP:     // When Up Arrow Is Pressed...
      glutFullScreen ( ); // Go Into Full Screen Mode
      break;
    case GLUT_KEY_DOWN:               // When Down Arrow Is Pressed...
      glutReshapeWindow ( WIDTH, HEIGHT ); // Go Into A HEIGHT By HEIGHT Window
      break;
    default:
      break;
  }
}

int _vscprintf(const char * format, va_list pargs) { 
    int retval; 
    va_list argcopy; 

    va_copy(argcopy, pargs); 
    retval = vsnprintf(NULL, 0, format, pargs); 
    va_end(argcopy); 
    return retval; 
 }

//-------------------------------------------------------------------------
//  Draws a string at the specified coordinates.
//-------------------------------------------------------------------------
void printw (float x, float y, float z, const char* format, ...)
{
	va_list args;	//  Variable argument list
	int len;		//	String length
	int i;			//  Iterator
	char * text;	//	Text

	va_start(args, format);
	len = _vscprintf(format, args) + 1; 
	text = new char[len];
	vsprintf(text, format, args);
	va_end(args);

  glPushAttrib(GL_LIGHTING_BIT);  
  glDisable(GL_LIGHTING);
  glColor3f(1,1,1); 

  glViewport(0, 0, g_width, g_height); 
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, g_width, 0, g_height, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  //glTranslated(x,y,0);
  glRasterPos3f(x,y,0);

	//  Draw the characters one by one
  for (i = 0; text[i] != '\0'; i++){
    glutBitmapCharacter(font_style, text[i]);
  }

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();   
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glPopAttrib();

	delete text;
}

void drawFPS()
{
	printw (0, 0, 0, "FPS: %4.2f", fps);
}

void calculateFPS()
{
  frameCount++;
  currentTime = glutGet(GLUT_ELAPSED_TIME);
  int timeInterval = currentTime - previousTime;
  if(timeInterval > 1000)
  {
    fps = frameCount / (timeInterval / 1000.0f);
    previousTime = currentTime;
    frameCount = 0;
  }
}

void timer(int id){
  gRotY += 0.1;
  calculateFPS();
  glutPostRedisplay ();
}

int main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
  glutInit            ( &argc, argv ); // Erm Just Write It =)
  glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE ); // Display Mode
  glutInitWindowSize  ( WIDTH, HEIGHT ); // If glutFullScreen wasn't called this is the window size
  glutCreateWindow    ( "Multiple render targets" ); // Window Title (argv[0] for current directory as title)
  //glutFullScreen      ( );          // Put Into Full Screen
  InitGL ();
  glutDisplayFunc     ( display );  // Matching Earlier Functions To Their Counterparts
  glutReshapeFunc     ( reshape );
  glutKeyboardFunc    ( keyboard );
  glutSpecialFunc     ( arrow_keys );
  //glutIdleFunc		  ( idle );

  mrtShader = create_shader("shaders/mrt.vert", "shaders/mrt.frag");
  deferredShader = create_shader("shaders/quad.vert", "shaders/omni.frag");
  omniShader = create_shader("shaders/minimal.vert", "shaders/omni.frag");
  omniStencilShader = create_shader("shaders/minimal.vert", 
                                    "shaders/omni.stencil.frag");
  quad_shader = create_shader("shaders/quad.tex.vert", 
                              "shaders/quad.tex.frag"); 

  srand(SEED);
  genCubes(gCubes, N_CUBES);
  genLights(gLights, N_LIGHTS);
  setup_quad_vb(&gQuadVertexbuffer);
  genUnitObjects(&gCubeList, &gSolidSphereList, &gWireSphereList);
  wglSwapIntervalEXT(0);

  glutMainLoop(); // Initialize The Main Loop
  return 0;
}

