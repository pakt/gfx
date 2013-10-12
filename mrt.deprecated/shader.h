#ifndef SHADER_H
#define SHADER_H

GLuint create_shader(const char *fn_vert, const char *fn_frag);
void shader_bind(GLuint p);
void shader_unbind();

#endif /* end of include guard: SHADER_H */
