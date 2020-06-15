
#include "tjsCommHead.h"
#include "WindowImpl.h"
#ifndef _WIN32
#include "VirtualKey.h"
#endif
#include "Application.h"
#include "SystemImpl.h"
#include "TVPWindow.h"
#include "SysInitIntf.h"
#include "CharacterSet.h"
#include "WaveImpl.h"
#include "TimerThread.h"
#include "MsgIntf.h"
#include "DebugIntf.h"
#include <SDL.h>

#include <unistd.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


class TVPWindowLayer;
static TVPWindowLayer *_lastWindowLayer, *_currentWindowLayer;

extern void sdlProcessEvents();

#define MK_SHIFT 4
#define MK_CONTROL 8
#define MK_ALT (0x20)

static SDL_Keycode vk_key_to_sdl_key(tjs_uint key)
{
	// This is generated using regex find replace
	if (key == VK_BACK) return SDLK_BACKSPACE;
	if (key == VK_TAB) return SDLK_TAB;
	if (key == VK_CLEAR) return SDLK_CLEAR;
	if (key == VK_RETURN) return SDLK_RETURN;
	if (key == VK_PAUSE) return SDLK_PAUSE;
	if (key == VK_CAPITAL) return SDLK_CAPSLOCK;
	if (key == VK_ESCAPE) return SDLK_ESCAPE;
	if (key == VK_SPACE) return SDLK_SPACE;
	if (key == VK_PRIOR) return SDLK_PAGEUP;
	if (key == VK_NEXT) return SDLK_PAGEDOWN;
	if (key == VK_END) return SDLK_END;
	if (key == VK_HOME) return SDLK_HOME;
	if (key == VK_LEFT) return SDLK_LEFT;
	if (key == VK_UP) return SDLK_UP;
	if (key == VK_RIGHT) return SDLK_RIGHT;
	if (key == VK_DOWN) return SDLK_DOWN;
	if (key == VK_SELECT) return SDLK_SELECT;
	if (key == VK_EXECUTE) return SDLK_EXECUTE;
	if (key == VK_SNAPSHOT) return SDLK_PRINTSCREEN;
	if (key == VK_INSERT) return SDLK_INSERT;
	if (key == VK_DELETE) return SDLK_DELETE;
	if (key == VK_HELP) return SDLK_HELP;
	if (key == VK_0) return SDLK_0;
	if (key == VK_1) return SDLK_1;
	if (key == VK_2) return SDLK_2;
	if (key == VK_3) return SDLK_3;
	if (key == VK_4) return SDLK_4;
	if (key == VK_5) return SDLK_5;
	if (key == VK_6) return SDLK_6;
	if (key == VK_7) return SDLK_7;
	if (key == VK_8) return SDLK_8;
	if (key == VK_9) return SDLK_9;
	if (key == VK_A) return SDLK_a;
	if (key == VK_B) return SDLK_b;
	if (key == VK_C) return SDLK_c;
	if (key == VK_D) return SDLK_d;
	if (key == VK_E) return SDLK_e;
	if (key == VK_F) return SDLK_f;
	if (key == VK_G) return SDLK_g;
	if (key == VK_H) return SDLK_h;
	if (key == VK_I) return SDLK_i;
	if (key == VK_J) return SDLK_j;
	if (key == VK_K) return SDLK_k;
	if (key == VK_L) return SDLK_l;
	if (key == VK_M) return SDLK_m;
	if (key == VK_N) return SDLK_n;
	if (key == VK_O) return SDLK_o;
	if (key == VK_P) return SDLK_p;
	if (key == VK_Q) return SDLK_q;
	if (key == VK_R) return SDLK_r;
	if (key == VK_S) return SDLK_s;
	if (key == VK_T) return SDLK_t;
	if (key == VK_U) return SDLK_u;
	if (key == VK_V) return SDLK_v;
	if (key == VK_W) return SDLK_w;
	if (key == VK_X) return SDLK_x;
	if (key == VK_Y) return SDLK_y;
	if (key == VK_Z) return SDLK_z;
	if (key == VK_LWIN) return SDLK_LGUI;
	if (key == VK_RWIN) return SDLK_RGUI;
	if (key == VK_SLEEP) return SDLK_SLEEP;
	if (key == VK_NUMPAD0) return SDLK_KP_0;
	if (key == VK_NUMPAD1) return SDLK_KP_1;
	if (key == VK_NUMPAD2) return SDLK_KP_2;
	if (key == VK_NUMPAD3) return SDLK_KP_3;
	if (key == VK_NUMPAD4) return SDLK_KP_4;
	if (key == VK_NUMPAD5) return SDLK_KP_5;
	if (key == VK_NUMPAD6) return SDLK_KP_6;
	if (key == VK_NUMPAD7) return SDLK_KP_7;
	if (key == VK_NUMPAD8) return SDLK_KP_8;
	if (key == VK_NUMPAD9) return SDLK_KP_9;
	if (key == VK_MULTIPLY) return SDLK_KP_MULTIPLY;
	if (key == VK_ADD) return SDLK_KP_PLUS;
	if (key == VK_SUBTRACT) return SDLK_KP_MINUS;
	if (key == VK_DECIMAL) return SDLK_KP_PERIOD;
	if (key == VK_DIVIDE) return SDLK_KP_DIVIDE;
	if (key == VK_F1) return SDLK_F1;
	if (key == VK_F2) return SDLK_F2;
	if (key == VK_F3) return SDLK_F3;
	if (key == VK_F4) return SDLK_F4;
	if (key == VK_F5) return SDLK_F5;
	if (key == VK_F6) return SDLK_F6;
	if (key == VK_F7) return SDLK_F7;
	if (key == VK_F8) return SDLK_F8;
	if (key == VK_F9) return SDLK_F9;
	if (key == VK_F10) return SDLK_F10;
	if (key == VK_F11) return SDLK_F11;
	if (key == VK_F12) return SDLK_F12;
	if (key == VK_F13) return SDLK_F13;
	if (key == VK_F14) return SDLK_F14;
	if (key == VK_F15) return SDLK_F15;
	if (key == VK_F16) return SDLK_F16;
	if (key == VK_F17) return SDLK_F17;
	if (key == VK_F18) return SDLK_F18;
	if (key == VK_F19) return SDLK_F19;
	if (key == VK_F20) return SDLK_F20;
	if (key == VK_F21) return SDLK_F21;
	if (key == VK_F22) return SDLK_F22;
	if (key == VK_F23) return SDLK_F23;
	if (key == VK_F24) return SDLK_F24;
	if (key == VK_NUMLOCK) return SDLK_NUMLOCKCLEAR;
	if (key == VK_SCROLL) return SDLK_SCROLLLOCK;
	if (key == VK_LSHIFT) return SDLK_LSHIFT;
	if (key == VK_RSHIFT) return SDLK_RSHIFT;
	if (key == VK_LCONTROL) return SDLK_LCTRL;
	if (key == VK_RCONTROL) return SDLK_RCTRL;
	if (key == VK_LMENU) return SDLK_MENU;
	if (key == VK_RMENU) return SDLK_MENU;
	if (key == VK_BROWSER_BACK) return SDLK_AC_BACK;
	if (key == VK_BROWSER_FORWARD) return SDLK_AC_FORWARD;
	if (key == VK_BROWSER_REFRESH) return SDLK_AC_REFRESH;
	if (key == VK_BROWSER_STOP) return SDLK_AC_STOP;
	if (key == VK_BROWSER_SEARCH) return SDLK_AC_SEARCH;
	if (key == VK_BROWSER_FAVORITES) return SDLK_AC_BOOKMARKS;
	if (key == VK_BROWSER_HOME) return SDLK_AC_HOME;
	if (key == VK_VOLUME_MUTE) return SDLK_MUTE;
	if (key == VK_VOLUME_DOWN) return SDLK_VOLUMEDOWN;
	if (key == VK_VOLUME_UP) return SDLK_VOLUMEUP;
	if (key == VK_MEDIA_NEXT_TRACK) return SDLK_AUDIONEXT;
	if (key == VK_MEDIA_PREV_TRACK) return SDLK_AUDIOPREV;
	if (key == VK_MEDIA_STOP) return SDLK_AUDIOSTOP;
	if (key == VK_MEDIA_PLAY_PAUSE) return SDLK_AUDIOPLAY;
	if (key == VK_LAUNCH_MAIL) return SDLK_MAIL;
	if (key == VK_LAUNCH_MEDIA_SELECT) return SDLK_MEDIASELECT;
	return 0;
}

