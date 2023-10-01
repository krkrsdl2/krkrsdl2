/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#include "tjsCommHead.h"
#include "WindowImpl.h"
#include "VirtualKey.h"
#include "Application.h"
#include "SystemImpl.h"
#include "TVPWindow.h"
#include "SysInitIntf.h"
#include "SysInitImpl.h"
#include "CharacterSet.h"
#include "WaveImpl.h"
#include "TimerThread.h"
#include "MsgIntf.h"
#include "DebugIntf.h"
#include "tjsArray.h"
#include "StorageIntf.h"
#include "SDLBitmapCompletion.h"
#include "ScriptMgnIntf.h"
#include "SystemControl.h"
#include "PluginImpl.h"
#ifdef KRKRZ_ENABLE_CANVAS
#include "OpenGLScreenSDL2.h"
#endif
#ifdef _WIN32
#include <SDL_syswm.h>
#endif
#include <SDL.h>
#ifdef USE_SDL_MAIN
#include <SDL_main.h>
#endif
#ifdef _WIN32
#include <shellapi.h>
#include <stdlib.h>
#endif

#include <unistd.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#ifdef __SWITCH__
#include <switch.h>
#endif

#ifdef __EMSCRIPTEN__
EM_JS_DEPS(main, "$FS,$IDBFS");
#endif

#if defined(__IPHONEOS__) || defined(__ANDROID__) || defined(__EMSCRIPTEN__) || defined(__vita__) || defined(__SWITCH__)
#define KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
#endif

extern void TVPLoadMessage();

class TVPWindowWindow;
static TVPWindowWindow *_lastWindowWindow, *_currentWindowWindow;
static SDL_GameController** sdl_controllers = NULL;
static int sdl_controller_num = 0;

#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
static void process_events();
#else
static bool process_events();
#endif

#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
static int sdl_event_watch(void *userdata, SDL_Event *in_event);
#endif

static void refresh_controllers()
{
#if defined(__IPHONEOS__) || defined(__ANDROID__)
	// For some reason, invalid pointers get set
	{
		return;
	}
#endif
	if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) == 0)
	{
		SDL_Init(SDL_INIT_GAMECONTROLLER);
	}
	if (sdl_controller_num != 0 && sdl_controllers != NULL)
	{
		for (int i = 0; i < sdl_controller_num; i += 1)
		{
			if (sdl_controllers[i] != NULL)
			{
				SDL_GameControllerClose(sdl_controllers[i]);
				sdl_controllers[i] = NULL;
			}
		}
		sdl_controller_num = 0;
		SDL_free(sdl_controllers);
		sdl_controllers = NULL;
	}
	sdl_controller_num = SDL_NumJoysticks();
	if (sdl_controller_num != 0)
	{
		sdl_controllers = (SDL_GameController**)SDL_malloc(sizeof(SDL_GameController*) * sdl_controller_num);
		if (!sdl_controllers)
		{
			sdl_controller_num = 0;
			TVPAddLog(ttstr("Could not allocate SDL controller memory"));
			return;
		}
		for (int i = 0; i < sdl_controller_num; i += 1)
		{
			if (SDL_IsGameController(i))
			{
				sdl_controllers[i] = SDL_GameControllerOpen(i);
				if (!sdl_controllers[i])
				{
					TVPAddLog(ttstr("Could not open controller: ") + ttstr(SDL_GetError()));
				}
			}
		}
	}
}

static Uint8 vk_key_to_sdl_gamecontrollerbutton(tjs_uint key)
{
	if (key == VK_PAD1) return SDL_CONTROLLER_BUTTON_A;
	if (key == VK_PAD2) return SDL_CONTROLLER_BUTTON_B;
	if (key == VK_PAD3) return SDL_CONTROLLER_BUTTON_X;
	if (key == VK_PAD4) return SDL_CONTROLLER_BUTTON_Y;
	if (key == VK_PAD7) return SDL_CONTROLLER_BUTTON_BACK;
	if (key == VK_PAD8) return SDL_CONTROLLER_BUTTON_START;
	if (key == VK_PAD9) return SDL_CONTROLLER_BUTTON_LEFTSTICK;
	if (key == VK_PAD10) return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
	if (key == VK_PAD5) return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	if (key == VK_PAD6) return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	if (key == VK_PADUP) return SDL_CONTROLLER_BUTTON_DPAD_UP;
	if (key == VK_PADDOWN) return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	if (key == VK_PADLEFT) return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	if (key == VK_PADRIGHT) return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
	return 0;
}

static tjs_uint sdl_gamecontrollerbutton_to_vk_key(Uint8 key)
{
	if (key == SDL_CONTROLLER_BUTTON_A) return VK_PAD1;
	if (key == SDL_CONTROLLER_BUTTON_B) return VK_PAD2;
	if (key == SDL_CONTROLLER_BUTTON_X) return VK_PAD3;
	if (key == SDL_CONTROLLER_BUTTON_Y) return VK_PAD4;
	if (key == SDL_CONTROLLER_BUTTON_BACK) return VK_PAD7;
	if (key == SDL_CONTROLLER_BUTTON_START) return VK_PAD8;
	if (key == SDL_CONTROLLER_BUTTON_LEFTSTICK) return VK_PAD9;
	if (key == SDL_CONTROLLER_BUTTON_RIGHTSTICK) return VK_PAD10;
	if (key == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) return VK_PAD5;
	if (key == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) return VK_PAD6;
	if (key == SDL_CONTROLLER_BUTTON_DPAD_UP) return VK_PADUP;
	if (key == SDL_CONTROLLER_BUTTON_DPAD_DOWN) return VK_PADDOWN;
	if (key == SDL_CONTROLLER_BUTTON_DPAD_LEFT) return VK_PADLEFT;
	if (key == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) return VK_PADRIGHT;
	return 0;
}

#ifdef MK_SHIFT
#undef MK_SHIFT
#endif
#ifdef MK_CONTROL
#undef MK_CONTROL
#endif
#ifdef MK_ALT
#undef MK_ALT
#endif
#define MK_SHIFT 4
#define MK_CONTROL 8
#define MK_ALT (0x20)

static SDL_Cursor* sdl_system_cursors[SDL_NUM_SYSTEM_CURSORS] = {0};

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
	if (key == VK_LMENU) return SDLK_LALT;
	if (key == VK_RMENU) return SDLK_RALT;
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
	if (key == SDLK_LALT) return VK_LMENU;
	if (key == SDLK_RALT) return VK_RMENU;
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

static int GetShiftState()
{
	int s = 0;
	if(TVPGetAsyncKeyState(VK_MENU)) s |= MK_ALT;
	if(TVPGetAsyncKeyState(VK_LMENU)) s |= MK_ALT;
	if(TVPGetAsyncKeyState(VK_RMENU)) s |= MK_ALT;
	if(TVPGetAsyncKeyState(VK_SHIFT)) s |= MK_SHIFT;
	if(TVPGetAsyncKeyState(VK_LSHIFT)) s |= MK_SHIFT;
	if(TVPGetAsyncKeyState(VK_RCONTROL)) s |= MK_SHIFT;
	if(TVPGetAsyncKeyState(VK_CONTROL)) s |= MK_CONTROL;
	if(TVPGetAsyncKeyState(VK_LCONTROL)) s |= MK_CONTROL;
	if(TVPGetAsyncKeyState(VK_RCONTROL)) s |= MK_CONTROL;
	return s;
}
static int GetMouseButtonState()
{
	int s = 0;
	if(TVPGetAsyncKeyState(VK_LBUTTON)) s |= ssLeft;
	if(TVPGetAsyncKeyState(VK_RBUTTON)) s |= ssRight;
	if(TVPGetAsyncKeyState(VK_MBUTTON)) s |= ssMiddle;
	if(TVPGetAsyncKeyState(VK_XBUTTON1)) s |= ssX1;
	if(TVPGetAsyncKeyState(VK_XBUTTON2)) s |= ssX2;
	return s;
}

#ifdef _WIN32
struct tTVPMessageReceiverRecord
{
	tTVPWindowMessageReceiver Proc;
	const void *UserData;
	bool Deliver(tTVPWindowMessage *Message)
	{ return Proc(const_cast<void*>(UserData), Message); }
};
#endif

class TVPWindowWindow : public TTVPWindowForm
{
protected:
	SDL_Window *window;

	TVPWindowWindow *_prevWindow, *_nextWindow;
	SDL_Texture* texture;
	SDL_Renderer* renderer;
	SDL_Surface* surface;
#ifdef KRKRZ_ENABLE_CANVAS
	SDL_GLContext context;
#endif
	tTJSNI_Window *TJSNativeInstance;
	bool hasDrawn = false;
	bool isVisible = true;
	bool visibilityHasInitialized = false;
	bool needs_graphic_update = false;
	bool isBeingDeleted = false;
	bool cursor_temporary_hidden = false;
	char* ime_composition;
	size_t ime_composition_cursor;
	size_t ime_composition_len;
	size_t ime_composition_selection;
	SDL_Rect attention_point_rect;
	iTJSDispatch2 * file_drop_array;
	tjs_int file_drop_array_count;
	TVPSDLBitmapCompletion * bitmap_completion;
#ifdef KRKRZ_ENABLE_CANVAS
	tTVPOpenGLScreen * open_gl_screen;
#endif
	int last_mouse_x;
	int last_mouse_y;

#ifdef KRKRSDL2_ENABLE_ZOOM
	tTVPRect FullScreenDestRect;
	tTVPRect LastSentDrawDeviceDestRect;

