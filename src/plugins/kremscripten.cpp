/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#include "ncbind/ncbind.hpp"
#include "CharacterSet.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>

#define __internal_JS2TJS_wrapper "__internal_JS2TJS_wrapper"
#define __internal_TJS2JS_wrapper TJS_W("__internal_TJS2JS_wrapper")
#define internal_TJS2JS_call_function_name "internal_TJS2JS_call_function"
#define internal_TJS2JS_construct_object_name "internal_TJS2JS_construct_object"
#define internal_TJS2JS_get_object_property_name "internal_TJS2JS_get_object_property"
#define internal_TJS2JS_set_object_property_name "internal_TJS2JS_set_object_property"
#define internal_TJS2JS_has_object_property_name "internal_TJS2JS_has_object_property"
#define internal_TJS2JS_delete_object_property_name "internal_TJS2JS_delete_object_property"
#define internal_TJS2JS_get_object_keys_name "internal_TJS2JS_get_object_keys"
#define internal_TJS2JS_throw_val_as_TJS_exception_name "internal_TJS2JS_throw_val_as_TJS_exception"

#define objthis_name "objthis"
#define obj_name "obj"

static emscripten::val js_eval = emscripten::val::undefined();
static emscripten::val js_wrap_exception = emscripten::val::undefined();
static emscripten::val js_get_own_property_descriptor = emscripten::val::undefined();
static emscripten::val js_reflect_own_keys = emscripten::val::undefined();
static emscripten::val js_reflect_delete_property = emscripten::val::undefined();
static emscripten::val js_reflect_set = emscripten::val::undefined();
static emscripten::val js_reflect_construct = emscripten::val::undefined();
static emscripten::val js_reflect_apply = emscripten::val::undefined();
static emscripten::val js_throw_error = emscripten::val::undefined();
static emscripten::val js_pcall = emscripten::val::undefined();
static emscripten::val js_curry_function = emscripten::val::undefined();
static emscripten::val js_curry_apply = emscripten::val::undefined();
static emscripten::val js_curry_construct_object = emscripten::val::undefined();
static emscripten::val js_curry_get_property = emscripten::val::undefined();
static emscripten::val js_curry_set_property = emscripten::val::undefined();
static emscripten::val js_curry_has_property = emscripten::val::undefined();
static emscripten::val js_curry_delete_property = emscripten::val::undefined();
static emscripten::val js_curry_get_keys = emscripten::val::undefined();
static emscripten::val js_curry_get_own_property_descriptor = emscripten::val::undefined();
static emscripten::val js_new_proxy = emscripten::val::undefined();
static emscripten::val js_new_map = emscripten::val::undefined();
static emscripten::val js_empty_function = emscripten::val::undefined();
static emscripten::val js_empty_class = emscripten::val::undefined();
static emscripten::val js_clone_uint8array = emscripten::val::undefined();
static emscripten::val js_string_length = emscripten::val::undefined();
static emscripten::val js_string_number = emscripten::val::undefined();
static emscripten::val js_string_boolean = emscripten::val::undefined();
static emscripten::val js_string_bigint = emscripten::val::undefined();
static emscripten::val js_string_string = emscripten::val::undefined();
static emscripten::val js_string_function = emscripten::val::undefined();
static emscripten::val js_string_object = emscripten::val::undefined();
static emscripten::val js_string_toString = emscripten::val::undefined();
static emscripten::val js_string___internal_JS2TJS_wrapper = emscripten::val::undefined();
static emscripten::val js_string_apply = emscripten::val::undefined();
static emscripten::val js_string_getOwnPropertyDescriptor = emscripten::val::undefined();
static emscripten::val js_string_construct = emscripten::val::undefined();
static emscripten::val js_string_get = emscripten::val::undefined();
static emscripten::val js_string_set = emscripten::val::undefined();
static emscripten::val js_string_has = emscripten::val::undefined();
static emscripten::val js_string_deleteProperty = emscripten::val::undefined();
static emscripten::val js_string_ownKeys = emscripten::val::undefined();
static emscripten::val js_string_message = emscripten::val::undefined();
static emscripten::val js_string_window = emscripten::val::undefined();
static emscripten::val js_string_console = emscripten::val::undefined();
static emscripten::val js_string_open = emscripten::val::undefined();
static emscripten::val js_string_alert = emscripten::val::undefined();
static emscripten::val js_string_log = emscripten::val::undefined();
static emscripten::val js_string_warn = emscripten::val::undefined();
static emscripten::val js_string_unknown_error = emscripten::val::undefined();

static tTJSVariant emscripten_val_to_tjs_variant(emscripten::val v, emscripten::val vthis = emscripten::val::null());
static emscripten::val tjs_variant_to_emscripten_val(tTJSVariant v);

static tTJSHashTable<ttstr, emscripten::val> TJS2JSStringCache;
static emscripten::val JS2TJSStringCache = emscripten::val::undefined();

