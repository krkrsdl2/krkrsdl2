#if 0
#define INITGUID // XP 有効にするとdxguid.libが使用できないため
#endif
#include "tjsCommHead.h"

#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>

#if 0
#include <eh.h>

#define DIRECTINPUT_VERSION 0x0500
#include <dinput.h>

#include <DbgHelp.h>
#include <Strsafe.h>
#endif

#include "tjsError.h"
#include "tjsDebug.h"

#include "Application.h"
#include "SysInitIntf.h"
#include "SysInitImpl.h"
#include "DebugIntf.h"
#include "MsgIntf.h"
#include "ScriptMgnIntf.h"
#include "tjsError.h"
#include "PluginImpl.h"
#include "SystemIntf.h"

#include "Exception.h"
#if 0
#include "WindowFormUnit.h"
#include "Resource.h"
#endif
#include "SystemControl.h"
#if 0
#include "MouseCursor.h"
#endif
#include "SystemImpl.h"
#include "WaveImpl.h"
#include "GraphicsLoadThread.h"
#include "CharacterSet.h"
#include "EventIntf.h"
#include "StorageIntf.h"
#include "TVPColor.h"
#include "WindowImpl.h"
#ifndef _WIN32
#include "VirtualKey.h"
#endif
#include "TVPWindow.h"
#include <unistd.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <SDL.h>

#if 0
#include "resource.h"
#endif

#ifdef _MSC_VER
#pragma comment(lib,"dbghelp.lib")
/*
kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;winmm.lib;dsound.lib;version.lib;mpr.lib;shlwapi.lib;vfw32.lib;imm32.lib;zlib_d.lib;jpeg-6bx_d.lib;libpng_d.lib;onig_s_d.lib;freetype250MT_D.lib;tvpgl_ia32.lib;tvpsnd_ia32.lib;%(AdditionalDependencies)
kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;winmm.lib;dsound.lib;version.lib;mpr.lib;shlwapi.lib;vfw32.lib;imm32.lib;zlib.lib;jpeg-6bx.lib;libpng.lib;onig_s.lib;freetype250MT.lib;tvpgl_ia32.lib;tvpsnd_ia32.lib;%(AdditionalDependencies)
*/
#endif

tTVPApplication* Application;

#if 0
#ifdef TJS_64BIT_OS
extern void TVPHandleSEHException( int ErrorCode, EXCEPTION_RECORD *P, unsigned long long osEsp, PCONTEXT ctx);
#else
extern void TVPHandleSEHException( int ErrorCode, EXCEPTION_RECORD *P, unsigned long osEsp, PCONTEXT ctx);
#endif

// アプリケーションの開始時に呼ぶ
inline void CheckMemoryLeaksStart()
{
#ifdef  _DEBUG
   _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif  // _DEBUG
}

inline void DumpMemoryLeaks()
{
#ifdef  _DEBUG
	int is_leak = _CrtDumpMemoryLeaks();
	assert( !is_leak );
#endif  // _DEBUG
}

#ifdef  _DEBUG
#ifdef _MSC_VER
class MemoryLeaksDebugBreakPoint {
public:
	MemoryLeaksDebugBreakPoint() {
		// WinMain よりも前のメモリリークを検出したい時、ここにブレークを張る
		int a = 0;
	}
};
#pragma init_seg(lib)
static MemoryLeaksDebugBreakPoint gMemoryLeaksDebugBreakPoint;
#endif
#endif
#endif

#include <unistd.h>
#if defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_MAC && !TARGET_OS_IPHONE
#include <libproc.h>
#endif
#endif
tjs_string ExePath() {
#if 0
	tjs_char szFull[_MAX_PATH];
	::GetModuleFileName(NULL, szFull, sizeof(szFull) / sizeof(tjs_char));
	return tjs_string(szFull);
#endif
	static tjs_string exepath(TJS_W(""));
#if defined(__APPLE__) && (TARGET_OS_MAC && !TARGET_OS_IPHONE)
	if (exepath.empty())
	{
		char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
		int ret = proc_pidpath(getpid(), pathbuf, sizeof(pathbuf));
		if (ret > 0)
		{
			std::string npathbuf = pathbuf;
			TVPUtf8ToUtf16(exepath, npathbuf);
		}
	}
#endif
#ifdef __linux__
	if (exepath.empty())
	{
		size_t size = 256;
		char *buf = (char *)malloc(size);
		ssize_t retsize = readlink("/proc/self/exe", buf, size);
		while (retsize != -1 && retsize == size && buf != nullptr)
		{
			size *= 2;
			buf = (char *)realloc(buf, size);
			retsize = readlink("/proc/self/exe", buf, size);
		}
		if (buf)
		{
			buf[retsize] = 0;
			std::string nbuf = buf;
			TVPUtf8ToUtf16(exepath, nbuf);
			free(buf);
		}
	}
#endif
#ifndef __EMSCRIPTEN__
	if (exepath.empty()) {
		exepath = tjs_string(_wargv[0]);
	}
#endif
	return exepath;
}

