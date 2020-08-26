
#include "tjsCommHead.h"

#include "TVPScreen.h"
#include "Application.h"

#include "DebugIntf.h"
#include "MsgIntf.h"

int tTVPScreen::GetWidth() {
#if 0
	HMONITOR hMonitor = ::MonitorFromWindow( Application->GetMainWindowHandle(), MONITOR_DEFAULTTOPRIMARY );
	MONITORINFO monitorinfo = {sizeof(MONITORINFO)};
	if( ::GetMonitorInfo( hMonitor, &monitorinfo ) != FALSE ) {
		return monitorinfo.rcMonitor.right - monitorinfo.rcMonitor.left;
	}
	return ::GetSystemMetrics(SM_CXSCREEN);
#endif
#if defined(__EMSCRIPTEN__) && defined(__EMSCRIPTEN_PTHREADS__)
	return 1920;
#else
	SDL_Rect r;
	if (SDL_GetDisplayUsableBounds(0, &r) != 0)
	{
		return 0;
	}
	return r.w;
#endif
}
int tTVPScreen::GetHeight() {
#if 0
	HMONITOR hMonitor = ::MonitorFromWindow( Application->GetMainWindowHandle(), MONITOR_DEFAULTTOPRIMARY );
	MONITORINFO monitorinfo = {sizeof(MONITORINFO)};
	if( ::GetMonitorInfo( hMonitor, &monitorinfo ) != FALSE ) {
		return monitorinfo.rcMonitor.bottom - monitorinfo.rcMonitor.top;
	}
	return ::GetSystemMetrics(SM_CYSCREEN);
#endif
#if defined(__EMSCRIPTEN__) && defined(__EMSCRIPTEN_PTHREADS__)
	return 1080;
#else
	SDL_Rect r;
	if (SDL_GetDisplayUsableBounds(0, &r) != 0)
	{
		return 0;
	}
	return r.h;
#endif
}

#if 0
void tTVPScreen::GetDesktopRect( RECT& r ) {
	HWND hWnd = Application->GetMainWindowHandle();
	if( hWnd != INVALID_HANDLE_VALUE ) {
		HMONITOR hMonitor = ::MonitorFromWindow( hWnd, MONITOR_DEFAULTTOPRIMARY );
		if( hMonitor != NULL ) {
			MONITORINFO monitorinfo = {sizeof(MONITORINFO)};
			if( ::GetMonitorInfo( hMonitor, &monitorinfo ) != FALSE ) {
				r = monitorinfo.rcWork;
				return;
			}
		}
	}
	::SystemParametersInfo( SPI_GETWORKAREA, 0, &r, 0 );
}
#endif
int tTVPScreen::GetDesktopLeft() {
#if 0
	RECT r;
	GetDesktopRect(r);
	return r.left;
#endif
#if defined(__EMSCRIPTEN__) && defined(__EMSCRIPTEN_PTHREADS__)
	return 0;
#else
	SDL_Rect r;
	if (SDL_GetDisplayUsableBounds(0, &r) != 0)
	{
		return 0;
	}
	return r.x;
#endif
}
int tTVPScreen::GetDesktopTop() {
#if 0
	RECT r;
	GetDesktopRect(r);
	return r.top;
#endif
#if defined(__EMSCRIPTEN__) && defined(__EMSCRIPTEN_PTHREADS__)
	return 0;
#else
	SDL_Rect r;
	if (SDL_GetDisplayUsableBounds(0, &r) != 0)
	{
		return 0;
	}
	return r.y;
#endif
}
int tTVPScreen::GetDesktopWidth() {
#if 0
	RECT r;
	GetDesktopRect(r);
	return r.right - r.left;
#endif
	return GetWidth();
}
int tTVPScreen::GetDesktopHeight() {
#if 0
	RECT r;
	GetDesktopRect(r);
	return r.bottom - r.top;
#endif
	return GetHeight();
}
#if 0
// Dump Video card infomation
void TVPDumpDisplayDevices() {
	DISPLAY_DEVICE	displayDevice;
	ZeroMemory( &displayDevice, sizeof(DISPLAY_DEVICE) );
	displayDevice.cb = sizeof(DISPLAY_DEVICE);
	DWORD iDevNum = 0;
	while( EnumDisplayDevices( nullptr, iDevNum, &displayDevice, 0) ) {
		ttstr gpuinfo( TVPFormatMessage(TJS_W("(info) Display Device #%1 : "), ttstr((tjs_int)iDevNum)) );
		gpuinfo += ttstr(TJS_W("Name : ")) + ttstr(displayDevice.DeviceName);
		gpuinfo += ttstr(TJS_W("  Description : ")) + ttstr(displayDevice.DeviceString);
		gpuinfo += ttstr(TJS_W("  ACTIVE")) + (( displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE ) ? TJS_W(":yes") : TJS_W(":no"));
		gpuinfo += ttstr(TJS_W("  MIRRORING")) + (( displayDevice.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER ) ? TJS_W(":yes") : TJS_W(":no"));
		gpuinfo += ttstr(TJS_W("  MODESPRUNED")) + (( displayDevice.StateFlags & DISPLAY_DEVICE_MODESPRUNED ) ? TJS_W(":yes") : TJS_W(":no"));
		gpuinfo += ttstr(TJS_W("  PRIMARY")) + (( displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) ? TJS_W(":yes") : TJS_W(":no"));
		gpuinfo += ttstr(TJS_W("  REMOVABLE")) + (( displayDevice.StateFlags & DISPLAY_DEVICE_REMOVABLE ) ? TJS_W(":yes") : TJS_W(":no"));
		gpuinfo += ttstr(TJS_W("  VGA COMPATIBLE")) + (( displayDevice.StateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE ) ? TJS_W(":yes") : TJS_W(":no"));
		TVPAddImportantLog(gpuinfo);

		ZeroMemory( &displayDevice, sizeof(DISPLAY_DEVICE) );
		displayDevice.cb = sizeof(DISPLAY_DEVICE);
		iDevNum++;
	}
}
#endif
