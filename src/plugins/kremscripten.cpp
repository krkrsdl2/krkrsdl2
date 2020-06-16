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

std::string evalTJSFromJS(std::string v)
{
	tjs_string v_utf16;
	if (TVPUtf8ToUtf16(v_utf16, v))
	{
		tTJSVariant r_variant;
		tTJS *script_engine_object = TVPGetScriptEngine();
		if (script_engine_object)
		{
			script_engine_object->EvalExpression(ttstr(v_utf16), &r_variant, NULL);
			std::string r_utf8;
			if (TVPUtf16ToUtf8(r_utf8, ttstr(r_variant).AsStdString()))
			{
				return r_utf8;
			}
		}
	}
	return "";
}

EMSCRIPTEN_BINDINGS(KirikiriEmscriptenInterface)
{
	emscripten::function("evalTJS", &evalTJSFromJS);
}

#endif
