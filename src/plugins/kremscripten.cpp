#include "ncbind/ncbind.hpp"
#include "CharacterSet.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>

class KirikiriEmscriptenInterface
{
	public: static tTJSVariant evalJS(ttstr v)
	{
		std::string v_utf8;
		if (TVPUtf16ToUtf8(v_utf8, v.AsStdString()))
		{
			tjs_string r_utf16;
			char *result = emscripten_run_script_string(v_utf8.c_str());
			if (TVPUtf8ToUtf16(r_utf16, result))
			{
				return ttstr(r_utf16);
			}
		}
		return tTJSVariant();
	}
};

NCB_REGISTER_CLASS(KirikiriEmscriptenInterface)
{
	NCB_METHOD(evalJS);
};

tjs_string evalTJSFromJS(tjs_string v)
{
	tTJSVariant r_variant;
	tTJS *script_engine_object = TVPGetScriptEngine();
	if (script_engine_object)
	{
		script_engine_object->EvalExpression(ttstr(v), &r_variant, NULL);
		return ttstr(r_variant).AsStdString();
	}
	return TJS_W("");
}

EMSCRIPTEN_BINDINGS(KirikiriEmscriptenInterface)
{
	emscripten::function("evalTJS", &evalTJSFromJS);
}

#endif
