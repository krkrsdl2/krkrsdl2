
copyright = """\
/*

	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
/* This file is always generated by makestub.pl . */
/* Modification by hand will be lost. */
"""

import re
import sys
import io
import hashlib
import zlib
import os
import json

output_tpstub_h = ""
output_tpstub_cpp = ""

if len(sys.argv) >= 2:
	# output_tpstub_h = "../../../tp_stub.h"
	# output_tpstub_cpp = "../../../tp_stub.cpp"
	output_tpstub_h = "tp_stub.h"
	output_tpstub_cpp = "tp_stub.cpp"


class_stub_round_1 = [
	"tTJSVariant",
	"tTJSVariantOctet",
	"tTJSString",
	"tTJSVariantString",
]

class_stub_round_2 = [
	"tTJSVariantString",
	"tTJSVariantOctet",
	"tTJSVariant",
	"tTJSString",
]

class_stub_name_to_file = {
	"tTJSString" : "../../tjs2/tjsString.h",
	"tTJSVariant" : "../../tjs2/tjsVariant.h",
	"tTJSVariantOctet" : "../../tjs2/tjsVariant.h",
	"tTJSVariantString" : "../../tjs2/tjsVariantString.h",
}

class_stub_do_not_create_directly = [
	"tTJSVariantOctet",
	"tTJSVariantString",
]

func_headers_exports_system = [
	"../../tjs2/tjsTypes.h",
	"../../tjs2/tjsConfig.h",
	"../../tjs2/tjsVariantString.h",
	"../../tjs2/tjsUtils.h",
	"../../tjs2/tjsString.h",
	"../../tjs2/tjsInterface.h",
	"../../tjs2/tjsErrorDefs.h",
	"../../tjs2/tjsNative.h",
	"../../tjs2/tjsVariant.h",
	"../../tjs2/tjsArray.h",
	"../../tjs2/tjsDictionary.h",
	"../../tjs2/tjs.h",
	"../../tjs2/tjsMessage.h",
	"../../tjs2/tjsGlobalStringMap.h",
	"../../tjs2/tjsObject.h",
	"../../tjs2/tjsObject.cpp",
]

func_headers_exports_misc = [
	"../StorageIntf.h",
	"../TextStream.h",
	"../CharacterSet.h",
	"../XP3Archive.h",
	"../EventIntf.h",
	"../SystemIntf.h",
	"./SystemImpl.h",
	"../ScriptMgnIntf.h",
	"../BinaryStream.h",
	"../android/StorageImpl.h",
	"../android/PluginImpl.h",
	"../SysInitIntf.h",
	"../android/SysInitImpl.h",
	"../../environ/android/DetectCPU.h",
	"../../utils/ThreadIntf.h",
	"../../utils/DebugIntf.h",
	"../../utils/Random.h",
	"../../utils/ClipboardIntf.h",
	"../../utils/TickCount.h",
	"../../msg/MsgIntf.h",
	"../../sound/WaveIntf.h",
	"../../visual/GraphicsLoaderIntf.h",
	"../../visual/tvpfontstruc.h",
	"../../visual/tvpinputdefs.h",
	"../../visual/LayerBitmapIntf.h",
	"../../visual/drawable.h",
	"../../visual/ComplexRect.h",
	"../../visual/LayerIntf.h",
	"../../visual/LayerManager.h",
	"../../visual/WindowIntf.h",
	"../../visual/android/WindowImpl.h",
	# "../../visual/DrawDevice.h",
	"../../visual/voMode.h",
	"../../visual/VideoOvlIntf.h",
	"../../visual/TransIntf.h",
	"../../visual/transhandler.h",
	"../../visual/tvpgl.h",
	# "../../visual/IA32/tvpgl_ia32_intf.h",
	"../../visual/opengl/OpenGLHeader.h",
]

# num = 0

input_paths = [
	"./",
	"../../../../external/krkrz/base/win32",
]

script_path = os.path.abspath(os.path.dirname(__file__))

def relative_open(fn, mode="r"):
	if "r" in mode:
		for x in input_paths:
			try:
				return open(script_path + "/" + x + "/" + fn, mode)
			except FileNotFoundError:
				continue
		raise Exception("File " + fn + " could not be opened")
	return open(script_path + "/" + fn, mode)