static tjs_uint sdl_key_to_vk_key(SDL_Keycode key)
{
	// This is generated using regex find replace
	if (key == SDLK_BACKSPACE) return VK_BACK;
	if (key == SDLK_TAB) return VK_TAB;
	if (key == SDLK_CLEAR) return VK_CLEAR;
	if (key == SDLK_RETURN) return VK_RETURN;
	if (key == SDLK_PAUSE) return VK_PAUSE;
	if (key == SDLK_CAPSLOCK) return VK_CAPITAL;
	if (key == SDLK_ESCAPE) return VK_ESCAPE;
	if (key == SDLK_SPACE) return VK_SPACE;
	if (key == SDLK_PAGEUP) return VK_PRIOR;
	if (key == SDLK_PAGEDOWN) return VK_NEXT;
	if (key == SDLK_END) return VK_END;
	if (key == SDLK_HOME) return VK_HOME;
	if (key == SDLK_LEFT) return VK_LEFT;
	if (key == SDLK_UP) return VK_UP;
	if (key == SDLK_RIGHT) return VK_RIGHT;
	if (key == SDLK_DOWN) return VK_DOWN;
	if (key == SDLK_SELECT) return VK_SELECT;
	if (key == SDLK_EXECUTE) return VK_EXECUTE;
	if (key == SDLK_PRINTSCREEN) return VK_SNAPSHOT;
	if (key == SDLK_INSERT) return VK_INSERT;
	if (key == SDLK_DELETE) return VK_DELETE;
	if (key == SDLK_HELP) return VK_HELP;
	if (key == SDLK_0) return VK_0;
	if (key == SDLK_1) return VK_1;
	if (key == SDLK_2) return VK_2;
	if (key == SDLK_3) return VK_3;
	if (key == SDLK_4) return VK_4;
	if (key == SDLK_5) return VK_5;
	if (key == SDLK_6) return VK_6;
	if (key == SDLK_7) return VK_7;
	if (key == SDLK_8) return VK_8;
	if (key == SDLK_9) return VK_9;
	if (key == SDLK_a) return VK_A;
	if (key == SDLK_b) return VK_B;
	if (key == SDLK_c) return VK_C;
	if (key == SDLK_d) return VK_D;
	if (key == SDLK_e) return VK_E;
	if (key == SDLK_f) return VK_F;
	if (key == SDLK_g) return VK_G;
	if (key == SDLK_h) return VK_H;
	if (key == SDLK_i) return VK_I;
	if (key == SDLK_j) return VK_J;
	if (key == SDLK_k) return VK_K;
	if (key == SDLK_l) return VK_L;
	if (key == SDLK_m) return VK_M;
	if (key == SDLK_n) return VK_N;
	if (key == SDLK_o) return VK_O;
	if (key == SDLK_p) return VK_P;
	if (key == SDLK_q) return VK_Q;
	if (key == SDLK_r) return VK_R;
	if (key == SDLK_s) return VK_S;
	if (key == SDLK_t) return VK_T;
	if (key == SDLK_u) return VK_U;
	if (key == SDLK_v) return VK_V;
	if (key == SDLK_w) return VK_W;
	if (key == SDLK_x) return VK_X;
	if (key == SDLK_y) return VK_Y;
	if (key == SDLK_z) return VK_Z;
	if (key == SDLK_LGUI) return VK_LWIN;
	if (key == SDLK_RGUI) return VK_RWIN;
	if (key == SDLK_SLEEP) return VK_SLEEP;
	if (key == SDLK_KP_0) return VK_NUMPAD0;
	if (key == SDLK_KP_1) return VK_NUMPAD1;
	if (key == SDLK_KP_2) return VK_NUMPAD2;
	if (key == SDLK_KP_3) return VK_NUMPAD3;
	if (key == SDLK_KP_4) return VK_NUMPAD4;
	if (key == SDLK_KP_5) return VK_NUMPAD5;
	if (key == SDLK_KP_6) return VK_NUMPAD6;
	if (key == SDLK_KP_7) return VK_NUMPAD7;
	if (key == SDLK_KP_8) return VK_NUMPAD8;
	if (key == SDLK_KP_9) return VK_NUMPAD9;
	if (key == SDLK_KP_MULTIPLY) return VK_MULTIPLY;
	if (key == SDLK_KP_PLUS) return VK_ADD;
	if (key == SDLK_KP_MINUS) return VK_SUBTRACT;
	if (key == SDLK_KP_PERIOD) return VK_DECIMAL;
	if (key == SDLK_KP_DIVIDE) return VK_DIVIDE;
	if (key == SDLK_F1) return VK_F1;
	if (key == SDLK_F2) return VK_F2;
	if (key == SDLK_F3) return VK_F3;
	if (key == SDLK_F4) return VK_F4;
	if (key == SDLK_F5) return VK_F5;
	if (key == SDLK_F6) return VK_F6;
	if (key == SDLK_F7) return VK_F7;
	if (key == SDLK_F8) return VK_F8;
	if (key == SDLK_F9) return VK_F9;
	if (key == SDLK_F10) return VK_F10;
	if (key == SDLK_F11) return VK_F11;
	if (key == SDLK_F12) return VK_F12;
	if (key == SDLK_F13) return VK_F13;
	if (key == SDLK_F14) return VK_F14;
	if (key == SDLK_F15) return VK_F15;
	if (key == SDLK_F16) return VK_F16;
	if (key == SDLK_F17) return VK_F17;
	if (key == SDLK_F18) return VK_F18;
	if (key == SDLK_F19) return VK_F19;
	if (key == SDLK_F20) return VK_F20;
	if (key == SDLK_F21) return VK_F21;
	if (key == SDLK_F22) return VK_F22;
	if (key == SDLK_F23) return VK_F23;
	if (key == SDLK_F24) return VK_F24;
	if (key == SDLK_NUMLOCKCLEAR) return VK_NUMLOCK;
	if (key == SDLK_SCROLLLOCK) return VK_SCROLL;
	if (key == SDLK_LSHIFT) return VK_LSHIFT;
	if (key == SDLK_RSHIFT) return VK_RSHIFT;
	if (key == SDLK_LCTRL) return VK_LCONTROL;
	if (key == SDLK_RCTRL) return VK_RCONTROL;
	if (key == SDLK_MENU) return VK_LMENU;
	if (key == SDLK_MENU) return VK_RMENU;
	if (key == SDLK_AC_BACK) return VK_BROWSER_BACK;
	if (key == SDLK_AC_FORWARD) return VK_BROWSER_FORWARD;
	if (key == SDLK_AC_REFRESH) return VK_BROWSER_REFRESH;
	if (key == SDLK_AC_STOP) return VK_BROWSER_STOP;
	if (key == SDLK_AC_SEARCH) return VK_BROWSER_SEARCH;
	if (key == SDLK_AC_BOOKMARKS) return VK_BROWSER_FAVORITES;
	if (key == SDLK_AC_HOME) return VK_BROWSER_HOME;
	if (key == SDLK_MUTE) return VK_VOLUME_MUTE;
	if (key == SDLK_VOLUMEDOWN) return VK_VOLUME_DOWN;
	if (key == SDLK_VOLUMEUP) return VK_VOLUME_UP;
	if (key == SDLK_AUDIONEXT) return VK_MEDIA_NEXT_TRACK;
	if (key == SDLK_AUDIOPREV) return VK_MEDIA_PREV_TRACK;
	if (key == SDLK_AUDIOSTOP) return VK_MEDIA_STOP;
	if (key == SDLK_AUDIOPLAY) return VK_MEDIA_PLAY_PAUSE;
	if (key == SDLK_MAIL) return VK_LAUNCH_MAIL;
	if (key == SDLK_MEDIASELECT) return VK_LAUNCH_MEDIA_SELECT;
	return 0;
}

