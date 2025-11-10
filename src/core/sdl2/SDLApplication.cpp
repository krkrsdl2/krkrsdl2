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
#ifdef _WIN32
#include <shellapi.h>
#include <stdlib.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#endif

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

#if defined(__linux__)
// By specification of SDL_RenderPresent, the backbuffer should be
// considered invalidated after each call. This is required for
// some renderers to be enabled.
#define KRKRSDL2_RENDERER_FULL_UPDATES
#endif

extern void TVPLoadMessage();

class TVPWindowWindow;
static TVPWindowWindow *_lastWindowWindow, *_currentWindowWindow;
static SDL_GameController **sdl_controllers = nullptr;
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
	// For some reason, invalid pointers get set in SDL's controller subsystem which causes segfault
	{
		return;
	}
#endif
	if (!SDL_WasInit(SDL_INIT_GAMECONTROLLER))
	{
		SDL_Init(SDL_INIT_GAMECONTROLLER);
	}
	if (sdl_controller_num && sdl_controllers)
	{
		for (int i = 0; i < sdl_controller_num; i += 1)
		{
			if (sdl_controllers[i])
			{
				SDL_GameControllerClose(sdl_controllers[i]);
				sdl_controllers[i] = nullptr;
			}
		}
		sdl_controller_num = 0;
		SDL_free(sdl_controllers);
		sdl_controllers = nullptr;
	}
	sdl_controller_num = SDL_NumJoysticks();
	if (sdl_controller_num)
	{
		sdl_controllers = (SDL_GameController**)SDL_malloc(sizeof(SDL_GameController*) * sdl_controller_num);
		if (!sdl_controllers)
		{
			sdl_controller_num = 0;
			TVPAddLog(ttstr("Could not allocate memory to store SDL controller information"));
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
	switch (key)
	{
		case VK_PAD1: return SDL_CONTROLLER_BUTTON_A;
		case VK_PAD2: return SDL_CONTROLLER_BUTTON_B;
		case VK_PAD3: return SDL_CONTROLLER_BUTTON_X;
		case VK_PAD4: return SDL_CONTROLLER_BUTTON_Y;
		case VK_PAD7: return SDL_CONTROLLER_BUTTON_BACK;
		case VK_PAD8: return SDL_CONTROLLER_BUTTON_START;
		case VK_PAD9: return SDL_CONTROLLER_BUTTON_LEFTSTICK;
		case VK_PAD10: return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
		case VK_PAD5: return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
		case VK_PAD6: return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
		case VK_PADUP: return SDL_CONTROLLER_BUTTON_DPAD_UP;
		case VK_PADDOWN: return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
		case VK_PADLEFT: return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
		case VK_PADRIGHT: return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
		default: return 0;
	}
}

static tjs_uint sdl_gamecontrollerbutton_to_vk_key(Uint8 key)
{
	switch (key)
	{
		case SDL_CONTROLLER_BUTTON_A: return VK_PAD1;
		case SDL_CONTROLLER_BUTTON_B: return VK_PAD2;
		case SDL_CONTROLLER_BUTTON_X: return VK_PAD3;
		case SDL_CONTROLLER_BUTTON_Y: return VK_PAD4;
		case SDL_CONTROLLER_BUTTON_BACK: return VK_PAD7;
		case SDL_CONTROLLER_BUTTON_START: return VK_PAD8;
		case SDL_CONTROLLER_BUTTON_LEFTSTICK: return VK_PAD9;
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return VK_PAD10;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return VK_PAD5;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return VK_PAD6;
		case SDL_CONTROLLER_BUTTON_DPAD_UP: return VK_PADUP;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return VK_PADDOWN;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return VK_PADLEFT;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return VK_PADRIGHT;
		default: return 0;
	}
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

static SDL_Cursor *sdl_system_cursors[SDL_NUM_SYSTEM_CURSORS] = {0};

static SDL_Keycode vk_key_to_sdl_key(tjs_uint key)
{
	// This is generated using regex find replace
	switch (key)
	{
		case VK_BACK: return SDLK_BACKSPACE;
		case VK_TAB: return SDLK_TAB;
		case VK_CLEAR: return SDLK_CLEAR;
		case VK_RETURN: return SDLK_RETURN;
		case VK_PAUSE: return SDLK_PAUSE;
		case VK_CAPITAL: return SDLK_CAPSLOCK;
		case VK_ESCAPE: return SDLK_ESCAPE;
		case VK_SPACE: return SDLK_SPACE;
		case VK_PRIOR: return SDLK_PAGEUP;
		case VK_NEXT: return SDLK_PAGEDOWN;
		case VK_END: return SDLK_END;
		case VK_HOME: return SDLK_HOME;
		case VK_LEFT: return SDLK_LEFT;
		case VK_UP: return SDLK_UP;
		case VK_RIGHT: return SDLK_RIGHT;
		case VK_DOWN: return SDLK_DOWN;
		case VK_SELECT: return SDLK_SELECT;
		case VK_EXECUTE: return SDLK_EXECUTE;
		case VK_SNAPSHOT: return SDLK_PRINTSCREEN;
		case VK_INSERT: return SDLK_INSERT;
		case VK_DELETE: return SDLK_DELETE;
		case VK_HELP: return SDLK_HELP;
		case VK_0: return SDLK_0;
		case VK_1: return SDLK_1;
		case VK_2: return SDLK_2;
		case VK_3: return SDLK_3;
		case VK_4: return SDLK_4;
		case VK_5: return SDLK_5;
		case VK_6: return SDLK_6;
		case VK_7: return SDLK_7;
		case VK_8: return SDLK_8;
		case VK_9: return SDLK_9;
		case VK_A: return SDLK_a;
		case VK_B: return SDLK_b;
		case VK_C: return SDLK_c;
		case VK_D: return SDLK_d;
		case VK_E: return SDLK_e;
		case VK_F: return SDLK_f;
		case VK_G: return SDLK_g;
		case VK_H: return SDLK_h;
		case VK_I: return SDLK_i;
		case VK_J: return SDLK_j;
		case VK_K: return SDLK_k;
		case VK_L: return SDLK_l;
		case VK_M: return SDLK_m;
		case VK_N: return SDLK_n;
		case VK_O: return SDLK_o;
		case VK_P: return SDLK_p;
		case VK_Q: return SDLK_q;
		case VK_R: return SDLK_r;
		case VK_S: return SDLK_s;
		case VK_T: return SDLK_t;
		case VK_U: return SDLK_u;
		case VK_V: return SDLK_v;
		case VK_W: return SDLK_w;
		case VK_X: return SDLK_x;
		case VK_Y: return SDLK_y;
		case VK_Z: return SDLK_z;
		case VK_LWIN: return SDLK_LGUI;
		case VK_RWIN: return SDLK_RGUI;
		case VK_SLEEP: return SDLK_SLEEP;
		case VK_NUMPAD0: return SDLK_KP_0;
		case VK_NUMPAD1: return SDLK_KP_1;
		case VK_NUMPAD2: return SDLK_KP_2;
		case VK_NUMPAD3: return SDLK_KP_3;
		case VK_NUMPAD4: return SDLK_KP_4;
		case VK_NUMPAD5: return SDLK_KP_5;
		case VK_NUMPAD6: return SDLK_KP_6;
		case VK_NUMPAD7: return SDLK_KP_7;
		case VK_NUMPAD8: return SDLK_KP_8;
		case VK_NUMPAD9: return SDLK_KP_9;
		case VK_MULTIPLY: return SDLK_KP_MULTIPLY;
		case VK_ADD: return SDLK_KP_PLUS;
		case VK_SUBTRACT: return SDLK_KP_MINUS;
		case VK_DECIMAL: return SDLK_KP_PERIOD;
		case VK_DIVIDE: return SDLK_KP_DIVIDE;
		case VK_F1: return SDLK_F1;
		case VK_F2: return SDLK_F2;
		case VK_F3: return SDLK_F3;
		case VK_F4: return SDLK_F4;
		case VK_F5: return SDLK_F5;
		case VK_F6: return SDLK_F6;
		case VK_F7: return SDLK_F7;
		case VK_F8: return SDLK_F8;
		case VK_F9: return SDLK_F9;
		case VK_F10: return SDLK_F10;
		case VK_F11: return SDLK_F11;
		case VK_F12: return SDLK_F12;
		case VK_F13: return SDLK_F13;
		case VK_F14: return SDLK_F14;
		case VK_F15: return SDLK_F15;
		case VK_F16: return SDLK_F16;
		case VK_F17: return SDLK_F17;
		case VK_F18: return SDLK_F18;
		case VK_F19: return SDLK_F19;
		case VK_F20: return SDLK_F20;
		case VK_F21: return SDLK_F21;
		case VK_F22: return SDLK_F22;
		case VK_F23: return SDLK_F23;
		case VK_F24: return SDLK_F24;
		case VK_NUMLOCK: return SDLK_NUMLOCKCLEAR;
		case VK_SCROLL: return SDLK_SCROLLLOCK;
		case VK_LSHIFT: return SDLK_LSHIFT;
		case VK_RSHIFT: return SDLK_RSHIFT;
		case VK_LCONTROL: return SDLK_LCTRL;
		case VK_RCONTROL: return SDLK_RCTRL;
		case VK_LMENU: return SDLK_LALT;
		case VK_RMENU: return SDLK_RALT;
		case VK_BROWSER_BACK: return SDLK_AC_BACK;
		case VK_BROWSER_FORWARD: return SDLK_AC_FORWARD;
		case VK_BROWSER_REFRESH: return SDLK_AC_REFRESH;
		case VK_BROWSER_STOP: return SDLK_AC_STOP;
		case VK_BROWSER_SEARCH: return SDLK_AC_SEARCH;
		case VK_BROWSER_FAVORITES: return SDLK_AC_BOOKMARKS;
		case VK_BROWSER_HOME: return SDLK_AC_HOME;
		case VK_VOLUME_MUTE: return SDLK_MUTE;
		case VK_VOLUME_DOWN: return SDLK_VOLUMEDOWN;
		case VK_VOLUME_UP: return SDLK_VOLUMEUP;
		case VK_MEDIA_NEXT_TRACK: return SDLK_AUDIONEXT;
		case VK_MEDIA_PREV_TRACK: return SDLK_AUDIOPREV;
		case VK_MEDIA_STOP: return SDLK_AUDIOSTOP;
		case VK_MEDIA_PLAY_PAUSE: return SDLK_AUDIOPLAY;
		case VK_LAUNCH_MAIL: return SDLK_MAIL;
		case VK_LAUNCH_MEDIA_SELECT: return SDLK_MEDIASELECT;
		default: return 0;
	}
}

static tjs_uint sdl_key_to_vk_key(SDL_Keycode key)
{
	// This is generated using regex find replace
	switch (key)
	{
		case SDLK_BACKSPACE: return VK_BACK;
		case SDLK_TAB: return VK_TAB;
		case SDLK_CLEAR: return VK_CLEAR;
		case SDLK_RETURN: return VK_RETURN;
		case SDLK_PAUSE: return VK_PAUSE;
		case SDLK_CAPSLOCK: return VK_CAPITAL;
		case SDLK_ESCAPE: return VK_ESCAPE;
		case SDLK_SPACE: return VK_SPACE;
		case SDLK_PAGEUP: return VK_PRIOR;
		case SDLK_PAGEDOWN: return VK_NEXT;
		case SDLK_END: return VK_END;
		case SDLK_HOME: return VK_HOME;
		case SDLK_LEFT: return VK_LEFT;
		case SDLK_UP: return VK_UP;
		case SDLK_RIGHT: return VK_RIGHT;
		case SDLK_DOWN: return VK_DOWN;
		case SDLK_SELECT: return VK_SELECT;
		case SDLK_EXECUTE: return VK_EXECUTE;
		case SDLK_PRINTSCREEN: return VK_SNAPSHOT;
		case SDLK_INSERT: return VK_INSERT;
		case SDLK_DELETE: return VK_DELETE;
		case SDLK_HELP: return VK_HELP;
		case SDLK_0: return VK_0;
		case SDLK_1: return VK_1;
		case SDLK_2: return VK_2;
		case SDLK_3: return VK_3;
		case SDLK_4: return VK_4;
		case SDLK_5: return VK_5;
		case SDLK_6: return VK_6;
		case SDLK_7: return VK_7;
		case SDLK_8: return VK_8;
		case SDLK_9: return VK_9;
		case SDLK_a: return VK_A;
		case SDLK_b: return VK_B;
		case SDLK_c: return VK_C;
		case SDLK_d: return VK_D;
		case SDLK_e: return VK_E;
		case SDLK_f: return VK_F;
		case SDLK_g: return VK_G;
		case SDLK_h: return VK_H;
		case SDLK_i: return VK_I;
		case SDLK_j: return VK_J;
		case SDLK_k: return VK_K;
		case SDLK_l: return VK_L;
		case SDLK_m: return VK_M;
		case SDLK_n: return VK_N;
		case SDLK_o: return VK_O;
		case SDLK_p: return VK_P;
		case SDLK_q: return VK_Q;
		case SDLK_r: return VK_R;
		case SDLK_s: return VK_S;
		case SDLK_t: return VK_T;
		case SDLK_u: return VK_U;
		case SDLK_v: return VK_V;
		case SDLK_w: return VK_W;
		case SDLK_x: return VK_X;
		case SDLK_y: return VK_Y;
		case SDLK_z: return VK_Z;
		case SDLK_LGUI: return VK_LWIN;
		case SDLK_RGUI: return VK_RWIN;
		case SDLK_SLEEP: return VK_SLEEP;
		case SDLK_KP_0: return VK_NUMPAD0;
		case SDLK_KP_1: return VK_NUMPAD1;
		case SDLK_KP_2: return VK_NUMPAD2;
		case SDLK_KP_3: return VK_NUMPAD3;
		case SDLK_KP_4: return VK_NUMPAD4;
		case SDLK_KP_5: return VK_NUMPAD5;
		case SDLK_KP_6: return VK_NUMPAD6;
		case SDLK_KP_7: return VK_NUMPAD7;
		case SDLK_KP_8: return VK_NUMPAD8;
		case SDLK_KP_9: return VK_NUMPAD9;
		case SDLK_KP_MULTIPLY: return VK_MULTIPLY;
		case SDLK_KP_PLUS: return VK_ADD;
		case SDLK_KP_MINUS: return VK_SUBTRACT;
		case SDLK_KP_PERIOD: return VK_DECIMAL;
		case SDLK_KP_DIVIDE: return VK_DIVIDE;
		case SDLK_F1: return VK_F1;
		case SDLK_F2: return VK_F2;
		case SDLK_F3: return VK_F3;
		case SDLK_F4: return VK_F4;
		case SDLK_F5: return VK_F5;
		case SDLK_F6: return VK_F6;
		case SDLK_F7: return VK_F7;
		case SDLK_F8: return VK_F8;
		case SDLK_F9: return VK_F9;
		case SDLK_F10: return VK_F10;
		case SDLK_F11: return VK_F11;
		case SDLK_F12: return VK_F12;
		case SDLK_F13: return VK_F13;
		case SDLK_F14: return VK_F14;
		case SDLK_F15: return VK_F15;
		case SDLK_F16: return VK_F16;
		case SDLK_F17: return VK_F17;
		case SDLK_F18: return VK_F18;
		case SDLK_F19: return VK_F19;
		case SDLK_F20: return VK_F20;
		case SDLK_F21: return VK_F21;
		case SDLK_F22: return VK_F22;
		case SDLK_F23: return VK_F23;
		case SDLK_F24: return VK_F24;
		case SDLK_NUMLOCKCLEAR: return VK_NUMLOCK;
		case SDLK_SCROLLLOCK: return VK_SCROLL;
		case SDLK_LSHIFT: return VK_LSHIFT;
		case SDLK_RSHIFT: return VK_RSHIFT;
		case SDLK_LCTRL: return VK_LCONTROL;
		case SDLK_RCTRL: return VK_RCONTROL;
		case SDLK_MENU: return VK_LMENU;
		case SDLK_LALT: return VK_LMENU;
		case SDLK_RALT: return VK_RMENU;
		case SDLK_AC_BACK: return VK_BROWSER_BACK;
		case SDLK_AC_FORWARD: return VK_BROWSER_FORWARD;
		case SDLK_AC_REFRESH: return VK_BROWSER_REFRESH;
		case SDLK_AC_STOP: return VK_BROWSER_STOP;
		case SDLK_AC_SEARCH: return VK_BROWSER_SEARCH;
		case SDLK_AC_BOOKMARKS: return VK_BROWSER_FAVORITES;
		case SDLK_AC_HOME: return VK_BROWSER_HOME;
		case SDLK_MUTE: return VK_VOLUME_MUTE;
		case SDLK_VOLUMEDOWN: return VK_VOLUME_DOWN;
		case SDLK_VOLUMEUP: return VK_VOLUME_UP;
		case SDLK_AUDIONEXT: return VK_MEDIA_NEXT_TRACK;
		case SDLK_AUDIOPREV: return VK_MEDIA_PREV_TRACK;
		case SDLK_AUDIOSTOP: return VK_MEDIA_STOP;
		case SDLK_AUDIOPLAY: return VK_MEDIA_PLAY_PAUSE;
		case SDLK_MAIL: return VK_LAUNCH_MAIL;
		case SDLK_MEDIASELECT: return VK_LAUNCH_MEDIA_SELECT;
		default: return 0;
	}
}

static int GetShiftState()
{
	int s = 0;
	if (TVPGetAsyncKeyState(VK_MENU)) s |= MK_ALT;
	if (TVPGetAsyncKeyState(VK_LMENU)) s |= MK_ALT;
	if (TVPGetAsyncKeyState(VK_RMENU)) s |= MK_ALT;
	if (TVPGetAsyncKeyState(VK_SHIFT)) s |= MK_SHIFT;
	if (TVPGetAsyncKeyState(VK_LSHIFT)) s |= MK_SHIFT;
	if (TVPGetAsyncKeyState(VK_RCONTROL)) s |= MK_SHIFT;
	if (TVPGetAsyncKeyState(VK_CONTROL)) s |= MK_CONTROL;
	if (TVPGetAsyncKeyState(VK_LCONTROL)) s |= MK_CONTROL;
	if (TVPGetAsyncKeyState(VK_RCONTROL)) s |= MK_CONTROL;
	return s;
}
static int GetMouseButtonState()
{
	int s = 0;
	if (TVPGetAsyncKeyState(VK_LBUTTON)) s |= ssLeft;
	if (TVPGetAsyncKeyState(VK_RBUTTON)) s |= ssRight;
	if (TVPGetAsyncKeyState(VK_MBUTTON)) s |= ssMiddle;
	if (TVPGetAsyncKeyState(VK_XBUTTON1)) s |= ssX1;
	if (TVPGetAsyncKeyState(VK_XBUTTON2)) s |= ssX2;
	return s;
}

#ifdef _WIN32
struct tTVPMessageReceiverRecord
{
	tTVPWindowMessageReceiver Proc;
	const void *UserData;
	bool Deliver(tTVPWindowMessage *Message)
	{
		return Proc(const_cast<void*>(UserData), Message);
	}
};
#endif

class TVPWindowWindow : public TTVPWindowForm
{
protected:
	SDL_Window *window;

	TVPWindowWindow *_prevWindow;
	TVPWindowWindow *_nextWindow;
	SDL_Texture *texture;
	SDL_Renderer *renderer;
	SDL_Surface *surface;
#ifdef KRKRZ_ENABLE_CANVAS
	SDL_GLContext context;
#endif
	tTJSNI_Window *TJSNativeInstance;
	bool hasDrawn = false;
	bool isVisible = true;
	bool visibilityHasInitialized = false;
	bool needsGraphicUpdate = false;
	bool isBeingDeleted = false;
	bool cursorTemporaryHidden = false;
	char *imeCompositionStr;
	size_t imeCompositionCursor;
	size_t imeCompositionLen;
	size_t imeCompositionSelection;
	SDL_Rect attentionPointRect;
	iTJSDispatch2 *fileDropArray;
	tjs_int fileDropArrayCount;
	TVPSDLBitmapCompletion *bitmapCompletion;
#ifdef KRKRZ_ENABLE_CANVAS
	tTVPOpenGLScreen *openGlScreen;
#endif
	int lastMouseX;
	int lastMouseY;

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
	bool Closing = false;
	bool ProgramClosing = false;
	bool CanCloseWork = false;
	bool in_mode_ = false; // is modal
	int modal_result_ = 0;
	enum CloseAction
	{
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
	virtual void RegisterWindowMessageReceiver(tTVPWMRRegMode mode, void *proc, const void *userdata) override;
	bool InternalDeliverMessageToReceiver(tTVPWindowMessage &msg);
	virtual HWND GetHandle() const override;
#endif
	bool should_try_parent_window(SDL_Event event);
	void window_receive_event(SDL_Event event);
	bool window_receive_event_input(SDL_Event event);
};

TVPWindowWindow::TVPWindowWindow(tTJSNI_Window *w)
{
	this->window = nullptr;
	this->ResetImeMode();
	this->fileDropArray = nullptr;
	this->fileDropArrayCount = 0;
	this->lastMouseX = 0;
	this->lastMouseY = 0;
	this->_nextWindow = nullptr;
	this->_prevWindow = _lastWindowWindow;
	_lastWindowWindow = this;
	if (this->_prevWindow)
	{
		this->_prevWindow->_nextWindow = this;
	}
	if (!_currentWindowWindow)
	{
		_currentWindowWindow = this;
	}
	this->TJSNativeInstance = w;

	if (!SDL_WasInit(SDL_INIT_VIDEO))
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
	if (!TVPIsEnableDrawDevice())
	{
#ifdef SDL_HINT_OPENGL_ES_DRIVER
		SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
#endif
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

	this->window = SDL_CreateWindow("krkrsdl2", new_window_x, new_window_y, new_window_w, new_window_h, window_flags);
	if (!this->window)
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
	this->context = nullptr;
	if (!TVPIsEnableDrawDevice())
	{
		this->context = SDL_GL_CreateContext(this->window);
		if (!this->context)
		{
			TVPThrowExceptionMessage(TJS_W("Cannot create SDL context: %1"), ttstr(SDL_GetError()));
		}
		SDL_GL_MakeCurrent(this->window, this->context);
	}
#endif
	this->renderer = nullptr;
	this->bitmapCompletion = nullptr;
#ifdef KRKRZ_ENABLE_CANVAS
	this->openGlScreen = nullptr;
#endif
	this->surface = nullptr;
#ifdef KRKRZ_ENABLE_CANVAS
	if (TVPIsEnableDrawDevice())
#endif
	{
#if !defined(__EMSCRIPTEN__) || (defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__))
		this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (!this->renderer)
		{
			TVPAddLog(ttstr("Cannot create SDL renderer: ") + ttstr(SDL_GetError()));
		}
#endif

#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
		// move the event watch to after the SDL_RendererEventWatch to ensure transformed values are received
		SDL_DelEventWatch(sdl_event_watch, nullptr);
		SDL_AddEventWatch(sdl_event_watch, nullptr);
#endif

		this->bitmapCompletion = new TVPSDLBitmapCompletion();
		if (!this->renderer)
		{
			this->surface = SDL_GetWindowSurface(this->window);
			if (!this->surface)
			{
				TVPAddLog(ttstr("Cannot get surface from SDL window: ") + ttstr(SDL_GetError()));
			}
			this->bitmapCompletion->surface = this->surface;
		}
		if (!this->renderer && !this->surface)
		{
			TVPThrowExceptionMessage(TJS_W("Cannot get surface or renderer from SDL window"));
		}
		this->texture = nullptr;
		if (this->renderer)
		{
			SDL_SetRenderDrawColor(this->renderer, 0x00, 0x00, 0x00, 0xFF);
		}
	}
#ifdef _WIN32
	::SetWindowLongPtr(this->GetHandle(), GWLP_USERDATA, (LONG_PTR)this);
#endif
	Application->AddWindow(this);
}

TVPWindowWindow::~TVPWindowWindow()
{
	if (_lastWindowWindow == this)
	{
		_lastWindowWindow = this->_prevWindow;
	}
	if (this->_nextWindow)
	{
		this->_nextWindow->_prevWindow = this->_prevWindow;
	}
	if (this->_prevWindow)
	{
		this->_prevWindow->_nextWindow = this->_nextWindow;
	}
	if (_currentWindowWindow == this)
	{
		_currentWindowWindow = _lastWindowWindow;
	}
	if (this->bitmapCompletion)
	{
		delete this->bitmapCompletion;
		this->bitmapCompletion = nullptr;
	}
#ifdef KRKRZ_ENABLE_CANVAS
	if (this->context)
	{
		SDL_GL_DeleteContext(this->context);
		this->context = nullptr;
	}
#endif
	if (this->texture && this->surface)
	{
		SDL_DestroyTexture(this->texture);
		this->texture = nullptr;
		SDL_FreeSurface(this->surface);
		this->surface = nullptr;
	}
	if (this->renderer)
	{
		SDL_DestroyRenderer(this->renderer);
		this->renderer = nullptr;
	}
	if (this->window)
	{
		SDL_DestroyWindow(this->window);
		this->window = nullptr;
	}

#ifdef _WIN32
	tjs_int count = this->WindowMessageReceivers.GetCount();
	for (tjs_int i = 0; i < count; i += 1)
	{
		tTVPMessageReceiverRecord *item = this->WindowMessageReceivers[i];
		if (!item)
		{
			continue;
		}
		delete item;
		this->WindowMessageReceivers.Remove(i);
	}
#endif

	Application->RemoveWindow(this);
}

void TVPWindowWindow::SetPaintBoxSize(tjs_int w, tjs_int h)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	this->LayerWidth = w;
	this->LayerHeight = h;
#endif
	if (this->renderer)
	{
		if (this->texture)
		{
			SDL_DestroyTexture(this->texture);
			this->texture = nullptr;
		}
		this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, w, h);
		if (!this->texture)
		{
			TVPThrowExceptionMessage(TJS_W("Cannot create texture texture: %1"), ttstr(SDL_GetError()));
		}
		this->bitmapCompletion->surface = nullptr;
		if (this->surface)
		{
			SDL_FreeSurface(this->surface);
			this->surface = nullptr;
		}
		this->surface = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0);
		if (!this->surface)
		{
			TVPThrowExceptionMessage(TJS_W("Cannot create surface: %1"), ttstr(SDL_GetError()));
		}
		this->bitmapCompletion->surface = this->surface;
	}
#ifndef KRKRSDL2_ENABLE_ZOOM
	SDL_Rect cliprect;
	cliprect.x = 0;
	cliprect.y = 0;
	cliprect.w = w;
	cliprect.h = h;
#endif
	if (this->renderer)
	{
#ifdef KRKRSDL2_ENABLE_ZOOM
		this->UpdateActualZoom();
#else
		SDL_RenderSetLogicalSize(this->renderer, w, h);
#endif
	}
	if (this->TJSNativeInstance)
	{
		tTVPRect r;
		r.left = 0;
		r.top = 0;
		r.right = w;
		r.bottom = h;
		this->TJSNativeInstance->NotifyWindowExposureToLayer(r);
		this->TJSNativeInstance->GetDrawDevice()->SetClipRectangle(r);
		this->TJSNativeInstance->GetDrawDevice()->SetDestRectangle(r);
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
	if (this->context)
	{
		return;
	}
#endif
	x -= this->LastSentDrawDeviceDestRect.left;
	y -= this->LastSentDrawDeviceDestRect.top;
	x = MulDiv(x, this->GetInnerWidth(), this->LastSentDrawDeviceDestRect.get_width());
	y = MulDiv(y, this->GetInnerHeight(), this->LastSentDrawDeviceDestRect.get_height());
#endif
}

void TVPWindowWindow::TranslateDrawAreaToWindow(int &x, int &y)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
#ifdef KRKRZ_ENABLE_CANVAS
	if (this->context)
	{
		return;
	}
#endif
	x = MulDiv(x, this->LastSentDrawDeviceDestRect.get_width(), this->GetInnerWidth());
	y = MulDiv(y, this->LastSentDrawDeviceDestRect.get_height(), this->GetInnerHeight());
	x += this->LastSentDrawDeviceDestRect.left;
	y += this->LastSentDrawDeviceDestRect.top;
#endif
}

bool TVPWindowWindow::GetFormEnabled()
{
	return (!!this->window && !!(SDL_GetWindowFlags(this->window) & SDL_WINDOW_SHOWN));
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
	this->cursorTemporaryHidden = (mcs == mcsTempHidden);
	SDL_ShowCursor((mcs == mcsVisible) ? SDL_ENABLE : SDL_DISABLE);
}
tTVPMouseCursorState TVPWindowWindow::GetMouseCursorState() const
{
	return this->cursorTemporaryHidden ? mcsTempHidden : ((SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE) ? mcsVisible : mcsHidden);
}
void TVPWindowWindow::HideMouseCursor()
{
	this->SetMouseCursorState(mcsTempHidden);
}
void TVPWindowWindow::RestoreMouseCursor()
{
	if (this->cursorTemporaryHidden)
	{
		this->SetMouseCursorState(mcsVisible);
	}
}
void TVPWindowWindow::GetCursorPos(tjs_int &x, tjs_int &y)
{
	x = this->lastMouseX;
	y = this->lastMouseY;
	if (this->window != SDL_GetMouseFocus())
	{
		return;
	}
	tjs_int new_x = 0;
	tjs_int new_y = 0;
	SDL_GetMouseState(&new_x, &new_y);
	if (this->renderer)
	{
#ifdef KRKRSDL2_ENABLE_ZOOM
		this->TranslateWindowToDrawArea(new_x, new_y);
#else
		float scale_x, scale_y;
		SDL_Rect viewport;
		int window_w, window_h;
		int output_w, output_h;
		SDL_RenderGetScale(this->renderer, &scale_x, &scale_y);
		SDL_RenderGetViewport(this->renderer, &viewport);
		SDL_GetWindowSize(this->window, &window_w, &window_h);
		SDL_GetRendererOutputSize(this->renderer, &output_w, &output_h);
		float dpi_scale_x = (float)window_w / output_w;
		float dpi_scale_y = (float)window_h / output_h;
		new_x -= (int)(viewport.x * dpi_scale_x);
		new_y -= (int)(viewport.y * dpi_scale_y);
		new_x = (int)(new_x / (scale_x * dpi_scale_x));
		new_y = (int)(new_y / (scale_x * dpi_scale_y));
#endif
	}
	x = new_x;
	y = new_y;
}
void TVPWindowWindow::SetCursorPos(tjs_int x, tjs_int y)
{
	this->RestoreMouseCursor();
	if (!this->window)
	{
		return;
	}
	tjs_int new_x = x;
	tjs_int new_y = y;

	if (this->renderer)
	{
#ifdef KRKRSDL2_ENABLE_ZOOM
		this->TranslateDrawAreaToWindow(new_x, new_y);
#else
		float scale_x, scale_y;
		SDL_Rect viewport;
		int window_w, window_h;
		int output_w, output_h;
		SDL_RenderGetScale(this->renderer, &scale_x, &scale_y);
		SDL_RenderGetViewport(this->renderer, &viewport);
		SDL_GetWindowSize(this->window, &window_w, &window_h);
		SDL_GetRendererOutputSize(this->renderer, &output_w, &output_h);
		float dpi_scale_x = (float)window_w / output_w;
		float dpi_scale_y = (float)window_h / output_h;
		new_x = (int)(new_x * (scale_x * dpi_scale_x));
		new_y = (int)(new_y * (scale_x * dpi_scale_y));
		new_x += (int)(viewport.x * dpi_scale_x);
		new_y += (int)(viewport.y * dpi_scale_y);
#endif
	}
	SDL_WarpMouseInWindow(this->window, new_x, new_y);
}
void TVPWindowWindow::SetAttentionPoint(tjs_int left, tjs_int top, const struct tTVPFont *font)
{
	if (!font)
	{
		return;
	}
	this->attentionPointRect.x = left;
	this->attentionPointRect.y = top;
	this->attentionPointRect.w = 0;
	this->attentionPointRect.h = font->Height;
	this->TranslateDrawAreaToWindow(this->attentionPointRect.x, this->attentionPointRect.y);
	SDL_SetTextInputRect(&this->attentionPointRect);
}
void TVPWindowWindow::BringToFront()
{
	if (_currentWindowWindow != this)
	{
		if (_currentWindowWindow)
		{
			_currentWindowWindow->TJSNativeInstance->OnReleaseCapture();
		}
		_currentWindowWindow = this;
	}
	if (this->window)
	{
		SDL_RaiseWindow(this->window);
	}
}
void TVPWindowWindow::ShowWindowAsModal()
{
#if defined(KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE)
	TVPThrowExceptionMessage(TJS_W("Showing window as modal is not supported"));
#else
	this->in_mode_ = true;
	this->BringToFront();
	this->modal_result_ = 0;
	while (this == _currentWindowWindow && !this->modal_result_)
	{
		process_events();
		if (::Application->IsTarminate())
		{
			this->modal_result_ = mrCancel;
		}
		else if (this->modal_result_)
		{
			break;
		}
	}
	this->in_mode_ = false;
#endif
}
bool TVPWindowWindow::GetVisible()
{
	return (!this->visibilityHasInitialized) ? this->isVisible : (!!this->window && !!(SDL_GetWindowFlags(this->window) & SDL_WINDOW_SHOWN));
}
void TVPWindowWindow::SetVisible(bool visible)
{
	this->isVisible = visible;
	if (!this->visibilityHasInitialized)
	{
		return;
	}
	if (this->window)
	{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
		if (visible)
		{
			SDL_ShowWindow(this->window);
		}
		else
		{
			SDL_HideWindow(this->window);
		}
#endif
	}
	if (visible)
	{
		this->BringToFront();
	}
	else if (_currentWindowWindow == this)
	{
		_currentWindowWindow = this->_prevWindow ? this->_prevWindow : this->_nextWindow;
		if (_currentWindowWindow)
		{
			_currentWindowWindow->BringToFront();
		}
	}
}
void TVPWindowWindow::SetFullScreenMode(bool fullscreen)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_SetWindowFullscreen(this->window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	}
	this->UpdateWindow(utNormal);
#endif
}
bool TVPWindowWindow::GetFullScreenMode()
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	return !!this->window && !!(SDL_GetWindowFlags(this->window) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP));
#else
	return false;
