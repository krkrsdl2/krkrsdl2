#include "ncbind/ncbind.hpp"
#include "CharacterSet.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>

tTJSVariant emscripten_val_to_tjs_variant(emscripten::val v)
{
	std::string type = v.typeof().as<std::string>();
	if (type == "boolean" || type == "bigint")
	{
		return tTJSVariant(v.as<tjs_int32>());
	}
	else if (type == "number")
	{
		return tTJSVariant(v.as<tjs_real>());
	}
	else if (type == "string")
	{
		return tTJSVariant(v.as<tjs_string>());
	}
	return tTJSVariant();
}

emscripten::val tjs_variant_to_emscripten_val(tTJSVariant v)
{
	tTJSVariantType type = v.Type();
	if (type == tvtInteger)
	{
		return emscripten::val((tjs_int32)v.AsInteger());
	}
	else if (type == tvtReal)
	{
		return emscripten::val(v.AsReal());
	}
	else if (type == tvtString)
	{
		return emscripten::val(tTJSString(v).AsStdString());
	}
	else if (type == tvtOctet)
	{
		return emscripten::val(emscripten::typed_memory_view(v.AsOctet()->GetLength(), v.AsOctet()->GetData()));
	}
	return emscripten::val::undefined();
}

class KirikiriEmscriptenInterface
{
	public: static tTJSVariant evalJS(ttstr v)
	{
		emscripten::val js_eval = emscripten::val::global("eval");
		if (js_eval.typeof().as<std::string>() == "function")
		{
			return emscripten_val_to_tjs_variant(js_eval(v.AsStdString()));
		}
		return tTJSVariant();
	}
};

NCB_REGISTER_CLASS(KirikiriEmscriptenInterface)
{
	NCB_METHOD(evalJS);
};

emscripten::val evalTJSFromJS(tjs_string v)
{
	tTJSVariant r_variant;
	tTJS *script_engine_object = TVPGetScriptEngine();
	if (script_engine_object)
	{
		script_engine_object->EvalExpression(ttstr(v), &r_variant, NULL);
		return tjs_variant_to_emscripten_val(r_variant);
	}
	return emscripten::val(tjs_string(TJS_W("")));
}

EMSCRIPTEN_BINDINGS(KirikiriEmscriptenInterface)
{
	emscripten::function("evalTJS", &evalTJSFromJS);
}

#endif