static int GetShiftState() {
	int s = 0;
	if(TVPGetAsyncKeyState(VK_MENU)) s |= MK_ALT;
	if(TVPGetAsyncKeyState(VK_SHIFT)) s |= MK_SHIFT;
	if(TVPGetAsyncKeyState(VK_CONTROL)) s |= MK_CONTROL;
	return s;
}
static int GetMouseButtonState() {
	int s = 0;
	if(TVPGetAsyncKeyState(VK_LBUTTON)) s |= ssLeft;
	if(TVPGetAsyncKeyState(VK_RBUTTON)) s |= ssRight;
	if(TVPGetAsyncKeyState(VK_MBUTTON)) s |= ssMiddle;
	if(TVPGetAsyncKeyState(VK_XBUTTON1)) s |= ssX1;
	if(TVPGetAsyncKeyState(VK_XBUTTON2)) s |= ssX2;
	return s;
}


class TVPWindowLayer : public TTVPWindowForm {
protected:
	SDL_Window *window;

	TVPWindowLayer *_prevWindow, *_nextWindow;
	SDL_Texture* framebuffer;
	SDL_Renderer* renderer;
	SDL_Surface* surface;
	tTJSNI_Window *TJSNativeInstance;
	bool hasDrawn = false;
	bool isBeingDeleted = false;
	char* ime_composition;
	size_t ime_composition_cursor;
	size_t ime_composition_len;
	size_t ime_composition_selection;
	SDL_Rect attention_point_rect;

public:
	TVPWindowLayer(tTJSNI_Window *w)
	{
		ime_composition = nullptr;
		ime_composition_cursor = 0;
		ime_composition_len = 0;
		ime_composition_selection = 0;
		attention_point_rect.x = 0;
		attention_point_rect.y = 0;
		attention_point_rect.w = 0;
		attention_point_rect.h = 0;
		_nextWindow = nullptr;
		_prevWindow = _lastWindowLayer;
		_lastWindowLayer = this;
		if (_prevWindow) {
			_prevWindow->_nextWindow = this;
		}
		if (!_currentWindowLayer) {
			_currentWindowLayer = this;
		}
		if (w) {
			TJSNativeInstance = w;
		}
		else {
			TJSNativeInstance = nullptr;
		}
		
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			TVPThrowExceptionMessage(TJS_W("Cannot initialize SDL video subsystem: %1"), ttstr(SDL_GetError()));
		}
		window = SDL_CreateWindow("krkrsdl2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
		if (window == nullptr)
		{
			TVPThrowExceptionMessage(TJS_W("Cannot create SDL window: %1"), ttstr(SDL_GetError()));
		}
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (renderer == nullptr)
		{
			TVPAddLog(ttstr("Cannot create SDL renderer: ") + ttstr(SDL_GetError()));
			surface = SDL_GetWindowSurface(window);
			if (surface == nullptr)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get surface from SDL window: %1"), ttstr(SDL_GetError()));
			}
		}
		framebuffer = NULL;
		if (renderer)
		{
			SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );
		}
	}

	virtual ~TVPWindowLayer() {
		if (_lastWindowLayer == this) _lastWindowLayer = _prevWindow;
		if (_nextWindow) _nextWindow->_prevWindow = _prevWindow;
		if (_prevWindow) _prevWindow->_nextWindow = _nextWindow;
		if (_currentWindowLayer == this) {
			_currentWindowLayer = _lastWindowLayer;
		}
		if (framebuffer)
		{
			SDL_DestroyTexture(framebuffer);
			framebuffer = NULL;
		}
		if (renderer)
		{
			SDL_DestroyRenderer(renderer);
			renderer = NULL;
		}
		if (window)
		{
			SDL_DestroyWindow(window);
			window = NULL;
		}
	}

	virtual void SetPaintBoxSize(tjs_int w, tjs_int h) override {
		if (renderer)
		{
			if (framebuffer)
			{
				SDL_DestroyTexture(framebuffer);
				framebuffer = NULL;
			}
			framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, w, h);
			if (framebuffer == nullptr)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot create framebuffer texture: %1"), ttstr(SDL_GetError()));
			}
		}
		if( TJSNativeInstance )
		{
			tTVPRect r;
			r.left = 0;
			r.top = 0;
			r.right = w;
			r.bottom = h;
			TJSNativeInstance->NotifyWindowExposureToLayer(r);
			TJSNativeInstance->GetDrawDevice()->SetClipRectangle(r);
			TJSNativeInstance->GetDrawDevice()->SetDestRectangle(r);
		}
	}
	virtual bool GetFormEnabled() override {
		return SDL_GetWindowFlags(window) & SDL_WINDOW_SHOWN;
	}
	virtual void SetDefaultMouseCursor() override {
	}
	virtual void GetCursorPos(tjs_int &x, tjs_int &y) override {
		SDL_GetMouseState(&x, &y);
	}
	virtual void SetCursorPos(tjs_int x, tjs_int y) override {
		SDL_WarpMouseInWindow(window, x, y);
	}
	virtual void SetHintText(const ttstr &text) override {
	}
	virtual void SetAttentionPoint(tjs_int left, tjs_int top, const struct tTVPFont * font) override {
		attention_point_rect.x = left;
		attention_point_rect.y = top;
		attention_point_rect.w = 0;
		attention_point_rect.h = font->Height;
		SDL_SetTextInputRect(&attention_point_rect);
	}
	virtual void ZoomRectangle(
		tjs_int & left, tjs_int & top,
		tjs_int & right, tjs_int & bottom) override {
	}
	virtual void BringToFront() override {
		if (_currentWindowLayer != this) {
			if (_currentWindowLayer) {
				_currentWindowLayer->TJSNativeInstance->OnReleaseCapture();
			}
			_currentWindowLayer = this;
		}
		SDL_RaiseWindow(window);
	}
	virtual void ShowWindowAsModal() override {
		in_mode_ = true;
		BringToFront();
		modal_result_ = 0;
		while (this == _currentWindowLayer && !modal_result_) {
			sdlProcessEvents();
			if (::Application->IsTarminate()) {
				modal_result_ = mrCancel;
			} else if (modal_result_ != 0) {
				break;
			}
		}
		in_mode_ = false;
	}
	virtual bool GetVisible() override {
		return SDL_GetWindowFlags(window) & SDL_WINDOW_SHOWN;
	}
	virtual void SetVisible(bool bVisible) override {
		if (bVisible) {
			SDL_ShowWindow(window);
			BringToFront();
		}
		else {
			SDL_HideWindow(window);
			if (_currentWindowLayer == this) {
				_currentWindowLayer = _prevWindow ? _prevWindow : _nextWindow;
			}
		}
	}
	virtual tjs_string GetCaption() override {
		std::string v_utf8 = SDL_GetWindowTitle(window);
		tjs_string v_utf16;
		TVPUtf8ToUtf16( v_utf16, v_utf8 );
		return v_utf16;
	}
	virtual void GetCaption(tjs_string & v) const override {
		v.clear();
		std::string v_utf8 = SDL_GetWindowTitle(window);
		TVPUtf8ToUtf16( v, v_utf8 );
	}
	virtual void SetCaption(const tjs_string & v) override {
		std::string v_utf8;
		if (TVPUtf16ToUtf8(v_utf8, v))
		{
			SDL_SetWindowTitle(window, v_utf8.c_str());
		}
	}
	virtual void SetWidth(tjs_int w) override {
		int h;
		SDL_GetWindowSize(window, NULL, &h);
		SDL_SetWindowSize(window, w, h);
		if (surface)
		{
			surface = SDL_GetWindowSurface(window);
			if (surface == nullptr)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get surface from SDL window: %1"), ttstr(SDL_GetError()));
			}
		}
	}
	virtual void SetHeight(tjs_int h) override {
		int w;
		SDL_GetWindowSize(window, &w, NULL);
		SDL_SetWindowSize(window, w, h);
		if (surface)
		{
			surface = SDL_GetWindowSurface(window);
			if (surface == nullptr)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get surface from SDL window: %1"), ttstr(SDL_GetError()));
			}
		}
	}
	virtual void SetSize(tjs_int w, tjs_int h) override {
		SDL_SetWindowSize(window, w, h);
		if (surface)
		{
			surface = SDL_GetWindowSurface(window);
			if (surface == nullptr)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get surface from SDL window: %1"), ttstr(SDL_GetError()));
			}
		}
	}
	virtual void GetSize(tjs_int &w, tjs_int &h) override {
		SDL_GetWindowSize(window, &w, &h);
	}
	virtual tjs_int GetWidth() const override {
		int w;
		SDL_GetWindowSize(window, &w, NULL);
		return w;
	}
	virtual tjs_int GetHeight() const override {
		int h;
		SDL_GetWindowSize(window, NULL, &h);
		return h;
	}
	virtual void GetWinSize(tjs_int &w, tjs_int &h) override {
		SDL_GetWindowSize(window, &w, &h);
	}
	virtual void SetZoom(tjs_int numer, tjs_int denom) override {
	}
	virtual void NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const class BitmapInfomation * bmpinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity) override {
		const TVPBITMAPINFO *bitmapinfo = bmpinfo->GetBITMAPINFO();
		tjs_int w = 0;
		tjs_int h = 0;
		if(!manager) return;
		if(!manager->GetPrimaryLayerSize(w, h))
		{
			w = 0;
			h = 0;
		}
		if(
			!(x < 0 || y < 0 ||
				x + cliprect.get_width() > w ||
				y + cliprect.get_height() > h) &&
			!(cliprect.left < 0 || cliprect.top < 0 ||
				cliprect.right > bitmapinfo->bmiHeader.biWidth ||
				cliprect.bottom > bitmapinfo->bmiHeader.biHeight))
		{
			// bitmapinfo で表された cliprect の領域を x,y にコピーする
			long src_y       = cliprect.top;
			long src_y_limit = cliprect.bottom;
			long src_x       = cliprect.left;
			long width_bytes   = cliprect.get_width() * 4; // 32bit
			long dest_y      = 0;
			long dest_x      = 0;
			const tjs_uint8 * src_p = (const tjs_uint8 *)bits;
			long src_pitch;

			if(bitmapinfo->bmiHeader.biHeight < 0)
			{
				// bottom-down
				src_pitch = bitmapinfo->bmiHeader.biWidth * 4;
				//src_pitch = -bitmapinfo->bmiHeader.biWidth * 4;
				//src_p += bitmapinfo->bmiHeader.biWidth * 4 * (bitmapinfo->bmiHeader.biHeight - 1);
			}
			else
			{
				// bottom-up
				src_pitch = -bitmapinfo->bmiHeader.biWidth * 4;
				src_p += bitmapinfo->bmiHeader.biWidth * 4 * (bitmapinfo->bmiHeader.biHeight - 1);
				//src_pitch = bitmapinfo->bmiHeader.biWidth * 4;
			}

			void* TextureBuffer;
			int TexturePitch;
			SDL_Rect dstrect;
			dstrect.x = x;
			dstrect.y = y;
			dstrect.w = cliprect.get_width();
			dstrect.h = cliprect.get_height();

			if (framebuffer)
			{
				SDL_LockTexture(framebuffer, &dstrect, &TextureBuffer, &TexturePitch);
				for(; src_y < src_y_limit; src_y ++, dest_y ++)
				{
					const void *srcp = src_p + src_pitch * src_y + src_x * 4;
					void *destp = (tjs_uint8*)TextureBuffer + TexturePitch * dest_y + dest_x * 4;
					memcpy(destp, srcp, width_bytes);
				}
				SDL_UnlockTexture(framebuffer);
			}
			else if (surface)
			{
				dstrect.h = 1;
				SDL_Surface* clip_surface = SDL_CreateRGBSurfaceFrom((void *)src_p, cliprect.get_width(), 1, 32, cliprect.get_width() * 4, 0x000000ff, 0x0000ff00, 0x00ff0000, 0);
				if (clip_surface == nullptr)
				{
					TVPAddLog(ttstr("Cannot create clip surface: ") + ttstr(SDL_GetError()));
					return;
				}
				for(; src_y < src_y_limit; src_y ++, dest_y ++)
				{
					const void *srcp = src_p + src_pitch * src_y + src_x * 4;
					SDL_LockSurface(clip_surface);
					clip_surface->pixels = (void *)srcp;
					SDL_UnlockSurface(clip_surface);
					int blit_result = SDL_BlitSurface(clip_surface, nullptr, surface, &dstrect);
					if (blit_result < 0)
					{
						TVPAddLog(ttstr("Cannot blit onto window surface: ") + ttstr(SDL_GetError()));
					}
					dstrect.y += 1;
				}
				SDL_FreeSurface(clip_surface);
			}

		}
	}
	virtual void Show() override {
		if (renderer)
		{
			SDL_RenderFillRect(renderer, NULL);
			if (framebuffer)
			{
				SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
			}
			SDL_RenderPresent(renderer);
			hasDrawn = true;
		}
		else if (window && surface)
		{
			SDL_UpdateWindowSurface(window);
			hasDrawn = true;
		}
	}
	virtual void InvalidateClose() override {
		isBeingDeleted = true;
	}
	virtual bool GetWindowActive() override {
		return _currentWindowLayer == this;
	}
	bool Closing = false, ProgramClosing = false, CanCloseWork = false;
	bool in_mode_ = false; // is modal
	int modal_result_ = 0;
	enum CloseAction {
		caNone,
		caHide,
		caFree,
		caMinimize
	};
	void OnClose(CloseAction& action) {
		if (modal_result_ == 0)
			action = caNone;
		else
			action = caHide;

		if (ProgramClosing) {
			if (TJSNativeInstance) {
				if (TJSNativeInstance->IsMainWindow()) {
					// this is the main window
				} else 			{
					// not the main window
					action = caFree;
				}
				iTJSDispatch2 * obj = TJSNativeInstance->GetOwnerNoAddRef();
				TJSNativeInstance->NotifyWindowClose();
				obj->Invalidate(0, NULL, NULL, obj);
				TJSNativeInstance = NULL;
				SetVisible(false);
				isBeingDeleted = true;
			}
		}
	}
	bool OnCloseQuery() {
		// closing actions are 3 patterns;
		// 1. closing action by the user
		// 2. "close" method
		// 3. object invalidation

		if (TVPGetBreathing()) {
			return false;
		}

		// the default event handler will invalidate this object when an onCloseQuery
		// event reaches the handler.
		if (TJSNativeInstance && (modal_result_ == 0 ||
			modal_result_ == mrCancel/* mrCancel=when close button is pushed in modal window */)) {
			iTJSDispatch2 * obj = TJSNativeInstance->GetOwnerNoAddRef();
			if (obj) {
				tTJSVariant arg[1] = { true };
				static ttstr eventname(TJS_W("onCloseQuery"));

				if (!ProgramClosing) {
					// close action does not happen immediately
					if (TJSNativeInstance) {
						TVPPostInputEvent(new tTVPOnCloseInputEvent(TJSNativeInstance));
					}

					Closing = true; // waiting closing...
				//	TVPSystemControl->NotifyCloseClicked();
					return false;
				} else {
					CanCloseWork = true;
					TVPPostEvent(obj, obj, eventname, 0, TVP_EPT_IMMEDIATE, 1, arg);
					sdlProcessEvents(); // for post event
					// this event happens immediately
					// and does not return until done
					return CanCloseWork; // CanCloseWork is set by the event handler
				}
			} else {
				return true;
			}
		} else {
			return true;
		}
	}
	virtual void Close() override {
		// closing action by "close" method
		if (Closing) return; // already waiting closing...

		ProgramClosing = true;
		try {
			//tTVPWindow::Close();
			if (in_mode_) {
				modal_result_ = mrCancel;
			} 
			else if (OnCloseQuery()) {
				CloseAction action = caFree;
				OnClose(action);
				switch (action) {
				case caNone:
					break;
				case caHide:
					SetVisible(false);
					break;
				case caMinimize:
					//::ShowWindow(GetHandle(), SW_MINIMIZE);
					break;
				case caFree:
				default:
					isBeingDeleted = true;
					//::PostMessage(GetHandle(), TVP_EV_WINDOW_RELEASE, 0, 0);
					break;
				}
			}
		}
		catch (...) {
			ProgramClosing = false;
			throw;
		}
		ProgramClosing = false;
	}
	virtual void OnCloseQueryCalled(bool b) override {
		// closing is allowed by onCloseQuery event handler
		if (!ProgramClosing) {
			// closing action by the user
			if (b) {
				if (in_mode_)
					modal_result_ = 1; // when modal
				else
					SetVisible(false);  // just hide

				Closing = false;
				if (TJSNativeInstance) {
					if (TJSNativeInstance->IsMainWindow()) {
						// this is the main window
						iTJSDispatch2 * obj = TJSNativeInstance->GetOwnerNoAddRef();
						obj->Invalidate(0, NULL, NULL, obj);
					}
				} else {
					delete this;
				}
			} else {
				Closing = false;
			}
		} else {
			// closing action by the program
			CanCloseWork = b;
		}
	}
	virtual void InternalKeyDown(tjs_uint16 key, tjs_uint32 shift) override {
	}
	virtual void OnKeyUp(tjs_uint16 vk, int shift) override {
	}
	virtual void OnKeyPress(tjs_uint16 vk, int repeat, bool prevkeystate, bool convertkey) override {
	}
	virtual tTVPImeMode GetDefaultImeMode() const override {
		return ::imDisable;
	}
	virtual void SetImeMode(tTVPImeMode mode) override {
		if (mode == ::imDisable || mode == ::imClose)
		{
			ResetImeMode();
		}
		else
		{
			if (!SDL_IsTextInputActive())
			{
				SDL_SetTextInputRect(&attention_point_rect);
				SDL_StartTextInput();
			}
		}
	}
	virtual void ResetImeMode() override {
		if (SDL_IsTextInputActive())
		{
			ime_composition = nullptr;
			ime_composition_len = 0;
			ime_composition_cursor = 0;
			ime_composition_selection = 0;
			attention_point_rect.x = 0;
			attention_point_rect.y = 0;
			attention_point_rect.w = 0;
			attention_point_rect.h = 0;
			SDL_SetTextInputRect(&attention_point_rect);
			SDL_StopTextInput();
		}
	}
	virtual void UpdateWindow(tTVPUpdateType type) override {
		if (TJSNativeInstance) {
			tTVPRect r;
			r.left = 0;
			r.top = 0;
			SDL_GetWindowSize(window, &r.right, &r.bottom);
			TJSNativeInstance->NotifyWindowExposureToLayer(r);
			TVPDeliverWindowUpdateEvents();
		}
	}
	virtual void SetVisibleFromScript(bool b) override {
		SetVisible(b);
	}
	virtual void SetUseMouseKey(bool b) override {
	}
	virtual bool GetUseMouseKey() const override {
		return false;
	}
	virtual void ResetMouseVelocity() override {
	}
	virtual void ResetTouchVelocity(tjs_int id) override {
	}
	virtual bool GetMouseVelocity(float& x, float& y, float& speed) const override {
		return false;
	}
	virtual void TickBeat() override {
	}
	void sdlRecvEvent(SDL_Event event) {
		if (isBeingDeleted) {
			delete this;
			return;
		}
		if (window && _prevWindow) {
			uint32_t windowID = SDL_GetWindowID(window);
			bool tryParentWindow = false;
			switch (event.type) {
				case SDL_DROPFILE:
				case SDL_DROPTEXT:
				case SDL_DROPBEGIN:
				case SDL_DROPCOMPLETE:
					tryParentWindow = event.drop.windowID != windowID;
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					tryParentWindow = event.key.windowID != windowID;
					break;
				case SDL_MOUSEMOTION:
					tryParentWindow = event.motion.windowID != windowID;
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					tryParentWindow = event.button.windowID != windowID;
					break;
				case SDL_MOUSEWHEEL:
					tryParentWindow = event.wheel.windowID != windowID;
					break;
				case SDL_TEXTEDITING:
					tryParentWindow = event.edit.windowID != windowID;
					break;
				case SDL_TEXTINPUT:
					tryParentWindow = event.text.windowID != windowID;
					break;
			}
			if (tryParentWindow) {
				if (!in_mode_) {
					_prevWindow->sdlRecvEvent(event);
				}
				return;
			}
		}
		if (window && hasDrawn) {
			tjs_uint32 s = TVP_TShiftState_To_uint32(GetShiftState());
			s |= GetMouseButtonState();
			if (TJSNativeInstance->CanDeliverEvents()) {
				switch (event.type) { 
					case SDL_MOUSEMOTION: {
						TVPPostInputEvent(new tTVPOnMouseMoveInputEvent(TJSNativeInstance, event.motion.x, event.motion.y, s));
						break;
					}
					case SDL_MOUSEBUTTONDOWN:
					case SDL_MOUSEBUTTONUP: {
						if (SDL_IsTextInputActive() && ime_composition != nullptr)
						{
							return;
						}
						tTVPMouseButton btn;
						bool hasbtn = true;
						switch(event.button.button) {
							case SDL_BUTTON_RIGHT:
								btn = tTVPMouseButton::mbRight;
								break;
							case SDL_BUTTON_MIDDLE:
								btn = tTVPMouseButton::mbMiddle;
								break;
							case SDL_BUTTON_LEFT:
								btn = tTVPMouseButton::mbLeft;
								break;
							case SDL_BUTTON_X1:
								btn = tTVPMouseButton::mbX1;
								break;
							case SDL_BUTTON_X2:
								btn = tTVPMouseButton::mbX2;
								break;
							default:
								hasbtn = false;
								break;
						}
						if (hasbtn) {
							switch (event.type) {
								case SDL_MOUSEBUTTONDOWN:
									TVPPostInputEvent(new tTVPOnMouseDownInputEvent(TJSNativeInstance, event.button.x, event.button.y, btn, s));
									break;
								case SDL_MOUSEBUTTONUP:
									TVPPostInputEvent(new tTVPOnClickInputEvent(TJSNativeInstance, event.button.x, event.button.y));
									TVPPostInputEvent(new tTVPOnMouseUpInputEvent(TJSNativeInstance, event.button.x, event.button.y, btn, s));
									break;
							}
						}
						break;
					}
					case SDL_KEYDOWN: {
						if (SDL_IsTextInputActive())
						{
							if (ime_composition != nullptr)
							{
								return;
							}
						}
						if (event.key.repeat) s |= TVP_SS_REPEAT;
						TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, sdl_key_to_vk_key(event.key.keysym.sym), s));
						SDL_SetTextInputRect(&attention_point_rect);
						break;
					}
					case SDL_KEYUP: {
						if (SDL_IsTextInputActive())
						{
							if (ime_composition != nullptr)
							{
								return;
							}
						}
						if (!SDL_IsTextInputActive())
						{
							TVPPostInputEvent(new tTVPOnKeyPressInputEvent(TJSNativeInstance, sdl_key_to_vk_key(event.key.keysym.sym)));
						}
						TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, sdl_key_to_vk_key(event.key.keysym.sym), s));
						SDL_SetTextInputRect(&attention_point_rect);
						break;
					}
					case SDL_TEXTINPUT:
					case SDL_TEXTEDITING: {
						if (!SDL_IsTextInputActive())
						{
							return;
						}
						// TODO: figure out vertical edit
						for (size_t i = 0; i < ime_composition_selection; i += 1)
						{
							TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, VK_LEFT, TVP_SS_SHIFT));
							TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, VK_LEFT, TVP_SS_SHIFT));
