#include "ncbind/ncbind.hpp"
#include "CharacterSet.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>

static emscripten::val js_eval = emscripten::val::undefined();
static emscripten::val js_throw_error = emscripten::val::undefined();
static emscripten::val js_delete = emscripten::val::undefined();
static emscripten::val js_set = emscripten::val::undefined();
static emscripten::val js_call_spread = emscripten::val::undefined();

static tTJSVariant emscripten_val_to_tjs_variant(emscripten::val v);
static emscripten::val tjs_variant_to_emscripten_val(tTJSVariant v);

class iTJSDispatch2WrapperForEmscripten : public tTJSDispatch
{
	public: iTJSDispatch2WrapperForEmscripten(emscripten::val v)
	{
		this->v = v;
	}
	
	public: ~iTJSDispatch2WrapperForEmscripten() {}

	public: tjs_error TJS_INTF_METHOD CreateNew(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, iTJSDispatch2 **result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		return TJS_E_NOTIMPL;
	}

	public: tjs_error TJS_INTF_METHOD FuncCall(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		emscripten::val temp_array = emscripten::val::array();
		for (tjs_int i = 0; i < numparams; i += 1)
		{
			temp_array.call<void>("push", tjs_variant_to_emscripten_val(*param[i]));
		}
		emscripten::val func_to_call = v;
		if (membername)
		{
			func_to_call = v[tjs_string(membername)];
		}
		*result = emscripten_val_to_tjs_variant(js_call_spread(func_to_call, temp_array));
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD FuncCallByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		emscripten::val temp_array = emscripten::val::array();
		for (tjs_int i = 0; i < numparams; i += 1)
		{
			temp_array.call<void>("push", tjs_variant_to_emscripten_val(*param[i]));
		}
		emscripten::val func_to_call = v[(tjs_int32)num];
		*result = emscripten_val_to_tjs_variant(js_call_spread(func_to_call, temp_array));
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD PropGet(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, tTJSVariant *result, iTJSDispatch2 *objthis)
	{
		if (!membername)
		{
			return TJS_E_NOTIMPL;
		}
		*result = emscripten_val_to_tjs_variant(v[tjs_string(membername)]);
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD PropGetByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result, iTJSDispatch2 *objthis)
	{
		*result = emscripten_val_to_tjs_variant(v[(tjs_int32)num]);
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD PropSet(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint, const tTJSVariant *param, iTJSDispatch2 *objthis)
	{
		js_set(v, tjs_string(membername), tjs_variant_to_emscripten_val(*param));
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD PropSetByNum(tjs_uint32 flag, tjs_int num, const tTJSVariant *param, iTJSDispatch2 *objthis)
	{
		js_set(v, (tjs_int32)num, tjs_variant_to_emscripten_val(*param));
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD DeleteMember(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint, iTJSDispatch2 *objthis)
	{
		js_delete(v, tjs_string(membername));
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD DeleteMemberByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis)
	{
		js_delete(v, (tjs_int32)num);
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD IsInstanceOf(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, const tjs_char * classname, iTJSDispatch2 *objthis)
	{
		return TJS_S_FALSE;
	}

	protected: emscripten::val v = emscripten::val::undefined();
};

static tTJSVariant emscripten_val_to_tjs_variant(emscripten::val v)
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
	else if (type == "function" || type == "object")
	{
		tTJSVariant result;
		iTJSDispatch2 *tjsobj = new iTJSDispatch2WrapperForEmscripten(v);
		if (tjsobj)
		{
			result = tTJSVariant(tjsobj, tjsobj);
			tjsobj->Release();
		}
		return result;
	}
	return tTJSVariant();
}

static emscripten::val tjs_variant_to_emscripten_val(tTJSVariant v)
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
		return emscripten_val_to_tjs_variant(js_eval(v.AsStdString()));
	}
};

NCB_REGISTER_CLASS(KirikiriEmscriptenInterface)
{
	NCB_METHOD(evalJS);
};

static void init_js_callbacks()
{
	js_eval = emscripten::val::global("eval");
	js_throw_error = js_eval(std::string("(function(e){throw e;})"));
	js_delete = js_eval(std::string("(function(a,b){delete a[b];})"));
	js_set = js_eval(std::string("(function(a,b,c){a[b]=c;})"));
	js_call_spread = js_eval(std::string("(function(a,b){return a(...b);})"));
}

NCB_PRE_REGIST_CALLBACK(init_js_callbacks);

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
