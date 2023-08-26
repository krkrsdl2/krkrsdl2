//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// "Plugins" class implementation / Service for plug-ins
//---------------------------------------------------------------------------
#ifndef PluginImplH
#define PluginImplH
//---------------------------------------------------------------------------
#include <memory.h>
#ifdef _WIN32
#include <objidl.h> // for IStream
#endif

#include "PluginIntf.h"

#ifdef TVP_SUPPORT_KPI
	#include "kmp_pi.h"
#endif

#ifndef _WIN32
#include "StorageImpl.h"
#endif

//---------------------------------------------------------------------------
/*[*/
//---------------------------------------------------------------------------
// iTVPFunctionExporter, exporting main module's functions
//---------------------------------------------------------------------------
struct iTVPFunctionExporter
{
	virtual bool TJS_INTF_METHOD QueryFunctions(const tjs_char **name, void **function,
		tjs_uint count) = 0;
	virtual bool TJS_INTF_METHOD QueryFunctionsByNarrowString(const char **name,
		void **function, tjs_uint count) = 0;
};
//---------------------------------------------------------------------------


/*]*/
//---------------------------------------------------------------------------

/*[*/
//---------------------------------------------------------------------------
// TSS related definitions for non-Win32 platforms
//---------------------------------------------------------------------------

#ifdef _WIN32
#ifndef TSS_LPWSTR
#define TSS_LPWSTR LPWSTR
#endif
#ifndef TSS_LONG
#define TSS_LONG long
#endif
#ifndef TSS_ULONG
#define TSS_ULONG unsigned long
#endif
#ifndef TSS_INT64
#define TSS_INT64 __int64
#endif
#ifndef TSS_UINT64
#define TSS_UINT64 unsigned __int64
#endif
#ifndef TSS_HWND
#define TSS_HWND HWND
#endif
#else
#ifndef TSS_LPWSTR
#define TSS_LPWSTR tjs_char *
#endif
#ifndef TSS_LONG
#define TSS_LONG tjs_int32
#endif
#ifndef TSS_ULONG
#define TSS_ULONG tjs_uint32
#endif
#ifndef TSS_INT64
#define TSS_INT64 tjs_int64
#endif
#ifndef TSS_UINT64
#define TSS_UINT64 tjs_uint64
#endif
#ifndef TSS_HWND
#define TSS_HWND void *
#endif
#endif
#ifndef _WIN32
#ifndef __RPC_FAR
#define __RPC_FAR
#endif
typedef struct tagTSSWaveFormat
{
	TSS_ULONG dwSamplesPerSec;
	TSS_ULONG dwChannels;
	TSS_ULONG dwBitsPerSample;
	TSS_ULONG dwSeekable;
	TSS_UINT64 ui64TotalSamples;
	TSS_ULONG dwTotalTime;
	TSS_ULONG dwReserved1;
	TSS_ULONG dwReserved2;
} TSSWaveFormat;

__attribute__((unused)) static const IID IID_ITSSMediaBaseInfo = {0xB4C4239B,0x7D27,0x43CC,{0x85,0x23,0x66,0x95,0x5B,0xDF,0x59,0xDF}};
class ITSSMediaBaseInfo : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE GetMediaType( 
		TSS_LPWSTR shortname,
		TSS_LPWSTR descbuf,
		TSS_ULONG descbuflen) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetLength( 
		TSS_ULONG __RPC_FAR *length) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetTitle( 
		TSS_LPWSTR buf,
		TSS_ULONG buflen) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetCopyright( 
		TSS_LPWSTR buf,
		TSS_ULONG buflen) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetComment( 
		TSS_LPWSTR buf,
		TSS_ULONG buflen) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetArtist( 
		TSS_LPWSTR buf,
		TSS_ULONG buflen) = 0;
};

__attribute__((unused)) static const IID IID_ITSSStorageProvider = {0x7DD61993,0x615D,0x481D,{0xB0,0x60,0xA9,0xFD,0x48,0x39,0x44,0x30}};
class ITSSStorageProvider : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE GetStreamForRead( 
		TSS_LPWSTR url,
		IUnknown __RPC_FAR *__RPC_FAR *stream) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetStreamForWrite( 
		TSS_LPWSTR url,
		IUnknown __RPC_FAR *__RPC_FAR *stream) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetStreamForUpdate( 
		TSS_LPWSTR url,
		IUnknown __RPC_FAR *__RPC_FAR *stream) = 0;
};

