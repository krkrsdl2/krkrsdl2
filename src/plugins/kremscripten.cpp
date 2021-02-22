#include "ncbind/ncbind.hpp"
#include "CharacterSet.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>

static emscripten::val js_eval = emscripten::val::undefined();
static emscripten::val js_wrap_exception = emscripten::val::undefined();
static emscripten::val js_get_own_property_descriptor = emscripten::val::undefined();
static emscripten::val js_get_own_property_names = emscripten::val::undefined();
static emscripten::val js_is_integer = emscripten::val::undefined();
static emscripten::val js_function_prototype_bind = emscripten::val::undefined();
static emscripten::val js_throw_error = emscripten::val::undefined();
static emscripten::val js_pcall = emscripten::val::undefined();
static emscripten::val js_delete = emscripten::val::undefined();
static emscripten::val js_set = emscripten::val::undefined();
static emscripten::val js_new_apply = emscripten::val::undefined();
static emscripten::val js_call_apply = emscripten::val::undefined();
static emscripten::val js_curry_function = emscripten::val::undefined();
static emscripten::val js_curry_get_property = emscripten::val::undefined();
static emscripten::val js_curry_set_property = emscripten::val::undefined();
static emscripten::val js_curry_get_own_property_descriptor = emscripten::val::undefined();
static emscripten::val js_new_proxy = emscripten::val::undefined();

static tTJSVariant emscripten_val_to_tjs_variant(emscripten::val v, emscripten::val vthis = emscripten::val::null());
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
		if (membername)
		{
			return TJS_E_NOTIMPL;
		}
		emscripten::val temp_array = emscripten::val::array();
		for (tjs_int i = 0; i < numparams; i += 1)
		{
			temp_array.call<void>("push", tjs_variant_to_emscripten_val(*param[i]));
		}
		*result = emscripten_val_to_tjs_variant(js_new_apply(temp_array, v));
		return TJS_S_OK;
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

	public: tjs_error TJS_INTF_METHOD EnumMembers(tjs_uint32 flags, tTJSVariantClosure *callback, iTJSDispatch2 *objthis)
	{
		emscripten::val property_names = js_get_own_property_names(v);
		tjs_int32 length = property_names["length"].as<tjs_int32>();
		tTJSVariant name;
		tTJSVariant newflags = 0;
		tTJSVariant value;
		tTJSVariant * params[3] = { &name, &newflags, &value };
		for (tjs_int i = 0; i < length; i += 1)
		{
			name = emscripten_val_to_tjs_variant(property_names[i]);
			if (!(flags & TJS_ENUM_NO_VALUE))
			{
				value = emscripten_val_to_tjs_variant(v[property_names[i]]);
			}
			tTJSVariant res;
			callback->FuncCall( 0, NULL, NULL, &res, (flags & TJS_ENUM_NO_VALUE) ? 2 : 3, params, NULL);
		}
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
		if (js_is_integer(v).as<bool>())
		{
			return tTJSVariant(v.as<tjs_int32>());
		}
		return tTJSVariant(v.as<tjs_real>());
	}
	else if (type == "string")
	{
		return tTJSVariant(v.as<tjs_string>());
	}
	else if (type == "function" || type == "object")
	{
		emscripten::val pd = js_pcall(js_get_own_property_descriptor, v, std::string("__internal_JS2TJS_wrapper"));
		if (pd.typeof().as<std::string>() == "object")
		{
			if (pd["__internal_JS2TJS_wrapper"].as<bool>())
			{
				iTJSDispatch2 *tjsobj = (iTJSDispatch2 *)pd["obj"].as<tjs_uint32>();
				iTJSDispatch2 *tjsobjthis = (iTJSDispatch2 *)pd["objthis"].as<tjs_uint32>();
				if (tjsobjthis == nullptr)
				{
					tjsobjthis = tjsobj;
				}
				return tTJSVariant(tjsobj, tjsobjthis);
			}
		}
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
	else if (type == tvtObject)
	{
		iTJSDispatch2 *obj = v.AsObject();
		iTJSDispatch2 *objthis = v.AsObjectThis();
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
		else if (obj->IsInstanceOf(0, nullptr, nullptr, TJS_W("Function"), nullptr) == TJS_S_TRUE)
		{
			emscripten::val jsvthis = emscripten::val::null();
			if (objthis != nullptr && objthis->IsInstanceOf(0, nullptr, nullptr, TJS_W("__internal_TJS2JS_wrapper"), nullptr) == TJS_S_TRUE)
			{
				iTJSDispatch2WrapperForEmscripten *objthis_wrap = (iTJSDispatch2WrapperForEmscripten *)objthis;
				jsvthis = objthis_wrap->get_val();
			}
			emscripten::val proxy_traps = emscripten::val::object();
			proxy_traps.set(std::string("getOwnPropertyDescriptor"), js_curry_get_own_property_descriptor((tjs_uint32)obj, (tjs_uint32)objthis));
			return js_new_proxy(js_curry_function((tjs_uint32)obj, jsvthis), proxy_traps);
		}
		else
		{
			emscripten::val proxy_traps = emscripten::val::object();
			proxy_traps.set(std::string("get"), js_curry_get_property((tjs_uint32)obj, (tjs_uint32)objthis));
			proxy_traps.set(std::string("set"), js_curry_set_property((tjs_uint32)obj, (tjs_uint32)objthis));
			proxy_traps.set(std::string("getOwnPropertyDescriptor"), js_curry_get_own_property_descriptor((tjs_uint32)obj, (tjs_uint32)objthis));
			return js_new_proxy(emscripten::val::object(), proxy_traps);
		}
#if 0
		obj->Release();
#endif
	}
	return emscripten::val::undefined();
}