#if 0
bool TVPCheckAbout();
#endif
bool TVPCheckPrintDataPath();
void TVPOnError();
void TVPLockSoundMixer();
void TVPUnlockSoundMixer();

int _argc;
tjs_char ** _wargv;
#if 0
extern void TVPInitCompatibleNativeFunctions();
extern void TVPLoadMessage();

AcceleratorKeyTable::AcceleratorKeyTable() {
	// デフォルトを読み込む
	hAccel_ = ::LoadAccelerators( (HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDC_TVPWIN32));
}
AcceleratorKeyTable::~AcceleratorKeyTable() {
	std::map<HWND,AcceleratorKey*>::iterator i = keys_.begin();
	for( ; i != keys_.end(); i++ ) {
		delete (i->second);
	}
}
void AcceleratorKeyTable::AddKey( HWND hWnd, WORD id, WORD key, BYTE virt ) {
	std::map<HWND,AcceleratorKey*>::iterator i = keys_.find(hWnd);
	if( i != keys_.end() ) {
		i->second->AddKey(id,key,virt);
	} else {
		AcceleratorKey* acc = new AcceleratorKey();
		acc->AddKey( id, key, virt );
		keys_.insert( std::map<HWND, AcceleratorKey*>::value_type( hWnd, acc ) );
	}
}
void AcceleratorKeyTable::DelKey( HWND hWnd, WORD id ) {
	std::map<HWND,AcceleratorKey*>::iterator i = keys_.find(hWnd);
	if( i != keys_.end() ) {
		i->second->DelKey(id);
	}
}

void AcceleratorKeyTable::DelTable( HWND hWnd ) {
	std::map<HWND,AcceleratorKey*>::iterator i = keys_.find(hWnd);
	if( i != keys_.end() ) {
		delete (i->second);
		keys_.erase(i);
	}
}
AcceleratorKey::AcceleratorKey() : hAccel_(NULL), keys_(NULL), key_count_(0) {
}
AcceleratorKey::~AcceleratorKey() {
	if( hAccel_ != NULL ) ::DestroyAcceleratorTable( hAccel_ );
	delete[] keys_;
}
void AcceleratorKey::AddKey( WORD id, WORD key, BYTE virt ) {
	// まずは存在するかチェックする
	bool found = false;
	int index = 0;
	for( int i = 0; i < key_count_; i++ ) {
		if( keys_[i].cmd == id ) {
			index = i;
			found = true;
			break;
		}
	}
	if( found ) {
		// 既に登録されているコマンドなのでキー情報の更新を行う
		if( keys_[index].key == key && keys_[index].fVirt == virt ) {
			// 変更されていない
			return;
		}
		keys_[index].key = key;
		keys_[index].fVirt = virt;
		HACCEL hAccel = ::CreateAcceleratorTable( keys_, key_count_ );
		if( hAccel_ != NULL ) ::DestroyAcceleratorTable( hAccel_ );
		hAccel_ = hAccel;
	} else {
		ACCEL* table = new ACCEL[key_count_+1];
		for( int i = 0; i < key_count_; i++ ) {
			table[i] = keys_[i];
		}
		table[key_count_].cmd = id;
		table[key_count_].key = key;
		table[key_count_].fVirt = virt;
		key_count_++;
		HACCEL hAccel = ::CreateAcceleratorTable( table, key_count_ );
		if( hAccel_ != NULL ) ::DestroyAcceleratorTable( hAccel_ );
		hAccel_ = hAccel;
		delete[] keys_;
		keys_ = table;
	}

}
void AcceleratorKey::DelKey( WORD id ) {
	// まずは存在するかチェックする
	bool found = false;
	for( int i = 0; i < key_count_; i++ ) {
		if( keys_[i].cmd == id ) {
			found = true;
			break;
		}
	}
	if( found == false ) return;

	// 存在した場合作り直し
	ACCEL* table = new ACCEL[key_count_-1];
	int dest = 0;
	for( int i = 0; i < key_count_; i++ ) {
		if( keys_[i].cmd != id ) {
			table[dest] = keys_[i];
			dest++;
		}
	}
	key_count_--;
	HACCEL hAccel = ::CreateAcceleratorTable( table, key_count_ );
	if( hAccel_ != NULL ) ::DestroyAcceleratorTable( hAccel_ );
	hAccel_ = hAccel;
	delete[] keys_;
	keys_ = table;
}