static ttstr map_js_string(emscripten::val v)
{
	if (JS2TJSStringCache != emscripten::val::undefined())
	{
		emscripten::val r = JS2TJSStringCache.call<emscripten::val>("get", v);
		if (r != emscripten::val::undefined())
		{
			// Check the TJS2JSStringCache hash table just to make sure we didn't receive an invalid value
			if (r.typeOf() == js_string_object && r[js_string_length].as<tjs_int32>() == 2)
			{
				ttstr key = ttstr((tTJSVariantString *)(r[0].as<tjs_uint32>()));
				tjs_int hash = r[1].as<tjs_int32>();
				emscripten::val *rr = TJS2JSStringCache.FindAndTouchWithHash(key, hash);
				if (rr)
				{
					return ttstr(key);
				}
			}
		}
	}
	return ttstr(v.as<tjs_string>());
}

static emscripten::val map_tjs_string(ttstr v)
{
	if (!(v.GetHint()) || !(*(v.GetHint())))
	{
		goto convert_passed_value;
	}
	{
		emscripten::val *r = TJS2JSStringCache.FindAndTouchWithHash(v, *(v.GetHint()));
		if (r)
		{
			return *r;
		}
	}
	{
		emscripten::val r = emscripten::val(v.AsStdString());
		TJS2JSStringCache.AddWithHash(v, *(v.GetHint()), r);
		emscripten::val temp_array = emscripten::val::array();
		if (JS2TJSStringCache != emscripten::val::undefined())
		{
			temp_array.call<void>("push", (tjs_uint32)(v.AsVariantStringNoAddRef()));
			temp_array.call<void>("push", *(v.GetHint()));
			JS2TJSStringCache.call<void>("set", r, temp_array);
		}
		return r;
	}
convert_passed_value:
	return emscripten::val(v.AsStdString());
}