class KirikiriEmscriptenInterface
{
	public: static tTJSVariant evalJS(ttstr v)
	{
		return emscripten_val_to_tjs_variant(js_eval(v.AsStdString()));
	}

	public: static tjs_error TJS_INTF_METHOD evalStorageJS(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;
		ttstr place = *param[0];
		ttstr modestr;
		if (numparams >= 2 && param[1]->Type() != tvtVoid)
		{
			modestr = *param[1];
		}
		
		iTJSTextReadStream * stream = TVPCreateTextStreamForRead(place, modestr);
		ttstr buffer;
		try
		{
			stream->Read(buffer, 0);
		}
		catch(...)
		{
			stream->Destruct();
			throw;
		}
		stream->Destruct();

		return emscripten_val_to_tjs_variant(js_eval(buffer.AsStdString()));
	}
};

NCB_REGISTER_CLASS(KirikiriEmscriptenInterface)
{
	NCB_METHOD(evalJS);
	RawCallback(TJS_W("evalStorageJS"), &KirikiriEmscriptenInterface::evalStorageJS, TJS_STATICMEMBER);
};

static void init_js_callbacks()
{
	js_eval = emscripten::val::global("eval");
	js_wrap_exception = js_eval(std::string("(function(a){return function(...b){try{return a(...b);}catch(e){Module.internal_TJS2JS_throw_val_as_TJS_exception(e);}};})"));
	js_eval = js_wrap_exception(js_eval);
	js_get_own_property_descriptor = js_wrap_exception(js_eval(std::string("(Object.getOwnPropertyDescriptor)")));
	js_get_own_property_names = js_wrap_exception(js_eval(std::string("(Object.getOwnPropertyNames)")));
	js_is_integer = js_wrap_exception(js_eval(std::string("(Number.isInteger)")));
	js_function_prototype_bind = js_wrap_exception(js_eval(std::string("(Function.prototype.bind)")));
	js_throw_error = js_wrap_exception(js_eval(std::string("(function(e){throw e;})")));
	js_pcall = js_wrap_exception(js_eval(std::string("(function(a,...b){try{return a(...b);}catch{return undefined;};})")));
	js_delete = js_wrap_exception(js_eval(std::string("(function(a,b){delete a[b];})")));
	js_set = js_wrap_exception(js_eval(std::string("(function(a,b,c){a[b]=c;})")));
	js_new_apply = js_wrap_exception(js_eval(std::string("(function(a,b){return new (Function.prototype.bind.apply(b,[null].concat(a)));})")));
	js_call_apply = js_wrap_exception(js_eval(std::string("(function(a,b,c){return a.apply(c,b);})")));
	js_curry_function = js_wrap_exception(js_eval(std::string("(function(a,b){return function(...c){return Module.internal_TJS2JS_call_function.call(b,a,[...c]);};})")));
	js_curry_get_property = js_wrap_exception(js_eval(std::string("(function(a,b){return function(c,d){return Module.internal_TJS2JS_get_object_property(a,b,d);};})")));
	js_curry_set_property = js_wrap_exception(js_eval(std::string("(function(a,b){return function(c,d,e){return Module.internal_TJS2JS_set_object_property(a,b,d,e);};})")));
	js_curry_get_own_property_descriptor = js_wrap_exception(js_eval(std::string("(function(a,b){return function(c,d){return (d == '__internal_JS2TJS_wrapper') ? {__internal_JS2TJS_wrapper:true,obj:a,objthis:b} : undefined;};})")));
	js_new_proxy = js_wrap_exception(js_eval(std::string("(function(f,g){return new Proxy(f,g);})")));
	// Work around RuntimeError: memory access out of bounds on first exception catch
#if !defined(__EMSCRIPTEN_PTHREADS__)
	tTJS *script_engine = TVPGetScriptEngine();
	if (script_engine)
	{
		iTJSConsoleOutput *output = script_engine->GetConsoleOutput();
		script_engine->SetConsoleOutput(nullptr);
		js_pcall(js_eval, std::string("Module.evalTJS('(function(){try{KirikiriEmscriptenInterface.evalJS(\"throw 1337\");}catch(e){throw e;}})()')"));
		script_engine->SetConsoleOutput(output);
	}
#endif
}

