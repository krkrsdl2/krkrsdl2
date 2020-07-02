#include "ncbind/ncbind.hpp"
#include "CharacterSet.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>

static emscripten::val js_eval = emscripten::val::undefined();
static emscripten::val js_throw_error = emscripten::val::undefined();
static emscripten::val js_delete = emscripten::val::undefined();
static emscripten::val js_set = emscripten::val::undefined();
static emscripten::val js_call_apply = emscripten::val::undefined();
static emscripten::val js_curry_function = emscripten::val::undefined();

static tTJSVariant emscripten_val_to_tjs_variant(emscripten::val v, emscripten::val vthis = emscripten::val::null());
static emscripten::val tjs_variant_to_emscripten_val(tTJSVariant v, iTJSDispatch2 *objthis = nullptr);

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
		emscripten::val vthis = emscripten::val::null();
		if (objthis != nullptr && objthis->IsInstanceOf(0, nullptr, nullptr, TJS_W("__internal_TJS2JS_wrapper"), nullptr) == TJS_S_TRUE)
		{
			iTJSDispatch2WrapperForEmscripten *objthis_wrap = (iTJSDispatch2WrapperForEmscripten *)objthis;
			vthis = objthis_wrap->get_val();
		}
		*result = emscripten_val_to_tjs_variant(js_call_apply(func_to_call, temp_array, vthis));
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
		emscripten::val vthis = emscripten::val::null();
		if (objthis != nullptr && objthis->IsInstanceOf(0, nullptr, nullptr, TJS_W("__internal_TJS2JS_wrapper"), nullptr) == TJS_S_TRUE)
		{
			iTJSDispatch2WrapperForEmscripten *objthis_wrap = (iTJSDispatch2WrapperForEmscripten *)objthis;
			vthis = objthis_wrap->get_val();
		}
		*result = emscripten_val_to_tjs_variant(js_call_apply(func_to_call, temp_array, vthis));
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD PropGet(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, tTJSVariant *result, iTJSDispatch2 *objthis)
	{
		if (!membername)
		{
			return TJS_E_NOTIMPL;
		}
		emscripten::val vthis = emscripten::val::null();
		if (objthis != nullptr && objthis->IsInstanceOf(0, nullptr, nullptr, TJS_W("__internal_TJS2JS_wrapper"), nullptr) == TJS_S_TRUE)
		{
			iTJSDispatch2WrapperForEmscripten *objthis_wrap = (iTJSDispatch2WrapperForEmscripten *)objthis;
			vthis = objthis_wrap->get_val();
		}
		*result = emscripten_val_to_tjs_variant(v[tjs_string(membername)], vthis);
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD PropGetByNum(tjs_uint32 flag, tjs_int num, tTJSVariant *result, iTJSDispatch2 *objthis)
	{
		emscripten::val vthis = emscripten::val::null();
		if (objthis != nullptr && objthis->IsInstanceOf(0, nullptr, nullptr, TJS_W("__internal_TJS2JS_wrapper"), nullptr) == TJS_S_TRUE)
		{
			iTJSDispatch2WrapperForEmscripten *objthis_wrap = (iTJSDispatch2WrapperForEmscripten *)objthis;
			vthis = objthis_wrap->get_val();
		}
		*result = emscripten_val_to_tjs_variant(v[(tjs_int32)num], vthis);
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
		if (membername == nullptr)
		{
			if (!TJS_strcmp(classname, TJS_W("__internal_TJS2JS_wrapper")))
			{
				return TJS_S_TRUE;
			}
		}
		return TJS_S_FALSE;
	}

	public: emscripten::val get_val()
	{
		return v;
	}

	protected: emscripten::val v = emscripten::val::undefined();
};

static tTJSVariant emscripten_val_to_tjs_variant(emscripten::val v, emscripten::val vthis)
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
		iTJSDispatch2 *tjsobjthis = nullptr;
		if (vthis.as<bool>())
		{
			tjsobjthis = new iTJSDispatch2WrapperForEmscripten(vthis);
		}
		if (tjsobj)
		{
			if (tjsobjthis == nullptr)
			{
				result = tTJSVariant(tjsobj, tjsobj);
			}
			else
			{
				result = tTJSVariant(tjsobj, tjsobjthis);
				tjsobjthis->Release();
			}
			tjsobj->Release();
		}
		return result;
	}
	return tTJSVariant();
}

static emscripten::val tjs_variant_to_emscripten_val(tTJSVariant v, iTJSDispatch2 *objthis)
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
	else if (type == tvtObject)
	{
		iTJSDispatch2 *obj = v.AsObject();
		if (obj == nullptr)
		{
			return emscripten::val::null();
		}
		if (obj->IsInstanceOf(0, nullptr, nullptr, TJS_W("__internal_TJS2JS_wrapper"), nullptr) == TJS_S_TRUE)
		{
			iTJSDispatch2WrapperForEmscripten *obj_wrap = (iTJSDispatch2WrapperForEmscripten *)obj;
			emscripten::val ret = obj_wrap->get_val();
			obj->Release();
			return ret;
		}
		else if (TJS_SUCCEEDED(obj->IsInstanceOf(0, nullptr, nullptr, TJS_W("Function"), nullptr)))
		{
			emscripten::val jsvthis = emscripten::val::null();
			if (objthis != nullptr && objthis->IsInstanceOf(0, nullptr, nullptr, TJS_W("__internal_TJS2JS_wrapper"), nullptr) == TJS_S_TRUE)
			{
				iTJSDispatch2WrapperForEmscripten *objthis_wrap = (iTJSDispatch2WrapperForEmscripten *)objthis;
				jsvthis = objthis_wrap->get_val();
			}
			return js_curry_function((tjs_uint32)obj, jsvthis);
		}
		else if (TJS_SUCCEEDED(obj->IsInstanceOf(0, nullptr, nullptr, TJS_W("Property"), nullptr)))
		{
			// TODO: wrap property using Proxy object
		}
		obj->Release();
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
	js_call_apply = js_eval(std::string("(function(a,b,c){return a.apply(c,b);})"));
	js_curry_function = js_eval(std::string("(function(a,b){return function(...c){return Module.internal_TJS2JS_call_function.call(b,a,[...c]);};})"));
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

emscripten::val internal_TJS2JS_call_function(tjs_uint32 ptr_function, emscripten::val args_array)
{
	iTJSDispatch2 *func = (iTJSDispatch2 *)ptr_function;
	if (func == nullptr)
	{
		return emscripten::val::undefined();
	}
	tjs_uint32 argc = args_array["length"].as<tjs_uint32>();
	tTJSVariant **argv = new tTJSVariant*[(size_t)argc];
	for (tjs_int i = 0; i < argc; i += 1)
	{
		argv[i] = new tTJSVariant();
		*argv[i] = emscripten_val_to_tjs_variant(args_array[i]);
	}
	tTJSVariant r;
	if (TJS_SUCCEEDED(func->FuncCall(0, nullptr, nullptr, &r, argc, argv, nullptr)))
	{
		delete[] argv;
		return tjs_variant_to_emscripten_val(r);
	}
	delete[] argv;
	return emscripten::val::undefined();
}

EMSCRIPTEN_BINDINGS(KirikiriEmscriptenInterface)
{
	emscripten::function("evalTJS", &evalTJSFromJS);
	emscripten::function("internal_TJS2JS_call_function", &internal_TJS2JS_call_function);
}

#endif