	//-- layer position / size
	tjs_int LayerLeft = 0;
	tjs_int LayerTop = 0;
	tjs_int LayerWidth = 32;
	tjs_int LayerHeight = 32;
	tjs_int ZoomDenom = 1; // Zooming factor denominator (setting)
	tjs_int ZoomNumer = 1; // Zooming factor numerator (setting)
	tjs_int ActualZoomDenom = 1; // Zooming factor denominator (actual)
	tjs_int ActualZoomNumer = 1; // Zooming factor numerator (actual)
	tjs_int InnerWidth = 32;
	tjs_int InnerHeight = 32;
#endif
#ifdef _WIN32
	tObjectList<tTVPMessageReceiverRecord> WindowMessageReceivers;
#endif

public:
	TVPWindowWindow(tTJSNI_Window *w);
	virtual ~TVPWindowWindow();
	/* Called from tTJSNI_Window */
	virtual void SetPaintBoxSize(tjs_int w, tjs_int h) override;
	void TranslateWindowToDrawArea(int &x, int &y);
	void TranslateDrawAreaToWindow(int &x, int &y);
	/* Called from tTJSNI_Window */
	virtual bool GetFormEnabled() override;
	/* Called from tTJSNI_Window */
	virtual void SetDefaultMouseCursor() override;
	/* Called from tTJSNI_Window */
	virtual void SetMouseCursor(tjs_int handle) override;
	/* Called from tTJSNI_Window */
	virtual void SetMouseCursorState(tTVPMouseCursorState mcs) override;
	/* Called from tTJSNI_Window */
	virtual tTVPMouseCursorState GetMouseCursorState() const override;
	void RestoreMouseCursor();
	/* Called from tTJSNI_Window */
	virtual void HideMouseCursor() override;
	/* Called from tTJSNI_Window */
	virtual void GetCursorPos(tjs_int &x, tjs_int &y) override;
	/* Called from tTJSNI_Window */
	virtual void SetCursorPos(tjs_int x, tjs_int y) override;
	/* Called from tTJSNI_Window */
	virtual void SetAttentionPoint(tjs_int left, tjs_int top, const struct tTVPFont * font) override;
	/* Called from tTJSNI_Window */
	virtual void BringToFront() override;
	/* Called from tTJSNI_Window */
	virtual void ShowWindowAsModal() override;
	/* Called from tTJSNI_Window */
	virtual bool GetVisible() override;
	/* Called from member SetVisibleFromScript */
	virtual void SetVisible(bool visible) override;
	/* Called from tTJSNI_Window */
	virtual void SetFullScreenMode(bool fullscreen) override;
	/* Called from tTJSNI_Window */
	virtual bool GetFullScreenMode() override;
	/* Called from tTJSNI_Window */
	virtual void SetBorderStyle(tTVPBorderStyle bs) override;
	/* Called from tTJSNI_Window */
	virtual tTVPBorderStyle GetBorderStyle() const override;
	/* Called from tTJSNI_Window */
	virtual tjs_string GetCaption() override;
	virtual void GetCaption(tjs_string & v) const override;
	/* Called from tTJSNI_Window */
	virtual void SetCaption(const tjs_string & v) override;
	/* Called from tTJSNI_Window */
	virtual void SetWidth(tjs_int w) override;
	/* Called from tTJSNI_Window */
	virtual void SetHeight(tjs_int h) override;
	/* Called from tTJSNI_Window */
	virtual void SetSize(tjs_int w, tjs_int h) override;
	virtual void GetSize(tjs_int &w, tjs_int &h) override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetWidth() const override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetHeight() const override;
	/* Called from tTJSNI_Window */
	virtual void SetMinWidth(tjs_int w) override;
	/* Called from tTJSNI_Window */
	virtual void SetMaxWidth(tjs_int w) override;
	/* Called from tTJSNI_Window */
	virtual void SetMinHeight(tjs_int h) override;
	/* Called from tTJSNI_Window */
	virtual void SetMaxHeight(tjs_int h) override;
	/* Called from tTJSNI_Window */
	virtual void SetMinSize(tjs_int w, tjs_int h) override;
	/* Called from tTJSNI_Window */
	virtual void SetMaxSize(tjs_int w, tjs_int h) override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetMinWidth() override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetMaxWidth() override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetMinHeight() override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetMaxHeight() override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetLeft() override;
	/* Called from tTJSNI_Window */
	virtual void SetLeft(tjs_int l) override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetTop() override;
	/* Called from tTJSNI_Window */
	virtual void SetTop(tjs_int t) override;
	/* Called from tTJSNI_Window */
	virtual void SetPosition(tjs_int l, tjs_int t) override;
	virtual TVPSDLBitmapCompletion *GetTVPSDLBitmapCompletion() override;
#ifdef KRKRZ_ENABLE_CANVAS
	virtual void SetOpenGLScreen(tTVPOpenGLScreen *s) override;
	virtual void SetSwapInterval(int interval) override;
	virtual void GetDrawableSize(tjs_int &w, tjs_int &h) override;
	virtual void Swap() override;
#endif
	virtual void Show() override;
	/* Called from tTJSNI_Window */
	virtual void TickBeat() override;
	/* Called from tTJSNI_Window */
	virtual void InvalidateClose() override;
	/* Called from tTJSNI_Window */
	virtual bool GetWindowActive() override;
	bool Closing = false, ProgramClosing = false, CanCloseWork = false;
	bool in_mode_ = false; // is modal
	int modal_result_ = 0;
	enum CloseAction {
		caNone,
		caHide,
		caFree,
		caMinimize
	};
	void OnClose(CloseAction& action);
	bool OnCloseQuery();
	/* Called from tTJSNI_Window */
	virtual void Close() override;
	/* Called from tTJSNI_Window */
	virtual void OnCloseQueryCalled(bool b) override;
	/* Called from tTJSNI_Window */
	virtual void SetImeMode(tTVPImeMode mode) override;
	/* Called from tTJSNI_Window */
	virtual void ResetImeMode() override;
	/* Called from tTJSNI_Window */
	virtual void UpdateWindow(tTVPUpdateType type) override;
	/* Called from tTJSNI_Window */
	virtual void InternalKeyDown(tjs_uint16 key, tjs_uint32 shift) override;
	/* Called from tTJSNI_Window */
	virtual void OnKeyUp(tjs_uint16 vk, int shift) override;
	/* Called from tTJSNI_Window */
	virtual void OnKeyPress(tjs_uint16 vk, int repeat, bool prevkeystate, bool convertkey) override;
	void UpdateActualZoom(void);
	void SetDrawDeviceDestRect(void);
	/* Called from tTJSNI_Window */
	virtual void SetZoom(tjs_int numer, tjs_int denom, bool set_logical = true) override;
	/* Called from tTJSNI_Window */
	virtual void SetZoomNumer(tjs_int n) override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetZoomNumer() const override;
	/* Called from tTJSNI_Window */
	virtual void SetZoomDenom(tjs_int d) override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetZoomDenom() const override;
	/* Called from tTJSNI_Window */
	virtual void SetInnerWidth(tjs_int v) override;
	/* Called from tTJSNI_Window */
	virtual void SetInnerHeight(tjs_int v) override;
	/* Called from tTJSNI_Window */
	virtual void SetInnerSize(tjs_int w, tjs_int h) override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetInnerWidth() override;
	/* Called from tTJSNI_Window */
	virtual tjs_int GetInnerHeight() override;
#ifdef _WIN32
	virtual void RegisterWindowMessageReceiver(tTVPWMRRegMode mode, void * proc, const void *userdata) override;
	bool InternalDeliverMessageToReceiver(tTVPWindowMessage &msg);
	virtual HWND GetHandle() const override;
#endif
	bool should_try_parent_window(SDL_Event event);
	void window_receive_event(SDL_Event event);
	bool window_receive_event_input(SDL_Event event);
};

TVPWindowWindow::TVPWindowWindow(tTJSNI_Window *w)
{
	ime_composition = nullptr;
	ime_composition_cursor = 0;
	ime_composition_len = 0;
	ime_composition_selection = 0;
	attention_point_rect.x = 0;
	attention_point_rect.y = 0;
	attention_point_rect.w = 0;
	attention_point_rect.h = 0;
	file_drop_array = nullptr;
	file_drop_array_count = 0;
	last_mouse_x = 0;
	last_mouse_y = 0;
	_nextWindow = nullptr;
	_prevWindow = _lastWindowWindow;
	_lastWindowWindow = this;
	if (_prevWindow) {
		_prevWindow->_nextWindow = this;
	}
	if (!_currentWindowWindow) {
		_currentWindowWindow = this;
	}
	if (w) {
		TJSNativeInstance = w;
	}
	else {
		TJSNativeInstance = nullptr;
	}

	if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			TVPThrowExceptionMessage(TJS_W("Cannot initialize SDL video subsystem: %1"), ttstr(SDL_GetError()));
		}
		refresh_controllers();
	}

	int new_window_x = SDL_WINDOWPOS_UNDEFINED;
	int new_window_y = SDL_WINDOWPOS_UNDEFINED;
	int new_window_w = 640;
	int new_window_h = 480;
	Uint32 window_flags = 0;

#ifdef SDL_HINT_RENDER_SCALE_QUALITY
	SDL_SetHintWithPriority(SDL_HINT_RENDER_SCALE_QUALITY, "2", SDL_HINT_DEFAULT);
#endif

#ifdef KRKRZ_ENABLE_CANVAS
	if (TVPIsEnableDrawDevice() == false)
	{
		SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		window_flags |= SDL_WINDOW_OPENGL;
	}
#endif

#ifdef SDL_HINT_TOUCH_MOUSE_EVENTS
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
#endif
#ifdef SDL_HINT_MOUSE_TOUCH_EVENTS
	SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");
#endif

#ifdef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	window_flags |= SDL_WINDOW_RESIZABLE;
	window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
#ifndef __EMSCRIPTEN__
	window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif
	new_window_w = 0;
	new_window_h = 0;