NCB_PRE_REGIST_CALLBACK(init_js_callbacks);

static void init_js_replacements()
{
#if !defined(__EMSCRIPTEN_PTHREADS__)
	tTJS *script_engine = TVPGetScriptEngine();
	if (script_engine)
	{
		iTJSDispatch2 * global = script_engine->GetGlobalNoAddRef();
		if (global)
		{
			tTJSVariant val;
			tjs_error er;

			// Add references to window and console to KirikiriEmscriptenInterface class.
			emscripten::val window = emscripten::val::undefined();
			window = js_eval(std::string("window"));
			emscripten::val console = emscripten::val::undefined();
			console = js_eval(std::string("console"));
			er = global->PropGet(TJS_MEMBERMUSTEXIST, TJS_W("KirikiriEmscriptenInterface"), NULL, &val, global);
			if (!TJS_FAILED(er) && val.Type() == tvtObject)
			{
				tTJSVariant window_val = emscripten_val_to_tjs_variant(window);
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, TJS_W("window"), NULL, &window_val, global);
				tTJSVariant console_val = emscripten_val_to_tjs_variant(console);
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, TJS_W("console"), NULL, &console_val, global);
			}

			// Replace System.{shellExecute,inform} with Javascript equivalents.
			er = global->PropGet(TJS_MEMBERMUSTEXIST, TJS_W("System"), NULL, &val, global);
			if (!TJS_FAILED(er) && val.Type() == tvtObject)
			{
				tTJSVariant shellExecute_val = emscripten_val_to_tjs_variant(window["open"], window);
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, TJS_W("shellExecute"), NULL, &shellExecute_val, global);
				tTJSVariant inform_val = emscripten_val_to_tjs_variant(window["alert"], window);
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, TJS_W("inform"), NULL, &inform_val, global);
			}

			// Replace Debug.{message,notice} with Javascript equivalents.
			er = global->PropGet(TJS_MEMBERMUSTEXIST, TJS_W("Debug"), NULL, &val, global);
			if (!TJS_FAILED(er) && val.Type() == tvtObject)
			{
				tTJSVariant message_val = emscripten_val_to_tjs_variant(console["log"], console);
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, TJS_W("message"), NULL, &message_val, global);
				tTJSVariant notice_val = emscripten_val_to_tjs_variant(console["warn"], console);
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, TJS_W("notice"), NULL, &notice_val, global);
			}
		}
	}
#endif
}
NCB_POST_REGIST_CALLBACK(init_js_replacements);

#define TJS2JS_EXCEPTION_HANDLER_GUARD(block) \
	{ \
		tTJS *exception_handler_script_engine = TVPGetScriptEngine(); \
		if (exception_handler_script_engine) \
		{ \
			try \
			{ \
				block; \
			} \
			catch(eTJSScriptException &e) \
			{ \
				tTJSVariant exception_handler_exception_object = e.GetValue(); \
				tTJSVariantType exception_handler_exception_object_type = exception_handler_exception_object.Type(); \
				if (exception_handler_exception_object_type == tvtString) \
				{ \
					emscripten::val(tTJSString(exception_handler_exception_object).AsStdString()).throw_(); \
				} \
				else if (exception_handler_exception_object_type == tvtObject) \
				{ \
					iTJSDispatch2 *exception_handler_exception_object_dispatch = exception_handler_exception_object.AsObject(); \
					if (exception_handler_exception_object_dispatch) \
					{ \
						if (exception_handler_exception_object_dispatch->IsInstanceOf(0, nullptr, nullptr, TJS_W("__internal_TJS2JS_wrapper"), nullptr) == TJS_S_TRUE) \
						{ \
							iTJSDispatch2WrapperForEmscripten *exception_handler_exception_object_dispatch_wrap = (iTJSDispatch2WrapperForEmscripten *)exception_handler_exception_object_dispatch; \
							emscripten::val ret = exception_handler_exception_object_dispatch_wrap->get_val(); \
							exception_handler_exception_object_dispatch->Release(); \
							ret.throw_(); \
						} \
						exception_handler_exception_object_dispatch->Release(); \
					} \
				} \
				emscripten::val(e.GetMessage().AsStdString()).throw_(); \
			} \
			catch(eTJSScriptError &e) \
			{ \
				emscripten::val(e.GetMessage().AsStdString()).throw_(); \
			} \
			catch(eTJS &e)  \
			{ \
				emscripten::val(e.GetMessage().AsStdString()).throw_(); \
			} \
			catch(std::exception &e) \
			{ \
				emscripten::val(e.what()).throw_(); \
			} \
			catch(...) \
			{ \
				emscripten::val(std::string("Unknown error")).throw_(); \
			} \
		} \
	}