int APIENTRY wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow ) {
	try {
		CheckMemoryLeaksStart();
		// ウォッチで _crtBreakAlloc にセットする

		// XP より後で使えるAPIを動的に読み込んで互換性を取る
		TVPInitCompatibleNativeFunctions();

		// メッセージ文字列をリソースから読込み
		TVPLoadMessage();

		_argc = __argc;
		_wargv = __wargv;

		MouseCursor::Initialize();
		Application = new tTVPApplication();
		Application->StartApplication( __argc, __wargv );
	
		// delete application and exit forcely
		// this prevents ugly exception message on exit
		// アプリケーションを削除し強制終了させる。
		// これは終了時の醜い例外メッセージを抑止する
		delete Application;

#ifndef _DEBUG
//		::ExitProcess(TVPTerminateCode); // ここで終了させるとメモリリーク表示が行われない
#endif
	} catch (...) {
		return 2;
	}
	return TVPTerminateCode;
}
#endif
tTVPApplication::tTVPApplication() : is_attach_console_(false), tarminate_(false), application_activating_(true)
	 , image_load_thread_(NULL), has_map_report_process_(false), console_cache_(1024)
{
	should_sync_savedata_ = false;
}
tTVPApplication::~tTVPApplication() {
#if 0
	while( windows_list_.size() ) {
		std::vector<class TTVPWindowForm*>::iterator i = windows_list_.begin();
		delete (*i);
		// TTVPWindowForm のデストラクタ内でリストから削除されるはず
	}
	windows_list_.clear();
#endif
#ifdef KRKRSDL2_ENABLE_ASYNC_IMAGE_LOAD
	delete image_load_thread_;
#endif
}
#if 0
struct SEHException {
	unsigned int Code;
	_EXCEPTION_POINTERS* ExceptionPointers;
	SEHException( unsigned int code, _EXCEPTION_POINTERS* ep )
		: Code(code), ExceptionPointers(ep)
	{}
};