#endif
}
void TVPWindowWindow::SetBorderStyle(tTVPBorderStyle bs)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_SetWindowBordered(this->window, (bs == bsNone) ? SDL_FALSE : SDL_TRUE);
		SDL_SetWindowResizable(this->window, (bs == bsSizeable || bs == bsSizeToolWin) ? SDL_TRUE : SDL_FALSE);
	}
#endif
}
tTVPBorderStyle TVPWindowWindow::GetBorderStyle() const
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		Uint32 flags = SDL_GetWindowFlags(this->window);
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
	std::string v_utf8 = this->window ? SDL_GetWindowTitle(this->window) : "";
	tjs_string v_utf16;
	TVPUtf8ToUtf16( v_utf16, v_utf8 );
	return v_utf16;
}
void TVPWindowWindow::GetCaption(tjs_string & v) const
{
	v.clear();
	std::string v_utf8 = this->window ? SDL_GetWindowTitle(this->window) : "";
	TVPUtf8ToUtf16( v, v_utf8 );
}
void TVPWindowWindow::SetCaption(const tjs_string & v)
{
	if (!this->window)
	{
		return;
	}
	std::string v_utf8;
	if (!TVPUtf16ToUtf8(v_utf8, v))
	{
		return;
	}
	SDL_SetWindowTitle(this->window, v_utf8.c_str());
}
void TVPWindowWindow::SetWidth(tjs_int w)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		int h;
		SDL_GetWindowSize(this->window, nullptr, &h);
		SDL_SetWindowSize(this->window, w, h);
		if (!this->renderer && this->surface)
		{
			this->bitmapCompletion->surface = nullptr;
			this->surface = SDL_GetWindowSurface(this->window);
			if (!this->surface)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get surface from SDL window: %1"), ttstr(SDL_GetError()));
			}
			this->bitmapCompletion->surface = this->surface;
		}
	}