__attribute__((unused)) static const IID IID_ITSSModule = {0xA938D1A5,0x2980,0x498B,{0xB0,0x51,0x99,0x93,0x1D,0x41,0x89,0x5D}};
class ITSSModule : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE GetModuleCopyright( 
		TSS_LPWSTR buffer,
		TSS_ULONG buflen) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetModuleDescription( 
		TSS_LPWSTR buffer,
		TSS_ULONG buflen) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetSupportExts( 
		TSS_ULONG index,
		TSS_LPWSTR mediashortname,
		TSS_LPWSTR buf,
		TSS_ULONG buflen) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetMediaInfo( 
		TSS_LPWSTR url,
		ITSSMediaBaseInfo __RPC_FAR *__RPC_FAR *info) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetMediaSupport( 
		TSS_LPWSTR url) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetMediaInstance( 
		TSS_LPWSTR url,
		IUnknown __RPC_FAR *__RPC_FAR *instance) = 0;
};

__attribute__((unused)) static const IID IID_ITSSWaveDecoder = {0x313864E2,0x910E,0x496F,{0x8A,0x6D,0x43,0x46,0x5C,0x10,0x5B,0x58}};
class ITSSWaveDecoder : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE GetFormat( 
		TSSWaveFormat __RPC_FAR *format) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE Render( 
		void __RPC_FAR *buf,
		TSS_ULONG bufsamplelen,
		TSS_ULONG __RPC_FAR *rendered,
		TSS_ULONG __RPC_FAR *status) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetPosition( 
		TSS_UINT64 samplepos) = 0;
};
#endif
/*]*/