def process_extra_args(content, searchpos):
	pos_end = content.rfind("\n", 0, searchpos)
	pos_start = content.rfind("\n", 0, pos_end)
	if -1 not in [pos_start, pos_end]:
		pos_start += 1
		lastline = content[pos_start:pos_end]
		if lastline.startswith("/* $$(") and lastline.endswith(")$$ */"):
			return json.loads(lastline[6:-6])
	return {}

def normalize_string(str_):
	array1 = []
	array2 = []
	str_ = re.sub(r"^\s*(.*?)\s*$", r"\1", str_)
	str_ = re.sub(r"\*\*", "* *", str_) # g
	str_ = re.sub(r"\*\*", "* *", str_) # g
	array1 = re.split(r"\s|\b", str_)
	for str__ in array1:
		if re.search(r"\s", str__) == None and str__ != "":
			array2.append(str__)
	ret = " ".join(array2)
	return ret

def get_arg_names(args):
	array1 = args.split(",")
	args = []
	for arg in array1:
		arg = re.sub(r"^\s*(.*?)\s*$", r"\1", arg)
		srch = re.search(r"^(.*)=(.*)$", arg)
		if srch != None:
			arg = srch.group(1)
		arg = re.sub(r"^\s*(.*?)\s*$", r"\1", arg)
		arg = re.search(r"(\w+)$", arg)
		if arg != None:
			args.append(arg.group(1))
	return ", ".join(args)

def except_arg_names(args):
	array1 = args.split(",")
	args = []
	for arg in array1:
		arg = re.sub(r"^\s*(.*?)\s*$", r"\1", arg)
		srch = re.search(r"^(.*)=(.*)$", arg)
		if srch != None:
			arg = srch.group(1)
		arg = re.sub(r"^\s*(.*?)\s*$", r"\1", arg)
		arg = re.sub(r"(.*?)(\w+)$", r"\1", arg)
		arg = re.sub(r"^\s*(.*?)\s*$", r"\1", arg)
		arg = re.search(r"^\s*(.*?)\s*$", arg)
		args.append(normalize_string(arg.group(1)))
	return ",".join(args)

def get_ret_type(type_, prefix):
	if type_ == (prefix + "_METHOD_RET_EMPTY"):
		return "void"
	srch = re.search(prefix + r"_METHOD_RET\((.*?)\)", type_)
	if srch != None:
		return normalize_string(srch.group(1))
	return normalize_string(type_)