#endif

#ifdef _WIN32
	window_flags |= SDL_WINDOW_HIDDEN;
#endif

	window = SDL_CreateWindow("krkrsdl2", new_window_x, new_window_y, new_window_w, new_window_h, window_flags);
	if (window == nullptr)
	{
		TVPThrowExceptionMessage(TJS_W("Cannot create SDL window: %1"), ttstr(SDL_GetError()));
	}
#if defined(__EMSCRIPTEN__) && defined(KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE)
	EmscriptenFullscreenStrategy strategy;
	SDL_memset(&strategy, 0, sizeof(strategy));
	strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
	strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
	emscripten_enter_soft_fullscreen("#canvas", &strategy);
#endif
#ifdef KRKRZ_ENABLE_CANVAS
	context = nullptr;
	if (TVPIsEnableDrawDevice() == false)
	{
		context = SDL_GL_CreateContext(window);
		if (context == nullptr)
		{
			TVPThrowExceptionMessage(TJS_W("Cannot create SDL context: %1"), ttstr(SDL_GetError()));
		}
		SDL_GL_MakeCurrent(window, context);
	}
#endif
	renderer = nullptr;
	bitmap_completion = nullptr;
#ifdef KRKRZ_ENABLE_CANVAS
	open_gl_screen = nullptr;
#endif
	surface = nullptr;
#ifdef KRKRZ_ENABLE_CANVAS
	if (TVPIsEnableDrawDevice() == true)
#endif
	{
#if !defined(__EMSCRIPTEN__) || (defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__))
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (renderer == nullptr)
		{
			TVPAddLog(ttstr("Cannot create SDL renderer: ") + ttstr(SDL_GetError()));
		}
#endif

#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
		// move the event watch to after the SDL_RendererEventWatch to ensure transformed values are received
		SDL_DelEventWatch(sdl_event_watch, NULL);
		SDL_AddEventWatch(sdl_event_watch, NULL);
#endif

		bitmap_completion = new TVPSDLBitmapCompletion();
		if (renderer == nullptr)
		{
			surface = SDL_GetWindowSurface(window);
			if (surface == nullptr)
			{
				TVPAddLog(ttstr("Cannot get surface from SDL window: ") + ttstr(SDL_GetError()));
			}
			bitmap_completion->surface = surface;
		}
		if (renderer == nullptr && surface == nullptr)
		{
			TVPThrowExceptionMessage(TJS_W("Cannot get surface or renderer from SDL window"));
		}
		texture = nullptr;
		if (renderer)
		{
			SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );
		}
	}
#ifdef _WIN32
	::SetWindowLongPtr( this->GetHandle(), GWLP_USERDATA, (LONG_PTR)this );
#endif
	Application->AddWindow(this);
}

TVPWindowWindow::~TVPWindowWindow()
{
	if (_lastWindowWindow == this) _lastWindowWindow = _prevWindow;
	if (_nextWindow) _nextWindow->_prevWindow = _prevWindow;
	if (_prevWindow) _prevWindow->_nextWindow = _nextWindow;
	if (_currentWindowWindow == this) {
		_currentWindowWindow = _lastWindowWindow;
	}
	if (bitmap_completion)
	{
		delete bitmap_completion;
		bitmap_completion = NULL;
	}
#ifdef KRKRZ_ENABLE_CANVAS
	if (context)
	{
		SDL_GL_DeleteContext(context);
		context = NULL;
	}
#endif
	if (texture && surface)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;
		SDL_FreeSurface(surface);
		surface = NULL;
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

#ifdef _WIN32
	tjs_int count = WindowMessageReceivers.GetCount();
	for(tjs_int i = 0 ; i < count; i++)
	{
		tTVPMessageReceiverRecord * item = WindowMessageReceivers[i];
		if(!item) continue;
		delete item;
		WindowMessageReceivers.Remove(i);
	}
#endif

	Application->RemoveWindow(this);
}

void TVPWindowWindow::SetPaintBoxSize(tjs_int w, tjs_int h)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	LayerWidth = w;
	LayerHeight = h;
#endif
	if (renderer)
	{
		if (texture)
		{
			SDL_DestroyTexture(texture);
			texture = NULL;
		}
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, w, h);
		if (texture == nullptr)
		{
			TVPThrowExceptionMessage(TJS_W("Cannot create texture texture: %1"), ttstr(SDL_GetError()));
		}
		bitmap_completion->surface = NULL;
		if (surface)
		{
			SDL_FreeSurface(surface);
			surface = NULL;
		}
		surface = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0);
		if (surface == nullptr)
		{
			TVPThrowExceptionMessage(TJS_W("Cannot create surface: %1"), ttstr(SDL_GetError()));
		}
		bitmap_completion->surface = surface;
	}
#ifndef KRKRSDL2_ENABLE_ZOOM
	SDL_Rect cliprect;
	cliprect.x = 0;
	cliprect.y = 0;
	cliprect.w = w;
	cliprect.h = h;
#endif
	if (renderer)
	{
#ifdef KRKRSDL2_ENABLE_ZOOM
		UpdateActualZoom();
#else
		SDL_RenderSetLogicalSize(renderer, w, h);
#endif
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

#ifndef _WIN32
static int MulDiv(int nNumber, int nNumerator, int nDenominator)
{
	return (int)(((int64_t)nNumber * (int64_t)nNumerator) / nDenominator);
}
#endif

void TVPWindowWindow::TranslateWindowToDrawArea(int &x, int &y)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
#ifdef KRKRZ_ENABLE_CANVAS
	if (context != NULL)
	{
		return;
	}
#endif
	x -= LastSentDrawDeviceDestRect.left;
	y -= LastSentDrawDeviceDestRect.top;
	x = MulDiv(x, InnerWidth, LastSentDrawDeviceDestRect.get_width());
	y = MulDiv(y, InnerHeight, LastSentDrawDeviceDestRect.get_height());
#endif
}

void TVPWindowWindow::TranslateDrawAreaToWindow(int &x, int &y)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
#ifdef KRKRZ_ENABLE_CANVAS
	if (context != NULL)
	{
		return;
	}
#endif
	x = MulDiv(x, LastSentDrawDeviceDestRect.get_width(), InnerWidth);
	y = MulDiv(y, LastSentDrawDeviceDestRect.get_height(), InnerHeight);
	x += LastSentDrawDeviceDestRect.left;
	y += LastSentDrawDeviceDestRect.top;
#endif
}