class tTJS2JSMapCompact : public tTVPCompactEventCallbackIntf
{
	void TJS_INTF_METHOD OnCompact(tjs_int level)
	{
		// OnCompact method from tTVPCompactEventCallbackIntf
		// called when the application is idle, deactivated, minimized, or etc...
		if(level >= TVP_COMPACT_LEVEL_DEACTIVATE)
		{
			TJS2JSStringCache.Clear();
			if (JS2TJSStringCache != emscripten::val::undefined())
			{
				JS2TJSStringCache.call<void>("clear");
			}
		}
	}
} static TJS2JSMapCompact;

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
		*result = emscripten_val_to_tjs_variant(js_reflect_construct(v, temp_array));
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
		if (objthis != nullptr && objthis->IsInstanceOf(0, nullptr, nullptr, __internal_TJS2JS_wrapper, nullptr) == TJS_S_TRUE)
		{
			iTJSDispatch2WrapperForEmscripten *objthis_wrap = (iTJSDispatch2WrapperForEmscripten *)objthis;
			vthis = objthis_wrap->get_val();
		}
		*result = emscripten_val_to_tjs_variant(js_reflect_apply(func_to_call, vthis, temp_array));
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
		if (objthis != nullptr && objthis->IsInstanceOf(0, nullptr, nullptr, __internal_TJS2JS_wrapper, nullptr) == TJS_S_TRUE)
		{
			iTJSDispatch2WrapperForEmscripten *objthis_wrap = (iTJSDispatch2WrapperForEmscripten *)objthis;
			vthis = objthis_wrap->get_val();
		}
		*result = emscripten_val_to_tjs_variant(js_reflect_apply(func_to_call, vthis, temp_array));
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD PropGet(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, tTJSVariant *result, iTJSDispatch2 *objthis)
	{
		if (!membername)
		{
			return TJS_E_NOTIMPL;
		}
		emscripten::val vthis = emscripten::val::null();
		if (objthis != nullptr && objthis->IsInstanceOf(0, nullptr, nullptr, __internal_TJS2JS_wrapper, nullptr) == TJS_S_TRUE)
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
		if (objthis != nullptr && objthis->IsInstanceOf(0, nullptr, nullptr, __internal_TJS2JS_wrapper, nullptr) == TJS_S_TRUE)
		{
			iTJSDispatch2WrapperForEmscripten *objthis_wrap = (iTJSDispatch2WrapperForEmscripten *)objthis;
			vthis = objthis_wrap->get_val();
		}
		*result = emscripten_val_to_tjs_variant(v[(tjs_int32)num], vthis);
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD PropSet(tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint, const tTJSVariant *param, iTJSDispatch2 *objthis)
	{
		js_reflect_set(v, tjs_string(membername), tjs_variant_to_emscripten_val(*param));
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD PropSetByNum(tjs_uint32 flag, tjs_int num, const tTJSVariant *param, iTJSDispatch2 *objthis)
	{
		js_reflect_set(v, (tjs_int32)num, tjs_variant_to_emscripten_val(*param));
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD EnumMembers(tjs_uint32 flags, tTJSVariantClosure *callback, iTJSDispatch2 *objthis)
	{
		emscripten::val property_names = js_reflect_own_keys(v);
		tjs_int32 length = property_names[js_string_length].as<tjs_int32>();
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
		js_reflect_delete_property(v, tjs_string(membername));
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD DeleteMemberByNum(tjs_uint32 flag, tjs_int num, iTJSDispatch2 *objthis)
	{
		js_reflect_delete_property(v, (tjs_int32)num);
		return TJS_S_OK;
	}

	public: tjs_error TJS_INTF_METHOD IsInstanceOf(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, const tjs_char * classname, iTJSDispatch2 *objthis)
	{
		if (membername == nullptr)
		{
			if (!TJS_strcmp(classname, __internal_TJS2JS_wrapper))
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
	emscripten::val type = v.typeOf();
	if (type == js_string_number || type == js_string_boolean)
	{
		return tTJSVariant(v.as<tjs_real>());
	}
#if 0
	else if (type == js_string_bigint)
	{
		return tTJSVariant(v.as<tjs_int64>());
	}
#endif
	else if (type == js_string_string)
	{
		return tTJSVariant(map_js_string(v));
	}
	else if (type == js_string_function || type == js_string_object)
	{
		emscripten::val pd = js_pcall(js_get_own_property_descriptor, v, js_string___internal_JS2TJS_wrapper);
		if (pd.typeOf() == js_string_object)
		{
			if (pd[js_string___internal_JS2TJS_wrapper].as<bool>())
			{
				iTJSDispatch2 *tjsobj = (iTJSDispatch2 *)pd[obj_name].as<tjs_uint32>();
				iTJSDispatch2 *tjsobjthis = (iTJSDispatch2 *)pd[objthis_name].as<tjs_uint32>();
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
	if (type == tvtReal || type == tvtInteger)
	{
		return emscripten::val(v.AsReal());
	}
#if 0
	else if (type == tvtInteger)
	{
		return emscripten::val(v.AsInteger());
	}
#endif
	else if (type == tvtString)
	{
		return map_tjs_string(ttstr(v));
	}
	else if (type == tvtOctet)
	{
		return emscripten::val(emscripten::typed_memory_view(v.AsOctet()->GetLength(), v.AsOctet()->GetData()));
	}
	else if (type == tvtObject)
	{
		iTJSDispatch2 *obj = v.AsObjectNoAddRef();
		iTJSDispatch2 *objthis = v.AsObjectThisNoAddRef();
		if (obj == nullptr)
		{
			return emscripten::val::null();
		}
		if (obj->IsInstanceOf(0, nullptr, nullptr, __internal_TJS2JS_wrapper, nullptr) == TJS_S_TRUE)
		{
			iTJSDispatch2WrapperForEmscripten *obj_wrap = (iTJSDispatch2WrapperForEmscripten *)obj;
			emscripten::val ret = obj_wrap->get_val();
			return ret;
		}
		else if (obj->IsInstanceOf(0, nullptr, nullptr, TJS_W("Function"), nullptr) == TJS_S_TRUE)
		{
			emscripten::val proxy_traps = emscripten::val::object();
			proxy_traps.set(js_string_apply, js_curry_apply((tjs_uint32)obj, (tjs_uint32)objthis));
			proxy_traps.set(js_string_getOwnPropertyDescriptor, js_curry_get_own_property_descriptor((tjs_uint32)obj, (tjs_uint32)objthis));
			// TODO: function to manually release object from proxy
			if (obj)
			{
				obj->AddRef();
			}
			if (objthis)
			{
				objthis->AddRef();
			}
			return js_new_proxy(js_empty_function, proxy_traps);
		}
		else
		{
			emscripten::val proxy_traps = emscripten::val::object();
			proxy_traps.set(js_string_construct, js_curry_construct_object((tjs_uint32)obj, (tjs_uint32)objthis));
			proxy_traps.set(js_string_get, js_curry_get_property((tjs_uint32)obj, (tjs_uint32)objthis));
			proxy_traps.set(js_string_set, js_curry_set_property((tjs_uint32)obj, (tjs_uint32)objthis));
			proxy_traps.set(js_string_has, js_curry_has_property((tjs_uint32)obj, (tjs_uint32)objthis));
			proxy_traps.set(js_string_deleteProperty, js_curry_delete_property((tjs_uint32)obj, (tjs_uint32)objthis));
			proxy_traps.set(js_string_ownKeys, js_curry_get_keys((tjs_uint32)obj, (tjs_uint32)objthis));
			proxy_traps.set(js_string_getOwnPropertyDescriptor, js_curry_get_own_property_descriptor((tjs_uint32)obj, (tjs_uint32)objthis));
			// TODO: function to manually release object from proxy
			if (obj)
			{
				obj->AddRef();
			}
			if (objthis)
			{
				objthis->AddRef();
			}
			return js_new_proxy(js_empty_class, proxy_traps);
		}
	}
	return emscripten::val::undefined();
}

class KirikiriEmscriptenInterface
{
	public: static tTJSVariant evalJS(ttstr v)
	{
		return emscripten_val_to_tjs_variant(js_eval(map_tjs_string(v)));
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

		return emscripten_val_to_tjs_variant(js_eval(map_tjs_string(buffer)));
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
	js_wrap_exception = js_eval(std::string("(function(a){return function(...b){try{return a(...b);}catch(e){Module." internal_TJS2JS_throw_val_as_TJS_exception_name "(e);}};})"));
	js_eval = js_wrap_exception(js_eval);
	js_get_own_property_descriptor = js_wrap_exception(js_eval(std::string("(Object.getOwnPropertyDescriptor)")));
	js_reflect_own_keys = js_wrap_exception(js_eval(std::string("(Reflect.ownKeys)")));
	js_reflect_delete_property = js_wrap_exception(js_eval(std::string("(Reflect.deleteProperty)")));
	js_reflect_set = js_wrap_exception(js_eval(std::string("(Reflect.set)")));
	js_reflect_construct = js_wrap_exception(js_eval(std::string("(Reflect.construct)")));
	js_reflect_apply = js_wrap_exception(js_eval(std::string("(Reflect.apply)")));
	js_throw_error = js_wrap_exception(js_eval(std::string("(function(e){throw e;})")));
	js_pcall = js_wrap_exception(js_eval(std::string("(function(a,...b){try{return a(...b);}catch{return undefined;};})")));
	js_curry_function = js_wrap_exception(js_eval(std::string("(function(a,b){return function(...c){return Module." internal_TJS2JS_call_function_name "(a,b,[...c]);};})")));
	js_curry_apply = js_wrap_exception(js_eval(std::string("(function(a,b){return function(c,d,e){return Module." internal_TJS2JS_call_function_name "(a,b,e);};})")));
	js_curry_construct_object = js_wrap_exception(js_eval(std::string("(function(a,b){return function(...c){return Module." internal_TJS2JS_construct_object_name "(a,b,[...c]);};})")));
	js_curry_get_property = js_wrap_exception(js_eval(std::string("(function(a,b){return function(c,d){return Module." internal_TJS2JS_get_object_property_name "(a,b,d);};})")));
	js_curry_set_property = js_wrap_exception(js_eval(std::string("(function(a,b){return function(c,d,e){return Module." internal_TJS2JS_set_object_property_name "(a,b,d,e);};})")));
	js_curry_has_property = js_wrap_exception(js_eval(std::string("(function(a,b){return function(c,d){return Module." internal_TJS2JS_has_object_property_name "(a,b,d);};})")));
	js_curry_delete_property = js_wrap_exception(js_eval(std::string("(function(a,b){return function(c,d){return Module." internal_TJS2JS_delete_object_property_name "(a,b,d);};})")));
	js_curry_get_keys = js_wrap_exception(js_eval(std::string("(function(a,b){return function(c){return Module." internal_TJS2JS_get_object_keys_name "(a,b);};})")));
	js_curry_get_own_property_descriptor = js_wrap_exception(js_eval(std::string("(function(a,b){return function(c,d){return (d == '" __internal_JS2TJS_wrapper "') ? {" __internal_JS2TJS_wrapper ":true," obj_name ":a," objthis_name ":b} : undefined;};})")));
	js_new_proxy = js_wrap_exception(js_eval(std::string("(function(f,g){return new Proxy(f,g);})")));
	js_new_map = js_wrap_exception(js_eval(std::string("(function(){return new Map();})")));
	js_empty_function = js_wrap_exception(js_eval(std::string("(function() {})")));
	js_empty_class = js_wrap_exception(js_eval(std::string("(class {})")));
	js_clone_uint8array = js_wrap_exception(js_eval(std::string("(function(src){var dst = new ArrayBuffer(src.byteLength);new Uint8Array(dst).set(new Uint8Array(src));return dst;})")));
	js_string_length = emscripten::val(std::string("length"));
	js_string_number = emscripten::val(std::string("number"));
	js_string_boolean = emscripten::val(std::string("boolean"));
	js_string_bigint = emscripten::val(std::string("bigint"));
	js_string_string = emscripten::val(std::string("string"));
	js_string_function = emscripten::val(std::string("function"));
	js_string_object = emscripten::val(std::string("object"));
	js_string_toString = emscripten::val(std::string("toString"));
	js_string___internal_JS2TJS_wrapper = emscripten::val(std::string(__internal_JS2TJS_wrapper));
	js_string_apply = emscripten::val(std::string("apply"));
	js_string_getOwnPropertyDescriptor = emscripten::val(std::string("getOwnPropertyDescriptor"));
	js_string_construct = emscripten::val(std::string("construct"));
	js_string_get = emscripten::val(std::string("get"));
	js_string_set = emscripten::val(std::string("set"));
	js_string_has = emscripten::val(std::string("has"));
	js_string_deleteProperty = emscripten::val(std::string("deleteProperty"));
	js_string_ownKeys = emscripten::val(std::string("ownKeys"));
	js_string_message = emscripten::val(std::string("message"));
	js_string_window = emscripten::val(std::string("window"));
	js_string_console = emscripten::val(std::string("console"));
	js_string_open = emscripten::val(std::string("open"));
	js_string_alert = emscripten::val(std::string("alert"));
	js_string_log = emscripten::val(std::string("log"));
	js_string_warn = emscripten::val(std::string("warn"));
	js_string_unknown_error = emscripten::val(std::string("Unknown error"));
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

	JS2TJSStringCache = js_new_map();
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
			window = js_eval(js_string_window);
			emscripten::val console = emscripten::val::undefined();
			console = js_eval(js_string_console);
			static ttstr KirikiriEmscriptenInterface_name(TJS_W("KirikiriEmscriptenInterface"));
			er = global->PropGet(TJS_MEMBERMUSTEXIST, KirikiriEmscriptenInterface_name.c_str(), KirikiriEmscriptenInterface_name.GetHint(), &val, global);
			if (!TJS_FAILED(er) && val.Type() == tvtObject)
			{
				tTJSVariant window_val = emscripten_val_to_tjs_variant(window);
				static ttstr window_name(TJS_W("window"));
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, window_name.c_str(), window_name.GetHint(), &window_val, global);
				tTJSVariant console_val = emscripten_val_to_tjs_variant(console);
				static ttstr console_name(TJS_W("console"));
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, console_name.c_str(), console_name.GetHint(), &console_val, global);
			}

			// Replace System.{shellExecute,inform} with Javascript equivalents.
			static ttstr System_name(TJS_W("System"));
			er = global->PropGet(TJS_MEMBERMUSTEXIST, System_name.c_str(), System_name.GetHint(), &val, global);
			if (!TJS_FAILED(er) && val.Type() == tvtObject)
			{
				tTJSVariant shellExecute_val = emscripten_val_to_tjs_variant(window[js_string_open], window);
				static ttstr shellExecute_name(TJS_W("shellExecute"));
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, shellExecute_name.c_str(), shellExecute_name.GetHint(), &shellExecute_val, global);
				tTJSVariant inform_val = emscripten_val_to_tjs_variant(window[js_string_alert], window);
				static ttstr inform_name(TJS_W("inform"));
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, inform_name.c_str(), inform_name.GetHint(), &inform_val, global);
			}

			// Replace Debug.{message,notice} with Javascript equivalents.
			static ttstr Debug_name(TJS_W("Debug"));
			er = global->PropGet(TJS_MEMBERMUSTEXIST, Debug_name.c_str(), Debug_name.GetHint(), &val, global);
			if (!TJS_FAILED(er) && val.Type() == tvtObject)
			{
				tTJSVariant message_val = emscripten_val_to_tjs_variant(console[js_string_log], console);
				static ttstr message_name(TJS_W("message"));
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, message_name.c_str(), message_name.GetHint(), &message_val, global);
				tTJSVariant notice_val = emscripten_val_to_tjs_variant(console[js_string_warn], console);
				static ttstr notice_name(TJS_W("notice"));
				val.AsObjectNoAddRef()->PropSet(TJS_MEMBERENSURE, notice_name.c_str(), notice_name.GetHint(), &notice_val, global);
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
					map_tjs_string(exception_handler_exception_object).throw_(); \
				} \
				else if (exception_handler_exception_object_type == tvtObject) \
				{ \
					iTJSDispatch2 *exception_handler_exception_object_dispatch = exception_handler_exception_object.AsObjectNoAddRef(); \
					if (exception_handler_exception_object_dispatch) \
					{ \
						if (exception_handler_exception_object_dispatch->IsInstanceOf(0, nullptr, nullptr, __internal_TJS2JS_wrapper, nullptr) == TJS_S_TRUE) \
						{ \
							iTJSDispatch2WrapperForEmscripten *exception_handler_exception_object_dispatch_wrap = (iTJSDispatch2WrapperForEmscripten *)exception_handler_exception_object_dispatch; \
							emscripten::val ret = exception_handler_exception_object_dispatch_wrap->get_val(); \
							ret.throw_(); \
						} \
					} \
				} \
				map_tjs_string(e.GetMessage()).throw_(); \
			} \
			catch(eTJSScriptError &e) \
			{ \
				map_tjs_string(e.GetMessage()).throw_(); \
			} \
			catch(eTJS &e)  \
			{ \
				map_tjs_string(e.GetMessage()).throw_(); \
			} \
			catch(std::exception &e) \
			{ \
				emscripten::val(e.what()).throw_(); \
			} \
			catch(...) \
			{ \
				js_string_unknown_error.throw_(); \
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
	return emscripten::val::undefined();
}

emscripten::val getLayerBitmapUInt8Array(emscripten::val v, bool for_write)
{
	emscripten::val r = emscripten::val::undefined();
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		iTJSDispatch2 *global = TVPGetScriptDispatch();
		if (global)
		{
			tTJSVariant layer_val;
			static ttstr Layer_name(TJS_W("Layer"));
			global->PropGet(0, Layer_name.c_str(), Layer_name.GetHint(), &layer_val, global);
			tTJSVariantClosure layer_valclosure = layer_val.AsObjectClosureNoAddRef();
			tTJSVariant layer_variant = emscripten_val_to_tjs_variant(v);
			if (layer_variant.Type() == tvtObject)
			{
				tTJSVariantClosure clo = layer_variant.AsObjectClosureNoAddRef();
				tjs_int bmppitch = 0;
				tjs_int bmpheight = 0;
				tjs_uint8* bmpdata = NULL;
				if (layer_valclosure.Object && clo.Object)
				{
					tTJSVariant val;
					{
						static ttstr mainImageBufferPitch_name(TJS_W("mainImageBufferPitch"));
						if (TJS_FAILED(layer_valclosure.PropGet(0, mainImageBufferPitch_name.c_str(), mainImageBufferPitch_name.GetHint(), &val, clo.Object)))
						{
							static ttstr bufferPitch_name(TJS_W("bufferPitch"));
							clo.PropGet(0, bufferPitch_name.c_str(), bufferPitch_name.GetHint(), &val, clo.Object);
						}
						bmppitch = (tjs_int)val;
					}
					{
						static ttstr imageHeight_name(TJS_W("imageHeight"));
						if (TJS_FAILED(layer_valclosure.PropGet(0, imageHeight_name.c_str(), imageHeight_name.GetHint(), &val, clo.Object)))
						{
							static ttstr height(TJS_W("height"));
							clo.PropGet(0, height.c_str(), height.GetHint(), &val, clo.Object);
						}
						bmpheight = (tjs_uint)(tTVInteger)val;
					}
					{
						if (for_write)
						{
							static ttstr mainImageBufferForWrite_name(TJS_W("mainImageBufferForWrite"));
							if (TJS_FAILED(layer_valclosure.PropGet(0, mainImageBufferForWrite_name.c_str(), mainImageBufferForWrite_name.GetHint(), &val, clo.Object)))
							{
								static ttstr bufferForWrite_name(TJS_W("bufferForWrite"));
								clo.PropGet(0, bufferForWrite_name.c_str(), bufferForWrite_name.GetHint(), &val, clo.Object);
							}
						}
						else
						{
							static ttstr mainImageBuffer_name(TJS_W("mainImageBuffer"));
							if (TJS_FAILED(layer_valclosure.PropGet(0, mainImageBuffer_name.c_str(), mainImageBuffer_name.GetHint(), &val, clo.Object)))
							{
								static ttstr buffer_name(TJS_W("buffer"));
								clo.PropGet(0, buffer_name.c_str(), buffer_name.GetHint(), &val, clo.Object);
							}
						}
						bmpdata = reinterpret_cast<tjs_uint8*>((tjs_intptr_t)(tjs_int64)val);
					}
				}
				if (bmppitch * bmpheight != 0 && bmpdata != NULL)
				{
					r = emscripten::val(emscripten::typed_memory_view((size_t)bmppitch * bmpheight, bmpdata));
				}
			}
		}
	});
	return r;
}

emscripten::val getStorageUInt8Array(tjs_string v)
{
	emscripten::val r = emscripten::val::undefined();
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		tTJSBinaryStream * Stream = TVPCreateStream(ttstr(v), TJS_BS_READ);
		if (Stream)
		{
			tjs_uint size = (tjs_uint)(Stream->GetSize());
			tjs_uint8 *nbuf = new tjs_uint8[size];
			try
			{
				Stream->ReadBuffer(nbuf, size);
				r = js_clone_uint8array(emscripten::typed_memory_view(size, nbuf));
			}
			catch(...)
			{
				delete [] nbuf;
				throw;
			}
			delete [] nbuf;
		}
	});
	return r;
}