def make_func_stub(ofh, func_list, h_stub, rettype, name, arg, extra_arg, type_, prefix, isconst, isstatic):
	rettype = rettype.replace("\n", " ").replace("\t", " ")
	name = name.replace("\n", " ").replace("\t", " ")
	arg = arg.replace("\n", " ").replace("\t", " ")
	
	func_exp_name = \
		("" if re.search(r"^" + prefix + r"_METHOD_RET", rettype) != None else normalize_string(rettype) + " ") + \
		type_ + "::" + normalize_string(name) + "(" + except_arg_names(arg) + ")" + (" const" if isconst else "")
	
	md5 = hashlib.md5(func_exp_name.encode("ASCII")).hexdigest()

	mangled = {}
	mangled["func_stub_name"] = ("TVP_Stub_" + md5)
	mangled["func_exp_name"] = (func_exp_name)
	mangled["func_prototype_ptr"] = (get_ret_type(rettype, prefix) + \
		"(STDCALL *  __TVP_Stub_" + md5 + ")(" + type_ + " *_this" + (", " if arg != "" else "") + normalize_string(arg) + ")")
	mangled["func_prototype"] = (("" if re.search(r"^" + prefix + r"_METHOD_RET", rettype) != None else normalize_string(rettype)) + \
		" " + normalize_string(type_) + "::" + normalize_string(name) + "(" + normalize_string(arg) + ")")
	mangled["name"] = ("TVP_Stub_" + md5)
	mangled["md5"] = (md5)
	mangled["arg_names"] = (get_arg_names(arg))
	functype = get_ret_type(rettype, prefix) + \
		"(STDCALL * __functype)(" + ("const " if isconst else "") + \
		("" if isstatic else (type_ + " *" + (", " if arg != "" else ""))) + \
		normalize_string(except_arg_names(arg)) + ")"
	mangled["functype"] = (functype)
	mangled["extra_arg"] = (extra_arg)

	noreturn = 0
	if rettype == prefix + "_METHOD_RET_EMPTY":
		noreturn = 1
	elif re.search(prefix + r"_METHOD_RET\((.*?)\)", rettype) != None:
		noreturn = 2

	rettype = get_ret_type(rettype, prefix)

	ifpp = ""
	endifpp = ""
	if "tp_stub_ppcond" in mangled["extra_arg"]:
		ifpp = "#if " + mangled["extra_arg"]["tp_stub_ppcond"] + "\n"
		endifpp = "#endif\n"

	ofh.write(ifpp)
	ofh.write("static ")
	ofh.write(normalize_string(rettype))
	ofh.write(" STDCALL ")
	ofh.write(mangled["func_stub_name"])
	if isstatic:
		ofh.write("(")
	else:
		ofh.write("(" + type_ + " * _this")
		if arg != "":
			ofh.write(", ")

	ofh.write(normalize_string(arg))
	ofh.write(")\n")
	ofh.write("{\n")
	if name == type_:
		# constructor
		ofh.write("\t::new (_this) " + name + "(" + mangled["arg_names"] + ");\n")
		h = "\t" + normalize_string(name) + "(" + normalize_string(arg) + ")\n" + \
			"\t{\n" + \
			"\t\tif(!TVPImportFuncPtr" + mangled["md5"] + ")\n" + \
			"\t\t{\n" + \
			"\t\t\tstatic char funcname[] = \"" + mangled["func_exp_name"] + "\";\n" + \
			"\t\t\tTVPImportFuncPtr" + mangled["md5"] + " = TVPGetImportFuncPtr(funcname);\n" + \
			"\t\t}\n" + \
			"\t\ttypedef " + mangled["functype"] + ";\n" + \
			"\t\t((__functype)(TVPImportFuncPtr" + mangled["md5"] + "))(" + ("" if isstatic else ("this" + (", " if mangled["arg_names"] != "" else ""))) + mangled["arg_names"] + ");\n" + \
			"\t}\n"
	elif name == ("~" + type_):
		# destructor
		ofh.write("\t_this->" + name + "(" + mangled["arg_names"] + ");\n")
		h = "\t" + normalize_string(name) + "(" + normalize_string(arg) + ")\n" + \
			"\t{\n" + \
			"\t\tif(!TVPImportFuncPtr" + mangled["md5"] + ")\n" + \
			"\t\t{\n" + \
			"\t\t\tstatic char funcname[] = \"" + mangled["func_exp_name"] + "\";\n" + \
			"\t\t\tTVPImportFuncPtr" + mangled["md5"] + " = TVPGetImportFuncPtr(funcname);\n" + \
			"\t\t}\n" + \
			"\t\ttypedef " + mangled["functype"] + ";\n" + \
			"\t\t((__functype)(TVPImportFuncPtr" + mangled["md5"] + "))(" + ("" if isstatic else ("this" + (", " if mangled["arg_names"] != "" else ""))) + mangled["arg_names"] + ");\n" + \
			"\t}\n"
	else:
		ofh.write("\t")
		ofh.write("return ")
		if isstatic:
			ofh.write(type_ + "::" + name + "(" + mangled["arg_names"] + ");\n")
		else:
			ofh.write("_this->" + name + "(" + mangled["arg_names"] + ");\n")
		h = "\t" + ("static " if isstatic else "") + ("" if noreturn else rettype + " ") + normalize_string(name) + "(" + normalize_string(arg) + \
			")" + (" const" if isconst else "") + "\n" + \
			"\t{\n" + \
			"\t\tif(!TVPImportFuncPtr" + mangled["md5"] + ")\n" + \
			"\t\t{\n" + \
			"\t\t\tstatic char funcname[] = \"" + mangled["func_exp_name"] + "\";\n" + \
			"\t\t\tTVPImportFuncPtr" + mangled["md5"] + " = TVPGetImportFuncPtr(funcname);\n" + \
			"\t\t}\n" + \
			"\t\ttypedef " + mangled["functype"] + ";\n" + \
			"\t\t" + ("" if rettype == "void" else "return ") + \
			"((__functype)(TVPImportFuncPtr" + mangled["md5"] + "))(" + ("" if isstatic else ("this" + (", " if mangled["arg_names"] != "" else ""))) + mangled["arg_names"] + ");\n" + \
			"\t}\n"
	ofh.write("}\n")
	ofh.write(endifpp)
	h = ifpp + h + endifpp

	func_list.append(mangled)
	h_stub.append(h)
	# num += 1