#endif
#ifdef KRKRSDL2_ENABLE_ZOOM
	this->UpdateActualZoom();
#else
	this->UpdateWindow(utNormal);
#endif
}
void TVPWindowWindow::SetHeight(tjs_int h)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		int w;
		SDL_GetWindowSize(this->window, &w, nullptr);
		SDL_SetWindowSize(this->window, w, h);
		if (!this->renderer && this->surface)
		{
			this->bitmapCompletion->surface = nullptr;
			this->surface = SDL_GetWindowSurface(this->window);
			if (!this->surface)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get surface from SDL window: %1"), ttstr(SDL_GetError()));
			}
			this->bitmapCompletion->surface = this->surface;
		}
	}
#endif
#ifdef KRKRSDL2_ENABLE_ZOOM
	this->UpdateActualZoom();
#else
	this->UpdateWindow(utNormal);
#endif
}
void TVPWindowWindow::SetSize(tjs_int w, tjs_int h)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_SetWindowSize(this->window, w, h);
		if (!this->renderer && this->surface)
		{
			this->bitmapCompletion->surface = nullptr;
			this->surface = SDL_GetWindowSurface(this->window);
			if (!this->surface)
			{
				TVPThrowExceptionMessage(TJS_W("Cannot get surface from SDL window: %1"), ttstr(SDL_GetError()));
			}
			this->bitmapCompletion->surface = this->surface;
		}
	}