emscripten::val getStorageString(tjs_string place, tjs_string modestr)
{
	emscripten::val r = emscripten::val::undefined();
	TJS2JS_EXCEPTION_HANDLER_GUARD({
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
		r = map_tjs_string(buffer);
	});
	return r;
}

emscripten::val internal_TJS2JS_call_function(tjs_uint32 ptr_object, tjs_uint32 ptr_objectthis, emscripten::val args_array)
{
	iTJSDispatch2 *obj = (iTJSDispatch2 *)ptr_object;
	if (obj == nullptr)
	{
		return emscripten::val::undefined();
	}
	iTJSDispatch2 *objthis = (iTJSDispatch2 *)ptr_objectthis;
	tjs_uint32 argc = args_array[js_string_length].as<tjs_uint32>();
	tTJSVariant **argv = new tTJSVariant*[(size_t)argc];
	for (tjs_int i = 0; i < argc; i += 1)
	{
		argv[i] = new tTJSVariant();
		*argv[i] = emscripten_val_to_tjs_variant(args_array[i]);
	}
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		tTJSVariant r;
		tjs_error hr = obj->FuncCall(0, nullptr, nullptr, &r, argc, argv, objthis ? objthis : obj);
		if (TJS_SUCCEEDED(hr))
		{
			delete[] argv;
			return tjs_variant_to_emscripten_val(r);
		}
		else
		{
			TJSThrowFrom_tjs_error(hr);
		}
		delete[] argv;
	});
	return emscripten::val::undefined();
}