bool TVPWindowWindow::GetFormEnabled()
{
	if (window)
	{
		return SDL_GetWindowFlags(window) & SDL_WINDOW_SHOWN;
	}
	return false;
}
void TVPWindowWindow::SetDefaultMouseCursor()
{
	if (!sdl_system_cursors[0])
	{
		for (int i = 0; i < SDL_NUM_SYSTEM_CURSORS; i += 1)
		{
			sdl_system_cursors[i] = SDL_CreateSystemCursor((SDL_SystemCursor)i);
		}
	}
	SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_ARROW]);
}
void TVPWindowWindow::SetMouseCursor(tjs_int handle)
{
	if (!sdl_system_cursors[0])
	{
		for (int i = 0; i < SDL_NUM_SYSTEM_CURSORS; i += 1)
		{
			sdl_system_cursors[i] = SDL_CreateSystemCursor((SDL_SystemCursor)i);
		}
	}
	switch (handle)
	{
		case -2: // crArrow
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_ARROW]);
			break;
		case -3: // crCross
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_CROSSHAIR]);
			break;
		case -4: // crIBeam
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_IBEAM]);
			break;
		case -5: // crSize
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_SIZEALL]);
			break;
		case -6: // crSizeNESW
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_SIZENESW]);
			break;
		case -7: // crSizeNS
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_SIZENS]);
			break;
		case -8: // crSizeNWSE
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_SIZENWSE]);
			break;
		case -9: // crSizeWE
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_SIZEWE]);
			break;
		case -11: // crHourGlass
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_WAIT]);
			break;
		case -18: // crNo
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_NO]);
			break;
		case -19: // crAppStart
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_WAITARROW]);
			break;
		case -21: // crHandPoint
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_HAND]);
			break;
		case -22: // crSizeAll
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_SIZEALL]);
			break;
		default:
			SDL_SetCursor(sdl_system_cursors[SDL_SYSTEM_CURSOR_ARROW]);
			break;
	}
}
void TVPWindowWindow::SetMouseCursorState(tTVPMouseCursorState mcs)
{
	cursor_temporary_hidden = (mcs == mcsTempHidden);
	SDL_ShowCursor((mcs == mcsVisible) ? SDL_ENABLE : SDL_DISABLE);
}
tTVPMouseCursorState TVPWindowWindow::GetMouseCursorState() const
{
	if (cursor_temporary_hidden)
	{
		return mcsTempHidden;
	}
	return (SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE) ? mcsVisible : mcsHidden;
}
void TVPWindowWindow::HideMouseCursor()
{
	SetMouseCursorState(mcsTempHidden);
}
void TVPWindowWindow::RestoreMouseCursor()
{
	if (cursor_temporary_hidden)
	{
		SetMouseCursorState(mcsVisible);
	}
}
void TVPWindowWindow::GetCursorPos(tjs_int &x, tjs_int &y)
{
	bool win_has_mouse_focus = window == SDL_GetMouseFocus();
	if (win_has_mouse_focus && renderer)
	{
		tjs_int new_x = 0;
		tjs_int new_y = 0;
		SDL_GetMouseState(&new_x, &new_y);
#ifdef KRKRSDL2_ENABLE_ZOOM
		TranslateWindowToDrawArea(new_x, new_y);
#else
		float scale_x, scale_y;
		SDL_Rect viewport;
		int window_w, window_h;
		int output_w, output_h;
		SDL_RenderGetScale(renderer, &scale_x, &scale_y);
		SDL_RenderGetViewport(renderer, &viewport);
		SDL_GetWindowSize(window, &window_w, &window_h);
		SDL_GetRendererOutputSize(renderer, &output_w, &output_h);
		float dpi_scale_x = (float)window_w / output_w;
		float dpi_scale_y = (float)window_h / output_h;
		new_x -= (int)(viewport.x * dpi_scale_x);
		new_y -= (int)(viewport.y * dpi_scale_y);
		new_x = (int)(new_x / (scale_x * dpi_scale_x));
		new_y = (int)(new_y / (scale_x * dpi_scale_y));
#endif
		x = new_x;
		y = new_y;
		return;
	}
	else if (win_has_mouse_focus)
	{
		SDL_GetMouseState(&x, &y);
		return;
	}
	x = last_mouse_x;
	y = last_mouse_y;
}
void TVPWindowWindow::SetCursorPos(tjs_int x, tjs_int y)
{
	RestoreMouseCursor();
	if (renderer && window)
	{
		tjs_int new_x = x;
		tjs_int new_y = y;
#ifdef KRKRSDL2_ENABLE_ZOOM
		TranslateDrawAreaToWindow(new_x, new_y);
#else
		float scale_x, scale_y;
		SDL_Rect viewport;
		int window_w, window_h;
		int output_w, output_h;
		SDL_RenderGetScale(renderer, &scale_x, &scale_y);
		SDL_RenderGetViewport(renderer, &viewport);
		SDL_GetWindowSize(window, &window_w, &window_h);
		SDL_GetRendererOutputSize(renderer, &output_w, &output_h);
		float dpi_scale_x = (float)window_w / output_w;
		float dpi_scale_y = (float)window_h / output_h;
		new_x = (int)(new_x * (scale_x * dpi_scale_x));
		new_y = (int)(new_y * (scale_x * dpi_scale_y));
		new_x += (int)(viewport.x * dpi_scale_x);
		new_y += (int)(viewport.y * dpi_scale_y);
#endif
		SDL_WarpMouseInWindow(window, new_x, new_y);
	}
	else if (window)
	{
		SDL_WarpMouseInWindow(window, x, y);
	}
}
void TVPWindowWindow::SetAttentionPoint(tjs_int left, tjs_int top, const struct tTVPFont * font)
{
	if (font == nullptr)
	{
		return;
	}
	attention_point_rect.x = left;
	attention_point_rect.y = top;
	attention_point_rect.w = 0;
	attention_point_rect.h = font->Height;
	TranslateDrawAreaToWindow(attention_point_rect.x, attention_point_rect.y);
	SDL_SetTextInputRect(&attention_point_rect);
}
void TVPWindowWindow::BringToFront()
{
	if (_currentWindowWindow != this) {
		if (_currentWindowWindow) {
			_currentWindowWindow->TJSNativeInstance->OnReleaseCapture();
		}
		_currentWindowWindow = this;
	}
	if (window)
	{
		SDL_RaiseWindow(window);
	}
}
void TVPWindowWindow::ShowWindowAsModal()
{
#if defined(KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE)
	TVPThrowExceptionMessage(TJS_W("Showing window as modal is not supported"));
#else
	in_mode_ = true;
	BringToFront();
	modal_result_ = 0;
	while (this == _currentWindowWindow && !modal_result_) {
		process_events();
		if (::Application->IsTarminate()) {
			modal_result_ = mrCancel;
		} else if (modal_result_ != 0) {
			break;
		}
	}
	in_mode_ = false;
#endif
}
bool TVPWindowWindow::GetVisible()
{
	if (!visibilityHasInitialized)
	{
		return isVisible;
	}
	if (window)
	{
		return SDL_GetWindowFlags(window) & SDL_WINDOW_SHOWN;
	}
	return false;
}
void TVPWindowWindow::SetVisible(bool visible)
{
	isVisible = visible;
	if (!visibilityHasInitialized)
	{
		return;
	}
	if (window)
	{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
		if (visible)
		{
			SDL_ShowWindow(window);
		}
		else
		{
			SDL_HideWindow(window);
		}
#endif
	}
	if (visible)
	{
		BringToFront();
	}
	else if (!visible && _currentWindowWindow == this)
	{
		_currentWindowWindow = _prevWindow ? _prevWindow : _nextWindow;
		if (_currentWindowWindow)
		{
			_currentWindowWindow->BringToFront();
		}
	}
}
void TVPWindowWindow::SetFullScreenMode(bool fullscreen)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	}
	UpdateWindow(utNormal);
#endif
}
bool TVPWindowWindow::GetFullScreenMode()
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		return SDL_GetWindowFlags(window) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
#endif
	return false;
}
void TVPWindowWindow::SetBorderStyle(tTVPBorderStyle bs)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		SDL_SetWindowBordered(window, (bs == bsNone) ? SDL_FALSE : SDL_TRUE);
		SDL_SetWindowResizable(window, (bs == bsSizeable || bs == bsSizeToolWin) ? SDL_TRUE : SDL_FALSE);
	}
#endif
}
tTVPBorderStyle TVPWindowWindow::GetBorderStyle() const
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		Uint32 flags = SDL_GetWindowFlags(window);
		if (flags & SDL_WINDOW_BORDERLESS)
		{
			return bsNone;
		}
		else if (flags & SDL_WINDOW_RESIZABLE)
		{
			return bsSizeable;
		}
	}
#endif
	return bsSingle;
}
tjs_string TVPWindowWindow::GetCaption()
{
	if (window)
	{
		std::string v_utf8 = SDL_GetWindowTitle(window);
		tjs_string v_utf16;
		TVPUtf8ToUtf16( v_utf16, v_utf8 );
		return v_utf16;
	}
	else
	{
		tjs_string empty = TJS_W("");
		return empty;
	}
}
void TVPWindowWindow::GetCaption(tjs_string & v) const
{
	v.clear();
	if (window)
	{
		std::string v_utf8 = SDL_GetWindowTitle(window);
		TVPUtf8ToUtf16( v, v_utf8 );
	}
}
void TVPWindowWindow::SetCaption(const tjs_string & v)
{
	if (window)
	{
		std::string v_utf8;
		if (TVPUtf16ToUtf8(v_utf8, v))
		{
			SDL_SetWindowTitle(window, v_utf8.c_str());
		}
	}
}
void TVPWindowWindow::SetWidth(tjs_int w)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		int h;
		SDL_GetWindowSize(window, NULL, &h);
		SDL_SetWindowSize(window, w, h);
		if (!renderer && surface)
		{
			bitmap_completion->surface = NULL;
			surface = SDL_GetWindowSurface(window);
			if (surface == nullptr)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get surface from SDL window: %1"), ttstr(SDL_GetError()));
			}
			bitmap_completion->surface = surface;
		}
	}
#endif
#ifdef KRKRSDL2_ENABLE_ZOOM
	UpdateActualZoom();
#else
	UpdateWindow(utNormal);
#endif
}
void TVPWindowWindow::SetHeight(tjs_int h)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		int w;
		SDL_GetWindowSize(window, &w, NULL);
		SDL_SetWindowSize(window, w, h);
		if (!renderer && surface)
		{
			bitmap_completion->surface = NULL;
			surface = SDL_GetWindowSurface(window);
			if (surface == nullptr)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get surface from SDL window: %1"), ttstr(SDL_GetError()));
			}
			bitmap_completion->surface = surface;
		}
	}
#endif
#ifdef KRKRSDL2_ENABLE_ZOOM
	UpdateActualZoom();
#else
	UpdateWindow(utNormal);
#endif
}
void TVPWindowWindow::SetSize(tjs_int w, tjs_int h)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		SDL_SetWindowSize(window, w, h);
		if (!renderer && surface)
		{
			bitmap_completion->surface = NULL;
			surface = SDL_GetWindowSurface(window);
			if (surface == nullptr)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get surface from SDL window: %1"), ttstr(SDL_GetError()));
			}
			bitmap_completion->surface = surface;
		}
	}
#endif
#ifdef KRKRSDL2_ENABLE_ZOOM
	UpdateActualZoom();
#else
	UpdateWindow(utNormal);
#endif
}
void TVPWindowWindow::GetSize(tjs_int &w, tjs_int &h)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		SDL_GetWindowSize(window, &w, &h);
		return;
	}
#endif
	if (renderer)
	{
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(renderer, &w, &h);
#endif
		return;
	}
	w = 0;
	h = 0;
}
tjs_int TVPWindowWindow::GetWidth() const
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		int w;
		SDL_GetWindowSize(window, &w, NULL);
		return w;
	}
#endif
	if (renderer)
	{
		int w, h;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(renderer, &w, &h);
#endif
		return w;
	}
	return 0;
}
tjs_int TVPWindowWindow::GetHeight() const
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		int h;
		SDL_GetWindowSize(window, NULL, &h);
		return h;
	}
#endif
	if (renderer)
	{
		int w, h;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(renderer, &w, &h);
#endif
		return h;
	}
	return 0;
}
void TVPWindowWindow::SetMinWidth(tjs_int w)
{
	SetMinSize(w, GetMinHeight());
}
void TVPWindowWindow::SetMaxWidth(tjs_int w)
{
	SetMaxSize(w, GetMaxHeight());
}
void TVPWindowWindow::SetMinHeight(tjs_int h)
{
	SetMinSize(GetMinWidth(), h);
}
void TVPWindowWindow::SetMaxHeight(tjs_int h)
{
	SetMaxSize(GetMaxWidth(), h);
}
void TVPWindowWindow::SetMinSize(tjs_int w, tjs_int h)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		SDL_SetWindowMinimumSize(window, w, h);
	}