#endif
#ifdef KRKRSDL2_ENABLE_ZOOM
	this->UpdateActualZoom();
#else
	this->UpdateWindow(utNormal);
#endif
}
void TVPWindowWindow::GetSize(tjs_int &w, tjs_int &h)
{
	w = 0;
	h = 0;
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_GetWindowSize(this->window, &w, &h);
		return;
	}
#endif
	if (this->renderer)
	{
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(this->renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(this->renderer, &w, &h);
#endif
		return;
	}
}
tjs_int TVPWindowWindow::GetWidth() const
{
	int w = 0;
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_GetWindowSize(this->window, &w, nullptr);
		return w;
	}
#endif
	if (this->renderer)
	{
		int h;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(this->renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(this->renderer, &w, &h);
#endif
	}
	return w;
}
tjs_int TVPWindowWindow::GetHeight() const
{
	int h = 0;
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_GetWindowSize(this->window, nullptr, &h);
		return h;
	}
#endif
	if (this->renderer)
	{
		int w;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(this->renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(this->renderer, &w, &h);
#endif
	}
	return h;
}
void TVPWindowWindow::SetMinWidth(tjs_int w)
{
	this->SetMinSize(w, this->GetMinHeight());
}
void TVPWindowWindow::SetMaxWidth(tjs_int w)
{
	this->SetMaxSize(w, this->GetMaxHeight());
}
void TVPWindowWindow::SetMinHeight(tjs_int h)
{
	this->SetMinSize(this->GetMinWidth(), h);
}
void TVPWindowWindow::SetMaxHeight(tjs_int h)
{
	this->SetMaxSize(this->GetMaxWidth(), h);
}
void TVPWindowWindow::SetMinSize(tjs_int w, tjs_int h)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_SetWindowMinimumSize(this->window, w, h);
	}
#endif
}
void TVPWindowWindow::SetMaxSize(tjs_int w, tjs_int h)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_SetWindowMaximumSize(this->window, w, h);
	}
#endif
}
tjs_int TVPWindowWindow::GetMinWidth()
{
	int w = 0;
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_GetWindowMinimumSize(this->window, &w, nullptr);
		return w;
	}
#endif
	if (this->renderer)
	{
		int h;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(this->renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(this->renderer, &w, &h);
#endif
	}
	return w;
}
tjs_int TVPWindowWindow::GetMaxWidth()
{
	int w = 0;
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_GetWindowMaximumSize(this->window, &w, nullptr);
		return w;
	}
#endif
	if (this->renderer)
	{
		int h;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(this->renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(this->renderer, &w, &h);
#endif
	}
	return w;
}
tjs_int TVPWindowWindow::GetMinHeight()
{
	int h = 0;
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_GetWindowMinimumSize(this->window, &h, nullptr);
		return h;
	}
#endif
	if (this->renderer)
	{
		int w;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(this->renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(this->renderer, &w, &h);
#endif
	}
	return h;
}
tjs_int TVPWindowWindow::GetMaxHeight()
{
	int h = 0;
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_GetWindowMaximumSize(this->window, &h, nullptr);
		return h;
	}
#endif
	if (this->renderer)
	{
		int w;
#ifdef KRKRSDL2_ENABLE_ZOOM
		SDL_GetRendererOutputSize(this->renderer, &w, &h);
#else
		SDL_RenderGetLogicalSize(this->renderer, &w, &h);
#endif
	}
	return h;
}
tjs_int TVPWindowWindow::GetLeft()
{
	int x = 0;
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_GetWindowPosition(this->window, &x, nullptr);
	}
#endif
	return x;
}
void TVPWindowWindow::SetLeft(tjs_int l)
{
	this->SetPosition(l, this->GetTop());
}
tjs_int TVPWindowWindow::GetTop()
{
	int y = 0;
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_GetWindowPosition(this->window, nullptr, &y);
	}