def list_func_stub(ofh, func_list, h_stub, prefix, content, type_):
	for match_obj in re.finditer(prefix + r"_METHOD_DEF\(\s*(.*?)\s*,\s*(.*?)\s*,\s*\((.*?)\)\s*\)", content, flags=re.S): # g
		make_func_stub(ofh=ofh, func_list=func_list, h_stub=h_stub, rettype=match_obj.group(1), name=match_obj.group(2), arg=match_obj.group(3), extra_arg=process_extra_args(content, match_obj.start()), type_=type_, prefix=prefix, isconst=False, isstatic=False)
	for match_obj in re.finditer(prefix + r"_CONST_METHOD_DEF\(\s*(.*?)\s*,\s*(.*?)\s*,\s*\((.*?)\)\s*\)", content, flags=re.S): # g
		make_func_stub(ofh=ofh, func_list=func_list, h_stub=h_stub, rettype=match_obj.group(1), name=match_obj.group(2), arg=match_obj.group(3), extra_arg=process_extra_args(content, match_obj.start()), type_=type_, prefix=prefix, isconst=True,  isstatic=False)
	for match_obj in re.finditer(prefix + r"_STATIC_METHOD_DEF\(\s*(.*?)\s*,\s*(.*?)\s*,\s*\((.*?)\)\s*\)", content, flags=re.S): # g
		make_func_stub(ofh=ofh, func_list=func_list, h_stub=h_stub, rettype=match_obj.group(1), name=match_obj.group(2), arg=match_obj.group(3), extra_arg=process_extra_args(content, match_obj.start()), type_=type_, prefix=prefix, isconst=False, isstatic=True)
	for match_obj in re.finditer(prefix + r"_STATIC_CONST_METHOD_DEF\(\s*(.*?)\s*,\s*(.*?)\s*,\s*\((.*?)\)\s*\)", content, flags=re.S): # g
		make_func_stub(ofh=ofh, func_list=func_list, h_stub=h_stub, rettype=match_obj.group(1), name=match_obj.group(2), arg=match_obj.group(3), extra_arg=process_extra_args(content, match_obj.start()), type_=type_, prefix=prefix, isconst=True,  isstatic=True)

def make_exp_stub(ofh, func_list, rettype, name, arg, extra_arg):
	rettype = rettype.replace("\n", " ").replace("\t", " ")
	name = name.replace("\n", " ").replace("\t", " ")
	arg = arg.replace("\n", " ").replace("\t", " ")
	
	func_exp_name = normalize_string(rettype) + " " + \
		"::" + normalize_string(name) + "(" + except_arg_names(arg) + ")";

	md5 = hashlib.md5(func_exp_name.encode("ASCII")).hexdigest()

	mangled = {}
	mangled["func_stub_name"] = ("TVP_Stub_" + md5)
	mangled["func_exp_name"] = (func_exp_name)
	mangled["func_prototype_ptr"] = (normalize_string(rettype) + \
		" (STDCALL *" + normalize_string(name) + ")(" + normalize_string(arg) + ")")
	mangled["func_prototype"] = (normalize_string(rettype) + " " + normalize_string(name) + "(" + \
		normalize_string(arg) + ")")
	mangled["name"] = (name)
	mangled["md5"] = (md5)
	mangled["arg_names"] = (get_arg_names(arg))
	mangled["functype"] = (normalize_string(rettype) + \
		" (STDCALL * __functype)(" + normalize_string(except_arg_names(arg)) + ")")
	mangled["rettype"] = (normalize_string(rettype))
	mangled["extra_arg"] = (extra_arg)

	ifpp = ""
	endifpp = ""
	if "tp_stub_ppcond" in mangled["extra_arg"]:
		ifpp = "#if " + mangled["extra_arg"]["tp_stub_ppcond"] + "\n"
		endifpp = "#endif\n"

	ofh.write(ifpp)
	ofh.write("static ")
	ofh.write(mangled["rettype"])
	ofh.write(" STDCALL ")
	ofh.write(mangled["func_stub_name"] + "(")
	ofh.write(normalize_string(arg))
	ofh.write(")\n")
	ofh.write("{\n")
	ofh.write("\t")
	ofh.write("return ")
	ofh.write(mangled["name"] + "(" + mangled["arg_names"] + ");\n")
	ofh.write("}\n")
	ofh.write(endifpp)

	func_list.append(mangled)
	# num += 1