#if 0
							TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, VK_DOWN, TVP_SS_SHIFT));
							TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, VK_DOWN, TVP_SS_SHIFT));
#endif
						}
						for (size_t i = 0; i < ime_composition_len - ime_composition_cursor; i += 1)
						{
							TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, VK_RIGHT, 0));
							TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, VK_RIGHT, 0));
#if 0
							TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, VK_DOWN, 0));
							TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, VK_DOWN, 0));
#endif
						}
						for (size_t i = 0; i < ime_composition_len; i += 1)
						{
							TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, VK_BACK, 0));
							TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, VK_BACK, 0));
						}
						switch (event.type) {
							case SDL_TEXTINPUT:
								ime_composition = event.text.text;
								ime_composition_cursor = 0;
								ime_composition_selection = 0;
								break;
							case SDL_TEXTEDITING:
								ime_composition = event.edit.text;
								ime_composition_cursor = event.edit.start;
								ime_composition_selection = event.edit.length;
								break;
						}
						size_t buffer_len = TVPUtf8ToWideCharString((const char*)(ime_composition), NULL);
						if (buffer_len == (size_t)-1)
						{
							return;
						}
						if (buffer_len != 0)
						{
							ime_composition_len = buffer_len;
							tjs_char *buffer = new tjs_char[buffer_len + 1];
							TVPUtf8ToWideCharString((const char*)(ime_composition), buffer);
							for (size_t i = 0; i < buffer_len; i += 1)
							{
								TVPPostInputEvent(new tTVPOnKeyPressInputEvent(TJSNativeInstance, buffer[i]));
							}
							delete[] buffer;
						}
						else
						{
							ime_composition = nullptr;
							ime_composition_len = 0;
							ime_composition_cursor = 0;
							ime_composition_selection = 0;
						}
						if (event.type == SDL_TEXTEDITING)
						{
							for (size_t i = 0; i < ime_composition_len - ime_composition_cursor; i += 1)
							{
								TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, VK_LEFT, 0));
								TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, VK_LEFT, 0));