#endif
}
void TVPWindowWindow::SetMaxSize(tjs_int w, tjs_int h)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		SDL_SetWindowMaximumSize(window, w, h);
	}
#endif
}
tjs_int TVPWindowWindow::GetMinWidth()
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		int w;
		SDL_GetWindowMinimumSize(window, &w, nullptr);
		return w;
	}
#endif
	if (renderer)
	{
		int w, h;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(renderer, &w, &h);
#endif
		return w;
	}
	return 0;
}
tjs_int TVPWindowWindow::GetMaxWidth()
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		int w;
		SDL_GetWindowMaximumSize(window, &w, nullptr);
		return w;
	}
#endif
	if (renderer)
	{
		int w, h;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(renderer, &w, &h);
#endif
		return w;
	}
	return 0;
}
tjs_int TVPWindowWindow::GetMinHeight()
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		int h;
		SDL_GetWindowMinimumSize(window, &h, nullptr);
		return h;
	}
#endif
	if (renderer)
	{
		int w, h;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(renderer, &w, &h);
#endif
		return h;
	}
	return 0;
}
tjs_int TVPWindowWindow::GetMaxHeight()
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		int h;
		SDL_GetWindowMaximumSize(window, &h, nullptr);
		return h;
	}
#endif
	if (renderer)
	{
		int w, h;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(renderer, &w, &h);
#endif
		return h;
	}
	return 0;
}
tjs_int TVPWindowWindow::GetLeft()
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		int x;
		SDL_GetWindowPosition(window, &x, nullptr);
		return x;
	}
#endif
	return 0;
}
void TVPWindowWindow::SetLeft(tjs_int l)
{
	SetPosition(l, GetTop());
}
tjs_int TVPWindowWindow::GetTop()
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		int y;
		SDL_GetWindowPosition(window, nullptr, &y);
		return y;
	}
#endif
	return 0;
}
void TVPWindowWindow::SetTop(tjs_int t)
{
	SetPosition(GetLeft(), t);
}
void TVPWindowWindow::SetPosition(tjs_int l, tjs_int t)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (window)
	{
		SDL_SetWindowPosition(window, l, t);
	}
#endif
}
TVPSDLBitmapCompletion *TVPWindowWindow::GetTVPSDLBitmapCompletion()
{
	needs_graphic_update = true;
	return bitmap_completion;
}
#ifdef KRKRZ_ENABLE_CANVAS
void TVPWindowWindow::SetOpenGLScreen(tTVPOpenGLScreen *s)
{
	open_gl_screen = s;
}
void TVPWindowWindow::SetSwapInterval(int interval)
{
	if (context)
	{
		SDL_GL_SetSwapInterval(interval);
	}
}
void TVPWindowWindow::GetDrawableSize(tjs_int &w, tjs_int &h)
{
	if (context)
	{
		SDL_GL_GetDrawableSize(window, &w, &h);
	}
	else
	{
		w = 0;
		h = 0;
	}
}
void TVPWindowWindow::Swap()
{
	if (context)
	{
		SDL_GL_SwapWindow(window);
	}
}
#endif
void TVPWindowWindow::Show()
{
}
void TVPWindowWindow::TickBeat()
{
	if (!visibilityHasInitialized)
	{
		visibilityHasInitialized = true;
		SetVisible(isVisible);
	}
	if (needs_graphic_update)
	{
		if (bitmap_completion)
		{
			SDL_Rect rect;
			rect.x = bitmap_completion->update_rect.left;
			rect.y = bitmap_completion->update_rect.top;
			rect.w = bitmap_completion->update_rect.get_width();
			rect.h = bitmap_completion->update_rect.get_height();
			if (renderer)
			{
				SDL_RenderFillRect(renderer, NULL);
				if (texture)
				{
					if (surface)
					{
						if ((rect.w + rect.x) > surface->w)
						{
							rect.w = surface->w;
						}
						if ((rect.h + rect.y) > surface->h)
						{
							rect.h = surface->h;
						}
						SDL_UpdateTexture(texture, &rect, surface->pixels, surface->pitch);
					}
#ifdef KRKRSDL2_ENABLE_ZOOM
					SDL_Rect destrect;
					destrect.x = LastSentDrawDeviceDestRect.left;
					destrect.y = LastSentDrawDeviceDestRect.top;
					destrect.w = LastSentDrawDeviceDestRect.get_width();
					destrect.h = LastSentDrawDeviceDestRect.get_height();
					SDL_Rect srcrect;
					srcrect.x = 0;
					srcrect.y = 0;
					srcrect.w = InnerWidth;
					srcrect.h = InnerHeight;
					SDL_RenderCopy(renderer, texture, &srcrect, &destrect);
#else
					SDL_RenderCopy(renderer, texture, NULL, NULL);
#endif
				}
				SDL_RenderPresent(renderer);
				hasDrawn = true;
			}
			else if (window && surface)
			{
				SDL_UpdateWindowSurfaceRects(window, &rect, 1);
				hasDrawn = true;
			}
			needs_graphic_update = false;
		}
	}
#ifdef KRKRZ_ENABLE_CANVAS
	else if (context && TJSNativeInstance)
	{
		TJSNativeInstance->StartDrawing();
		hasDrawn = true;
	}
#endif
}
void TVPWindowWindow::InvalidateClose()
{
	TJSNativeInstance = NULL;
	SetVisible(false);
	delete this;
}
bool TVPWindowWindow::GetWindowActive()
{
	return _currentWindowWindow == this && SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS;
}
void TVPWindowWindow::OnClose(CloseAction& action)
{
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
		}
	}
}
bool TVPWindowWindow::OnCloseQuery()
{
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
				process_events(); // for post event
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
void TVPWindowWindow::Close()
{
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
				if (window)
				{
					SDL_MinimizeWindow(window);
				}
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
void TVPWindowWindow::OnCloseQueryCalled(bool b)
{
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
void TVPWindowWindow::SetImeMode(tTVPImeMode mode)
{
	if (!window || mode == ::imDisable || mode == ::imClose)
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
void TVPWindowWindow::ResetImeMode()
{
	ime_composition = nullptr;
	ime_composition_len = 0;
	ime_composition_cursor = 0;
	ime_composition_selection = 0;
	attention_point_rect.x = 0;
	attention_point_rect.y = 0;
	attention_point_rect.w = 0;
	attention_point_rect.h = 0;
	if (window && SDL_IsTextInputActive())
	{
		SDL_SetTextInputRect(&attention_point_rect);
		SDL_StopTextInput();
	}
}
void TVPWindowWindow::UpdateWindow(tTVPUpdateType type)
{
	if (TJSNativeInstance) {
		tTVPRect r;
#ifdef KRKRSDL2_ENABLE_ZOOM
		r.left = 0;
		r.top = 0;
		r.right = LayerWidth;
		r.bottom = LayerHeight;
#else
		r.clear();
		if (renderer)
		{
			SDL_RenderGetLogicalSize(renderer, &(r.right), &(r.bottom));
			SDL_RenderSetLogicalSize(renderer, r.right, r.bottom);
		}
		else if (window)
		{
			SDL_GetWindowSize(window, &(r.right), &(r.bottom));
		}
#endif
		TJSNativeInstance->NotifyWindowExposureToLayer(r);
		TVPDeliverWindowUpdateEvents();
	}
}
void TVPWindowWindow::InternalKeyDown(tjs_uint16 key, tjs_uint32 shift)
{
	TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, key, shift));
}
void TVPWindowWindow::OnKeyUp(tjs_uint16 vk, int shift)
{
	TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, vk, shift));
}
void TVPWindowWindow::OnKeyPress(tjs_uint16 vk, int repeat, bool prevkeystate, bool convertkey)
{
	TVPPostInputEvent(new tTVPOnKeyPressInputEvent(TJSNativeInstance, vk));
}

#ifdef KRKRSDL2_ENABLE_ZOOM
//---------------------------------------------------------------------------
//! @brief	do reduction for numer over denom
static void TVPDoReductionNumerAndDenom(tjs_int &n, tjs_int &d)
{
	tjs_int a = n;
	tjs_int b = d;
	while(b)
	{
		tjs_int t = b;
		b = a % b;
		a = t;
	}
	n = n / a;
	d = d / a;
}
#endif

void TVPWindowWindow::UpdateActualZoom(void)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	if (renderer == NULL)
	{
		return;
	}
#ifdef KRKRZ_ENABLE_CANVAS
	if (context != NULL)
	{
		return;
	}
#endif
	// determine fullscreen zoom factor and client size
	int sb_w, sb_h, zoom_d, zoom_n, output_w, output_h;
	SDL_GetRendererOutputSize(renderer, &output_w, &output_h);

	float layer_aspect = (float)InnerWidth / InnerHeight;
	float output_aspect = (float)output_w / output_h;

	// 0=letterbox, 1=crop
	int scale_policy = 0;
	SDL_Rect viewport;
	if (SDL_fabs(layer_aspect - output_aspect) < 0.0001)
	{
		zoom_n = 1;
		zoom_d = 1;
		viewport.x = 0;
		viewport.y = 0;
		viewport.w = InnerWidth;
		viewport.h = InnerHeight;
	}
	else if (layer_aspect > output_aspect)
	{
		if (scale_policy == 1)
		{
			// Crop left and right
			zoom_n = output_h;
			zoom_d = InnerHeight;
			TVPDoReductionNumerAndDenom(zoom_n, zoom_d);
			viewport.y = 0;
			viewport.h = output_h;
			viewport.w = MulDiv(InnerWidth, zoom_n, zoom_d);
			viewport.x = (output_w - viewport.w) / 2;
		}
		else
		{
			// Top and bottom black bars (letterbox)
			zoom_n = output_w;
			zoom_d = InnerWidth;
			TVPDoReductionNumerAndDenom(zoom_n, zoom_d);
			viewport.x = 0;
			viewport.w = output_w;
			viewport.h = MulDiv(InnerHeight, zoom_n, zoom_d);
			viewport.y = (output_h - viewport.h) / 2;
		}
	}
	else
	{
		if (scale_policy == 1)
		{
			// Crop top and bottom
			zoom_n = output_w;
			zoom_d = InnerWidth;
			TVPDoReductionNumerAndDenom(zoom_n, zoom_d);
			viewport.x = 0;
			viewport.w = output_w;
			viewport.h = MulDiv(InnerHeight, zoom_n, zoom_d);
			viewport.y = (output_h - viewport.h) / 2;
		}
		else
		{
			// Left and right black bars (letterbox)
			zoom_n = output_h;
			zoom_d = InnerHeight;
			TVPDoReductionNumerAndDenom(zoom_n, zoom_d);
			viewport.y = 0;
			viewport.h = output_h;
			viewport.w = MulDiv(InnerWidth, zoom_n, zoom_d);
			viewport.x = (output_w - viewport.w) / 2;
		}
	}
	FullScreenDestRect.set_size(viewport.w, viewport.h);
	FullScreenDestRect.set_offsets(viewport.x, viewport.y);

	ActualZoomNumer = zoom_n;
	ActualZoomDenom = zoom_d;
	SetDrawDeviceDestRect();