emscripten::val internal_TJS2JS_construct_object(tjs_uint32 ptr_object, tjs_uint32 ptr_objectthis, emscripten::val args_array)
{
	iTJSDispatch2 *obj = (iTJSDispatch2 *)ptr_object;
	if (obj == nullptr)
	{
		return emscripten::val::undefined();
	}
	iTJSDispatch2 *objthis = (iTJSDispatch2 *)ptr_objectthis;
	tjs_uint32 argc = args_array[js_string_length].as<tjs_uint32>();
	tTJSVariant **argv = new tTJSVariant*[(size_t)argc];
	for (tjs_int i = 0; i < argc; i += 1)
	{
		argv[i] = new tTJSVariant();
		*argv[i] = emscripten_val_to_tjs_variant(args_array[i]);
	}
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		iTJSDispatch2 *rd;
		tjs_error hr = obj->CreateNew(0, nullptr, nullptr, &rd, argc, argv, objthis ? objthis : obj);
		if (TJS_SUCCEEDED(hr))
		{
			delete[] argv;
			tTJSVariant r(rd, rd);
			return tjs_variant_to_emscripten_val(r);
		}
		else
		{
			TJSThrowFrom_tjs_error(hr);
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
	emscripten::val type = v.typeOf();
	emscripten::val r = emscripten::val::undefined();
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		if (type == js_string_number || type == js_string_bigint)
		{
			tTJSVariant rv;
			tjs_error hr = obj->PropGetByNum(0, v.as<tjs_int32>(), &rv, objthis ? objthis : obj);
			if (TJS_SUCCEEDED(hr))
			{
				r = tjs_variant_to_emscripten_val(rv);
			}
			else
			{
				TJSThrowFrom_tjs_error(hr);
			}
		}
		else if (type == js_string_string)
		{
			tTJSVariant rv;
			ttstr pv = map_js_string(v);
			tjs_error hr = obj->PropGet(0, pv.c_str(), pv.GetHint(), &rv, objthis ? objthis : obj);
			if (TJS_SUCCEEDED(hr))
			{
				r = tjs_variant_to_emscripten_val(rv);
			}
			else
			{
				TJSThrowFrom_tjs_error(hr);
			}
		}
		if (r == emscripten::val::undefined())
		{
			if (type == js_string_string && v == js_string_toString)
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
	emscripten::val type = v.typeOf();
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		if (type == js_string_number || type == js_string_bigint)
		{
			tTJSVariant rv = emscripten_val_to_tjs_variant(property_value);
			tjs_error hr = obj->PropSetByNum(TJS_MEMBERENSURE, v.as<tjs_int32>(), &rv, objthis ? objthis : obj);
			if (TJS_SUCCEEDED(hr))
			{
				return true;
			}
			else
			{
				TJSThrowFrom_tjs_error(hr);
			}
		}
		else if (type == js_string_string)
		{
			tTJSVariant rv = emscripten_val_to_tjs_variant(property_value);
			ttstr pv = map_js_string(v);
			tjs_error hr = obj->PropSet(TJS_MEMBERENSURE, pv.c_str(), pv.GetHint(), &rv, objthis ? objthis : obj);
			if (TJS_SUCCEEDED(hr))
			{
				return true;
			}
			else
			{
				TJSThrowFrom_tjs_error(hr);
			}
		}
	});
	return false;
}