#if 0
								TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, VK_UP, 0));
								TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, VK_UP, 0));
#endif
							}
							for (size_t i = 0; i < ime_composition_selection; i += 1)
							{
								TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, VK_RIGHT, TVP_SS_SHIFT));
								TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, VK_RIGHT, TVP_SS_SHIFT));
#if 0
								TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, VK_DOWN, TVP_SS_SHIFT));
								TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, VK_DOWN, TVP_SS_SHIFT));
#endif
							}
						}
						if (event.type == SDL_TEXTINPUT)
						{
							ime_composition = nullptr;
							ime_composition_len = 0;
						}
						break;
					}
					case SDL_MOUSEWHEEL: {
						int x, y;
						SDL_GetMouseState(&x, &y);
						TVPPostInputEvent(new tTVPOnMouseWheelInputEvent(TJSNativeInstance, event.wheel.x, event.wheel.y, x, y));
						break;
					}
					case SDL_WINDOWEVENT_CLOSE:
					case SDL_QUIT: {
						TVPPostInputEvent(new tTVPOnCloseInputEvent(TJSNativeInstance));
					}
				}
			}
		}
	}
};

void sdlProcessEvents()
{
	if (SDL_WasInit(SDL_INIT_EVENTS) != 0)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (_currentWindowLayer) {
				_currentWindowLayer->sdlRecvEvent(event);
			}
		}
	}