int TVPWriteHWEDumpFile( EXCEPTION_POINTERS* pExceptionPointers ) {
	BOOL bMiniDumpSuccessful;
	tjs_char szPath[MAX_PATH]; 
	tjs_char szFileName[MAX_PATH]; 
	const tjs_char* szAppName = TVPKirikiri;
	const tjs_char* szVersion = TVPGetVersionString().c_str();

	TVPEnsureDataPathDirectory();
	TJS_strcpy(szPath, TVPNativeDataPath.c_str());

	SYSTEMTIME stLocalTime;
	::GetLocalTime( &stLocalTime );
	StringCchPrintf( szFileName, MAX_PATH, TJS_W("%s%s%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp"),
				szPath, szAppName, szVersion,
				stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
				stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
				GetCurrentProcessId(), GetCurrentThreadId());
	HANDLE hDumpFile = ::CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	MINIDUMP_EXCEPTION_INFORMATION ExpParam;
	ExpParam.ThreadId = ::GetCurrentThreadId();
	ExpParam.ExceptionPointers = pExceptionPointers;
	ExpParam.ClientPointers = TRUE;
	bMiniDumpSuccessful = MiniDumpWriteDump( ::GetCurrentProcess(), ::GetCurrentProcessId(), hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);
	return EXCEPTION_EXECUTE_HANDLER;
}
static bool TVPIsHandledHWException = false;
void se_translator_function(unsigned int code, struct _EXCEPTION_POINTERS* ep) {
	if( !TVPIsHandledHWException ) {
		//ShowStackTrace( ep->ContextRecord );
		TVPWriteHWEDumpFile( ep );
#ifdef TJS_64BIT_OS
		TVPHandleSEHException( code, ep->ExceptionRecord, ep->ContextRecord->Rsp, ep->ContextRecord );
#else
		TVPHandleSEHException( code, ep->ExceptionRecord, ep->ContextRecord->Esp, ep->ContextRecord );
#endif
		TVPIsHandledHWException = true;
	}
	throw SEHException(code,ep);
}
const tjs_char* SECodeToMessage( unsigned int code ) {
	switch(code){
	case EXCEPTION_ACCESS_VIOLATION: return TVPExceptionAccessViolation;
	case EXCEPTION_BREAKPOINT: return TVPExceptionBreakpoint;
	case EXCEPTION_DATATYPE_MISALIGNMENT: return TVPExceptionDatatypeMisalignment;
	case EXCEPTION_SINGLE_STEP: return TVPExceptionSingleStep;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return TVPExceptionArrayBoundsExceeded;
	case EXCEPTION_FLT_DENORMAL_OPERAND: return TVPExceptionFltDenormalOperand;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO: return TVPExceptionFltDivideByZero;
	case EXCEPTION_FLT_INEXACT_RESULT: return TVPExceptionFltInexactResult;
	case EXCEPTION_FLT_INVALID_OPERATION: return TVPExceptionFltInvalidOperation;
	case EXCEPTION_FLT_OVERFLOW: return TVPExceptionFltOverflow;
	case EXCEPTION_FLT_STACK_CHECK: return TVPExceptionFltStackCheck;
	case EXCEPTION_FLT_UNDERFLOW: return TVPExceptionFltUnderflow;
	case EXCEPTION_INT_DIVIDE_BY_ZERO: return TVPExceptionIntDivideByZero;
	case EXCEPTION_INT_OVERFLOW: return TVPExceptionIntOverflow;
	case EXCEPTION_PRIV_INSTRUCTION: return TVPExceptionPrivInstruction;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return TVPExceptionNoncontinuableException;
	case EXCEPTION_GUARD_PAGE: return TVPExceptionGuardPage;
	case EXCEPTION_ILLEGAL_INSTRUCTION: return TVPExceptionIllegalInstruction;
	case EXCEPTION_IN_PAGE_ERROR: return TVPExceptionInPageError;
	case EXCEPTION_INVALID_DISPOSITION: return TVPExceptionInvalidDisposition;
	case EXCEPTION_INVALID_HANDLE: return TVPExceptionInvalidHandle;
	case EXCEPTION_STACK_OVERFLOW: return TVPExceptionStackOverflow;
	case STATUS_UNWIND_CONSOLIDATE: return TVPExceptionUnwindCconsolidate;
	}
	return TJS_W("Unknown");
}

