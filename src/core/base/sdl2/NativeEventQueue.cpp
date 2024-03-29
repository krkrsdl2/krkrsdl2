/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#include "tjsCommHead.h"
#include "NativeEventQueue.h"
#if 0
#include "WindowsUtil.h"
#endif

#if 0
int NativeEventQueueImplement::CreateUtilWindow() {
	::ZeroMemory( &wc_, sizeof(wc_) );
	wc_.cbSize = sizeof(WNDCLASSEX);
	wc_.lpfnWndProc = ::DefWindowProc;
	wc_.hInstance = ::GetModuleHandle(NULL);
	//wc_.lpszClassName = TJS_W("TVPUtilWindow");
	wc_.lpszClassName = TJS_W("TVPQueueWindow");

	WNDCLASSEX tmpwc = { sizeof(WNDCLASSEX) };
	BOOL ClassRegistered = ::GetClassInfoEx( wc_.hInstance, wc_.lpszClassName, &tmpwc );
	if( ClassRegistered == 0 ) {
		if( ::RegisterClassEx( &wc_ ) == 0 ) {
			TVP_WINDOWS_ERROR_LOG;
			return HRESULT_FROM_WIN32(::GetLastError());
		}
	}
	window_handle_ = ::CreateWindowEx( WS_EX_TOOLWINDOW, wc_.lpszClassName, TJS_W(""),
						WS_POPUP, 0, 0, 0, 0, NULL, NULL, wc_.hInstance, NULL );

	if( window_handle_ == NULL ) {
		TVP_WINDOWS_ERROR_LOG;
		return HRESULT_FROM_WIN32(::GetLastError());
	}
    ::SetWindowLongPtr( window_handle_, GWLP_WNDPROC, (LONG_PTR)NativeEventQueueImplement::WndProc );
	::SetWindowLongPtr( window_handle_, GWLP_USERDATA, (LONG_PTR)this );
	return S_OK;
}

LRESULT WINAPI NativeEventQueueImplement::WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	NativeEvent event;
	event.Result = 0;
	event.HWnd = hWnd;
	event.Message = msg;
	event.WParam = wParam;
	event.LParam = lParam;
	NativeEventQueueIntarface *win = reinterpret_cast<NativeEventQueueIntarface*>(::GetWindowLongPtr(hWnd,GWLP_USERDATA));
	if( win != NULL ) {
		win->Dispatch( event );
		return event.Result;
	} else {
		return ::DefWindowProc(event.HWnd,event.Message,event.WParam,event.LParam);
	}
}

// デフォルトハンドラ
void NativeEventQueueImplement::HandlerDefault( NativeEvent& event ) {
	event.Result = ::DefWindowProc(event.HWnd,event.Message,event.WParam,event.LParam);
}
void NativeEventQueueImplement::Allocate() {
	CreateUtilWindow();
}
void NativeEventQueueImplement::Deallocate() {
	if( window_handle_ != NULL ) {
		::SetWindowLongPtr( window_handle_, GWLP_USERDATA, (LONG_PTR)NULL );
		::DestroyWindow( window_handle_ );
		window_handle_ = NULL;
	}
}
void NativeEventQueueImplement::PostEvent( const NativeEvent& event ) {
	::PostMessage( window_handle_, event.Message, event.WParam, event.LParam );
}
#endif

#include "Application.h"
#include "DebugIntf.h"
#include <SDL.h>

tjs_uint32 NativeEventQueueImplement::native_event_queue_custom_event_type = 0;

NativeEventQueueImplement::NativeEventQueueImplement() {
	if (NativeEventQueueImplement::native_event_queue_custom_event_type == 0)
	{
		if (SDL_WasInit(SDL_INIT_EVENTS) == 0)
		{
			SDL_Init(SDL_INIT_EVENTS);
		}
		NativeEventQueueImplement::native_event_queue_custom_event_type = SDL_RegisterEvents(1);
	}
}

void NativeEventQueueImplement::PostEvent(const NativeEvent& ev) {
	if (NativeEventQueueImplement::native_event_queue_custom_event_type != 0)
	{
		NativeEvent * tmp_ev = new NativeEvent(ev);
		SDL_Event event;
		SDL_memset(&event, 0, sizeof(event));
		event.type = NativeEventQueueImplement::native_event_queue_custom_event_type;
		event.user.code = 0;
		event.user.data2 = (void *)tmp_ev;
		tmp_ev->SetQueue(this);
		int res = 0;
		while (true)
		{
			res = SDL_PushEvent(&event);
			if (res == 1)
			{
				break;
			}
			SDL_Delay(100);
		}
	}
}