#ifdef __EMSCRIPTEN__
	tTJSNI_WaveSoundBuffer::Trigger();
	tTVPTimerThread::Trigger();
#endif
	::Application->Run();
}

int main(int argc, char **argv) {
	_argc = argc;
	_wargv = new tjs_char*[argc];

	for (int i = 0; i < argc; i += 1) {
		const tjs_char* warg;
		if (!i)
			warg = ttstr(realpath(argv[i], NULL)).c_str();
		else
			warg = ttstr(argv[i]).c_str();

		tjs_char* warg_copy = new tjs_char[strlen(argv[i]) + 1];
		memcpy(warg_copy, warg, sizeof(tjs_char) * (strlen(argv[i]) + 1));
		_wargv[i] = warg_copy;
	}
#ifdef __EMSCRIPTEN__
	chdir("/data");
#endif

	::Application = new tTVPApplication();
	::Application->StartApplication( _argc, _wargv );

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(sdlProcessEvents, 0, 0);
#else
	while (1) {
		sdlProcessEvents();
	}
#endif
	return 0;
}

bool TVPGetKeyMouseAsyncState(tjs_uint keycode, bool getcurrent)
{
	if(keycode >= VK_LBUTTON && keycode <= VK_XBUTTON2 && keycode != VK_CANCEL)
	{
		Uint32 state = SDL_GetMouseState(NULL, NULL);
		switch (keycode) {
			case VK_LBUTTON:
				return !!(state & SDL_BUTTON(SDL_BUTTON_LEFT));
			case VK_RBUTTON:
				return !!(state & SDL_BUTTON(SDL_BUTTON_RIGHT));
			case VK_MBUTTON:
				return !!(state & SDL_BUTTON(SDL_BUTTON_MIDDLE));
			case VK_XBUTTON1:
				return !!(state & SDL_BUTTON(SDL_BUTTON_X1));
			case VK_XBUTTON2:
				return !!(state & SDL_BUTTON(SDL_BUTTON_X2));
			default:
				return false;
		}
	}
	if(keycode >= VK_SHIFT && keycode <= VK_MENU)
	{
		Uint32 state = SDL_GetModState();
		switch (keycode) {
			case VK_SHIFT:
				return !!(state & KMOD_SHIFT);
			case VK_MENU:
				return !!(state & KMOD_ALT);
			case VK_CONTROL:
				return !!(state & KMOD_CTRL);
			default:
				return false;
		}
	}
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	return !!(state[SDL_GetScancodeFromKey(vk_key_to_sdl_key(keycode))]);
}

