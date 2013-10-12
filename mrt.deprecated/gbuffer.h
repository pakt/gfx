#ifndef GBUFFER_H
#define GBUFFER_H

#include "glee/GLee.h"

class GBuffer
{
public:

    enum GBUFFER_TEXTURE_TYPE {
            GBUFFER_TEX_DIFFUSE,
            GBUFFER_TEX_NORMAL,
            GBUFFER_TEX_POSITION,
            GBUFFER_TEX_FINAL,
            GBUFFER_NUM_TEXTURES
    };

    GBuffer();

    ~GBuffer();

    void init(unsigned int width, unsigned int height);

    void clearTextures();
    void bindForGeomPass();
    void bindForStencilPass();
    void bindForLightPass();
    void bindForFinalPass();

    GLuint getTex(int id);

private:
    GLuint genTexture(int, int);

    GLuint m_fbo;
    GLuint m_textures[GBUFFER_NUM_TEXTURES];
    GLuint m_texDepthStencil;
};



#endif /* end of include guard: GBUFFER_H */