#endif
}

void TVPWindowWindow::SetDrawDeviceDestRect(void)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	tTVPRect destrect;
	tjs_int w = MulDiv(InnerWidth,  ActualZoomNumer, ActualZoomDenom);
	tjs_int h = MulDiv(InnerHeight, ActualZoomNumer, ActualZoomDenom);
	if (w < 1)
	{
		w = 1;
	}
	if (h < 1)
	{
		h = 1;
	}
	{
		destrect.left = FullScreenDestRect.left;
		destrect.top = FullScreenDestRect.top;
		destrect.right = destrect.left + w;
		destrect.bottom = destrect.top + h;
	}

	if (LastSentDrawDeviceDestRect != destrect)
	{
		LastSentDrawDeviceDestRect = destrect;
		UpdateWindow(utNormal);
	}
#endif
}

void TVPWindowWindow::SetZoom(tjs_int numer, tjs_int denom, bool set_logical)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	bool ischanged = false;
	// set layer zooming factor;
	// the zooming factor is passed in numerator/denoiminator style.
	// we must find GCM to optimize numer/denium via Euclidean algorithm.
	TVPDoReductionNumerAndDenom(numer, denom);
	if( set_logical )
	{
		if( ZoomNumer != numer || ZoomDenom != denom )
		{
			ischanged = true;
		}
		ZoomNumer = numer;
		ZoomDenom = denom;
	}
	UpdateActualZoom();
#endif
}

void TVPWindowWindow::SetZoomNumer(tjs_int n)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	SetZoom(n, ZoomDenom);
#endif
}

tjs_int TVPWindowWindow::GetZoomNumer() const
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	return ZoomNumer;
#else
	return 1;
#endif
}

void TVPWindowWindow::SetZoomDenom(tjs_int d)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	SetZoom(ZoomNumer, d);
#endif
}

tjs_int TVPWindowWindow::GetZoomDenom() const
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	return ZoomDenom;
#else
	return 1;
#endif
}

void TVPWindowWindow::SetInnerWidth(tjs_int v)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	SetInnerSize(v, InnerHeight);
#else
	SetWidth(v);
#endif
}

void TVPWindowWindow::SetInnerHeight(tjs_int v)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	SetInnerSize(InnerWidth, v);
#else
	SetHeight(v);
#endif
}

void TVPWindowWindow::SetInnerSize(tjs_int w, tjs_int h)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	InnerWidth = w;
	InnerHeight = h;
	UpdateActualZoom();
#endif
	SetSize(w, h);
}

tjs_int TVPWindowWindow::GetInnerWidth()
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	return InnerWidth;
#else
	return GetWidth();
#endif
}

tjs_int TVPWindowWindow::GetInnerHeight()
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	return InnerHeight;
#else
	return GetHeight();
#endif
}

#ifdef _WIN32
void TVPWindowWindow::RegisterWindowMessageReceiver(tTVPWMRRegMode mode, void * proc, const void *userdata)
{
	if( mode == wrmRegister ) {
		// register
		tjs_int count = WindowMessageReceivers.GetCount();
		tjs_int i;
		for(i = 0 ; i < count; i++) {
			tTVPMessageReceiverRecord *item = WindowMessageReceivers[i];
			if(!item) continue;
			if((void*)item->Proc == proc) break; // have already registered
		}
		if(i == count) {
			// not have registered
			tTVPMessageReceiverRecord *item = new tTVPMessageReceiverRecord();
			item->Proc = (tTVPWindowMessageReceiver)proc;
			item->UserData = userdata;
			WindowMessageReceivers.Add(item);
		}
	} else if(mode == wrmUnregister) {
		// unregister
		tjs_int count = WindowMessageReceivers.GetCount();
		for(tjs_int i = 0 ; i < count; i++) {
			tTVPMessageReceiverRecord *item = WindowMessageReceivers[i];
			if(!item) continue;
			if((void*)item->Proc == proc) {
				// found
				WindowMessageReceivers.Remove(i);
				delete item;
			}
		}
		WindowMessageReceivers.Compact();
	}
}

bool TVPWindowWindow::InternalDeliverMessageToReceiver(tTVPWindowMessage &msg)
{
	if( WindowMessageReceivers.GetCount() == 0 ) return false;
	if( !TJSNativeInstance ) return false;
#ifdef KRKRSDL2_ENABLE_PLUGINS
	if( TVPPluginUnloadedAtSystemExit ) return false;
#endif

	tObjectListSafeLockHolder<tTVPMessageReceiverRecord> holder(WindowMessageReceivers);
	tjs_int count = WindowMessageReceivers.GetSafeLockedObjectCount();

	bool block = false;
	for( tjs_int i = 0; i < count; i++ ) {
		tTVPMessageReceiverRecord *item = WindowMessageReceivers.GetSafeLockedObjectAt(i);
		if(!item) continue;
		bool b = item->Deliver(&msg);
		block = block || b;
	}
	return block;
}

HWND TVPWindowWindow::GetHandle() const
{
	SDL_SysWMinfo syswminfo;
	SDL_VERSION(&syswminfo.version);
	if (SDL_GetWindowWMInfo(window, &syswminfo))
	{
		return syswminfo.info.win.window;
	}
	return NULL;
}
#endif

bool TVPWindowWindow::should_try_parent_window(SDL_Event event)
{
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
			case SDL_WINDOWEVENT:
				tryParentWindow = event.window.windowID != windowID;
				break;
			default:
				tryParentWindow = false;
				break;
		}
		return tryParentWindow;
	}
	return false;
}

void TVPWindowWindow::window_receive_event(SDL_Event event)
{
	if (isBeingDeleted) {
		delete this;
		return;
	}
	if (this->should_try_parent_window(event))
	{
		if (!in_mode_)
		{
			_prevWindow->window_receive_event(event);
		}
		return;
	}
	if (window && hasDrawn) {
		tjs_uint32 s = TVP_TShiftState_To_uint32(GetShiftState());
		s |= GetMouseButtonState();
		if (TJSNativeInstance->CanDeliverEvents()) {
			switch (event.type) {
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
					return;
				}
				case SDL_DROPBEGIN: {
					if (!file_drop_array)
					{
						file_drop_array = TJSCreateArrayObject();
					}
					return;
				}
				case SDL_DROPCOMPLETE: {
					if (file_drop_array)
					{
						tTJSVariant arg(file_drop_array, file_drop_array);
						TVPPostInputEvent(new tTVPOnFileDropInputEvent(TJSNativeInstance, arg));
						file_drop_array->Release();
						file_drop_array = nullptr;
						file_drop_array_count = 0;
					}
					return;
				}
				case SDL_DROPFILE:
				case SDL_DROPTEXT: {
					if (file_drop_array && event.drop.file)
					{
						std::string f_utf8 = event.drop.file;
						tjs_string f_utf16;
						TVPUtf8ToUtf16( f_utf16, f_utf8 );
						SDL_free(event.drop.file);
						if (TVPIsExistentStorageNoSearch(f_utf16))
						{
							tTJSVariant val = TVPNormalizeStorageName(ttstr(f_utf16));
							file_drop_array->PropSetByNum(TJS_MEMBERENSURE|TJS_IGNOREPROP, file_drop_array_count, &val, file_drop_array);
							file_drop_array_count += 1;
						}
					}
					return;
				}
				case SDL_CONTROLLERDEVICEADDED:
				case SDL_CONTROLLERDEVICEREMOVED:
				case SDL_CONTROLLERDEVICEREMAPPED:
				{
					refresh_controllers();
					return;
				}
				case SDL_WINDOWEVENT: {
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_EXPOSED: {
							UpdateWindow(utNormal);
							return;
						}
						case SDL_WINDOWEVENT_MINIMIZED:
						case SDL_WINDOWEVENT_MAXIMIZED:
						case SDL_WINDOWEVENT_RESTORED:
						case SDL_WINDOWEVENT_RESIZED:
						case SDL_WINDOWEVENT_SIZE_CHANGED: {
#ifdef KRKRSDL2_ENABLE_ZOOM
							UpdateActualZoom();
#else
							UpdateWindow(utNormal);
#endif
							TVPPostInputEvent(new tTVPOnResizeInputEvent(TJSNativeInstance), TVP_EPT_REMOVE_POST);
							return;
						}
						case SDL_WINDOWEVENT_ENTER: {
							TVPPostInputEvent(new tTVPOnMouseEnterInputEvent(TJSNativeInstance));
							return;
						}
						case SDL_WINDOWEVENT_LEAVE: {
							TVPPostInputEvent(new tTVPOnMouseOutOfWindowInputEvent(TJSNativeInstance));
							TVPPostInputEvent(new tTVPOnMouseLeaveInputEvent(TJSNativeInstance));
							return;
						}
						case SDL_WINDOWEVENT_FOCUS_GAINED:
						case SDL_WINDOWEVENT_FOCUS_LOST: {
							TVPPostInputEvent(new tTVPOnWindowActivateEvent(TJSNativeInstance, event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED), TVP_EPT_REMOVE_POST);
							return;
						}
						case SDL_WINDOWEVENT_CLOSE: {
							TVPPostInputEvent(new tTVPOnCloseInputEvent(TJSNativeInstance));
							return;
						}
						default: {
							return;
						}
					}
				}
				case SDL_QUIT: {
					TVPPostInputEvent(new tTVPOnCloseInputEvent(TJSNativeInstance));
					return;
				}
				default: {
#if !(defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__))
					this->window_receive_event_input(event);
#endif
					return;
				}
			}
		}
	}
}