#endif
	return y;
}
void TVPWindowWindow::SetTop(tjs_int t)
{
	this->SetPosition(this->GetLeft(), t);
}
void TVPWindowWindow::SetPosition(tjs_int l, tjs_int t)
{
#ifndef KRKRSDL2_WINDOW_SIZE_IS_LAYER_SIZE
	if (this->window)
	{
		SDL_SetWindowPosition(this->window, l, t);
	}
#endif
}
TVPSDLBitmapCompletion *TVPWindowWindow::GetTVPSDLBitmapCompletion()
{
	this->needsGraphicUpdate = true;
	return this->bitmapCompletion;
}
#ifdef KRKRZ_ENABLE_CANVAS
void TVPWindowWindow::SetOpenGLScreen(tTVPOpenGLScreen *s)
{
	this->openGlScreen = s;
}
void TVPWindowWindow::SetSwapInterval(int interval)
{
	if (this->context)
	{
		SDL_GL_SetSwapInterval(interval);
	}
}
void TVPWindowWindow::GetDrawableSize(tjs_int &w, tjs_int &h)
{
	w = 0;
	h = 0;
	if (this->context)
	{
		SDL_GL_GetDrawableSize(this->window, &w, &h);
	}
}
void TVPWindowWindow::Swap()
{
	if (this->context)
	{
		SDL_GL_SwapWindow(this->window);
	}
}
#endif
void TVPWindowWindow::Show()
{
}
void TVPWindowWindow::TickBeat()
{
	if (!this->visibilityHasInitialized)
	{
		this->visibilityHasInitialized = true;
		this->SetVisible(this->isVisible);
	}
	if (this->needsGraphicUpdate)
	{
		if (this->bitmapCompletion)
		{
			SDL_Rect rect;
			rect.x = this->bitmapCompletion->update_rect.left;
			rect.y = this->bitmapCompletion->update_rect.top;
			rect.w = this->bitmapCompletion->update_rect.get_width();
			rect.h = this->bitmapCompletion->update_rect.get_height();
			if (this->renderer)
			{
#if defined(KRKRSDL2_ENABLE_ZOOM) || defined(KRKRSDL2_RENDERER_FULL_UPDATES)
				SDL_RenderFillRect(this->renderer, nullptr);
#else
				SDL_Rect logical_rect;
				SDL_RenderGetLogicalSize(this->renderer, &(logical_rect.w), &(logical_rect.h));
				if (logical_rect.w == rect.w && logical_rect.h == rect.h)
				{
					// Clear extra artifacts
					SDL_RenderSetLogicalSize(this->renderer, 0, 0);
					SDL_RenderFillRect(this->renderer, nullptr);
					SDL_RenderSetLogicalSize(this->renderer, logical_rect.w, logical_rect.h);
				}
#endif
				if (this->texture)
				{
					if (this->surface)
					{
						if ((rect.w + rect.x) > this->surface->w)
						{
							rect.w = this->surface->w;
						}
						if ((rect.h + rect.y) > this->surface->h)
						{
							rect.h = this->surface->h;
						}
						SDL_UpdateTexture(this->texture, &rect, this->surface->pixels, this->surface->pitch);
					}
#if defined(KRKRSDL2_ENABLE_ZOOM)
					SDL_Rect destrect;
					destrect.x = this->LastSentDrawDeviceDestRect.left;
					destrect.y = this->LastSentDrawDeviceDestRect.top;
					destrect.w = this->LastSentDrawDeviceDestRect.get_width();
					destrect.h = this->LastSentDrawDeviceDestRect.get_height();
					SDL_Rect srcrect;
					srcrect.x = 0;
					srcrect.y = 0;
					srcrect.w = this->GetInnerWidth();
					srcrect.h = this->GetInnerHeight();
					SDL_RenderCopy(this->renderer, this->texture, &srcrect, &destrect);
#elif defined(KRKRSDL2_RENDERER_FULL_UPDATES)
					SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
#else
					SDL_RenderCopy(this->renderer, this->texture, &rect, &rect);
#endif
				}
				SDL_RenderPresent(this->renderer);
#if !defined(KRKRSDL2_ENABLE_ZOOM) && !defined(KRKRSDL2_RENDERER_FULL_UPDATES)
				if (logical_rect.w == rect.w && logical_rect.h == rect.h)
				{
					// Clear extra artifacts (for the back buffer)
					SDL_RenderSetLogicalSize(this->renderer, 0, 0);
					SDL_RenderFillRect(this->renderer, nullptr);
					SDL_RenderSetLogicalSize(this->renderer, logical_rect.w, logical_rect.h);
				}
				if (this->texture)
				{
					SDL_RenderCopy(this->renderer, this->texture, &rect, &rect);
				}
#endif
				this->hasDrawn = true;
			}
			else if (this->window && this->surface)
			{
				SDL_UpdateWindowSurfaceRects(this->window, &rect, 1);
				this->hasDrawn = true;
			}
			this->needsGraphicUpdate = false;
		}
	}
#ifdef KRKRZ_ENABLE_CANVAS
	else if (this->context && this->TJSNativeInstance)
	{
		this->TJSNativeInstance->StartDrawing();
		this->hasDrawn = true;
	}
#endif
}
void TVPWindowWindow::InvalidateClose()
{
	this->TJSNativeInstance = nullptr;
	this->SetVisible(false);
	delete this;
}
bool TVPWindowWindow::GetWindowActive()
{
	return _currentWindowWindow == this && SDL_GetWindowFlags(this->window) & SDL_WINDOW_INPUT_FOCUS;
}
void TVPWindowWindow::OnClose(CloseAction& action)
{
	action = (!this->modal_result_) ? caNone : caHide;

	if (!this->ProgramClosing || !this->TJSNativeInstance)
	{
		return;
	}
	if (!this->TJSNativeInstance->IsMainWindow())
	{
		action = caFree;
	}
	iTJSDispatch2 *obj = this->TJSNativeInstance->GetOwnerNoAddRef();
	this->TJSNativeInstance->NotifyWindowClose();
	obj->Invalidate(0, nullptr, nullptr, obj);
	this->TJSNativeInstance = nullptr;
	this->SetVisible(false);
}
bool TVPWindowWindow::OnCloseQuery()
{
	// closing actions are 3 patterns;
	// 1. closing action by the user
	// 2. "close" method
	// 3. object invalidation

	if (TVPGetBreathing())
	{
		return false;
	}

	// the default event handler will invalidate this object when an onCloseQuery
	// event reaches the handler.
	if (!this->TJSNativeInstance)
	{
		return true;
	}
	if (this->modal_result_ && this->modal_result_ != mrCancel/* mrCancel=when close button is pushed in modal window */)
	{
		return true;
	}
	iTJSDispatch2 *obj = this->TJSNativeInstance->GetOwnerNoAddRef();
	if (!obj)
	{
		return true;
	}
	tTJSVariant arg[1] = { true };
	static ttstr eventname(TJS_W("onCloseQuery"));

	if (!this->ProgramClosing)
	{
		// close action does not happen immediately
		if (this->TJSNativeInstance)
		{
			TVPPostInputEvent(new tTVPOnCloseInputEvent(this->TJSNativeInstance));
		}

		this->Closing = true; // waiting closing...
	//	TVPSystemControl->NotifyCloseClicked();
		return false;
	}
	else
	{
		this->CanCloseWork = true;
		TVPPostEvent(obj, obj, eventname, 0, TVP_EPT_IMMEDIATE, sizeof(arg)/sizeof(arg[0]), arg);
		process_events(); // for post event
		// this event happens immediately
		// and does not return until done
		return this->CanCloseWork; // CanCloseWork is set by the event handler
	}
}
void TVPWindowWindow::Close()
{
	// closing action by "close" method
	if (this->Closing)
	{
		return; // already waiting closing...
	}

	this->ProgramClosing = true;
	try
	{
		//tTVPWindow::Close();
		if (this->in_mode_)
		{
			this->modal_result_ = mrCancel;
		}
		else if (this->OnCloseQuery())
		{
			CloseAction action = caFree;
			this->OnClose(action);
			switch (action)
			{
				case caNone:
					break;
				case caHide:
					this->SetVisible(false);
					break;
				case caMinimize:
					if (this->window)
					{
						SDL_MinimizeWindow(this->window);
					}
					break;
				case caFree:
				default:
					this->isBeingDeleted = true;
					//::PostMessage(this->GetHandle(), TVP_EV_WINDOW_RELEASE, 0, 0);
					break;
			}
		}
	}
	catch (...)
	{
		this->ProgramClosing = false;
		throw;
	}
	this->ProgramClosing = false;
}
void TVPWindowWindow::OnCloseQueryCalled(bool b)
{
	// closing is allowed by onCloseQuery event handler
	if (!this->ProgramClosing)
	{
		// closing action by the user
		if (b)
		{
			if (this->in_mode_)
			{
				this->modal_result_ = 1; // when modal
			}
			else
			{
				this->SetVisible(false);  // just hide
			}

			this->Closing = false;
			if (this->TJSNativeInstance)
			{
				if (this->TJSNativeInstance->IsMainWindow())
				{
					// this is the main window
					iTJSDispatch2 *obj = this->TJSNativeInstance->GetOwnerNoAddRef();
					obj->Invalidate(0, nullptr, nullptr, obj);
				}
			}
			else
			{
				delete this;
			}
		}
		else
		{
			this->Closing = false;
		}
	}
	else
	{
		// closing action by the program
		this->CanCloseWork = b;
	}
}
void TVPWindowWindow::SetImeMode(tTVPImeMode mode)
{
	if (!this->window || mode == ::imDisable || mode == ::imClose)
	{
		this->ResetImeMode();
	}
	else if (!SDL_IsTextInputActive())
	{
		SDL_SetTextInputRect(&this->attentionPointRect);
		SDL_StartTextInput();
	}
}
void TVPWindowWindow::ResetImeMode()
{
	this->imeCompositionStr = nullptr;
	this->imeCompositionLen = 0;
	this->imeCompositionCursor = 0;
	this->imeCompositionSelection = 0;
	this->attentionPointRect.x = 0;
	this->attentionPointRect.y = 0;
	this->attentionPointRect.w = 0;
	this->attentionPointRect.h = 0;
	if (this->window && SDL_IsTextInputActive())
	{
		SDL_SetTextInputRect(&this->attentionPointRect);
		SDL_StopTextInput();
	}
}
void TVPWindowWindow::UpdateWindow(tTVPUpdateType type)
{
	if (!this->TJSNativeInstance)
	{
		return;
	}
	tTVPRect r;
#ifdef KRKRSDL2_ENABLE_ZOOM
	r.left = 0;
	r.top = 0;
	r.right = this->LayerWidth;
	r.bottom = this->LayerHeight;
#else
	r.clear();
	if (this->renderer)
	{
		SDL_RenderGetLogicalSize(this->renderer, &(r.right), &(r.bottom));
		SDL_RenderSetLogicalSize(this->renderer, r.right, r.bottom);
	}
	else if (this->window)
	{
		SDL_GetWindowSize(this->window, &(r.right), &(r.bottom));
	}
#endif
	this->TJSNativeInstance->NotifyWindowExposureToLayer(r);
	TVPDeliverWindowUpdateEvents();
}
void TVPWindowWindow::InternalKeyDown(tjs_uint16 key, tjs_uint32 shift)
{
	TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, key, shift));
}
void TVPWindowWindow::OnKeyUp(tjs_uint16 vk, int shift)
{
	TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, vk, shift));
}
void TVPWindowWindow::OnKeyPress(tjs_uint16 vk, int repeat, bool prevkeystate, bool convertkey)
{
	TVPPostInputEvent(new tTVPOnKeyPressInputEvent(this->TJSNativeInstance, vk));
}

