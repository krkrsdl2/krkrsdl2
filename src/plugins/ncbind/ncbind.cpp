#include "tjsCommHead.h"
#include "Extension.h"
#include "ncbind/ncbind.hpp"

//---------------------------------------------------------------------------
// tTJSNC_KirikiriSDL2Internal : Kirikiri SDL2 internal class
//---------------------------------------------------------------------------
class tTJSNC_KirikiriSDL2Internal : public tTJSNativeClass
{
public:
	tTJSNC_KirikiriSDL2Internal();

	static tjs_uint32 ClassID;

protected:
	tTJSNativeInstance * CreateNativeInstance();
};

//---------------------------------------------------------------------------
// tTJSNC_KirikiriSDL2Internal
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_KirikiriSDL2Internal::ClassID = -1;
tTJSNC_KirikiriSDL2Internal::tTJSNC_KirikiriSDL2Internal() : tTJSNativeClass(TJS_W("KirikiriSDL2Internal"))
{
	TJS_BEGIN_NATIVE_MEMBERS(KirikiriSDL2Internal)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(/*TJS class name*/KirikiriSDL2Internal)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/KirikiriSDL2Internal)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
	TJS_END_NATIVE_MEMBERS

} // end of tTJSNC_Debug::tTJSNC_Debug
//---------------------------------------------------------------------------
tTJSNativeInstance *tTJSNC_KirikiriSDL2Internal::CreateNativeInstance()
{
	return NULL;
}
static iTJSDispatch2 * TVPCreateNativeClass_KirikiriSDL2Internal(iTJSDispatch2* global)
{
	ncbAutoRegister::AllRegist();
	iTJSDispatch2 *cls = new tTJSNC_KirikiriSDL2Internal();
	return cls;
}

static tTVPAtInstallClass TVPInstallClassKirikiriSDL2Internal(TJS_W("KirikiriSDL2Internal"), TVPCreateNativeClass_KirikiriSDL2Internal, TJS_W(""));

//---------------------------------------------------------------------------
// static変数の実体

// auto register 先頭ポインタ
ncbAutoRegister::ThisClassT const*
ncbAutoRegister::_top[ncbAutoRegister::LINE_COUNT] = NCB_INNER_AUTOREGISTER_LINES_INSTANCE;