bool TVPWindowWindow::window_receive_event_input(SDL_Event event)
{
	if (isBeingDeleted) {
		delete this;
		return false;
	}
	if (this->should_try_parent_window(event))
	{
		if (!in_mode_)
		{
			return _prevWindow->window_receive_event_input(event);
		}
		return false;
	}
	if (window && hasDrawn) {
		tjs_uint32 s = TVP_TShiftState_To_uint32(GetShiftState());
		s |= GetMouseButtonState();
		if (TJSNativeInstance->CanDeliverEvents()) {
			switch (event.type) {
				case SDL_MOUSEMOTION: {
					RestoreMouseCursor();
					last_mouse_x = event.motion.x;
					last_mouse_y = event.motion.y;
					TranslateWindowToDrawArea(last_mouse_x, last_mouse_y);
					TVPPostInputEvent(new tTVPOnMouseMoveInputEvent(TJSNativeInstance, last_mouse_x, last_mouse_y, s));
					return true;
				}
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP: {
					if (SDL_IsTextInputActive() && ime_composition != nullptr)
					{
						return false;
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
						last_mouse_x = event.button.x;
						last_mouse_y = event.button.y;
						TranslateWindowToDrawArea(last_mouse_x, last_mouse_y);
						TVPPostInputEvent(new tTVPOnMouseMoveInputEvent(TJSNativeInstance, last_mouse_x, last_mouse_y, s));
						switch (event.type) {
							case SDL_MOUSEBUTTONDOWN:
								TVPPostInputEvent(new tTVPOnMouseDownInputEvent(TJSNativeInstance, last_mouse_x, last_mouse_y, btn, s));
								break;
							case SDL_MOUSEBUTTONUP:
								if (event.button.clicks >= 2)
								{
									TVPPostInputEvent(new tTVPOnDoubleClickInputEvent(TJSNativeInstance, last_mouse_x, last_mouse_y));
								}
								else
								{
									TVPPostInputEvent(new tTVPOnClickInputEvent(TJSNativeInstance, last_mouse_x, last_mouse_y));
								}
								TVPPostInputEvent(new tTVPOnMouseUpInputEvent(TJSNativeInstance, last_mouse_x, last_mouse_y, btn, s));
								break;
						}
						return true;
					}
					return false;
				}
				case SDL_MOUSEWHEEL: {
					TranslateWindowToDrawArea(last_mouse_x, last_mouse_y);
					TVPPostInputEvent(new tTVPOnMouseWheelInputEvent(TJSNativeInstance, event.wheel.x, event.wheel.y, last_mouse_x, last_mouse_y));
					return true;
				}
				case SDL_FINGERMOTION: {
					TVPPostInputEvent(new tTVPOnTouchMoveInputEvent(TJSNativeInstance, event.tfinger.x, event.tfinger.y, 1, 1, event.tfinger.fingerId));
					return true;
				}
				case SDL_FINGERDOWN:
				case SDL_FINGERUP: {
					switch (event.tfinger.type) {
						case SDL_FINGERDOWN:
							TVPPostInputEvent(new tTVPOnTouchDownInputEvent(TJSNativeInstance, event.tfinger.x, event.tfinger.y, 1, 1, event.tfinger.fingerId));
							break;
						case SDL_FINGERUP:
							TVPPostInputEvent(new tTVPOnTouchUpInputEvent(TJSNativeInstance, event.tfinger.x, event.tfinger.y, 1, 1, event.tfinger.fingerId));
							break;
					}
					return true;
				}
				case SDL_MULTIGESTURE: {
					TVPPostInputEvent(new tTVPOnTouchScalingInputEvent(TJSNativeInstance, 0, event.mgesture.dDist, event.mgesture.x, event.mgesture.y, 0));
					TVPPostInputEvent(new tTVPOnTouchRotateInputEvent(TJSNativeInstance, 0, event.mgesture.dTheta, event.mgesture.dDist, event.mgesture.x, event.mgesture.y, 0));
					return true;
				}
				case SDL_CONTROLLERBUTTONDOWN:
				case SDL_CONTROLLERBUTTONUP: {
					switch (event.cbutton.state) {
						case SDL_PRESSED:
							TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, sdl_gamecontrollerbutton_to_vk_key(event.cbutton.button), s));
							break;
						case SDL_RELEASED:
							if (!SDL_IsTextInputActive())
							{
								TVPPostInputEvent(new tTVPOnKeyPressInputEvent(TJSNativeInstance, sdl_gamecontrollerbutton_to_vk_key(event.cbutton.button)));
							}
							TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, sdl_gamecontrollerbutton_to_vk_key(event.cbutton.button), s));
							break;
					}
					return true;
				}
				case SDL_KEYDOWN: {
					if (SDL_IsTextInputActive())
					{
						if (ime_composition != nullptr)
						{
							return false;
						}
					}
					if (event.key.repeat) s |= TVP_SS_REPEAT;
					tjs_uint unified_vk_key = 0;
					switch (event.key.keysym.sym)
					{
						case SDLK_LSHIFT:
						case SDLK_RSHIFT:
							unified_vk_key = VK_SHIFT;
							break;
						case SDLK_LCTRL:
						case SDLK_RCTRL:
							unified_vk_key = VK_CONTROL;
							break;
						case SDLK_LALT:
						case SDLK_RALT:
							unified_vk_key = VK_MENU;
							break;
					}
					TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, sdl_key_to_vk_key(event.key.keysym.sym), s));
					if (unified_vk_key)
					{
						TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, unified_vk_key, s));
					}
					SDL_SetTextInputRect(&attention_point_rect);
					return true;
				}
				case SDL_KEYUP: {
					if (SDL_IsTextInputActive())
					{
						if (ime_composition != nullptr)
						{
							return false;
						}
					}
					tjs_uint unified_vk_key = 0;
					switch (event.key.keysym.sym)
					{
						case SDLK_LSHIFT:
						case SDLK_RSHIFT:
							unified_vk_key = VK_SHIFT;
							break;
						case SDLK_LCTRL:
						case SDLK_RCTRL:
							unified_vk_key = VK_CONTROL;
							break;
						case SDLK_LALT:
						case SDLK_RALT:
							unified_vk_key = VK_MENU;
							break;
					}
					if (!SDL_IsTextInputActive())
					{
						TVPPostInputEvent(new tTVPOnKeyPressInputEvent(TJSNativeInstance, sdl_key_to_vk_key(event.key.keysym.sym)));
						if (unified_vk_key)
						{
							TVPPostInputEvent(new tTVPOnKeyPressInputEvent(TJSNativeInstance, unified_vk_key));
						}
					}
					TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, sdl_key_to_vk_key(event.key.keysym.sym), s));
					if (unified_vk_key)
					{
						TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, unified_vk_key, s));
					}
					SDL_SetTextInputRect(&attention_point_rect);
					return true;
				}
				default: {
					return false;
				}
			}
		}
	}
	return false;
}

void sdl_process_events()
{
	if (SDL_WasInit(SDL_INIT_EVENTS) != 0)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == NativeEventQueueImplement::native_event_queue_custom_event_type)
			{
				((NativeEvent*)event.user.data2)->HandleEvent();
			}
			else
			{
				if (_currentWindowWindow)
				{
					_currentWindowWindow->window_receive_event(event);
				}
				else
				{
					if (event.type == SDL_QUIT)
					{
						Application->Terminate();
					}
				}
			}
		}
	}
}

#ifdef _WIN32
static void sdl_windows_message_hook(void *userdata, void *hWnd, unsigned int message, Uint64 wParam, Sint64 lParam)
{
	TVPWindowWindow *win = reinterpret_cast<TVPWindowWindow*>(::GetWindowLongPtr((HWND)hWnd, GWLP_USERDATA));
	if (win != NULL)
	{
		tTVPWindowMessage Message;
		Message.LParam = lParam;
		Message.WParam = wParam;
		Message.Msg = message;
		Message.Result = 0;
		if (win->InternalDeliverMessageToReceiver(Message))
		{
			// TODO: return Message.result and block
		}
	}
}
#endif

#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
static int sdl_event_watch(void *userdata, SDL_Event *in_event)
{
	SDL_Event event;
	SDL_memcpy(&event, in_event, sizeof(SDL_Event));
	if (event.type != NativeEventQueueImplement::native_event_queue_custom_event_type)
	{
		if (_currentWindowWindow)
		{
			if (_currentWindowWindow->window_receive_event_input(event))
			{
				if (TVPSystemControl)
				{
					// process events now
					// Some JS functions will only work in e.g. mouse down callback due to browser restruction
					TVPSystemControl->ApplicationIdle();
				}
			}
		}
	}
	return 1;
}
#endif