#ifdef KRKRSDL2_ENABLE_ZOOM
//---------------------------------------------------------------------------
//! @brief	do reduction for numer over denom
static void TVPDoReductionNumerAndDenom(tjs_int &n, tjs_int &d)
{
	tjs_int a = n;
	tjs_int b = d;
	while (b)
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
	if (!this->renderer)
	{
		return;
	}
#ifdef KRKRZ_ENABLE_CANVAS
	if (this->context)
	{
		return;
	}
#endif
	// determine fullscreen zoom factor and client size
	int sb_w, sb_h, zoom_d, zoom_n, output_w, output_h;
	SDL_GetRendererOutputSize(this->renderer, &output_w, &output_h);

	float layer_aspect = (float)this->GetInnerWidth() / this->GetInnerHeight();
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
		viewport.w = this->GetInnerWidth();
		viewport.h = this->GetInnerHeight();
	}
	else if (layer_aspect > output_aspect)
	{
		if (scale_policy == 1)
		{
			// Crop left and right
			zoom_n = output_h;
			zoom_d = this->GetInnerHeight();
			TVPDoReductionNumerAndDenom(zoom_n, zoom_d);
			viewport.y = 0;
			viewport.h = output_h;
			viewport.w = MulDiv(this->GetInnerWidth(), zoom_n, zoom_d);
			viewport.x = (output_w - viewport.w) / 2;
		}
		else
		{
			// Top and bottom black bars (letterbox)
			zoom_n = output_w;
			zoom_d = this->GetInnerWidth();
			TVPDoReductionNumerAndDenom(zoom_n, zoom_d);
			viewport.x = 0;
			viewport.w = output_w;
			viewport.h = MulDiv(this->GetInnerHeight(), zoom_n, zoom_d);
			viewport.y = (output_h - viewport.h) / 2;
		}
	}
	else
	{
		if (scale_policy == 1)
		{
			// Crop top and bottom
			zoom_n = output_w;
			zoom_d = this->GetInnerWidth();
			TVPDoReductionNumerAndDenom(zoom_n, zoom_d);
			viewport.x = 0;
			viewport.w = output_w;
			viewport.h = MulDiv(this->GetInnerHeight(), zoom_n, zoom_d);
			viewport.y = (output_h - viewport.h) / 2;
		}
		else
		{
			// Left and right black bars (letterbox)
			zoom_n = output_h;
			zoom_d = this->GetInnerHeight();
			TVPDoReductionNumerAndDenom(zoom_n, zoom_d);
			viewport.y = 0;
			viewport.h = output_h;
			viewport.w = MulDiv(this->GetInnerWidth(), zoom_n, zoom_d);
			viewport.x = (output_w - viewport.w) / 2;
		}
	}
	this->FullScreenDestRect.set_size(viewport.w, viewport.h);
	this->FullScreenDestRect.set_offsets(viewport.x, viewport.y);

	this->ActualZoomNumer = zoom_n;
	this->ActualZoomDenom = zoom_d;
	this->SetDrawDeviceDestRect();
#endif
}

void TVPWindowWindow::SetDrawDeviceDestRect(void)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	tTVPRect destrect;
	tjs_int w = MulDiv(this->GetInnerWidth(),  this->ActualZoomNumer, this->ActualZoomDenom);
	tjs_int h = MulDiv(this->GetInnerHeight(), this->ActualZoomNumer, this->ActualZoomDenom);
	if (w < 1)
	{
		w = 1;
	}
	if (h < 1)
	{
		h = 1;
	}
	{
		destrect.left = this->FullScreenDestRect.left;
		destrect.top = this->FullScreenDestRect.top;
		destrect.right = destrect.left + w;
		destrect.bottom = destrect.top + h;
	}

	if (this->LastSentDrawDeviceDestRect != destrect)
	{
		this->LastSentDrawDeviceDestRect = destrect;
		this->UpdateWindow(utNormal);
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
	if (set_logical)
	{
		if (this->ZoomNumer != numer || this->ZoomDenom != denom)
		{
			ischanged = true;
		}
		this->ZoomNumer = numer;
		this->ZoomDenom = denom;
	}
	this->UpdateActualZoom();
#endif
}

void TVPWindowWindow::SetZoomNumer(tjs_int n)
{
	this->SetZoom(n, this->GetZoomDenom());
}

tjs_int TVPWindowWindow::GetZoomNumer() const
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	return this->ZoomNumer;
#else
	return 1;
#endif
}

void TVPWindowWindow::SetZoomDenom(tjs_int d)
{
	this->SetZoom(this->GetZoomNumer(), d);
}

tjs_int TVPWindowWindow::GetZoomDenom() const
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	return this->ZoomDenom;
#else
	return 1;
#endif
}

void TVPWindowWindow::SetInnerWidth(tjs_int v)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	this->SetInnerSize(v, this->GetInnerHeight());
#else
	this->SetWidth(v);
#endif
}

void TVPWindowWindow::SetInnerHeight(tjs_int v)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	this->SetInnerSize(this->GetInnerWidth(), v);
#else
	this->SetHeight(v);
#endif
}

void TVPWindowWindow::SetInnerSize(tjs_int w, tjs_int h)
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	this->InnerWidth = w;
	this->InnerHeight = h;
	this->UpdateActualZoom();
#endif
	this->SetSize(w, h);
}

tjs_int TVPWindowWindow::GetInnerWidth()
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	return this->InnerWidth;
#else
	return this->GetWidth();
#endif
}

tjs_int TVPWindowWindow::GetInnerHeight()
{
#ifdef KRKRSDL2_ENABLE_ZOOM
	return this->InnerHeight;
#else
	return this->GetHeight();
#endif
}

#ifdef _WIN32
void TVPWindowWindow::RegisterWindowMessageReceiver(tTVPWMRRegMode mode, void *proc, const void *userdata)
{
	switch (mode)
	{
		case wrmRegister:
		case wrmUnregister:
		{
			tjs_int count = this->WindowMessageReceivers.GetCount();
			tjs_int i;
			for (i = 0; i < count; i += 1)
			{
				tTVPMessageReceiverRecord *item = this->WindowMessageReceivers[i];
				if (!item)
				{
					continue;
				}
				if ((void*)item->Proc == proc)
				{
					if (mode == wrmRegister)
					{
						break; // have already registered
					}
					if (mode == wrmUnregister)
					{
						// found
						this->WindowMessageReceivers.Remove(i);
						delete item;
					}
				}
			}
			if (mode == wrmRegister)
			{
				if (i == count)
				{
					// not have registered
					tTVPMessageReceiverRecord *item = new tTVPMessageReceiverRecord();
					item->Proc = (tTVPWindowMessageReceiver)proc;
					item->UserData = userdata;
					this->WindowMessageReceivers.Add(item);
				}
			}
			if (mode == wrmUnregister)
			{
				this->WindowMessageReceivers.Compact();
			}
			break;
		}
		default:
			break;
	}
}

bool TVPWindowWindow::InternalDeliverMessageToReceiver(tTVPWindowMessage &msg)
{
	if (!this->WindowMessageReceivers.GetCount() || !this->TJSNativeInstance)
	{
		return false;
	}
#ifdef KRKRSDL2_ENABLE_PLUGINS
	if (TVPPluginUnloadedAtSystemExit)
	{
		return false;
	}
#endif

	tObjectListSafeLockHolder<tTVPMessageReceiverRecord> holder(this->WindowMessageReceivers);
	tjs_int count = this->WindowMessageReceivers.GetSafeLockedObjectCount();

	bool block = false;
	for (tjs_int i = 0; i < count; i += 1)
	{
		tTVPMessageReceiverRecord *item = this->WindowMessageReceivers.GetSafeLockedObjectAt(i);
		if (!item)
		{
			continue;
		}
		bool b = item->Deliver(&msg);
		block = block || b;
	}
	return block;
}

HWND TVPWindowWindow::GetHandle() const
{
	SDL_SysWMinfo syswminfo;
	SDL_VERSION(&syswminfo.version);
	return SDL_GetWindowWMInfo(this->window, &syswminfo) ? syswminfo.info.win.window : nullptr;
}
#endif

bool TVPWindowWindow::should_try_parent_window(SDL_Event event)
{
	bool tryParentWindow = false;
	if (this->window && this->_prevWindow)
	{
		uint32_t windowID = SDL_GetWindowID(this->window);
		switch (event.type)
		{
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
				break;
		}
	}
	return tryParentWindow;
}

void TVPWindowWindow::window_receive_event(SDL_Event event)
{
	if (this->isBeingDeleted)
	{
		delete this;
		return;
	}
	if (this->should_try_parent_window(event))
	{
		if (!this->in_mode_)
		{
			this->_prevWindow->window_receive_event(event);
		}
		return;
	}
	if (this->window && this->hasDrawn)
	{
		tjs_uint32 s = TVP_TShiftState_To_uint32(GetShiftState());
		s |= GetMouseButtonState();
		if (this->TJSNativeInstance->CanDeliverEvents())
		{
			switch (event.type)
			{
				case SDL_TEXTINPUT:
				case SDL_TEXTEDITING:
				{
					if (!SDL_IsTextInputActive())
					{
						return;
					}
					// TODO: figure out vertical edit
					for (size_t i = 0; i < this->imeCompositionSelection; i += 1)
					{
						TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, VK_LEFT, TVP_SS_SHIFT));
						TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, VK_LEFT, TVP_SS_SHIFT));
#if 0
						TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, VK_DOWN, TVP_SS_SHIFT));
						TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, VK_DOWN, TVP_SS_SHIFT));
#endif
					}
					for (size_t i = 0; i < this->imeCompositionLen - this->imeCompositionCursor; i += 1)
					{
						TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, VK_RIGHT, 0));
						TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, VK_RIGHT, 0));
#if 0
						TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, VK_DOWN, 0));
						TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, VK_DOWN, 0));
#endif
					}
					for (size_t i = 0; i < this->imeCompositionLen; i += 1)
					{
						TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, VK_BACK, 0));
						TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, VK_BACK, 0));
					}
					switch (event.type)
					{
						case SDL_TEXTINPUT:
							this->imeCompositionStr = event.text.text;
							this->imeCompositionCursor = 0;
							this->imeCompositionSelection = 0;
							break;
						case SDL_TEXTEDITING:
							this->imeCompositionStr = event.edit.text;
							this->imeCompositionCursor = event.edit.start;
							this->imeCompositionSelection = event.edit.length;
							break;
					}
					size_t buffer_len = TVPUtf8ToWideCharString((const char*)(this->imeCompositionStr), nullptr);
					if (buffer_len == (size_t)-1)
					{
						return;
					}
					if (buffer_len)
					{
						this->imeCompositionLen = buffer_len;
						tjs_char *buffer = new tjs_char[buffer_len + 1];
						TVPUtf8ToWideCharString((const char*)(this->imeCompositionStr), buffer);
						for (size_t i = 0; i < buffer_len; i += 1)
						{
							TVPPostInputEvent(new tTVPOnKeyPressInputEvent(this->TJSNativeInstance, buffer[i]));
						}
						delete[] buffer;
					}
					else
					{
						this->imeCompositionStr = nullptr;
						this->imeCompositionLen = 0;
						this->imeCompositionCursor = 0;
						this->imeCompositionSelection = 0;
					}
					if (event.type == SDL_TEXTEDITING)
					{
						for (size_t i = 0; i < this->imeCompositionLen - this->imeCompositionCursor; i += 1)
						{
							TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, VK_LEFT, 0));
							TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, VK_LEFT, 0));
#if 0
							TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, VK_UP, 0));
							TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, VK_UP, 0));
#endif
						}
						for (size_t i = 0; i < this->imeCompositionSelection; i += 1)
						{
							TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, VK_RIGHT, TVP_SS_SHIFT));
							TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, VK_RIGHT, TVP_SS_SHIFT));