bool internal_TJS2JS_has_object_property(tjs_uint32 ptr_object, tjs_uint32 ptr_objectthis, emscripten::val v)
{
	iTJSDispatch2 *obj = (iTJSDispatch2 *)ptr_object;
	if (obj == nullptr)
	{
		return false;
	}
	iTJSDispatch2 *objthis = (iTJSDispatch2 *)ptr_objectthis;
	emscripten::val type = v.typeOf();
	bool r = false;
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		if (type == js_string_number || type == js_string_bigint)
		{
			tTJSVariant rv;
			r = obj->PropGetByNum(TJS_MEMBERMUSTEXIST|TJS_IGNOREPROP, v.as<tjs_int32>(), &rv, objthis ? objthis : obj) == TJS_S_OK;
		}
		else if (type == js_string_string)
		{
			tTJSVariant rv;
			ttstr pv = map_js_string(v);
			r = obj->PropGet(TJS_MEMBERMUSTEXIST|TJS_IGNOREPROP, pv.c_str(), pv.GetHint(), &rv, objthis ? objthis : obj) == TJS_S_OK;
		}
	});
	return r;
}

bool internal_TJS2JS_delete_object_property(tjs_uint32 ptr_object, tjs_uint32 ptr_objectthis, emscripten::val v)
{
	iTJSDispatch2 *obj = (iTJSDispatch2 *)ptr_object;
	if (obj == nullptr)
	{
		return false;
	}
	iTJSDispatch2 *objthis = (iTJSDispatch2 *)ptr_objectthis;
	emscripten::val type = v.typeOf();
	bool r = false;
	TJS2JS_EXCEPTION_HANDLER_GUARD({
		if (type == js_string_number || type == js_string_bigint)
		{
			r = !!TJS_SUCCEEDED(obj->DeleteMemberByNum(TJS_MEMBERMUSTEXIST|TJS_IGNOREPROP, v.as<tjs_int32>(), objthis ? objthis : obj));
		}
		else if (type == js_string_string)
		{
			ttstr pv = map_js_string(v);
			r = !!TJS_SUCCEEDED(obj->DeleteMember(TJS_MEMBERMUSTEXIST|TJS_IGNOREPROP, pv.c_str(), pv.GetHint(), objthis ? objthis : obj));
		}
	});
	return r;
}

