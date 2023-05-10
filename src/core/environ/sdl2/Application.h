/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#ifndef __T_APPLICATION_H__
#define __T_APPLICATION_H__

#include "tjsVariant.h"
#include "tjsString.h"
#include "CharacterSet.h"
#include <vector>
#include <functional>
#include <tuple>
#include <map>
#include <stack>
#include <SDL.h>

tjs_string ExePath();

// 見通しのよい方法に変更した方が良い
extern int _argc;
extern tjs_char ** _wargv;

enum {
	mrOk,
	mrAbort,
	mrCancel,
};

#if 0
enum {
  mtWarning = MB_ICONWARNING,
  mtError = MB_ICONERROR,
  mtInformation = MB_ICONINFORMATION,
  mtConfirmation = MB_ICONQUESTION,
  mtStop = MB_ICONSTOP,
  mtCustom = 0
};
enum {
	mbOK = MB_OK,
};
#endif

enum {
  mtWarning = SDL_MESSAGEBOX_WARNING,
  mtError = SDL_MESSAGEBOX_ERROR,
  mtInformation = SDL_MESSAGEBOX_INFORMATION,
  mtConfirmation = SDL_MESSAGEBOX_INFORMATION,
  mtStop = SDL_MESSAGEBOX_ERROR,
  mtCustom = 0
};
enum {
	mbOK = 0,
};

enum class eTVPActiveEvent {
	onActive,
	onDeactive,
};
#if 0
class AcceleratorKey {
	HACCEL hAccel_;
	ACCEL* keys_;
	int key_count_;

public:
	AcceleratorKey();
	~AcceleratorKey();
	void AddKey( WORD id, WORD key, BYTE virt );
	void DelKey( WORD id );
	HACCEL GetHandle() { return hAccel_; }
};
class AcceleratorKeyTable {
	std::map<HWND,AcceleratorKey*> keys_;
	HACCEL hAccel_;

public:
	AcceleratorKeyTable();
	~AcceleratorKeyTable();
	void AddKey( HWND hWnd, WORD id, WORD key, BYTE virt );
	void DelKey( HWND hWnd, WORD id );
	void DelTable( HWND hWnd );
	HACCEL GetHandle(HWND hWnd) {
		std::map<HWND,AcceleratorKey*>::iterator i = keys_.find(hWnd);
		if( i != keys_.end() ) {
			return i->second->GetHandle();
		}
		return hAccel_;
	}
};
#endif
class tTVPApplication {
#if 0
	std::vector<class TTVPWindowForm*> windows_list_;
#endif
	tjs_string title_;

	bool is_attach_console_;
	tjs_string console_title_;
#if 0
	AcceleratorKeyTable accel_key_;
#endif
	bool tarminate_;
	bool application_activating_;
	bool has_map_report_process_;

	bool should_sync_savedata_;
	bool syncfs_is_finished_;

#ifdef KRKRSDL2_ENABLE_ASYNC_IMAGE_LOAD
	class tTVPAsyncImageLoader* image_load_thread_;
#else
	void * image_load_thread_;
#endif

#if 0
	std::stack<class tTVPWindow*> modal_window_stack_;
#endif
	std::vector<char> console_cache_;

private:
	void CheckConsole();
#if 0
	void CloseConsole();
	void CheckDigitizer();
#endif
	void ShowException( const tjs_char* e );
#if 0
	void Initialize() {}
#endif

public:
	tTVPApplication();
	~tTVPApplication();
	bool StartApplication( int argc, tjs_char* argv[] );
	void Run();

	void PrintConsole( const tjs_char* mes, unsigned long len, bool iserror = false );
	bool IsAttachConsole() { return is_attach_console_; }

	bool IsTarminate() const { return tarminate_; }

#if 0
	HWND GetHandle();
	bool IsIconic() {
		HWND hWnd = GetHandle();
		if( hWnd != INVALID_HANDLE_VALUE ) {
			return 0 != ::IsIconic(hWnd);
		}
		return true; // そもそもウィンドウがない
	}
	void Minimize();
	void Restore();
	void BringToFront();

	void AddWindow( class TTVPWindowForm* win ) {
		windows_list_.push_back( win );
	}
	void RemoveWindow( class TTVPWindowForm* win );
	unsigned int GetWindowCount() const {
		return (unsigned int)windows_list_.size();
	}

	void FreeDirectInputDeviceForWindows();

	bool ProcessMessage( MSG &msg );
	void ProcessMessages();
	void HandleMessage();
	void HandleIdle(MSG &msg);
#endif

	tjs_string GetTitle() const { return title_; }
	void SetTitle( const tjs_string& caption );

	static inline int MessageDlg( const tjs_string& string, const tjs_string& caption, int type, int button ) {
#if 0
		return ::MessageBox( nullptr, string.c_str(), caption.c_str(), type|button  );
#endif
		tjs_string s_utf16 = string;
		std::string s_utf8;
		tjs_string c_utf16 = caption;
		std::string c_utf8;
		if (TVPUtf16ToUtf8(s_utf8, s_utf16) && TVPUtf16ToUtf8(c_utf8, c_utf16))
		{
			return SDL_ShowSimpleMessageBox(type, c_utf8.c_str(), s_utf8.c_str(), nullptr);
		}
		return 0;
	}
	void Terminate() {
#if 0
		::PostQuitMessage(0);
#endif
		tarminate_ = true;
	}
#if 0
	void SetHintHidePause( int v ) {}
	void SetShowHint( bool b ) {}
	void SetShowMainForm( bool b ) {}


	HWND GetMainWindowHandle() const;
#endif

	int ArgC;
	tjs_char ** ArgV;

#if 0
	void PostMessageToMainWindow(UINT message, WPARAM wParam, LPARAM lParam);

	void ModalStarted( class tTVPWindow* form ) {
		modal_window_stack_.push(form);
	}
	void ModalFinished();
	void OnActiveAnyWindow();
	void DisableWindows();
	void EnableWindows( const std::vector<class TTVPWindowForm*>& win );
	void GetDisableWindowList( std::vector<class TTVPWindowForm*>& win );
	void GetEnableWindowList( std::vector<class TTVPWindowForm*>& win, class TTVPWindowForm* activeWindow );


	void RegisterAcceleratorKey(HWND hWnd, char virt, short key, short cmd);
	void UnregisterAcceleratorKey(HWND hWnd, short cmd);
	void DeleteAcceleratorKeyTable( HWND hWnd );

	void OnActivate( HWND hWnd );
	void OnDeactivate( HWND hWnd );
#endif
	bool GetActivating() const { return application_activating_; }
#if 0
	bool GetNotMinimizing() const;
#endif

	/**
	 * 画像の非同期読込み要求
	 */
	void LoadImageRequest( class iTJSDispatch2 *owner, class tTJSNI_Bitmap* bmp, const ttstr &name );

	void SyncSavedata()
	{
		should_sync_savedata_ = true;
	}

	void FinishedSyncSavedata()
	{
		syncfs_is_finished_ = true;
	}
};
#if 0
std::vector<std::string>* LoadLinesFromFile( const tjs_string& path );

inline HINSTANCE GetHInstance() { return ((HINSTANCE)GetModuleHandle(0)); }
#endif
extern class tTVPApplication* Application;


#endif // __T_APPLICATION_H__
