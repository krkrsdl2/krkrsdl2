
#include "tjsCommHead.h"
#include "OpenGLScreenSDL2.h"
#include <assert.h>
#include "DebugIntf.h"
#include "GLTexture.h"

extern int TVPOpenGLESVersion;

tTVPOpenGLScreen::tTVPOpenGLScreen( TTVPWindowForm* form ) : Form(form),
mDefaultFrameBufferId(0)
{
	Form = form;
	mDefaultFrameBufferId = 0;
	Form->SetOpenGLScreen(this);
}
bool tTVPOpenGLScreen::Initialize() {
	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &mDefaultFrameBufferId );
	return true;
}

void tTVPOpenGLScreen::Destroy() {
}
bool tTVPOpenGLScreen::IsInitialized() const {
	return true;
}

void tTVPOpenGLScreen::Swap() {
	Form->Swap();
}
EGLint tTVPOpenGLScreen::GetSurfaceWidth() const {
#if 0
	int w, h;
	Form->GetDrawableSize(w, h);
	return w;
#endif
	return Form->GetWidth();
}
EGLint tTVPOpenGLScreen::GetSurfaceHeight() const {
#if 0
	int w, h;
	Form->GetDrawableSize(w, h);
	return h;
#endif
	return Form->GetHeight();
}
void tTVPOpenGLScreen::GetDrawableSize(EGLint &w, EGLint &h) const {
	Form->GetDrawableSize(w, h);
}
bool tTVPOpenGLScreen::CheckGLErrorAndLog(const tjs_char* funcname) {
	GLenum error_code = glGetError();
	if( error_code == GL_NO_ERROR ) return true;
	if( funcname != nullptr ) {
		TVPAddLog( ttstr(funcname) );
	}
	switch( error_code ) {
	case GL_INVALID_ENUM: TVPAddLog( TJS_W( "GL error : GL_INVALID_ENUM." ) ); break;
	case GL_INVALID_VALUE: TVPAddLog( TJS_W( "GL error : GL_INVALID_VALUE." ) ); break;
	case GL_INVALID_OPERATION: TVPAddLog( TJS_W( "GL error : GL_INVALID_OPERATION." ) ); break;
	case GL_OUT_OF_MEMORY: TVPAddLog( TJS_W( "GL error : GL_OUT_OF_MEMORY." ) ); break;
	case GL_INVALID_FRAMEBUFFER_OPERATION: TVPAddLog( TJS_W( "GL error : GL_INVALID_FRAMEBUFFER_OPERATION." ) ); break;
	default: TVPAddLog( (tjs_string(TJS_W( "GL error code : " )) + to_tjs_string( (unsigned long)error_code )).c_str() ); break;
	}
	return false;
}
void tTVPOpenGLScreen::SetScissorRect( const tTVPRect& rect ) {
	// Scissorは左下原点
	int width, height;
	Form->GetDrawableSize(width, height);
	glScissor( rect.left, height-rect.bottom, rect.get_width(), rect.get_height() );
	glEnable( GL_SCISSOR_TEST );
}
void tTVPOpenGLScreen::DisableScissorRect() {
	glDisable( GL_SCISSOR_TEST );
}
void tTVPOpenGLScreen::SetWaitVSync( bool b ) {
	Form->SetSwapInterval(b ? 1 : 0);
}