#endif
bool tTVPApplication::StartApplication( int argc, tjs_char* argv[] ) {
	// _set_se_translator(se_translator_function);

	ArgC = argc;
	ArgV = argv;
#if 0
	for( int i = 0; i < argc; i++ ) {
		if(!TJS_strcmp(argv[i], TJS_W("-@processohmlog"))) {
			has_map_report_process_ = true;
		}
	}
#endif
	TVPTerminateCode = 0;

#if 0
	CheckConsole();
#endif

	// try starting the program!
	bool engine_init = false;
	try {
#if 0
		if(TVPCheckProcessLog()) return true; // sub-process for processing object hash map log
#endif

		TVPInitScriptEngine();
		engine_init = true;

		// banner
		TVPAddImportantLog( TVPFormatMessage(TVPProgramStartedOn, TVPGetOSName(), TVPGetPlatformName()) );

		// TVPInitializeBaseSystems
		TVPInitializeBaseSystems();

#if 0
		Initialize();
#endif

		if(TVPCheckPrintDataPath()) return true;
#if 0
		if(TVPExecuteUserConfig()) return true;
#endif
		
#ifdef KRKRSDL2_ENABLE_ASYNC_IMAGE_LOAD
		image_load_thread_ = new tTVPAsyncImageLoader();
#endif

		TVPSystemInit();

#if 0
		if(TVPCheckAbout()) return true; // version information dialog box;
#endif

		SetTitle( tjs_string(TVPKirikiri) );
		TVPSystemControl = new tTVPSystemControl();
#if 0
#ifndef TVP_IGNORE_LOAD_TPM_PLUGIN
		TVPLoadPluigins(); // load plugin module *.tpm
#endif
		// Check digitizer
		CheckDigitizer();
#endif

#ifdef KRKRSDL2_ENABLE_ASYNC_IMAGE_LOAD
		// start image load thread
		image_load_thread_->StartTread();
#endif

		if(TVPProjectDirSelected) TVPInitializeStartupScript();

#if 0
		Run();
#endif

#if 0
		try {
			// image_load_thread_->ExitRequest();
			delete image_load_thread_;
			image_load_thread_ = NULL;
		} catch(...) {
			// ignore errors
		}
		try {
			TVPSystemUninit();
		} catch(...) {
			// ignore errors
		}
#endif
		return false;
	} catch( const EAbort & ) {
		// nothing to do
	} catch( const Exception &exception ) {
		TVPOnError();
		if(!TVPSystemUninitCalled)
			ShowException(exception.what());
	} catch( const TJS::eTJSScriptError &e ) {
		TVPOnError();
		if(!TVPSystemUninitCalled)
			ShowException( e.GetMessage().c_str() );
	} catch( const TJS::eTJS &e) {
		TVPOnError();
		if(!TVPSystemUninitCalled)
			ShowException( e.GetMessage().c_str() );
	} catch( const std::exception &e ) {
		ShowException( ttstr(e.what()).c_str() );
	} catch( const char* e ) {
		ShowException( ttstr(e).c_str() );
	} catch( const tjs_char* e ) {
		ShowException( e );
#if 0
	} catch( const SEHException& e ) {
		PEXCEPTION_RECORD rec = e.ExceptionPointers->ExceptionRecord;
		tjs_string text(SECodeToMessage(e.Code));
		ttstr result = TJSGetStackTraceString( 10 );
		PrintConsole( result.c_str(), result.length(), true );

		TVPDumpHWException();
		ShowException( text.c_str() );
#endif
	} catch(...) {
		ShowException( (const tjs_char*)TVPUnknownError );
	}

#if 0
	if(engine_init) TVPUninitScriptEngine();

	if(TVPSystemControl) delete TVPSystemControl;
	TVPSystemControl = NULL;

	CloseConsole();
#endif

	return true;
}
#if 0
/**
 * コンソールからの起動か確認し、コンソールからの起動の場合は、標準出力を割り当てる
 */
void tTVPApplication::CheckConsole() {
#ifdef TVP_LOG_TO_COMMANDLINE_CONSOLE
	if( has_map_report_process_ ) return; // 書き出し用子プロセスして起動されていた時はコンソール接続しない
	HANDLE hin  = ::GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hout = ::GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE herr = ::GetStdHandle(STD_ERROR_HANDLE);

	DWORD curProcId = ::GetCurrentProcessId();
	DWORD processList[256];
	DWORD count = ::GetConsoleProcessList( processList, 256 );
	bool thisProcHasConsole = false;
	for( DWORD i = 0; i < count; i++ ) {
		if( processList[i] == curProcId ) {
			thisProcHasConsole = true;
			break;
		}
	}
	bool attachedConsole = true;
	if( thisProcHasConsole == false ) {
		attachedConsole = ::AttachConsole(ATTACH_PARENT_PROCESS) != 0;
	}

	if( (hin==0||hout==0||herr==0) && attachedConsole ) {
		tjs_char console[256];
		::GetConsoleTitle( console, 256 );
		console_title_ = tjs_string( console );
		// 元のハンドルを再割り当て
		if (hin)  ::SetStdHandle(STD_INPUT_HANDLE, hin);
		if (hout) ::SetStdHandle(STD_OUTPUT_HANDLE, hout);
		if (herr) ::SetStdHandle(STD_ERROR_HANDLE, herr);
	}
	is_attach_console_ = attachedConsole;
#endif
}

void tTVPApplication::CloseConsole() {
	tjs_char buf[100];
	DWORD len = TJS_snprintf(buf, 100, TVPExitCode, TVPTerminateCode);
	PrintConsole(buf, len);
	if( is_attach_console_ ) {
		::SetConsoleTitle( console_title_.c_str() );
		::FreeConsole();
		is_attach_console_ = false;
	}
}
#endif

