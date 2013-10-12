#include <stdio.h>
#include <assert.h>
#include "gbuffer.h"
#include "err.h"

GBuffer::GBuffer(){
  m_fbo = 0;
  m_texDepthStencil = 0;
}

GBuffer::~GBuffer(){
}

GLuint GBuffer::genTexture(int w, int h){
	GLuint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	// Give an empty image to OpenGL ( the last "0" means "empty" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, 
               GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  return tex;
}

void GBuffer::init(unsigned int width, unsigned int height){
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

  for (int i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
    m_textures[i] = genTexture(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
                           GL_TEXTURE_2D, m_textures[i], 0);
  }
  // depth / stencil
  glGenTextures(1, &m_texDepthStencil);
  glBindTexture(GL_TEXTURE_2D, m_texDepthStencil);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, width, height, 0, 
      GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
      GL_TEXTURE_2D, m_texDepthStencil, 0);
  
  // Always check that our framebuffer is ok
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    fatal("GBuffer::init: setting framebuffer failed");
  }
}


void GBuffer::clearTextures(){
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
  for (int i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
    glDrawBuffer(GL_COLOR_ATTACHMENT0+i);
    glClear(GL_COLOR_BUFFER_BIT);
  }
  glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void GBuffer::bindForGeomPass(){
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  //diffuse, normal, position
	GLenum drawBuffers[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                          GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, drawBuffers); 
}

void GBuffer::bindForStencilPass(){
  //we are writing to stencil buffer only
	glDrawBuffer(GL_NONE);
}

void GBuffer::bindForLightPass(){
  //blend omni lights contributions to final texture
  glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEX_FINAL);
}

void GBuffer::bindForFinalPass(){
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
  glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEX_FINAL);
}

GLuint GBuffer::getTex(int texId){
  assert(0<= texId && texId < GBUFFER_NUM_TEXTURES);
  return m_textures[texId];
}