def process_exp_stub(ofh, defs, impls, func_list, file):

	content = ""
	with relative_open(file, "r") as f:
		content = f.read()

	for match_obj in re.finditer(r"\/\*\[\*\/(.*?)\/\*\]\*\/", content, flags=re.S): # g
		defs.append(match_obj.group(1))
	for match_obj in re.finditer(r"\/\*\[C\*\/(.*?)\/\*C]\*\/", content, flags=re.S): # g
		impls.append(match_obj.group(1))
	for match_obj in re.finditer(r"TJS_EXP_FUNC_DEF\(\s*(.*?)\s*,\s*(.*?)\s*,\s*\((.*?)\)\s*\)", content, flags=re.S): # g
		make_exp_stub(ofh=ofh, func_list=func_list, rettype=match_obj.group(1), name=match_obj.group(2), arg=match_obj.group(3), extra_arg=process_extra_args(content, match_obj.start()))
	for match_obj in re.finditer(r"TVP_GL_FUNC_PTR_EXTERN_DECL\(\s*(.*?)\s*,\s*(.*?)\s*,\s*\((.*?)\)\s*\)", content, flags=re.S): # g
		make_exp_stub(ofh=ofh, func_list=func_list, rettype=match_obj.group(1), name=match_obj.group(2), arg=match_obj.group(3), extra_arg=process_extra_args(content, match_obj.start()))

# undef $/
func_list = []

ofh = relative_open("FuncStubs.h", "w")

ofh.write(copyright)

ofh.write("""
extern void TVPExportFunctions();

""")


ofh = relative_open("FuncStubs.cpp", "w")

ofh.write(copyright)

ofh.write("""

#include "tjsCommHead.h"

#include "tjsVariant.h"
#include "tjsString.h"
#include "PluginImpl.h"

#define STDCALL

""")

class_stub_h_stub = {}
method_list = []

for type_ in class_stub_round_1:
	entry_file = class_stub_name_to_file[type_]

	fh = relative_open(entry_file)
	content = fh.read()

	h_stub = []

	srch = re.search(r"\/\*start-of-%s\*\/(.*?)\/\*end-of-%s\*\/" % (type_, type_), content, flags=re.S)
	list_func_stub(ofh=ofh, func_list=method_list, h_stub=h_stub, prefix="TJS", content=srch.group(1), type_=type_)
	class_stub_h_stub[type_] = h_stub

impls = []

func_list = []

defs_system = []

for file in func_headers_exports_system:
	basename = os.path.basename(file)
	splitext = os.path.splitext(basename)
	if len(splitext) >= 2 and splitext[1] == ".h":
		ofh.write("#include \"%s\"\n" % basename)
	process_exp_stub(ofh=ofh, defs=defs_system, impls=impls, func_list=func_list, file=file)

defs_misc = []

for file in func_headers_exports_misc:
	basename = os.path.basename(file)
	splitext = os.path.splitext(basename)
	if len(splitext) >= 2 and splitext[1] == ".h":
		ofh.write("#include \"%s\"\n" % basename)
	process_exp_stub(ofh=ofh, defs=defs_misc, impls=impls, func_list=func_list, file=file)