bool TVPGetJoyPadAsyncState(tjs_uint keycode, bool getcurrent)
{
	return false;
}

TTVPWindowForm *TVPCreateAndAddWindow(tTJSNI_Window *w) {
	return new TVPWindowLayer(w);
}

tjs_uint32 TVPGetCurrentShiftKeyState()
{
	tjs_uint32 f = 0;

	if(TVPGetAsyncKeyState(VK_SHIFT)) f |= TVP_SS_SHIFT;
	if(TVPGetAsyncKeyState(VK_MENU)) f |= TVP_SS_ALT;
	if(TVPGetAsyncKeyState(VK_CONTROL)) f |= TVP_SS_CTRL;
	if(TVPGetAsyncKeyState(VK_LBUTTON)) f |= TVP_SS_LEFT;
	if(TVPGetAsyncKeyState(VK_RBUTTON)) f |= TVP_SS_RIGHT;
	if(TVPGetAsyncKeyState(VK_MBUTTON)) f |= TVP_SS_MIDDLE;

	return f;
}

ttstr TVPGetPlatformName()
{
	return SDL_GetPlatform();
}

ttstr TVPGetOSName()
{
	return TVPGetPlatformName();
}

tjs_uint32 TVP_TShiftState_To_uint32(TShiftState state) {
	tjs_uint32 result = 0;
	if (state & MK_SHIFT) {
		result |= ssShift;
	}
	if (state & MK_CONTROL) {
		result |= ssCtrl;
	}
	if (state & MK_ALT) {
		result |= ssAlt;
	}
	return result;
}
TShiftState TVP_TShiftState_From_uint32(tjs_uint32 state){
	TShiftState result = 0;
	if (state & ssShift) {
		result |= MK_SHIFT;
	}
	if (state & ssCtrl) {
		result |= MK_CONTROL;
	}
	if (state & ssAlt) {
		result |= MK_ALT;
	}
	return result;
}

