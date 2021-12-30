
#ifndef OpenGLScreenH
#define OpenGLScreenH

#include <SDL.h>

#include "OpenGLHeader.h"
#include "ComplexRect.h"
#include "WindowImpl.h"

class tTVPOpenGLScreen
{
	TTVPWindowForm* Form;

	GLint mDefaultFrameBufferId;
public:
	tTVPOpenGLScreen( TTVPWindowForm* form );

	bool Initialize();
	void Destroy();
	bool IsInitialized() const;

	void Swap();

	EGLint GetSurfaceWidth() const;
	EGLint GetSurfaceHeight() const;
	void GetDrawableSize(EGLint &w, EGLint &h) const;

	static bool CheckGLErrorAndLog(const tjs_char* funcname=nullptr);

	void SetScissorRect( const tTVPRect& rect );
	void DisableScissorRect();

	GLint GetDefaultFrameBufferId() const { return mDefaultFrameBufferId; }

	void SetWaitVSync( bool b );
};
#endif
