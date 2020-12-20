
#include "tjsCommHead.h"

#include "OpenGLHeaderSDL2.h"

#ifndef __EMSCRIPTEN__
///libEGL
#ifdef EGLAPI
#undef EGLAPI
#endif
#define EGLAPI

extern "C" {
EGLAPI EGLContext (EGLAPIENTRY* eglGetCurrentContext)( void );
EGLAPI EGLBoolean (EGLAPIENTRY* eglChooseConfig)(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);
EGLAPI EGLBoolean (EGLAPIENTRY* eglCopyBuffers)(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target);
EGLAPI EGLContext (EGLAPIENTRY* eglCreateContext)(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list);
EGLAPI EGLSurface (EGLAPIENTRY* eglCreatePbufferSurface)(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list);
EGLAPI EGLSurface (EGLAPIENTRY* eglCreatePixmapSurface)(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list);
EGLAPI EGLSurface (EGLAPIENTRY* eglCreateWindowSurface)(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list);
EGLAPI EGLBoolean (EGLAPIENTRY* eglDestroyContext)(EGLDisplay dpy, EGLContext ctx);
EGLAPI EGLBoolean (EGLAPIENTRY* eglDestroySurface)(EGLDisplay dpy, EGLSurface surface);
EGLAPI EGLBoolean (EGLAPIENTRY* eglGetConfigAttrib)(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value);
EGLAPI EGLBoolean (EGLAPIENTRY* eglGetConfigs)(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config);
EGLAPI EGLDisplay (EGLAPIENTRY* eglGetCurrentDisplay)(void);
EGLAPI EGLSurface (EGLAPIENTRY* eglGetCurrentSurface)(EGLint readdraw);
EGLAPI EGLDisplay (EGLAPIENTRY* eglGetDisplay)(EGLNativeDisplayType display_id);
EGLAPI EGLint (EGLAPIENTRY* eglGetError)(void);
EGLAPI __eglMustCastToProperFunctionPointerType (EGLAPIENTRY* eglGetProcAddress)(const char *procname);
EGLAPI EGLBoolean (EGLAPIENTRY* eglInitialize)(EGLDisplay dpy, EGLint *major, EGLint *minor);
EGLAPI EGLBoolean (EGLAPIENTRY* eglMakeCurrent)(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
EGLAPI EGLBoolean (EGLAPIENTRY* eglQueryContext)(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value);
EGLAPI const char *(EGLAPIENTRY* eglQueryString)(EGLDisplay dpy, EGLint name);
EGLAPI EGLBoolean (EGLAPIENTRY* eglQuerySurface)(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value);
EGLAPI EGLBoolean (EGLAPIENTRY* eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLAPI EGLBoolean (EGLAPIENTRY* eglTerminate)(EGLDisplay dpy);
EGLAPI EGLBoolean (EGLAPIENTRY* eglWaitGL)(void);
EGLAPI EGLBoolean (EGLAPIENTRY* eglWaitNative)(EGLint engine);

EGLAPI EGLBoolean (EGLAPIENTRY* eglBindTexImage)(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
EGLAPI EGLBoolean (EGLAPIENTRY* eglReleaseTexImage)(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
EGLAPI EGLBoolean (EGLAPIENTRY* eglSurfaceAttrib)(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);
EGLAPI EGLBoolean (EGLAPIENTRY* eglSwapInterval)(EGLDisplay dpy, EGLint interval);

EGLAPI EGLBoolean (EGLAPIENTRY* eglBindAPI)(EGLenum api);
EGLAPI EGLenum (EGLAPIENTRY* eglQueryAPI)(void);
EGLAPI EGLSurface (EGLAPIENTRY* eglCreatePbufferFromClientBuffer)(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list);
EGLAPI EGLBoolean (EGLAPIENTRY* eglReleaseThread)(void);
EGLAPI EGLBoolean (EGLAPIENTRY* eglWaitClient)(void);

EGLAPI EGLDisplay (EGLAPIENTRY* eglGetPlatformDisplayEXT)(EGLenum platform, void *native_display, const EGLint *attrib_list);
}

#ifdef GL_APICALL
#undef GL_APICALL
#endif
#define GL_APICALL
extern "C" {
// 2.0
GL_APICALL void (GL_APIENTRY* glActiveTexture)(GLenum texture);
GL_APICALL void (GL_APIENTRY* glAttachShader)(GLuint program, GLuint shader);
GL_APICALL void (GL_APIENTRY* glBindAttribLocation)(GLuint program, GLuint index, const GLchar *name);
GL_APICALL void (GL_APIENTRY* glBindBuffer)(GLenum target, GLuint buffer);
GL_APICALL void (GL_APIENTRY* glBindFramebuffer)(GLenum target, GLuint framebuffer);
GL_APICALL void (GL_APIENTRY* glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
GL_APICALL void (GL_APIENTRY* glBindTexture)(GLenum target, GLuint texture);
GL_APICALL void (GL_APIENTRY* glBlendColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GL_APICALL void (GL_APIENTRY* glBlendEquation)(GLenum mode);
GL_APICALL void (GL_APIENTRY* glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
GL_APICALL void (GL_APIENTRY* glBlendFunc)(GLenum sfactor, GLenum dfactor);
GL_APICALL void (GL_APIENTRY* glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
GL_APICALL void (GL_APIENTRY* glBufferData)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
GL_APICALL void (GL_APIENTRY* glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
GL_APICALL GLenum (GL_APIENTRY* glCheckFramebufferStatus)(GLenum target);
GL_APICALL void (GL_APIENTRY* glClear)(GLbitfield mask);
GL_APICALL void (GL_APIENTRY* glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GL_APICALL void (GL_APIENTRY* glClearDepthf)(GLfloat d);
GL_APICALL void (GL_APIENTRY* glClearStencil)(GLint s);
GL_APICALL void (GL_APIENTRY* glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
GL_APICALL void (GL_APIENTRY* glCompileShader)(GLuint shader);
GL_APICALL void (GL_APIENTRY* glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
GL_APICALL void (GL_APIENTRY* glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
GL_APICALL void (GL_APIENTRY* glCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
GL_APICALL void (GL_APIENTRY* glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GL_APICALL GLuint (GL_APIENTRY* glCreateProgram)(void);
GL_APICALL GLuint (GL_APIENTRY* glCreateShader)(GLenum type);
GL_APICALL void (GL_APIENTRY* glCullFace)(GLenum mode);
GL_APICALL void (GL_APIENTRY* glDeleteBuffers)(GLsizei n, const GLuint *buffers);
GL_APICALL void (GL_APIENTRY* glDeleteFramebuffers)(GLsizei n, const GLuint *framebuffers);
GL_APICALL void (GL_APIENTRY* glDeleteProgram)(GLuint program);
GL_APICALL void (GL_APIENTRY* glDeleteRenderbuffers)(GLsizei n, const GLuint *renderbuffers);
GL_APICALL void (GL_APIENTRY* glDeleteShader)(GLuint shader);
GL_APICALL void (GL_APIENTRY* glDeleteTextures)(GLsizei n, const GLuint *textures);
GL_APICALL void (GL_APIENTRY* glDepthFunc)(GLenum func);
GL_APICALL void (GL_APIENTRY* glDepthMask)(GLboolean flag);
GL_APICALL void (GL_APIENTRY* glDepthRangef)(GLfloat n, GLfloat f);
GL_APICALL void (GL_APIENTRY* glDetachShader)(GLuint program, GLuint shader);
GL_APICALL void (GL_APIENTRY* glDisable)(GLenum cap);
GL_APICALL void (GL_APIENTRY* glDisableVertexAttribArray)(GLuint index);
GL_APICALL void (GL_APIENTRY* glDrawArrays)(GLenum mode, GLint first, GLsizei count);
GL_APICALL void (GL_APIENTRY* glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void *indices);
GL_APICALL void (GL_APIENTRY* glEnable)(GLenum cap);
GL_APICALL void (GL_APIENTRY* glEnableVertexAttribArray)(GLuint index);
GL_APICALL void (GL_APIENTRY* glFinish)(void);
GL_APICALL void (GL_APIENTRY* glFlush)(void);
GL_APICALL void (GL_APIENTRY* glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
GL_APICALL void (GL_APIENTRY* glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GL_APICALL void (GL_APIENTRY* glFrontFace)(GLenum mode);
GL_APICALL void (GL_APIENTRY* glGenBuffers)(GLsizei n, GLuint *buffers);
GL_APICALL void (GL_APIENTRY* glGenerateMipmap)(GLenum target);
GL_APICALL void (GL_APIENTRY* glGenFramebuffers)(GLsizei n, GLuint *framebuffers);
GL_APICALL void (GL_APIENTRY* glGenRenderbuffers)(GLsizei n, GLuint *renderbuffers);
GL_APICALL void (GL_APIENTRY* glGenTextures)(GLsizei n, GLuint *textures);
GL_APICALL void (GL_APIENTRY* glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
GL_APICALL void (GL_APIENTRY* glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
GL_APICALL void (GL_APIENTRY* glGetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
GL_APICALL GLint (GL_APIENTRY* glGetAttribLocation)(GLuint program, const GLchar *name);
GL_APICALL void (GL_APIENTRY* glGetBooleanv)(GLenum pname, GLboolean *data);
GL_APICALL void (GL_APIENTRY* glGetBufferParameteriv)(GLenum target, GLenum pname, GLint *params);
GL_APICALL GLenum (GL_APIENTRY* glGetError)(void);
GL_APICALL void (GL_APIENTRY* glGetFloatv)(GLenum pname, GLfloat *data);
GL_APICALL void (GL_APIENTRY* glGetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint *params);
GL_APICALL void (GL_APIENTRY* glGetIntegerv)(GLenum pname, GLint *data);
GL_APICALL void (GL_APIENTRY* glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
GL_APICALL void (GL_APIENTRY* glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
GL_APICALL void (GL_APIENTRY* glGetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint *params);
GL_APICALL void (GL_APIENTRY* glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
GL_APICALL void (GL_APIENTRY* glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
GL_APICALL void (GL_APIENTRY* glGetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);
GL_APICALL void (GL_APIENTRY* glGetShaderSource)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
GL_APICALL const GLubyte *(GL_APIENTRY* glGetString)(GLenum name);
GL_APICALL void (GL_APIENTRY* glGetTexParameterfv)(GLenum target, GLenum pname, GLfloat *params);
GL_APICALL void (GL_APIENTRY* glGetTexParameteriv)(GLenum target, GLenum pname, GLint *params);
GL_APICALL void (GL_APIENTRY* glGetUniformfv)(GLuint program, GLint location, GLfloat *params);
GL_APICALL void (GL_APIENTRY* glGetUniformiv)(GLuint program, GLint location, GLint *params);
GL_APICALL GLint (GL_APIENTRY* glGetUniformLocation)(GLuint program, const GLchar *name);
GL_APICALL void (GL_APIENTRY* glGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat *params);
GL_APICALL void (GL_APIENTRY* glGetVertexAttribiv)(GLuint index, GLenum pname, GLint *params);
GL_APICALL void (GL_APIENTRY* glGetVertexAttribPointerv)(GLuint index, GLenum pname, void **pointer);
GL_APICALL void (GL_APIENTRY* glHint)(GLenum target, GLenum mode);
GL_APICALL GLboolean (GL_APIENTRY* glIsBuffer)(GLuint buffer);
GL_APICALL GLboolean (GL_APIENTRY* glIsEnabled)(GLenum cap);
GL_APICALL GLboolean (GL_APIENTRY* glIsFramebuffer)(GLuint framebuffer);
GL_APICALL GLboolean (GL_APIENTRY* glIsProgram)(GLuint program);
GL_APICALL GLboolean (GL_APIENTRY* glIsRenderbuffer)(GLuint renderbuffer);
GL_APICALL GLboolean (GL_APIENTRY* glIsShader)(GLuint shader);
GL_APICALL GLboolean (GL_APIENTRY* glIsTexture)(GLuint texture);
GL_APICALL void (GL_APIENTRY* glLineWidth)(GLfloat width);
GL_APICALL void (GL_APIENTRY* glLinkProgram)(GLuint program);
GL_APICALL void (GL_APIENTRY* glPixelStorei)(GLenum pname, GLint param);
GL_APICALL void (GL_APIENTRY* glPolygonOffset)(GLfloat factor, GLfloat units);
GL_APICALL void (GL_APIENTRY* glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
GL_APICALL void (GL_APIENTRY* glReleaseShaderCompiler)(void);
GL_APICALL void (GL_APIENTRY* glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
GL_APICALL void (GL_APIENTRY* glSampleCoverage)(GLfloat value, GLboolean invert);
GL_APICALL void (GL_APIENTRY* glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
GL_APICALL void (GL_APIENTRY* glShaderBinary)(GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length);
GL_APICALL void (GL_APIENTRY* glShaderSource)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
GL_APICALL void (GL_APIENTRY* glStencilFunc)(GLenum func, GLint ref, GLuint mask);
GL_APICALL void (GL_APIENTRY* glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
GL_APICALL void (GL_APIENTRY* glStencilMask)(GLuint mask);
GL_APICALL void (GL_APIENTRY* glStencilMaskSeparate)(GLenum face, GLuint mask);
GL_APICALL void (GL_APIENTRY* glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
GL_APICALL void (GL_APIENTRY* glStencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
GL_APICALL void (GL_APIENTRY* glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
GL_APICALL void (GL_APIENTRY* glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
GL_APICALL void (GL_APIENTRY* glTexParameterfv)(GLenum target, GLenum pname, const GLfloat *params);
GL_APICALL void (GL_APIENTRY* glTexParameteri)(GLenum target, GLenum pname, GLint param);
GL_APICALL void (GL_APIENTRY* glTexParameteriv)(GLenum target, GLenum pname, const GLint *params);
GL_APICALL void (GL_APIENTRY* glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
GL_APICALL void (GL_APIENTRY* glUniform1f)(GLint location, GLfloat v0);
GL_APICALL void (GL_APIENTRY* glUniform1fv)(GLint location, GLsizei count, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUniform1i)(GLint location, GLint v0);
GL_APICALL void (GL_APIENTRY* glUniform1iv)(GLint location, GLsizei count, const GLint *value);
GL_APICALL void (GL_APIENTRY* glUniform2f)(GLint location, GLfloat v0, GLfloat v1);
GL_APICALL void (GL_APIENTRY* glUniform2fv)(GLint location, GLsizei count, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUniform2i)(GLint location, GLint v0, GLint v1);
GL_APICALL void (GL_APIENTRY* glUniform2iv)(GLint location, GLsizei count, const GLint *value);
GL_APICALL void (GL_APIENTRY* glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GL_APICALL void (GL_APIENTRY* glUniform3fv)(GLint location, GLsizei count, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);
GL_APICALL void (GL_APIENTRY* glUniform3iv)(GLint location, GLsizei count, const GLint *value);
GL_APICALL void (GL_APIENTRY* glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GL_APICALL void (GL_APIENTRY* glUniform4fv)(GLint location, GLsizei count, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
GL_APICALL void (GL_APIENTRY* glUniform4iv)(GLint location, GLsizei count, const GLint *value);
GL_APICALL void (GL_APIENTRY* glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUseProgram)(GLuint program);
GL_APICALL void (GL_APIENTRY* glValidateProgram)(GLuint program);
GL_APICALL void (GL_APIENTRY* glVertexAttrib1f)(GLuint index, GLfloat x);
GL_APICALL void (GL_APIENTRY* glVertexAttrib1fv)(GLuint index, const GLfloat *v);
GL_APICALL void (GL_APIENTRY* glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
GL_APICALL void (GL_APIENTRY* glVertexAttrib2fv)(GLuint index, const GLfloat *v);
GL_APICALL void (GL_APIENTRY* glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
GL_APICALL void (GL_APIENTRY* glVertexAttrib3fv)(GLuint index, const GLfloat *v);
GL_APICALL void (GL_APIENTRY* glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GL_APICALL void (GL_APIENTRY* glVertexAttrib4fv)(GLuint index, const GLfloat *v);
GL_APICALL void (GL_APIENTRY* glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
GL_APICALL void (GL_APIENTRY* glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

// 3.0
GL_APICALL void (GL_APIENTRY* glReadBuffer)(GLenum src);
GL_APICALL void (GL_APIENTRY* glDrawRangeElements)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
GL_APICALL void (GL_APIENTRY* glTexImage3D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
GL_APICALL void (GL_APIENTRY* glTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
GL_APICALL void (GL_APIENTRY* glCopyTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GL_APICALL void (GL_APIENTRY* glCompressedTexImage3D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
GL_APICALL void (GL_APIENTRY* glCompressedTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
GL_APICALL void (GL_APIENTRY* glGenQueries)(GLsizei n, GLuint *ids);
GL_APICALL void (GL_APIENTRY* glDeleteQueries)(GLsizei n, const GLuint *ids);
GL_APICALL GLboolean (GL_APIENTRY* glIsQuery)(GLuint id);
GL_APICALL void (GL_APIENTRY* glBeginQuery)(GLenum target, GLuint id);
GL_APICALL void (GL_APIENTRY* glEndQuery)(GLenum target);
GL_APICALL void (GL_APIENTRY* glGetQueryiv)(GLenum target, GLenum pname, GLint *params);
GL_APICALL void (GL_APIENTRY* glGetQueryObjectuiv)(GLuint id, GLenum pname, GLuint *params);
GL_APICALL GLboolean (GL_APIENTRY* glUnmapBuffer)(GLenum target);
GL_APICALL void (GL_APIENTRY* glGetBufferPointerv)(GLenum target, GLenum pname, void **params);
GL_APICALL void (GL_APIENTRY* glDrawBuffers)(GLsizei n, const GLenum *bufs);
GL_APICALL void (GL_APIENTRY* glUniformMatrix2x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUniformMatrix3x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUniformMatrix2x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUniformMatrix4x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUniformMatrix3x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glUniformMatrix4x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
GL_APICALL void (GL_APIENTRY* glRenderbufferStorageMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
GL_APICALL void (GL_APIENTRY* glFramebufferTextureLayer)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
GL_APICALL void *(GL_APIENTRY* glMapBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
GL_APICALL void (GL_APIENTRY* glFlushMappedBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length);
GL_APICALL void (GL_APIENTRY* glBindVertexArray)(GLuint array);
GL_APICALL void (GL_APIENTRY* glDeleteVertexArrays)(GLsizei n, const GLuint *arrays);
GL_APICALL void (GL_APIENTRY* glGenVertexArrays)(GLsizei n, GLuint *arrays);
GL_APICALL GLboolean (GL_APIENTRY* glIsVertexArray)(GLuint array);
GL_APICALL void (GL_APIENTRY* glGetIntegeri_v)(GLenum target, GLuint index, GLint *data);
GL_APICALL void (GL_APIENTRY* glBeginTransformFeedback)(GLenum primitiveMode);
GL_APICALL void (GL_APIENTRY* glEndTransformFeedback)(void);
GL_APICALL void (GL_APIENTRY* glBindBufferRange)(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
GL_APICALL void (GL_APIENTRY* glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);
GL_APICALL void (GL_APIENTRY* glTransformFeedbackVaryings)(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode);
GL_APICALL void (GL_APIENTRY* glGetTransformFeedbackVarying)(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
GL_APICALL void (GL_APIENTRY* glVertexAttribIPointer)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
GL_APICALL void (GL_APIENTRY* glGetVertexAttribIiv)(GLuint index, GLenum pname, GLint *params);
GL_APICALL void (GL_APIENTRY* glGetVertexAttribIuiv)(GLuint index, GLenum pname, GLuint *params);
GL_APICALL void (GL_APIENTRY* glVertexAttribI4i)(GLuint index, GLint x, GLint y, GLint z, GLint w);
GL_APICALL void (GL_APIENTRY* glVertexAttribI4ui)(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
GL_APICALL void (GL_APIENTRY* glVertexAttribI4iv)(GLuint index, const GLint *v);
GL_APICALL void (GL_APIENTRY* glVertexAttribI4uiv)(GLuint index, const GLuint *v);
GL_APICALL void (GL_APIENTRY* glGetUniformuiv)(GLuint program, GLint location, GLuint *params);
GL_APICALL GLint (GL_APIENTRY* glGetFragDataLocation)(GLuint program, const GLchar *name);
GL_APICALL void (GL_APIENTRY* glUniform1ui)(GLint location, GLuint v0);
GL_APICALL void (GL_APIENTRY* glUniform2ui)(GLint location, GLuint v0, GLuint v1);
GL_APICALL void (GL_APIENTRY* glUniform3ui)(GLint location, GLuint v0, GLuint v1, GLuint v2);
GL_APICALL void (GL_APIENTRY* glUniform4ui)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
GL_APICALL void (GL_APIENTRY* glUniform1uiv)(GLint location, GLsizei count, const GLuint *value);
GL_APICALL void (GL_APIENTRY* glUniform2uiv)(GLint location, GLsizei count, const GLuint *value);
GL_APICALL void (GL_APIENTRY* glUniform3uiv)(GLint location, GLsizei count, const GLuint *value);
GL_APICALL void (GL_APIENTRY* glUniform4uiv)(GLint location, GLsizei count, const GLuint *value);
GL_APICALL void (GL_APIENTRY* glClearBufferiv)(GLenum buffer, GLint drawbuffer, const GLint *value);
GL_APICALL void (GL_APIENTRY* glClearBufferuiv)(GLenum buffer, GLint drawbuffer, const GLuint *value);
GL_APICALL void (GL_APIENTRY* glClearBufferfv)(GLenum buffer, GLint drawbuffer, const GLfloat *value);
GL_APICALL void (GL_APIENTRY* glClearBufferfi)(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
GL_APICALL const GLubyte *(GL_APIENTRY* glGetStringi)(GLenum name, GLuint index);
GL_APICALL void (GL_APIENTRY* glCopyBufferSubData)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
GL_APICALL void (GL_APIENTRY* glGetUniformIndices)(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);
GL_APICALL void (GL_APIENTRY* glGetActiveUniformsiv)(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
GL_APICALL GLuint (GL_APIENTRY* glGetUniformBlockIndex)(GLuint program, const GLchar *uniformBlockName);
GL_APICALL void (GL_APIENTRY* glGetActiveUniformBlockiv)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
GL_APICALL void (GL_APIENTRY* glGetActiveUniformBlockName)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
GL_APICALL void (GL_APIENTRY* glUniformBlockBinding)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
GL_APICALL void (GL_APIENTRY* glDrawArraysInstanced)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
GL_APICALL void (GL_APIENTRY* glDrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
GL_APICALL GLsync (GL_APIENTRY* glFenceSync)(GLenum condition, GLbitfield flags);
GL_APICALL GLboolean (GL_APIENTRY* glIsSync)(GLsync sync);
GL_APICALL void (GL_APIENTRY* glDeleteSync)(GLsync sync);
GL_APICALL GLenum (GL_APIENTRY* glClientWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
GL_APICALL void (GL_APIENTRY* glWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
GL_APICALL void (GL_APIENTRY* glGetInteger64v)(GLenum pname, GLint64 *data);
GL_APICALL void (GL_APIENTRY* glGetSynciv)(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);
GL_APICALL void (GL_APIENTRY* glGetInteger64i_v)(GLenum target, GLuint index, GLint64 *data);
GL_APICALL void (GL_APIENTRY* glGetBufferParameteri64v)(GLenum target, GLenum pname, GLint64 *params);
GL_APICALL void (GL_APIENTRY* glGenSamplers)(GLsizei count, GLuint *samplers);
GL_APICALL void (GL_APIENTRY* glDeleteSamplers)(GLsizei count, const GLuint *samplers);
GL_APICALL GLboolean (GL_APIENTRY* glIsSampler)(GLuint sampler);
GL_APICALL void (GL_APIENTRY* glBindSampler)(GLuint unit, GLuint sampler);
GL_APICALL void (GL_APIENTRY* glSamplerParameteri)(GLuint sampler, GLenum pname, GLint param);
GL_APICALL void (GL_APIENTRY* glSamplerParameteriv)(GLuint sampler, GLenum pname, const GLint *param);
GL_APICALL void (GL_APIENTRY* glSamplerParameterf)(GLuint sampler, GLenum pname, GLfloat param);
GL_APICALL void (GL_APIENTRY* glSamplerParameterfv)(GLuint sampler, GLenum pname, const GLfloat *param);
GL_APICALL void (GL_APIENTRY* glGetSamplerParameteriv)(GLuint sampler, GLenum pname, GLint *params);
GL_APICALL void (GL_APIENTRY* glGetSamplerParameterfv)(GLuint sampler, GLenum pname, GLfloat *params);
GL_APICALL void (GL_APIENTRY* glVertexAttribDivisor)(GLuint index, GLuint divisor);
GL_APICALL void (GL_APIENTRY* glBindTransformFeedback)(GLenum target, GLuint id);
GL_APICALL void (GL_APIENTRY* glDeleteTransformFeedbacks)(GLsizei n, const GLuint *ids);
GL_APICALL void (GL_APIENTRY* glGenTransformFeedbacks)(GLsizei n, GLuint *ids);
GL_APICALL GLboolean (GL_APIENTRY* glIsTransformFeedback)(GLuint id);
GL_APICALL void (GL_APIENTRY* glPauseTransformFeedback)(void);
GL_APICALL void (GL_APIENTRY* glResumeTransformFeedback)(void);
GL_APICALL void (GL_APIENTRY* glGetProgramBinary)(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
GL_APICALL void (GL_APIENTRY* glProgramBinary)(GLuint program, GLenum binaryFormat, const void *binary, GLsizei length);
GL_APICALL void (GL_APIENTRY* glProgramParameteri)(GLuint program, GLenum pname, GLint value);
GL_APICALL void (GL_APIENTRY* glInvalidateFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum *attachments);
GL_APICALL void (GL_APIENTRY* glInvalidateSubFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
GL_APICALL void (GL_APIENTRY* glTexStorage2D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
GL_APICALL void (GL_APIENTRY* glTexStorage3D)(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
GL_APICALL void (GL_APIENTRY* glGetInternalformativ)(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params);
}
#endif

static bool TVPANGLEInit = false;
int TVPOpenGLESVersion = 200;
void TVPInitializeOpenGLPlatform()
{
	// Stubbed for now
}
void* TVPeglGetProcAddress(const char *procname)
{
	return NULL;
}