#if 0
							TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, VK_DOWN, TVP_SS_SHIFT));
							TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, VK_DOWN, TVP_SS_SHIFT));
#endif
						}
					}
					if (event.type == SDL_TEXTINPUT)
					{
						this->imeCompositionStr = nullptr;
						this->imeCompositionLen = 0;
					}
					return;
				}
				case SDL_DROPBEGIN:
				{
					if (!this->fileDropArray)
					{
						this->fileDropArray = TJSCreateArrayObject();
					}
					return;
				}
				case SDL_DROPCOMPLETE:
				{
					if (this->fileDropArray)
					{
						tTJSVariant arg(this->fileDropArray, this->fileDropArray);
						TVPPostInputEvent(new tTVPOnFileDropInputEvent(this->TJSNativeInstance, arg));
						this->fileDropArray->Release();
						this->fileDropArray = nullptr;
						this->fileDropArrayCount = 0;
					}
					return;
				}
				case SDL_DROPFILE:
				case SDL_DROPTEXT:
				{
					if (event.drop.file)
					{
						std::string f_utf8 = event.drop.file;
						tjs_string f_utf16;
						TVPUtf8ToUtf16( f_utf16, f_utf8 );
						SDL_free(event.drop.file);
						if (TVPIsExistentStorageNoSearch(f_utf16))
						{
							tTJSVariant val = TVPNormalizeStorageName(ttstr(f_utf16));
							if (this->fileDropArray)
							{
								this->fileDropArray->PropSetByNum(TJS_MEMBERENSURE|TJS_IGNOREPROP, this->fileDropArrayCount, &val, this->fileDropArray);
								this->fileDropArrayCount += 1;
							}
							else
							{
								iTJSDispatch2 *file_drop_array_single = TJSCreateArrayObject();
								file_drop_array_single->PropSetByNum(TJS_MEMBERENSURE|TJS_IGNOREPROP, 0, &val, file_drop_array_single);
								{
									tTJSVariant arg(file_drop_array_single, file_drop_array_single);
									TVPPostInputEvent(new tTVPOnFileDropInputEvent(this->TJSNativeInstance, arg));
								}
								file_drop_array_single->Release();
							}
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
				case SDL_WINDOWEVENT:
				{
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_EXPOSED:
						{
							this->UpdateWindow(utNormal);
							return;
						}
						case SDL_WINDOWEVENT_MINIMIZED:
						case SDL_WINDOWEVENT_MAXIMIZED:
						case SDL_WINDOWEVENT_RESTORED:
						case SDL_WINDOWEVENT_RESIZED:
						case SDL_WINDOWEVENT_SIZE_CHANGED:
						{
#ifdef KRKRSDL2_ENABLE_ZOOM
							this->UpdateActualZoom();
#else
							this->UpdateWindow(utNormal);
#endif
							TVPPostInputEvent(new tTVPOnResizeInputEvent(this->TJSNativeInstance), TVP_EPT_REMOVE_POST);
							return;
						}
						case SDL_WINDOWEVENT_ENTER:
						{
							TVPPostInputEvent(new tTVPOnMouseEnterInputEvent(this->TJSNativeInstance));
							return;
						}
						case SDL_WINDOWEVENT_LEAVE:
						{
							TVPPostInputEvent(new tTVPOnMouseOutOfWindowInputEvent(this->TJSNativeInstance));
							TVPPostInputEvent(new tTVPOnMouseLeaveInputEvent(this->TJSNativeInstance));
							return;
						}
						case SDL_WINDOWEVENT_FOCUS_GAINED:
						case SDL_WINDOWEVENT_FOCUS_LOST:
						{
							TVPPostInputEvent(new tTVPOnWindowActivateEvent(this->TJSNativeInstance, event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED), TVP_EPT_REMOVE_POST);
							return;
						}
						case SDL_WINDOWEVENT_CLOSE:
						{
							TVPPostInputEvent(new tTVPOnCloseInputEvent(this->TJSNativeInstance));
							return;
						}
						default:
						{
							return;
						}
					}
				}
				case SDL_QUIT:
				{
					TVPPostInputEvent(new tTVPOnCloseInputEvent(this->TJSNativeInstance));
					return;
				}
				default:
				{
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
	if (this->isBeingDeleted)
	{
		delete this;
		return false;
	}
	if (this->should_try_parent_window(event))
	{
		if (!this->in_mode_)
		{
			return this->_prevWindow->window_receive_event_input(event);
		}
		return false;
	}
	if (this->window && this->hasDrawn)
	{
		tjs_uint32 s = TVP_TShiftState_To_uint32(GetShiftState());
		s |= GetMouseButtonState();
		if (this->TJSNativeInstance->CanDeliverEvents())
		{
			switch (event.type)
			{
				case SDL_MOUSEMOTION:
				{
					this->RestoreMouseCursor();
					this->lastMouseX = event.motion.x;
					this->lastMouseY = event.motion.y;
					this->TranslateWindowToDrawArea(this->lastMouseX, this->lastMouseY);
					TVPPostInputEvent(new tTVPOnMouseMoveInputEvent(this->TJSNativeInstance, this->lastMouseX, this->lastMouseY, s));
					return true;
				}
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
				{
					if (SDL_IsTextInputActive() && this->imeCompositionStr)
					{
						return false;
					}
					tTVPMouseButton btn;
					bool hasbtn = true;
					switch (event.button.button)
					{
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
					if (hasbtn)
					{
						this->lastMouseX = event.button.x;
						this->lastMouseY = event.button.y;
						this->TranslateWindowToDrawArea(this->lastMouseX, this->lastMouseY);
						TVPPostInputEvent(new tTVPOnMouseMoveInputEvent(this->TJSNativeInstance, this->lastMouseX, this->lastMouseY, s));
						switch (event.type)
						{
							case SDL_MOUSEBUTTONDOWN:
								TVPPostInputEvent(new tTVPOnMouseDownInputEvent(this->TJSNativeInstance, this->lastMouseX, this->lastMouseY, btn, s));
								break;
							case SDL_MOUSEBUTTONUP:
								if (event.button.clicks >= 2)
								{
									TVPPostInputEvent(new tTVPOnDoubleClickInputEvent(this->TJSNativeInstance, this->lastMouseX, this->lastMouseY));
								}
								else
								{
									TVPPostInputEvent(new tTVPOnClickInputEvent(this->TJSNativeInstance, this->lastMouseX, this->lastMouseY));
								}
								TVPPostInputEvent(new tTVPOnMouseUpInputEvent(this->TJSNativeInstance, this->lastMouseX, this->lastMouseY, btn, s));
								break;
						}
						return true;
					}
					return false;
				}
				case SDL_MOUSEWHEEL:
				{
					this->TranslateWindowToDrawArea(this->lastMouseX, this->lastMouseY);
					TVPPostInputEvent(new tTVPOnMouseWheelInputEvent(this->TJSNativeInstance, event.wheel.x, event.wheel.y, this->lastMouseX, this->lastMouseY));
					return true;
				}
				case SDL_FINGERMOTION:
				{
					TVPPostInputEvent(new tTVPOnTouchMoveInputEvent(this->TJSNativeInstance, event.tfinger.x, event.tfinger.y, 1, 1, event.tfinger.fingerId));
					return true;
				}
				case SDL_FINGERDOWN:
				case SDL_FINGERUP:
				{
					switch (event.tfinger.type)
					{
						case SDL_FINGERDOWN:
							TVPPostInputEvent(new tTVPOnTouchDownInputEvent(this->TJSNativeInstance, event.tfinger.x, event.tfinger.y, 1, 1, event.tfinger.fingerId));
							break;
						case SDL_FINGERUP:
							TVPPostInputEvent(new tTVPOnTouchUpInputEvent(this->TJSNativeInstance, event.tfinger.x, event.tfinger.y, 1, 1, event.tfinger.fingerId));
							break;
					}
					return true;
				}
				case SDL_MULTIGESTURE:
				{
					TVPPostInputEvent(new tTVPOnTouchScalingInputEvent(this->TJSNativeInstance, 0, event.mgesture.dDist, event.mgesture.x, event.mgesture.y, 0));
					TVPPostInputEvent(new tTVPOnTouchRotateInputEvent(this->TJSNativeInstance, 0, event.mgesture.dTheta, event.mgesture.dDist, event.mgesture.x, event.mgesture.y, 0));
					return true;
				}
				case SDL_CONTROLLERBUTTONDOWN:
				case SDL_CONTROLLERBUTTONUP:
				{
					switch (event.cbutton.state)
					{
						case SDL_PRESSED:
							TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, sdl_gamecontrollerbutton_to_vk_key(event.cbutton.button), s));
							break;
						case SDL_RELEASED:
							if (!SDL_IsTextInputActive())
							{
								TVPPostInputEvent(new tTVPOnKeyPressInputEvent(this->TJSNativeInstance, sdl_gamecontrollerbutton_to_vk_key(event.cbutton.button)));
							}
							TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, sdl_gamecontrollerbutton_to_vk_key(event.cbutton.button), s));
							break;
					}
					return true;
				}
				case SDL_KEYDOWN:
				{
					if (SDL_IsTextInputActive())
					{
						if (this->imeCompositionStr)
						{
							return false;
						}
					}
					if (event.key.repeat)
					{
						s |= TVP_SS_REPEAT;
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
					TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, sdl_key_to_vk_key(event.key.keysym.sym), s));
					if (unified_vk_key)
					{
						TVPPostInputEvent(new tTVPOnKeyDownInputEvent(this->TJSNativeInstance, unified_vk_key, s));
					}
					SDL_SetTextInputRect(&this->attentionPointRect);
					return true;
				}
				case SDL_KEYUP:
				{
					if (SDL_IsTextInputActive())
					{
						if (this->imeCompositionStr)
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
						TVPPostInputEvent(new tTVPOnKeyPressInputEvent(this->TJSNativeInstance, sdl_key_to_vk_key(event.key.keysym.sym)));
						if (unified_vk_key)
						{
							TVPPostInputEvent(new tTVPOnKeyPressInputEvent(this->TJSNativeInstance, unified_vk_key));
						}
					}
					TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, sdl_key_to_vk_key(event.key.keysym.sym), s));
					if (unified_vk_key)
					{
						TVPPostInputEvent(new tTVPOnKeyUpInputEvent(this->TJSNativeInstance, unified_vk_key, s));
					}
					SDL_SetTextInputRect(&this->attentionPointRect);
					return true;
				}
				default:
				{
					return false;
				}
			}
		}
	}
	return false;
}

void sdl_process_events()
{
	if (!SDL_WasInit(SDL_INIT_EVENTS))
	{
		return;
	}
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
#else
		if (event.type == NativeEventQueueImplement::native_event_queue_custom_event_type)
		{
			((NativeEvent*)event.user.data2)->HandleEvent();
		}
		else
#endif
		if (_currentWindowWindow)
		{
			_currentWindowWindow->window_receive_event(event);
		}
		else if (event.type == SDL_QUIT)
		{
			Application->Terminate();
		}
	}
}