void tTVPApplication::PrintConsole( const tjs_char* mes, unsigned long len, bool iserror ) {
#if 0
	HANDLE hStdOutput = ::GetStdHandle(iserror ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
	if (hStdOutput > 0) {
		DWORD mode;
		if (GetConsoleMode(hStdOutput, &mode)) {
			// 実コンソール
			DWORD wlen;
			::WriteConsoleW( hStdOutput, mes, len, &wlen, NULL );
			::WriteConsoleW( hStdOutput, TJS_W("\n"), 1, &wlen, NULL );
		} else {
			// その他のハンドル, UTF-8で出力
			if( console_cache_.size() < (len*3+1) ) {
				console_cache_.resize(len*3+1);
			}
			tjs_int u8len = TVPWideCharToUtf8String( mes, &(console_cache_[0]) );
			DWORD wlen;
			::WriteFile( hStdOutput, &(console_cache_[0]), u8len, &wlen, NULL );
			::WriteFile( hStdOutput, "\n", 1, &wlen, NULL );
		}
	}
#endif
	if( console_cache_.size() < (len*3+1) ) {
		console_cache_.resize(len*3+1);
	}
	tjs_int u8len = TVPWideCharToUtf8String( mes, &(console_cache_[0]) );
	console_cache_[u8len] = '\0';
#ifdef __ANDROID__
	if( iserror ) {
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", &(console_cache_[0]) );
	} else {
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "%s", &(console_cache_[0]) );
	}
#else
	if( iserror ) {
		fprintf(stdout, "%s\n", &(console_cache_[0]) );
	} else {
		fprintf(stdout, "%s\n", &(console_cache_[0]) );
	}
#endif
#if 0
#ifdef _DEBUG
	::OutputDebugString( mes );
	::OutputDebugString( TJS_W("\n") );
#endif
#endif
}
#if 0
HWND tTVPApplication::GetHandle() {
	if( windows_list_.size() > 0 ) {
		return windows_list_[0]->GetHandle();
	} else {
		return INVALID_HANDLE_VALUE;
	}
}
void tTVPApplication::Minimize() {
	size_t size = windows_list_.size();
	for( size_t i = 0; i < size; i++ ) {
		if( windows_list_[i]->GetVisible() ) {
			::ShowWindow( windows_list_[i]->GetHandle(), SW_MINIMIZE );
		}
	}
}
void tTVPApplication::Restore() {
	size_t size = windows_list_.size();
	for( size_t i = 0; i < size; i++ ) {
		if( windows_list_[i]->GetVisible() ) {
			::ShowWindow( windows_list_[i]->GetHandle(), SW_RESTORE );
		}
	}
}

void tTVPApplication::BringToFront() {
	size_t size = windows_list_.size();
	for( size_t i = 0; i < size; i++ ) {
		windows_list_[i]->BringToFront();
	}
}
#endif
void tTVPApplication::ShowException( const tjs_char* e ) {
#if 0
	::MessageBox( NULL, e, TVPFatalError, MB_OK );
#endif
	TVPAddLog(ttstr(TVPFatalError) + TJS_W(": ") + e);
	tjs_string e_utf16 = e;
	std::string e_utf8;
	tjs_string v_utf16 = (const tjs_char *)TVPFatalError;
	std::string v_utf8;
	if (TVPUtf16ToUtf8(v_utf8, v_utf16) && TVPUtf16ToUtf8(e_utf8, e_utf16))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, v_utf8.c_str(), e_utf8.c_str(), nullptr);
	}
}
void tTVPApplication::Run() {
#if 0
	TVPTerminateCode = 0;

	// メイン メッセージ ループ:
	while( tarminate_ == false ) {
		HandleMessage();
	}
	tarminate_ = true;
#endif
	sdl_process_events();
	if (tarminate_) {
		return;
	}
	bool done = false;
	tTVPTimerThread::Trigger();
	if (TVPSystemControl)
	{
		done = TVPSystemControl->ApplicationIdle();
	}
	tjs_int count = TVPGetWindowCount();
	for( tjs_int i = 0; i<count; i++ ) {
		tTJSNI_Window *win = TVPGetWindowListAt(i);
		win->TickBeat();
	}
	if (should_sync_savedata_)
	{
		should_sync_savedata_ = false;
#ifdef __EMSCRIPTEN__
		emscripten_run_script("FS.syncfs(false, function (err) {});");
#endif
	}
#if 0
#ifndef __EMSCRIPTEN__
	if (done)
	{
		if (SDL_WasInit(SDL_INIT_EVENTS) != 0)
		{
			SDL_WaitEvent(NULL);
		}
	}
#endif
#endif
}