all_list = [*method_list, *func_list]

ofh.write("\n#include <zlib.h>")

func_data = b"".join([pair["func_exp_name"].encode("ASCII") + b"\x00" for pair in all_list])

deflateout = b""

deflateout = zlib.compress(func_data)

emitdata = "".join([("0x%02x, " % x) for x in deflateout])
emitdata = "".join([emitdata[i:i + 96] + "\n" for i in range(0, len(emitdata), 96)])
# $emitdata =~ s/(.*?), \n/\t__emit__($1);\n/sg;

ofh.write("""
/* function table is pretty large; is compressed via zlib */
static tjs_uint8 compressed_functable[] = {
""" + \
emitdata + \
"""
};
static void * func_ptrs[] = {
""")

ofh.write("".join(["\t(void*)" + pair["func_stub_name"] + ",\n" for pair in all_list]))

ofh.write("""
};
""")

ofh.write("""

void TVPExportFunctions()
{
""")

ofh.write("\tconst unsigned long compressed_size = " + str(len(deflateout)) + ";\n")
ofh.write("\tconst unsigned long decompressed_size = " + str(len(func_data)) + ";\n")
ofh.write("\tconst tjs_int function_count = " + str(len(all_list)) + ";\n")

ofh.write("""\
	unsigned char * dest = new unsigned char [decompressed_size];

	try
	{
		unsigned long dest_size = decompressed_size;

		int result = uncompress(dest, &dest_size,
			(unsigned char*)compressed_functable, compressed_size);
		if(result != Z_OK || dest_size != decompressed_size) { TVPThrowInternalError; }

		const unsigned char *p = dest;

		for(tjs_int i = 0; i < function_count; i++)
		{
			TVPAddExportFunction((const char *)p, ((void **)func_ptrs)[i]);
			while(*p) p++;
			p++;
		}
	}
	catch(...)
	{
		delete [] dest;
		throw;
	}
	delete [] dest;
}
""")

# stub library for plugin

if output_tpstub_h == "":
	ohfh = io.StringIO()
else:
	ohfh = open(output_tpstub_h, "w+")

if output_tpstub_cpp == "":
	ocfh = io.StringIO()
else:
	ocfh = open(output_tpstub_cpp, "w+")

ohfh.write(copyright)
ocfh.write(copyright)

func_count = len(all_list) + 1



ocfh.write("""
#include "tp_stub.h"

#define TVP_IN_PLUGIN_STUB

tjs_int TVPPluginGlobalRefCount = 0;

//---------------------------------------------------------------------------
// stubs
//---------------------------------------------------------------------------
""")

ohfh.write("""\
#ifndef __TP_STUB_H__
#define __TP_STUB_H__

#ifndef __cplusplus
	#error Sorry, currently tp_stub.h can only be used in C++ mode.
#endif

#define STDCALL
#define DLL_EXPORT
#include <string>
#include <stdarg.h>


""")

ohfh.write("\n".join(defs_system) + "\n")

ohfh.write("""
#ifdef __BORLANDC__
#pragma warn -8027
#endif

//---------------------------------------------------------------------------
// function import pointers
//---------------------------------------------------------------------------

extern void * TVPGetImportFuncPtr(const char *name);


""")

ohfh.write("".join(["extern void * TVPImportFuncPtr" + pair["md5"] + ";\n" for pair in all_list]))
ohfh.write("\n\n")

class_template_1 = """\
//---------------------------------------------------------------------------
// %s
//---------------------------------------------------------------------------

"""

class_template_2 = """\
class %s : protected %s_S
{
%s
public:
"""

class_template_2_1 = "	// do not create an instance of this class directly.\n"

class_template_3 = """\
};
"""