#include <dirent.h>
extern void TVPInitializeFont();

static ttstr TVPDefaultFaceNames;
extern void TVPAddSystemFontToFreeType( const std::string& storage, std::vector<tjs_string>* faces );
extern void TVPGetSystemFontListFromFreeType( std::vector<tjs_string>& faces );
static bool TVPIsGetAllFontList = false;
static ttstr TVPDefaultFontNameX;
void TVPGetAllFontList( std::vector<tjs_string>& list ) {
	TVPInitializeFont();
	if( TVPIsGetAllFontList ) {
		TVPGetSystemFontListFromFreeType( list );
	}

	DIR* dr;
	if( ( dr = opendir("/System/Library/Fonts/") ) != nullptr ) {
		struct dirent* entry;
		while( ( entry = readdir( dr ) ) != nullptr ) {
			if( entry->d_type == DT_REG ) {
				std::string path(entry->d_name);
				std::string::size_type extp = path.find_last_of(".");
				if( extp != std::string::npos ) {
					std::string ext = path.substr(extp);
					if( ext == std::string(".ttf") || ext == std::string(".ttc") || ext == std::string(".otf") ) {
						// .ttf | .ttc | .otf
						std::string fullpath( std::string("/System/Library/Fonts/") + path );
						TVPAddSystemFontToFreeType( fullpath, &list );
					}
				}
			}
		}
		closedir( dr );
		TVPIsGetAllFontList = true;
	}
#if 0
	for( std::list<std::string>::const_iterator i = fontfiles.begin(); i != fontfiles.end(); ++i ) {
		FT_Face face = nullptr;
		std::string fullpath( std::string("/system/fonts/") + *i );
		FT_Open_Args args;
		memset(&args, 0, sizeof(args));
		args.flags = FT_OPEN_PATHNAME;
		args.pathname = fullpath.c_str();
		tjs_uint face_num = 1;
		std::list<std::string> facenames;
		for( tjs_uint f = 0; f < face_num; f++ ) {
			FT_Error err = FT_Open_Face( FreeTypeLibrary, &args, 0, &face);
			if( err == 0 ) {
				facenames.push_back( std::string(face->family_name) );
				std::string(face->style_name);	// スタイル名
				if( face->face_flags & FT_FACE_FLAG_SCALABLE ) {
					// 可変サイズフォントのみ採用
					if( face->num_glyphs > 2965 ) {
						// JIS第一水準漢字以上のグリフ数
						if( face->style_flags & FT_STYLE_FLAG_ITALIC ) {}
						if( face->style_flags & FT_STYLE_FLAG_BOLD ) {}
						face_num = face->num_faces;
						int numcharmap = face->num_charmaps;
						for( int c = 0; c < numcharmap; c++ ) {
							FT_Encoding enc = face->charmaps[c]->encoding;
							if( enc == FT_ENCODING_SJIS ) {
								// mybe japanese
							}
							if( enc == FT_ENCODING_UNICODE ) {
							}
						}
					}
				}
			}
			if(face) FT_Done_Face(face), face = nullptr;
		}
	}
#endif
}
static bool IsInitDefalutFontName = false;
static bool SelectFont( const std::vector<tjs_string>& faces, tjs_string& face ) {
	std::vector<tjs_string> fonts;
	TVPGetAllFontList( fonts );
	for( auto i = faces.begin(); i != faces.end(); ++i ) {
		auto found = std::find( fonts.begin(), fonts.end(), *i );
		if( found != fonts.end() ) {
			face = *i;
			return true;
		}
	}
	return false;
}
const tjs_char *TVPGetDefaultFontName() {
	if( IsInitDefalutFontName ) {
		return TVPDefaultFontNameX.c_str();
	}
	TVPDefaultFontNameX = TJS_W("Noto Sans CJK JP");
	IsInitDefalutFontName =  true;

	// コマンドラインで指定がある場合、そのフォントを使用する
	tTJSVariant opt;
	if(TVPGetCommandLine(TJS_W("-deffont"), &opt)) {
		ttstr str(opt);
		TVPDefaultFontNameX = str;
	} else {
		tjs_string face;
		std::vector<tjs_string> facenames{tjs_string(TJS_W("Noto Sans CJK JP"))};
		if( SelectFont( facenames, face ) ) {
			TVPDefaultFontNameX = face;
		}
	}
	return TVPDefaultFontNameX.c_str();
}
void TVPSetDefaultFontName( const tjs_char * name ) {
	TVPDefaultFontNameX = name;
}
const ttstr &TVPGetDefaultFaceNames() {
#if 0
	if( !TVPDefaultFaceNames.IsEmpty() ) {
		return TVPDefaultFaceNames;
	} else {
		TVPDefaultFaceNames = ttstr( TVPGetDefaultFontName() );
		return TVPDefaultFaceNames;
	}
#endif
	return ttstr(TJS_W("Noto Sans,MotoyaLMaru,Roboto"));
}