#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
static void process_events()
#else
static bool process_events()
#endif
{
	try
	{
		try
		{
#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
			tTJSNI_WaveSoundBuffer::Trigger();
			tTVPTimerThread::Trigger();
#endif
			::Application->Run();
			if (::Application->IsTarminate())
			{
				TVPSystemUninit();
				if (TVPSystemControl)
				{
					delete TVPSystemControl;
				}
				TVPSystemControl = NULL;
#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
				emscripten_cancel_main_loop();
#else
				return false;
#endif
			}
		}
		TJS_CONVERT_TO_TJS_EXCEPTION
	}
	TVP_CATCH_AND_SHOW_SCRIPT_EXCEPTION(TJS_W("SDL event processing"));

#if !defined(__EMSCRIPTEN__) || (defined(__EMSCRIPTEN__) && defined(__EMSCRIPTEN_PTHREADS__))
	return true;
#endif
}

#if defined(USE_SDL_MAIN)
extern "C" int SDL_main(int argc, char **argv)
#elif defined(_WIN32) && defined(_UNICODE)
extern "C" int wmain(int argc, wchar_t **argv)
#else
extern "C" int main(int argc, char **argv)
#endif
{
	TVPTerminateCode = 0;
	try
	{
#ifdef _WIN32
		_set_error_mode(_OUT_TO_STDERR);
#endif

#ifdef _WIN32
		_argc = argc;
		_wargv = argv;
#else
		_argc = argc;
		_wargv = new tjs_char*[argc];

		for (int i = 0; i < argc; i += 1)
		{
			const char* narg;
#if !defined(__EMSCRIPTEN__) && !defined(__vita__) && !defined(__SWITCH__)
			if (!i)
			{
				narg = realpath(argv[i], NULL);
			}
			else
#endif
			{
				narg = argv[i];
			}
			if (!narg)
			{
				tjs_char* warg_copy = new tjs_char[1];
				warg_copy[0] = '\0';
				_wargv[i] = warg_copy;
				continue;
			}
			std::string v_utf8 = narg;
			tjs_string v_utf16;
			TVPUtf8ToUtf16( v_utf16, v_utf8 );
#if !defined(__EMSCRIPTEN__) && !defined(__vita__) && !defined(__SWITCH__)
			if (!i)
			{
				free((void*)narg);
			}
#endif
			tjs_char* warg_copy = new tjs_char[v_utf16.length() + 1];
			SDL_memcpy(warg_copy, v_utf16.c_str(), sizeof(tjs_char) * (v_utf16.length()));
			warg_copy[v_utf16.length()] = '\0';
			_wargv[i] = warg_copy;
		}
#endif

#ifdef __SWITCH__
		romfsInit();
		socketInitializeDefault();
		nxlinkStdio();
#endif

		SDL_setenv("VITA_DISABLE_TOUCH_BACK", "1", 1);
		SDL_setenv("DBUS_FATAL_WARNINGS", "0", 0);

#ifdef _WIN32
#ifdef SDL_HINT_AUDIODRIVER
		SDL_SetHintWithPriority(SDL_HINT_AUDIODRIVER, "directsound", SDL_HINT_DEFAULT);
#endif
#endif

#ifdef TVP_LOG_TO_COMMANDLINE_CONSOLE
		SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
#endif

		TVPLoadMessage();

#ifdef _WIN32
		SDL_SetWindowsMessageHook(sdl_windows_message_hook, NULL);
#endif
#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
		SDL_AddEventWatch(sdl_event_watch, NULL);
#endif
		
		::Application = new tTVPApplication();
		if (::Application->StartApplication( _argc, _wargv ))
		{
			return 0;
		}

#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
		emscripten_set_main_loop(process_events, 0, 0);
#else
		while (process_events());
#endif

#ifndef __EMSCRIPTEN__
		// delete application and exit forcely
		// this prevents ugly exception message on exit
		delete ::Application;
		::Application = nullptr;
#endif
	}
	catch (...)
	{
		return 2;
	}
#ifdef _WIN32
	::TerminateProcess(GetCurrentProcess(), (UINT)TVPTerminateCode);
#endif
	return TVPTerminateCode;
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
	bool is_pressed = false;
	if (sdl_controllers != NULL)
	{
		for (int i = 0; i < sdl_controller_num; i += 1)
		{
			if (sdl_controllers[i] && sdl_controllers[i] != NULL)
			{
				is_pressed |= !!SDL_GameControllerGetButton(sdl_controllers[i], (SDL_GameControllerButton)vk_key_to_sdl_gamecontrollerbutton(keycode));
			}
		}
	}
	return is_pressed;
}

TTVPWindowForm *TVPCreateAndAddWindow(tTJSNI_Window *w)
{
	return new TVPWindowWindow(w);
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

tjs_uint32 TVP_TShiftState_To_uint32(TShiftState state)
{
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
TShiftState TVP_TShiftState_From_uint32(tjs_uint32 state)
{
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

void TVPGetAllFontList(std::vector<tjs_string>& list) {}

const tjs_char *TVPGetDefaultFontName()
{
	if (!TVPGetCommandLine(TJS_W("-deffont"), NULL))
	{
		TVPSetCommandLine(TJS_W("-deffont"), TJS_W("Noto Sans CJK JP"));
	}
	static tjs_int ArgumentGeneration = 0;
	if (ArgumentGeneration != TVPGetCommandLineArgumentGeneration())
	{
		ArgumentGeneration = TVPGetCommandLineArgumentGeneration();
		// Use that font, if specified on the command line
		tTJSVariant opt;
		if(TVPGetCommandLine(TJS_W("-deffont"), &opt))
		{
			ttstr str(opt);
			TVPDefaultFontName.AssignMessage(str.c_str());
		}
	}
	return TVPDefaultFontName;
}

void TVPSetDefaultFontName(const tjs_char * name)
{
	TVPSetCommandLine(TJS_W("-deffont"), name);
}

static ttstr TVPDefaultFaceNames;
const ttstr &TVPGetDefaultFaceNames()
{
	static tjs_int ArgumentGeneration = 0;
	if (ArgumentGeneration != TVPGetCommandLineArgumentGeneration())
	{
		ArgumentGeneration = TVPGetCommandLineArgumentGeneration();
		TVPDefaultFaceNames = TJS_W("");
	}
	if( !TVPDefaultFaceNames.IsEmpty() )
	{
		return TVPDefaultFaceNames;
	}
	else
	{
		TVPDefaultFaceNames = ttstr(TVPGetDefaultFontName());
		return TVPDefaultFaceNames;
	}
}

#if defined(__vita__)
#define KRKRSDL2_OVERRIDE_NEW_ALLOCATOR_FUNCTIONS
#endif

// Override allocation functions by removing the std::bad_alloc throw and doing garbage collection.
#ifdef KRKRSDL2_OVERRIDE_NEW_ALLOCATOR_FUNCTIONS
void * operator new(std::size_t size) noexcept
{
	bool has_gced = false;

	if (size == 0)
		size = 1;
	void* p;
	while ((p = ::malloc(size)) == 0)
	{
		// If malloc fails, try to free up memory.
		if (!has_gced) {
			TVPDeliverCompactEvent(TVP_COMPACT_LEVEL_MAX);
			has_gced = true;
		}
		else
			break;
	}
	return p;
}

void* operator new(size_t size, const std::nothrow_t&) noexcept
{
	void* p = 0;
	p = ::operator new(size);
	return p;
}

void* operator new[](size_t size) noexcept
{
	return ::operator new(size);
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
	void* p = 0;
	p = ::operator new[](size);
	return p;
}

void operator delete(void* ptr) noexcept
{
	::free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
	::operator delete(ptr);
}

void operator delete(void* ptr, size_t) noexcept
{
	::operator delete(ptr);
}

void operator delete[] (void* ptr) noexcept
{
	::operator delete(ptr);
}

void operator delete[] (void* ptr, const std::nothrow_t&) noexcept
{
	::operator delete[](ptr);
}

void operator delete[] (void* ptr, size_t) noexcept
{
	::operator delete[](ptr);
}

#ifdef __cpp_aligned_new
void * operator new(std::size_t size, std::align_val_t alignment) noexcept
{
	bool has_gced = false;

	if (size == 0)
		size = 1;
	if (static_cast<size_t>(alignment) < sizeof(void*))
		alignment = std::align_val_t(sizeof(void*));
	void* p;
	while (::posix_memalign(&p, static_cast<size_t>(alignment), size) != 0)
	{
		// If posix_memalign fails, try to free up memory.
		if (!has_gced) {
			TVPDeliverCompactEvent(TVP_COMPACT_LEVEL_MAX);
			has_gced = true;
		}
		else {
			p = nullptr; // posix_memalign doesn't initialize 'p' on failure
			break;
		}
	}
	return p;
}

void* operator new(size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
	void* p = 0;
	p = ::operator new(size, alignment);
	return p;
}

void* operator new[](size_t size, std::align_val_t alignment) noexcept
{
	return ::operator new(size, alignment);
}

void* operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
	void* p = 0;
	p = ::operator new[](size, alignment);
	return p;
}

void operator delete(void* ptr, std::align_val_t) noexcept
{
	::free(ptr);
}

void operator delete(void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
	::operator delete(ptr, alignment);
}

void operator delete(void* ptr, size_t, std::align_val_t alignment) noexcept
{
	::operator delete(ptr, alignment);
}

void operator delete[] (void* ptr, std::align_val_t alignment) noexcept
{
	::operator delete(ptr, alignment);
}

void operator delete[] (void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
	::operator delete[](ptr, alignment);
}

void operator delete[] (void* ptr, size_t, std::align_val_t alignment) noexcept
{
	::operator delete[](ptr, alignment);
}
#endif

#endif