for type_ in class_stub_round_2:
	entry_file = class_stub_name_to_file[type_]

	ohfh.write(class_template_1 % type_)

	fh = relative_open(entry_file)
	content = fh.read()

	do_not_create_directly_comment = "" if type_ not in class_stub_do_not_create_directly else class_template_2_1

	ohfh.write(class_template_2 % (type_, type_, do_not_create_directly_comment))

	ohfh.write("".join([each + "\n" for each in class_stub_h_stub[type_]]))

	srch = re.search(r"\/\*start-of-%s\*\/(.*?)\/\*end-of-%s\*\/" % (type_, type_), content, flags=re.S)
	class_ = srch.group(1)

	ohfh.write("".join(["\t" + match_obj.group(1) + "\n\n" for match_obj in re.finditer(r"\/\*m\[\*\/(.*?)\/\*\]m\*\/", class_, flags=re.S)])) # g

	ohfh.write(class_template_3)


ohfh.write("""\

//---------------------------------------------------------------------------
// stubs (misc)
//---------------------------------------------------------------------------

""")

ohfh.write("\n".join(defs_misc) + "\n")

ohfh.write("""\
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// stubs
//---------------------------------------------------------------------------

""")

for pair in func_list:
	ifpp = ""
	endifpp = ""
	if "tp_stub_ppcond" in pair["extra_arg"]:
		ifpp = "#if " + pair["extra_arg"]["tp_stub_ppcond"] + "\n"
		endifpp = "#endif\n"

	ohfh.write(ifpp)
	ohfh.write("inline " + pair["func_prototype"] + "\n")
	ohfh.write("{\n")
	ohfh.write( \
		"\tif(!TVPImportFuncPtr" + pair["md5"] + ")\n" + \
		"\t{\n" + \
		"\t\tstatic char funcname[] = \"" + pair["func_exp_name"] + "\";\n" + \
		"\t\tTVPImportFuncPtr" + pair["md5"] + " = TVPGetImportFuncPtr(funcname);\n" + \
		"\t}\n" \
		)
	ohfh.write("\ttypedef " + pair["functype"] + ";\n")
	ohfh.write("\t" + ("" if pair["rettype"] == "void" else "return ") + "((__functype)(TVPImportFuncPtr" + pair["md5"] + "))")
	ohfh.write("(" + pair["arg_names"] + ");\n")
	ohfh.write("}\n")
	ohfh.write(endifpp)

ocfh.write("""\
//---------------------------------------------------------------------------
// Stub library setup
//---------------------------------------------------------------------------

static iTVPFunctionExporter * TVPFunctionExporter = NULL;

void * TVPGetImportFuncPtr(const char *name)
{
	void *ptr;
	if(TVPFunctionExporter->QueryFunctionsByNarrowString(&name, &ptr, 1))
	{
		// succeeded
	}
	else
	{
		// failed
		static const char *funcname = "void ::TVPThrowPluginUnboundFunctionError(const char *)";
		if(!TVPFunctionExporter->QueryFunctionsByNarrowString(&funcname, &ptr, 1))
		{
			*(int*)0 = 0; // causes an error
		}
		typedef void (STDCALL * __functype)(const char *);
		((__functype)(ptr))(name);
	}
	return ptr;
}

/* TVPInitImportStub : stub initialization */
bool TVPInitImportStub(iTVPFunctionExporter * exporter)
{
	// set TVPFunctionExporter
	TVPFunctionExporter = exporter;
	return true;
}

/* TVPUninitImportStub : stub uninitialization */
void TVPUninitImportStub()
{
}



""")

ocfh.write("".join(["void * TVPImportFuncPtr" + pair["md5"] + " = NULL;\n" for pair in all_list]))

ocfh.write("\n".join(impls) + "\n")

ohfh.write("""
#ifdef __BORLANDC__
#pragma warn .8027
#endif

//---------------------------------------------------------------------------
// Stub library setup
//---------------------------------------------------------------------------
extern bool TVPInitImportStub(iTVPFunctionExporter * exporter);
extern void TVPUninitImportStub();
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global reference count
//---------------------------------------------------------------------------
extern tjs_int TVPPluginGlobalRefCount;
//---------------------------------------------------------------------------

""")


#---------------------------------------------------------------------------
# exception protected function stub
#---------------------------------------------------------------------------

# currently only for iTJSDispatch2

# read the header
ohfh.seek(0)
# undef $/
oh = ohfh.read()