#if 0
bool tTVPApplication::ProcessMessage( MSG &msg ) {
	bool result = false;
	if( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE) ) {
		BOOL msgExists = ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE);
		if( msgExists == 0 ) {
			return result;
		}
		result = true;
		if( msg.message != WM_QUIT ) {
			HACCEL hAccelTable = accel_key_.GetHandle(msg.hwnd);
			if( !TranslateAccelerator(msg.hwnd, hAccelTable, &msg) ) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {
			TVPTerminateCode = (int)msg.wParam;
			tarminate_ = true;
		}
	}
	return result;
}
void tTVPApplication::ProcessMessages() {
	MSG msg = {0};
	while(ProcessMessage(msg));
}
void tTVPApplication::HandleMessage() {
	MSG msg = {0};
	if( !ProcessMessage(msg) ) {
		HandleIdle(msg);
	}
}
void tTVPApplication::HandleIdle(MSG &) {
	bool done = true;
	if( TVPSystemControl ) {
		done = TVPSystemControl->ApplicationIdle();
	}
	if( done ) ::WaitMessage();
}
#endif
void tTVPApplication::SetTitle( const tjs_string& caption ) {
	title_ = caption;
#if 0
	if( windows_list_.size() > 0 ) {
		windows_list_[0]->SetCaption( caption );
	}
	if( is_attach_console_ ) {
		::SetConsoleTitle( caption.c_str() );
	}
#endif
}

#if 0
HWND tTVPApplication::GetMainWindowHandle() const {
	if( windows_list_.size() > 0 ) {
		return windows_list_[0]->GetHandle();
	}
	return INVALID_HANDLE_VALUE;
}

void tTVPApplication::RemoveWindow( TTVPWindowForm* win ) {
	std::vector<class TTVPWindowForm*>::iterator it = std::remove( windows_list_.begin(), windows_list_.end(), win );
	if( it != windows_list_.end() ) {
		windows_list_.erase( it, windows_list_.end() );
	}
}

void tTVPApplication::PostMessageToMainWindow(UINT message, WPARAM wParam, LPARAM lParam) {
	if( windows_list_.size() > 0 ) {
		::PostMessage( windows_list_[0]->GetHandle(), message, wParam, lParam );
	}
}
void tTVPApplication::GetDisableWindowList( std::vector<class TTVPWindowForm*>& win ) {
	size_t count = windows_list_.size();
	for( size_t i = 0; i < count; i++ ) {
		if( windows_list_[i]->GetEnable() == false ) {
			win.push_back( windows_list_[i] );
		}
	}
}
void tTVPApplication::GetEnableWindowList( std::vector<class TTVPWindowForm*>& win, class TTVPWindowForm* activeWindow ) {
	size_t count = windows_list_.size();
	for( size_t i = 0; i < count; i++ ) {
		if( activeWindow != windows_list_[i] && windows_list_[i]->GetEnable() ) {
			win.push_back( windows_list_[i] );
		}
	}
}

void tTVPApplication::DisableWindows() {
	size_t count = windows_list_.size();
	for( size_t i = 0; i < count; i++ ) {
		windows_list_[i]->SetEnable( false );
	}
}
void tTVPApplication::EnableWindows( const std::vector<TTVPWindowForm*>& win ) {
	size_t count = win.size();
	for( size_t i = 0; i < count; i++ ) {
		win[i]->SetEnable( true );
	}
	/*
	size_t count = windows_list_.size();
	for( size_t i = 0; i < count; i++ ) {
		TTVPWindowForm* win = windows_list_[i];
		std::vector<TTVPWindowForm*>::const_iterator f = std::find( ignores.begin(), ignores.end(), win );
		if( f == ignores.end() ) {
			windows_list_[i]->SetEnable( true );
		}
	}
	*/
}
void tTVPApplication::FreeDirectInputDeviceForWindows() {
	size_t count = windows_list_.size();
	for( size_t i = 0; i < count; i++ ) {
		windows_list_[i]->FreeDirectInputDevice();
	}
}