emscripten::val evalTJSFromJS(tjs_string v)
{
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		tTJSVariant r_variant;
		tTJS *script_engine_object = TVPGetScriptEngine();
		if (script_engine_object)
		{
			script_engine_object->EvalExpression(ttstr(v), &r_variant, NULL);
			return tjs_variant_to_emscripten_val(r_variant);
		}
	});
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
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		tTJSVariant r;
		if (TJS_SUCCEEDED(func->FuncCall(0, nullptr, nullptr, &r, argc, argv, nullptr)))
		{
			delete[] argv;
			return tjs_variant_to_emscripten_val(r);
		}
		delete[] argv;
	});
	return emscripten::val::undefined();
}

emscripten::val internal_TJS2JS_get_object_property(tjs_uint32 ptr_object, tjs_uint32 ptr_objectthis, emscripten::val v)
{
	iTJSDispatch2 *obj = (iTJSDispatch2 *)ptr_object;
	if (obj == nullptr)
	{
		return emscripten::val::undefined();
	}
	iTJSDispatch2 *objthis = (iTJSDispatch2 *)ptr_objectthis;
	std::string type = v.typeof().as<std::string>();
	emscripten::val r = emscripten::val::undefined();
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		if (type == "number" || type == "bigint")
		{
			tTJSVariant rv;
			if (TJS_SUCCEEDED(obj->PropGetByNum(0, v.as<tjs_int32>(), &rv, objthis)))
			{
				r = tjs_variant_to_emscripten_val(rv);
			}
		}
		else if (type == "string")
		{
			tTJSVariant rv;
			if (TJS_SUCCEEDED(obj->PropGet(0, v.as<tjs_string>().c_str(), nullptr, &rv, objthis)))
			{
				r = tjs_variant_to_emscripten_val(rv);
			}
		}
		if (r == emscripten::val::undefined())
		{
			if (type == "string" && v.as<std::string>() == "toString")
			{
				if (objthis == nullptr)
				{
					objthis = obj;
				}
				tTJSVariant rv(obj, objthis);
				rv.ToString();
				r = tjs_variant_to_emscripten_val(rv);
			}
		}
	});
	return r;
}

bool internal_TJS2JS_set_object_property(tjs_uint32 ptr_object, tjs_uint32 ptr_objectthis, emscripten::val v, emscripten::val property_value)
{
	iTJSDispatch2 *obj = (iTJSDispatch2 *)ptr_object;
	if (obj == nullptr)
	{
		return false;
	}
	iTJSDispatch2 *objthis = (iTJSDispatch2 *)ptr_objectthis;
	std::string type = v.typeof().as<std::string>();
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		if (type == "number" || type == "bigint")
		{
			tTJSVariant rv = emscripten_val_to_tjs_variant(property_value);
			if (TJS_SUCCEEDED(obj->PropSetByNum(0, v.as<tjs_int32>(), &rv, objthis)))
			{
				return true;
			}
		}
		else if (type == "string")
		{
			tTJSVariant rv = emscripten_val_to_tjs_variant(property_value);
			if (TJS_SUCCEEDED(obj->PropSet(0, v.as<tjs_string>().c_str(), nullptr, &rv, objthis)))
			{
				return true;
			}
		}
	});
	return false;
}

void internal_TJS2JS_throw_val_as_TJS_exception(emscripten::val v)
{
	tTJSVariant ve = emscripten_val_to_tjs_variant(v);
	ttstr msg = "TJS2JS exception boundary";
	std::string type = v.typeof().as<std::string>();
	if (type == "string")
	{
		msg = v.as<tjs_string>();
	}
	else if (type == "object")
	{
		emscripten::val v2 = v["message"];
		std::string type2 = v2.typeof().as<std::string>();
		if (type2 == "string")
		{
			msg = v2.as<tjs_string>();
		}
	}
	TJS_eTJSScriptException(msg, (tTJSScriptBlock *)nullptr, 0, ve);
}

EMSCRIPTEN_BINDINGS(KirikiriEmscriptenInterface)
{
	emscripten::function("evalTJS", &evalTJSFromJS);
	emscripten::function("internal_TJS2JS_call_function", &internal_TJS2JS_call_function);
	emscripten::function("internal_TJS2JS_get_object_property", &internal_TJS2JS_get_object_property);
	emscripten::function("internal_TJS2JS_set_object_property", &internal_TJS2JS_set_object_property);
	emscripten::function("internal_TJS2JS_throw_val_as_TJS_exception", &internal_TJS2JS_throw_val_as_TJS_exception);
}

#endif