//---------------------------------------------------------------------------
struct ITSSModule;
struct IWaveUnpacker;
struct ITSSStorageProvider;
extern "C"
{
	iTVPFunctionExporter * TVPGetFunctionExporter();

	// V2 plug-in
	typedef tjs_error (* tTVPV2LinkProc)(iTVPFunctionExporter *);
	typedef tjs_error (* tTVPV2UnlinkProc)();

	// TSS
	typedef HRESULT (STDMETHODCALLTYPE * tTVPGetModuleInstanceProc)(ITSSModule **out,
		ITSSStorageProvider *provider, IStream * config, TSS_HWND mainwin);
#if 0
	typedef ULONG (_stdcall * tTVPGetModuleThreadModelProc)(void);
	typedef HRESULT (_stdcall * tTVPShowConfigWindowProc)(HWND parentwin,
		IStream * storage );
	typedef ULONG (_stdcall * tTVPCanUnloadNowProc)(void);

#ifdef TVP_SUPPORT_OLD_WAVEUNPACKER
	// WaveUnpacker
	typedef HRESULT (_stdcall * tTVPCreateWaveUnpackerProc)(IStream *storage,long size,
		char *name,IWaveUnpacker **out); // old WaveUnpacker stuff
#endif
#endif
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
struct ITSSWaveDecoder;
extern void TVPRegisterTSSWaveDecoder(tTVPGetModuleInstanceProc GetModuleInstance);
extern ITSSWaveDecoder * TVPSearchAvailTSSWaveDecoder(const ttstr & storage, const ttstr & extension);
#if 0
#ifdef TVP_SUPPORT_OLD_WAVEUNPACKER
class IWaveUnpacker;
extern IWaveUnpacker * TVPSearchAvailWaveUnpacker(const ttstr & storage, IStream **stream);
#endif
#ifdef TVP_SUPPORT_KPI
extern void * TVPSearchAvailKMPWaveDecoder(const ttstr & storage, KMPMODULE ** module,
	SOUNDINFO * info);
#endif
#endif
extern void TVPAddExportFunction(const tjs_char *name, void *ptr);
extern void TVPAddExportFunction(const char *name, void *ptr);
TJS_EXP_FUNC_DEF(void, TVPThrowPluginUnboundFunctionError, (const char *funcname));
TJS_EXP_FUNC_DEF(void, TVPThrowPluginUnboundFunctionError, (const tjs_char *funcname));

inline TJS_EXP_FUNC_DEF(void *, TVP_malloc, (size_t size)) { return malloc(size); }
inline TJS_EXP_FUNC_DEF(void *, TVP_realloc, (void *pp, size_t size)) { return realloc(pp, size); }
inline TJS_EXP_FUNC_DEF(void, TVP_free, (void *pp)) { return free(pp); }
TJS_EXP_FUNC_DEF(tjs_int, TVPGetAutoLoadPluginCount, ());
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(int, ZLIB_uncompress, (unsigned char *dest, unsigned long *destlen, const unsigned char *source, unsigned long sourcelen));
TJS_EXP_FUNC_DEF(int, ZLIB_compress, (unsigned char *dest, unsigned long *destlen, const unsigned char *source, unsigned long sourcelen));
TJS_EXP_FUNC_DEF(int, ZLIB_compress2, (unsigned char *dest, unsigned long *destlen, const unsigned char *source, unsigned long sourcelen, int level));

/*[*/

//---------------------------------------------------------------------------
// this stub includes exported function from Independent implementation of
// MD5 (RFC 1321) by Aladdin Enterprises.
//---------------------------------------------------------------------------
// TVP_md5_init, TVP_md5_append, TVP_md5_finish are exported
typedef struct TVP_md5_state_s { tjs_uint8 buffer[4*2+8+4*4+8+64]; } TVP_md5_state_t; // md5_state_t
//---------------------------------------------------------------------------


/*]*/

TJS_EXP_FUNC_DEF(void, TVP_md5_init, (TVP_md5_state_t *pms));
TJS_EXP_FUNC_DEF(void, TVP_md5_append, (TVP_md5_state_t *pms, const tjs_uint8 *data, int nbytes));
TJS_EXP_FUNC_DEF(void, TVP_md5_finish, (TVP_md5_state_t *pms, tjs_uint8 *digest));

#if defined(_WIN32)
/* $$({"tp_stub_ppcond":"defined(_WIN32)"})$$ */
TJS_EXP_FUNC_DEF(HWND, TVPGetApplicationWindowHandle, ());
#endif
TJS_EXP_FUNC_DEF(void, TVPProcessApplicationMessages, ());
TJS_EXP_FUNC_DEF(void, TVPHandleApplicationMessage, ());

TJS_EXP_FUNC_DEF(bool, TVPRegisterGlobalObject, (const tjs_char *name, iTJSDispatch2 * dsp));
TJS_EXP_FUNC_DEF(bool, TVPRemoveGlobalObject, (const tjs_char *name));

/*[*/
//---------------------------------------------------------------------------
// data types for TVPDoTryBlock
//---------------------------------------------------------------------------
	// TVPDoTryBlock executes specified 'tryblock' in try block.
	// If any exception occured,
	// 'catchblock' is to be executed. 'data' is applicatoin defined data
	// block passed to 'tryblock' and 'catchblock' and 'finallyblock'.
	// if the 'catchblock' returns true, the exception is to be rethrown.
	// if false then the exception is to be vanished.
	// 'finallyblock' can be null, is to be executed whatever the exception
	// is generated or not.

struct tTVPExceptionDesc
{
	ttstr type; // the exception type, currently 'eTJS' or 'unknown'
	ttstr message; // the exception message (if exists. otherwise empty).
};

typedef void (TJS_USERENTRY *tTVPTryBlockFunction)(void * data);
typedef bool (TJS_USERENTRY *tTVPCatchBlockFunction)(void * data, const tTVPExceptionDesc & desc);
typedef void (TJS_USERENTRY *tTVPFinallyBlockFunction)(void *data);
//---------------------------------------------------------------------------




/*]*/

TJS_EXP_FUNC_DEF(void, TVPDoTryBlock, (tTVPTryBlockFunction tryblock, tTVPCatchBlockFunction catchblock, tTVPFinallyBlockFunction finallyblock, void *data));


#if defined(_WIN32)
/* $$({"tp_stub_ppcond":"defined(_WIN32)"})$$ */
TJS_EXP_FUNC_DEF(bool, TVPGetFileVersionOf, (const wchar_t* module_filename, tjs_int &major, tjs_int &minor, tjs_int &release, tjs_int &build));
#endif


//---------------------------------------------------------------------------
#ifdef KRKRSDL2_ENABLE_PLUGINS
extern bool TVPPluginUnloadedAtSystemExit;
#endif
extern void TVPLoadPluigins(void);
//---------------------------------------------------------------------------


#endif