void tTVPApplication::RegisterAcceleratorKey(HWND hWnd, char virt, short key, short cmd) {
	accel_key_.AddKey( hWnd, cmd, key, virt );
}
void tTVPApplication::UnregisterAcceleratorKey(HWND hWnd, short cmd) {
	accel_key_.DelKey( hWnd, cmd );
}
void tTVPApplication::DeleteAcceleratorKeyTable( HWND hWnd ) {
	accel_key_.DelTable( hWnd );
}
void tTVPApplication::CheckDigitizer() {
	// Windows 7 以降でのみ有効
	int value = ::GetSystemMetrics(SM_DIGITIZER);
	if( value == 0 ) return;

	TVPAddLog( (const tjs_char*)TVPEnableDigitizer );
	if( value & NID_INTEGRATED_TOUCH ) {
		TVPAddLog( (const tjs_char*)TVPTouchIntegratedTouch );
	}
	if( value & NID_EXTERNAL_TOUCH ) {
		TVPAddLog( (const tjs_char*)TVPTouchExternalTouch );
	}
	if( value & NID_INTEGRATED_PEN ) {
		TVPAddLog( (const tjs_char*)TVPTouchIntegratedPen );
	}
	if( value & NID_EXTERNAL_PEN ) {
		TVPAddLog( (const tjs_char*)TVPTouchExternalPen );
	}
	if( value & NID_MULTI_INPUT ) {
		TVPAddLog( (const tjs_char*)TVPTouchMultiInput );
	}
	if( value & NID_READY ) {
		TVPAddLog( (const tjs_char*)TVPTouchReady );
	}
}
void tTVPApplication::OnActivate( HWND hWnd )
{
	application_activating_ = true;
	
	TVPRestoreFullScreenWindowAtActivation();
	TVPResetVolumeToAllSoundBuffer();

	// trigger System.onActivate event
	TVPPostApplicationActivateEvent();
}
void tTVPApplication::OnDeactivate( HWND hWnd )
{
	if( hWnd != GetMainWindowHandle() ) return;

	application_activating_ = false;
	
	TVPMinimizeFullScreenWindowAtInactivation();
	
	// fire compact event
	TVPDeliverCompactEvent(TVP_COMPACT_LEVEL_DEACTIVATE);

	// set sound volume
	TVPResetVolumeToAllSoundBuffer();

	// trigger System.onDeactivate event
	TVPPostApplicationDeactivateEvent();
}
bool tTVPApplication::GetNotMinimizing() const
{
	HWND hWnd = GetMainWindowHandle();
	if( hWnd != INVALID_HANDLE_VALUE && hWnd != NULL ) {
		return ::IsIconic( hWnd ) == 0;
	}
	return true; // メインがない時は最小化されているとみなす
}

void tTVPApplication::OnActiveAnyWindow() {
	if( modal_window_stack_.empty() != true ) {
		tTVPWindow* win = modal_window_stack_.top();
		if( win->GetVisible() && win->GetEnable() ) {
			win->BringToFront();
		}
	}
}
void tTVPApplication::ModalFinished() {
	modal_window_stack_.pop();
	if( modal_window_stack_.empty() != true ) {
		tTVPWindow* win = modal_window_stack_.top();
		if( win->GetVisible() && win->GetEnable() ) {
			win->BringToFront();
		}
	}
}
#endif
void tTVPApplication::LoadImageRequest( class iTJSDispatch2 *owner, class tTJSNI_Bitmap* bmp, const ttstr &name ) {
#ifdef KRKRSDL2_ENABLE_ASYNC_IMAGE_LOAD
	if( image_load_thread_ ) {
		image_load_thread_->LoadRequest( owner, bmp, name );
	}
#endif
}

#if 0
std::vector<std::string>* LoadLinesFromFile( const tjs_string& path ) {
	FILE *fp = NULL;
	_wfopen_s( &fp, path.c_str(), TJS_W("r"));
    if( fp == NULL ) {
		return NULL;
    }
	char buff[1024];
	std::vector<std::string>* ret = new std::vector<std::string>();
    while( fgets(buff, 1024, fp) != NULL ) {
		ret->push_back( std::string(buff) );
    }
    fclose(fp);
	return ret;
}

void TVPRegisterAcceleratorKey(HWND hWnd, char virt, short key, short cmd) {
	if( Application ) Application->RegisterAcceleratorKey( hWnd, virt, key, cmd );
}
void TVPUnregisterAcceleratorKey(HWND hWnd, short cmd) {
	if( Application ) Application->UnregisterAcceleratorKey( hWnd, cmd );
}
void TVPDeleteAcceleratorKeyTable( HWND hWnd ) {
	if( Application ) Application->DeleteAcceleratorKeyTable( hWnd );
}
#endif
