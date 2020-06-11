
#ifndef __T_APPLICATION_H__
#define __T_APPLICATION_H__

#include "tjsVariant.h"
#include "tjsString.h"
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

#define MB_ICONWARNING 0x00000030L
#define MB_ICONERROR 0x00000010L
#define MB_ICONINFORMATION 0x00000040L
#define MB_ICONQUESTION 0x00000020L
#define MB_ICONSTOP 0x00000010L
#define MB_OK 0x00000000L

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

	class tTVPAsyncImageLoader* image_load_thread_;

#if 0
	std::stack<class tTVPWindow*> modal_window_stack_;
#endif
	std::vector<char> console_cache_;

private:
	void CheckConsole();
	void CloseConsole();
	void CheckDigitizer();
	void ShowException( const tjs_char* e );
	void Initialize() {}

public:
	tTVPApplication();
	~tTVPApplication();
	bool StartApplication( int argc, tjs_char* argv[] );
	void Run();
	void ProcessMessages();

	void PrintConsole( const tjs_char* mes, unsigned long len, bool iserror = false );
	bool IsAttachConsole() { return is_attach_console_; }

	bool IsTarminate() const { return tarminate_; }

#if 0
	HWND GetHandle();
#endif
	bool IsIconic() {
#if 0
		HWND hWnd = GetHandle();
		if( hWnd != INVALID_HANDLE_VALUE ) {
			return 0 != ::IsIconic(hWnd);
		}
#endif
		return true; // そもそもウィンドウがない
	}
#if 0
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

#if 0
	static inline int MessageDlg( const tjs_string& string, const tjs_string& caption, int type, int button ) {
		return ::MessageBox( nullptr, string.c_str(), caption.c_str(), type|button  );
	}
#endif
	void Terminate() {
#if 0
		::PostQuitMessage(0);
#endif
		tarminate_ = true;
	}
	void SetHintHidePause( int v ) {}
	void SetShowHint( bool b ) {}
	void SetShowMainForm( bool b ) {}


#if 0
	HWND GetMainWindowHandle() const;
#endif

	int ArgC;
	tjs_char ** ArgV;
	typedef std::function<void()> tMsg;

#if 0
	void PostMessageToMainWindow(UINT message, WPARAM wParam, LPARAM lParam);
#endif

	void PostUserMessage(const std::function<void()> &func, void* param1 = nullptr, int param2 = 0);
	void FilterUserMessage(const std::function<void(std::vector<std::tuple<void*, int, tMsg> > &)> &func);

#if 0
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
#endif
	void OnActivate(  );
	void OnDeactivate(  );

#if 0
	void OnActivate( HWND hWnd );
	void OnDeactivate( HWND hWnd );
#endif
	bool GetActivating() const { return application_activating_; }
	bool GetNotMinimizing() const;

	/**
	 * 画像の非同期読込み要求
	 */
	void LoadImageRequest( class iTJSDispatch2 *owner, class tTJSNI_Bitmap* bmp, const ttstr &name );
	void RegisterActiveEvent(void *host, const std::function<void(void*, eTVPActiveEvent)>& func/*empty = unregister*/);

private:
	SDL_mutex *m_msgQueueLock;

	std::vector<std::tuple<void*, int, tMsg> > m_lstUserMsg;
	std::map<void*, std::function<void(void*, eTVPActiveEvent)> > m_activeEvents;
};
#if 0
std::vector<std::string>* LoadLinesFromFile( const tjs_string& path );
#endif

#if 0
inline HINSTANCE GetHInstance() { return ((HINSTANCE)GetModuleHandle(0)); }
#endif
extern class tTVPApplication* Application;


#endif // __T_APPLICATION_H__
