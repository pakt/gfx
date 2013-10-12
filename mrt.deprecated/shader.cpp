#include <windows.h>   // Standard Header For Most Programs
#include "glee/GLee.h"
#define GLUT_DISABLE_ATEXIT_HACK
#include <gl/glut.h>   // The GL Utility Toolkit (Glut) Header
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "err.h"

char *read_file(const char *fn){
  FILE *f;
  char *buf;
  long lSize;

  f = fopen(fn, "rb");
  if(!f){
    fatal("Can't open: %s", fn);
  }

  fseek(f, 0, SEEK_END);
  lSize = ftell(f);
  rewind(f);

  buf = new char[lSize+1];
  if(lSize != (long)fread(buf, 1, lSize, f)){
    fatal("Error reading: %s", fn);
  }
  buf[lSize] = 0;

  fclose(f);
  return buf;
}

void print_info_log(GLuint obj)
{
  int infologLength1 = 0, infologLength2=0;
  int charsWritten  = 0;
  char *infoLog, *what;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength1);
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength2);

  if (infologLength1 + infologLength2 > 0)
  {
    if(glIsShader(obj) == GL_TRUE){
      infoLog = new char[infologLength1];
      what = (char*)"Shader info log";
      glGetShaderInfoLog(obj, infologLength1, &charsWritten, infoLog);
    }
    else if(glIsProgram(obj) == GL_TRUE){
      infoLog = new char[infologLength2];
      what = (char*)"Program info log";
      glGetProgramInfoLog(obj, infologLength2, &charsWritten, infoLog);
    }
    else{
      assert(0);
    }
    fprintf(stderr, "========== %s\n", what);
    fprintf(stderr, "%s\n",infoLog);
    fprintf(stderr, "===fin====\n");
    delete infoLog;
  }
}

/*
 * Returns a shader object containing a shader
 * compiled from the given GLSL shader file.
 */
static GLuint
compile_from_file(GLenum type, const char *filePath)
{
	char *source;
	GLuint shader;
	GLint length, result;

	/* get shader source */
	source = read_file(filePath);
	if(!source){
    fatal("Can't load shader source");
		return 0;
  }

	/* create shader object, set the source, and compile */
	shader = glCreateShader(type);
	length = strlen(source);
	glShaderSource(shader, 1, (const char **)&source, &length);
	glCompileShader(shader);
	delete source;

	/* make sure the compilation was successful */
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE) {
    print_info_log(shader);
		fatal("shaderCompileFromFile(): Unable to compile %s\n", filePath);
		return 0;
	}

	return shader;
}

GLuint create_shader(const char *fn_vert, const char *fn_frag){

  GLuint v,f,p;
  GLint status;

  v = compile_from_file(GL_VERTEX_SHADER, fn_vert);
  f = compile_from_file(GL_FRAGMENT_SHADER, fn_frag);

  p = glCreateProgram();
  glAttachShader(p, v);
  glAttachShader(p, f);

  glLinkProgram(p);
  glGetProgramiv(p, GL_LINK_STATUS, &status);
  if(status == GL_FALSE){
    print_info_log(p);
    fatal("create_shader(%s, %s): linking failed\n", fn_vert, fn_frag);
  }

  return p;
}

void shader_bind(GLuint p){
  glUseProgram(p);
}

void shader_unbind(){
  glUseProgram(0);
}