emscripten::val internal_TJS2JS_get_object_keys(tjs_uint32 ptr_object, tjs_uint32 ptr_objectthis)
{
	iTJSDispatch2 *obj = (iTJSDispatch2 *)ptr_object;
	if (obj == nullptr)
	{
		return emscripten::val::undefined();
	}
	iTJSDispatch2 *objthis = (iTJSDispatch2 *)ptr_objectthis;
	emscripten::val r = emscripten::val::array();

	class tCallback : public tTJSDispatch
	{
	public:
		emscripten::val Dest = emscripten::val::undefined();
		tjs_error TJS_INTF_METHOD FuncCall(
			tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
			tTJSVariant *result, tjs_int numparams, tTJSVariant **param,
			iTJSDispatch2 *objthis)
		{
			Dest.call<void>("push", tjs_variant_to_emscripten_val(*param[0]));
			if (result)
			{
				*result = (tjs_int)(1);
			}
			return TJS_S_OK;
		}
	};

	tCallback callback;
	callback.Dest = r;

	tTJSVariantClosure clo(&callback, nullptr);
	obj->EnumMembers(TJS_IGNOREPROP|TJS_ENUM_NO_VALUE, &clo, objthis ? objthis : obj);
	return r;
}

void internal_TJS2JS_throw_val_as_TJS_exception(emscripten::val v)
{
	tTJSVariant ve = emscripten_val_to_tjs_variant(v);
	static ttstr default_msg = TJS_W("TJS2JS exception boundary");
	ttstr msg = default_msg;
	emscripten::val type = v.typeOf();
	if (type == js_string_string)
	{
		msg = map_js_string(v);
	}
	else if (type == js_string_object)
	{
		emscripten::val v2 = v[js_string_message];
		emscripten::val type2 = v2.typeOf();
		if (type2 == js_string_string)
		{
			msg = map_js_string(v2);
		}
	}
	TJS_eTJSScriptException(msg, (tTJSScriptBlock *)nullptr, 0, ve);
}