#ifdef _WIN32
static void sdl_windows_message_hook(void *userdata, void *hWnd, unsigned int message, Uint64 wParam, Sint64 lParam)
{
	TVPWindowWindow *win = reinterpret_cast<TVPWindowWindow*>(::GetWindowLongPtr((HWND)hWnd, GWLP_USERDATA));
	tTVPWindowMessage Message;
	Message.LParam = lParam;
	Message.WParam = wParam;
	Message.Msg = message;
	Message.Result = 0;
	if (win && win->InternalDeliverMessageToReceiver(Message))
	{
		// TODO: return Message.result and block
	}
}
#endif

#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
static int sdl_event_watch(void *userdata, SDL_Event *in_event)
{
	SDL_Event event;
	SDL_memcpy(&event, in_event, sizeof(SDL_Event));
#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
#else
	if (event.type == NativeEventQueueImplement::native_event_queue_custom_event_type)
	{
		return 1;
	}
#endif
	if (_currentWindowWindow && _currentWindowWindow->window_receive_event_input(event) && TVPSystemControl)
	{
		// process events now
		// Some JS functions will only work in e.g. mouse down callback due to browser restrictions
		TVPSystemControl->ApplicationIdle();
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
					TVPSystemControl = nullptr;
				}
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

void krkrsdl2_pre_init_platform(void)
{
	TVPTerminateCode = 0;
#ifdef _WIN32
	_set_error_mode(_OUT_TO_STDERR);
#endif
}

void krkrsdl2_set_args(int argc, tjs_char **argv)
{
	_argc = argc;
	_wargv = argv;
}

void krkrsdl2_convert_set_args(int argc, char **argv)
{
	_argc = argc;
	_wargv = new tjs_char*[argc];

	for (int i = 0; i < argc; i += 1)
	{
		const char *narg;
#if !defined(__EMSCRIPTEN__) && !defined(__vita__) && !defined(__SWITCH__) && !defined(_WIN32)
		if (!i)
		{
			narg = realpath(argv[i], nullptr);
		}
		else
#endif
		{
			narg = argv[i];
		}
		if (!narg)
		{
			tjs_char *warg_copy = new tjs_char[1];
			warg_copy[0] = '\0';
			_wargv[i] = warg_copy;
			continue;
		}
		std::string v_utf8 = narg;
		tjs_string v_utf16;
		TVPUtf8ToUtf16( v_utf16, v_utf8 );
#if !defined(__EMSCRIPTEN__) && !defined(__vita__) && !defined(__SWITCH__) && !defined(_WIN32)
		if (!i)
		{
			free((void*)narg);
		}
#endif
		tjs_char *warg_copy = new tjs_char[v_utf16.length() + 1];
		SDL_memcpy(warg_copy, v_utf16.c_str(), sizeof(tjs_char) * (v_utf16.length()));
		warg_copy[v_utf16.length()] = '\0';
		_wargv[i] = warg_copy;
	}
}

bool krkrsdl2_init_platform(void)
{
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
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHintWithPriority(SDL_HINT_IME_SHOW_UI, "1", SDL_HINT_DEFAULT);
#endif
#endif

#ifdef TVP_LOG_TO_COMMANDLINE_CONSOLE
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE);
#endif

	TVPLoadMessage();

#ifdef _WIN32
	SDL_SetWindowsMessageHook(sdl_windows_message_hook, nullptr);
#endif
#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
	SDL_AddEventWatch(sdl_event_watch, nullptr);
#endif
	::Application = new tTVPApplication();
	return !!::Application->StartApplication(_argc, _wargv);
}

void krkrsdl2_run_main_loop(void)
{
#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__)
	emscripten_set_main_loop(process_events, 0, 0);
#else
	while (process_events());
#endif
}

void krkrsdl2_cleanup(void)
{
	// delete application and exit forcely
	// this prevents ugly exception message on exit
	delete ::Application;
	::Application = nullptr;
}

bool TVPGetKeyMouseAsyncState(tjs_uint keycode, bool getcurrent)
{
	if (keycode >= VK_LBUTTON && keycode <= VK_XBUTTON2 && keycode != VK_CANCEL)
	{
		Uint32 state = SDL_GetMouseState(nullptr, nullptr);
		switch (keycode)
		{
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
	if (keycode >= VK_SHIFT && keycode <= VK_MENU)
	{
		Uint32 state = SDL_GetModState();
		switch (keycode)
		{
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
	const Uint8 *state = SDL_GetKeyboardState(nullptr);
	return !!(state[SDL_GetScancodeFromKey(vk_key_to_sdl_key(keycode))]);
}

bool TVPGetJoyPadAsyncState(tjs_uint keycode, bool getcurrent)
{
	bool is_pressed = false;
	if (sdl_controllers)
	{
		for (int i = 0; i < sdl_controller_num; i += 1)
		{
			if (sdl_controllers[i])
			{
				is_pressed = is_pressed || !!SDL_GameControllerGetButton(sdl_controllers[i], (SDL_GameControllerButton)vk_key_to_sdl_gamecontrollerbutton(keycode));
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
	if (TVPGetAsyncKeyState(VK_SHIFT)) f |= TVP_SS_SHIFT;
	if (TVPGetAsyncKeyState(VK_MENU)) f |= TVP_SS_ALT;
	if (TVPGetAsyncKeyState(VK_CONTROL)) f |= TVP_SS_CTRL;
	if (TVPGetAsyncKeyState(VK_LBUTTON)) f |= TVP_SS_LEFT;
	if (TVPGetAsyncKeyState(VK_RBUTTON)) f |= TVP_SS_RIGHT;
	if (TVPGetAsyncKeyState(VK_MBUTTON)) f |= TVP_SS_MIDDLE;
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
	if (state & MK_SHIFT) result |= ssShift;
	if (state & MK_CONTROL) result |= ssCtrl;
	if (state & MK_ALT) result |= ssAlt;
	return result;
}
TShiftState TVP_TShiftState_From_uint32(tjs_uint32 state)
{
	TShiftState result = 0;
	if (state & ssShift) result |= MK_SHIFT;
	if (state & ssCtrl) result |= MK_CONTROL;
	if (state & ssAlt) result |= MK_ALT;
	return result;
}

void TVPGetAllFontList(std::vector<tjs_string>& list) {}

const tjs_char *TVPGetDefaultFontName()
{
	if (!TVPGetCommandLine(TJS_W("-deffont"), nullptr))
	{
		TVPSetCommandLine(TJS_W("-deffont"), TJS_W("Noto Sans CJK JP"));
	}
	static tjs_int ArgumentGeneration = 0;
	if (ArgumentGeneration != TVPGetCommandLineArgumentGeneration())
	{
		ArgumentGeneration = TVPGetCommandLineArgumentGeneration();
		// Use that font, if specified on the command line
		tTJSVariant opt;
		if (TVPGetCommandLine(TJS_W("-deffont"), &opt))
		{
			ttstr str(opt);
			TVPDefaultFontName.AssignMessage(str.c_str());
		}
	}
	return TVPDefaultFontName;
}

void TVPSetDefaultFontName(const tjs_char *name)
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
	if (TVPDefaultFaceNames.IsEmpty())
	{
		TVPDefaultFaceNames = ttstr(TVPGetDefaultFontName());
	}
	return TVPDefaultFaceNames;
}

#if defined(__vita__)
#define KRKRSDL2_OVERRIDE_NEW_ALLOCATOR_FUNCTIONS
#endif

// Override allocation functions by removing the std::bad_alloc throw and doing garbage collection.
#ifdef KRKRSDL2_OVERRIDE_NEW_ALLOCATOR_FUNCTIONS
void *operator new(std::size_t size) noexcept
{
	bool has_gced = false;

	if (!size)
		size = 1;
	void *p;
	while ((p = ::malloc(size)) == nullptr)
	{
		// If malloc fails, try to free up memory.
		if (has_gced)
			break;
		else
		{
			TVPDeliverCompactEvent(TVP_COMPACT_LEVEL_MAX);
			has_gced = true;
		}
	}
	return p;
}

void *operator new(size_t size, const std::nothrow_t&) noexcept
{
	void *p = 0;
	p = ::operator new(size);
	return p;
}

void *operator new[](size_t size) noexcept
{
	return ::operator new(size);
}

void *operator new[](size_t size, const std::nothrow_t&) noexcept
{
	void *p = 0;
	p = ::operator new[](size);
	return p;
}

void operator delete(void *ptr) noexcept
{
	::free(ptr);
}

void operator delete(void *ptr, const std::nothrow_t&) noexcept
{
	::operator delete(ptr);
}

void operator delete(void *ptr, size_t) noexcept
{
	::operator delete(ptr);
}

void operator delete[] (void *ptr) noexcept
{
	::operator delete(ptr);
}

void operator delete[] (void *ptr, const std::nothrow_t&) noexcept
{
	::operator delete[](ptr);
}

void operator delete[] (void *ptr, size_t) noexcept
{
	::operator delete[](ptr);
}

#ifdef __cpp_aligned_new
void *operator new(std::size_t size, std::align_val_t alignment) noexcept
{
	bool has_gced = false;

	if (!size)
		size = 1;
	if (static_cast<size_t>(alignment) < sizeof(void*))
		alignment = std::align_val_t(sizeof(void*));
	void *p;
	while (::posix_memalign(&p, static_cast<size_t>(alignment), size))
	{
		// If posix_memalign fails, try to free up memory.
		if (has_gced)
		{
			p = nullptr; // posix_memalign doesn't initialize 'p' on failure
			break;
		}
		else
		{
			TVPDeliverCompactEvent(TVP_COMPACT_LEVEL_MAX);
			has_gced = true;
		}
	}
	return p;
}

void *operator new(size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
	void *p = nullptr;
	p = ::operator new(size, alignment);
	return p;
}

void *operator new[](size_t size, std::align_val_t alignment) noexcept
{
	return ::operator new(size, alignment);
}

void *operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
	void *p = nullptr;
	p = ::operator new[](size, alignment);
	return p;
}

void operator delete(void *ptr, std::align_val_t) noexcept
{
	::free(ptr);
}

void operator delete(void *ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
	::operator delete(ptr, alignment);
}

void operator delete(void *ptr, size_t, std::align_val_t alignment) noexcept
{
	::operator delete(ptr, alignment);
}

void operator delete[] (void *ptr, std::align_val_t alignment) noexcept
{
	::operator delete(ptr, alignment);
}

void operator delete[] (void *ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
	::operator delete[](ptr, alignment);
}

void operator delete[] (void *ptr, size_t, std::align_val_t alignment) noexcept
{
	::operator delete[](ptr, alignment);
}
#endif

#endif