srch = re.search(r"class\s+iTJSDispatch2\s+\{(.*?)\}", oh, flags=re.S)
class_iTJSDispatch2 = srch.group(1)

# eliminate comments
class_iTJSDispatch2 = re.sub(r"//.*?\r?\n", r"", class_iTJSDispatch2, flags=re.S) # g
class_iTJSDispatch2 = re.sub(r"/\*.*?\*/", r"", class_iTJSDispatch2, flags=re.S) # g

# extract method declarations
hc = []
cc = []

cc.append("""

static bool TJS_USERENTRY _CatchFuncCall(void *data, const tTVPExceptionDesc & desc)
{
	throw desc;
}
""")


for match_obj in re.finditer(r"virtual\s+(\w+)\s+TJS_INTF_METHOD\s+(\w+)\s*\(\s*(.*?)\s*\)", class_iTJSDispatch2, flags=re.S): # g
	ret_type = match_obj.group(1)
	method_name = match_obj.group(2)
	tmp = match_obj.group(3)
	args = re.split(r"\s*,\s*", "" if tmp == "void" else tmp)
	if args[-1] == "":
		del args[-1]

	hc.append(\
		("extern " + ret_type + " Try_iTJSDispatch2_" + method_name + "(" + \
			", ".join(["iTJSDispatch2 * _this", *args]) + ");\n"))


	cc.append("struct t_iTJSDispatch2_" + method_name + "\n")
	cc.append("{\n")
	if ret_type != "void":
		cc.append("\t" + ret_type + " _ret;\n")

	for arg in ["iTJSDispatch2 * _this", *args]:
		cc.append("\t" + arg + ";\n")

	arg_names = []
	for arg in args:
		srch = re.search(r"(\w+)$", arg)
		if srch != None:
			arg_names.append(srch.group(1))
	cc.append("\tt_iTJSDispatch2_" + method_name + "(\n\t\t\t")
	cc.append("_,\n\t\t\t".join(["iTJSDispatch2 * _this", *args]))
	cc.append("_\n\t\t\t) :\n\t\t")

	arg_initials = []
	for arg_name in ["_this", *arg_names]:
		arg_initials.append(arg_name + "(" + arg_name + "_)")

	cc.append(",\n\t\t".join(arg_initials))
	cc.append("\t{;}\n")

	cc.append("\n};\n")

	cc.append("static void TJS_USERENTRY _Try_iTJSDispatch2_" + method_name + "(void *data)\n")
	cc.append("{\n")
	cc.append("	t_iTJSDispatch2_" + method_name + " * arg = (t_iTJSDispatch2_" + method_name + " *)data;\n")
	if ret_type != "void":
		cc.append("	arg->_ret = \n")
	cc.append("	arg->_this->" + method_name + "(\n		")
	arg_args = []
	for arg_name in arg_names:
		arg_args.append("arg->" + arg_name)

	cc.append(",\n\t\t".join(arg_args))
	cc.append("\n		);\n")
	cc.append("}\n")
	cc.append(\
		(ret_type + " Try_iTJSDispatch2_" + method_name + "(" + \
			", ".join(["iTJSDispatch2 * _this", *args]) + ")\n"))
	cc.append("{\n")
	cc.append("	t_iTJSDispatch2_" + method_name + " arg(\n		")
	cc.append(",\n		".join(["_this", *arg_names]))
	cc.append("\n	);\n")
	cc.append("	TVPDoTryBlock(_Try_iTJSDispatch2_" + method_name + ", _CatchFuncCall, NULL, &arg);\n")
	if ret_type != "void":
		cc.append("	return arg._ret;\n")
	cc.append("}\n")

ohfh.seek(0, 2)
ohfh.write("""\
//---------------------------------------------------------------------------
// exception protected function stub
//---------------------------------------------------------------------------

""")

ohfh.write("".join(hc))
ohfh.write("""
#endif
""")

ocfh.seek(0, 2)
ocfh.write("""\
//---------------------------------------------------------------------------
// exception protected function stub
//---------------------------------------------------------------------------
""")

ocfh.write("".join(cc))

#---------------------------------------------------------------------------
