
#include "tjsCommHead.h"
#include "WindowImpl.h"
#ifndef _WIN32
#include "VirtualKey.h"
#endif
#include "Application.h"
#include "SystemImpl.h"
#include "TVPWindow.h"
#include "SysInitIntf.h"
#include <SDL.h>

#include <unistd.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


class TVPWindowLayer;
static TVPWindowLayer *_lastWindowLayer, *_currentWindowLayer;

void sdlProcessEvents();

#define MK_SHIFT 4
#define MK_CONTROL 8
#define MK_ALT (0x20)

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
	tTJSNI_Window *TJSNativeInstance;
	bool hasDrawn = false;
	bool isBeingDeleted = false;

public:
	TVPWindowLayer(tTJSNI_Window *w)
	{
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
		
		window = SDL_CreateWindow("krkrsdl2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		framebuffer = NULL;
		SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );
	}

	virtual ~TVPWindowLayer() {
		if (_lastWindowLayer == this) _lastWindowLayer = _prevWindow;
		if (_nextWindow) _nextWindow->_prevWindow = _prevWindow;
		if (_prevWindow) _prevWindow->_nextWindow = _nextWindow;
		if (_currentWindowLayer == this) {
			_currentWindowLayer = _lastWindowLayer;
		}
		SDL_DestroyTexture(framebuffer);
		framebuffer = NULL;
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
		SDL_DestroyWindow(window);
		window = NULL;
	}

	virtual void SetPaintBoxSize(tjs_int w, tjs_int h) override {
		if (framebuffer) {
			SDL_DestroyTexture(framebuffer);
			framebuffer = NULL;
		}
		framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, w, h);
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
	virtual const char *GetCaption() override {
		return SDL_GetWindowTitle(window);
	}
	virtual void SetCaption(const tjs_string & ws) override {
#if 0
		SDL_SetWindowTitle(window, ttstr(ws).AsNarrowStdString().c_str());
#endif
	}
	virtual void SetWidth(tjs_int w) override {
		int h;
		SDL_GetWindowSize(window, NULL, &h);
		SDL_SetWindowSize(window, w, h);
	}
	virtual void SetHeight(tjs_int h) override {
		int w;
		SDL_GetWindowSize(window, &w, NULL);
		SDL_SetWindowSize(window, w, h);
	}
	virtual void SetSize(tjs_int w, tjs_int h) override {
		SDL_SetWindowSize(window, w, h);
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
		if( framebuffer &&
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

			SDL_LockTexture(framebuffer, &dstrect, &TextureBuffer, &TexturePitch);
			for(; src_y < src_y_limit; src_y ++, dest_y ++)
			{
				const void *srcp = src_p + src_pitch * src_y + src_x * 4;
				void *destp = (tjs_uint8*)TextureBuffer + TexturePitch * dest_y + dest_x * 4;
				memcpy(destp, srcp, width_bytes);
			}
			SDL_UnlockTexture(framebuffer);
		}
	}
	virtual void Show() override {
		SDL_RenderFillRect(renderer, NULL);
		if (framebuffer)
		{
			SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
		}
		SDL_RenderPresent(renderer);
		hasDrawn = true;
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
	}
	virtual void ResetImeMode() override {
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
						if (event.key.repeat) s |= TVP_SS_REPEAT;
						TVPPostInputEvent(new tTVPOnKeyDownInputEvent(TJSNativeInstance, event.key.keysym.sym, s));
						break;
					}
					case SDL_KEYUP: {
						TVPPostInputEvent(new tTVPOnKeyPressInputEvent(TJSNativeInstance, event.key.keysym.sym));
						TVPPostInputEvent(new tTVPOnKeyUpInputEvent(TJSNativeInstance, event.key.keysym.sym, s));
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
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (_currentWindowLayer) {
			_currentWindowLayer->sdlRecvEvent(event);
		}
	}
	::Application->Run();
}

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_EVERYTHING);

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
				return state & SDL_BUTTON(SDL_BUTTON_LEFT);
			case VK_RBUTTON:
				return state & SDL_BUTTON(SDL_BUTTON_RIGHT);
			case VK_MBUTTON:
				return state & SDL_BUTTON(SDL_BUTTON_MIDDLE);
			case VK_XBUTTON1:
				return state & SDL_BUTTON(SDL_BUTTON_X1);
			case VK_XBUTTON2:
				return state & SDL_BUTTON(SDL_BUTTON_X2);
			default:
				return false;
		}
	}
	if(keycode >= VK_SHIFT && keycode <= VK_MENU)
	{
		Uint32 state = SDL_GetModState();
		switch (keycode) {
			case VK_SHIFT:
				return state & KMOD_SHIFT;
			case VK_MENU:
				return state & KMOD_ALT;
			case VK_CONTROL:
				return state & KMOD_CTRL;
			default:
				return false;
		}
	}
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	return state[SDL_GetScancodeFromKey(keycode)];
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