EMSCRIPTEN_BINDINGS(KirikiriEmscriptenInterface)
{
	emscripten::function("evalTJS", &evalTJSFromJS);
	emscripten::function("getLayerBitmapUInt8Array", &getLayerBitmapUInt8Array);
	emscripten::function("getStorageUInt8Array", &getStorageUInt8Array);
	emscripten::function("getStorageString", &getStorageString);
	emscripten::function(internal_TJS2JS_call_function_name, &internal_TJS2JS_call_function);
	emscripten::function(internal_TJS2JS_construct_object_name, &internal_TJS2JS_construct_object);
	emscripten::function(internal_TJS2JS_get_object_property_name, &internal_TJS2JS_get_object_property);
	emscripten::function(internal_TJS2JS_set_object_property_name, &internal_TJS2JS_set_object_property);
	emscripten::function(internal_TJS2JS_has_object_property_name, &internal_TJS2JS_has_object_property);
	emscripten::function(internal_TJS2JS_delete_object_property_name, &internal_TJS2JS_delete_object_property);
	emscripten::function(internal_TJS2JS_get_object_keys_name, &internal_TJS2JS_get_object_keys);
	emscripten::function(internal_TJS2JS_throw_val_as_TJS_exception_name, &internal_TJS2JS_throw_val_as_TJS_exception);
}

#endif
