
#include "tjsCommHead.h"

#include "TVPScreen.h"
#include "Application.h"

int tTVPScreen::GetWidth() {
#if 0
	HMONITOR hMonitor = ::MonitorFromWindow( Application->GetMainWindowHandle(), MONITOR_DEFAULTTOPRIMARY );
	MONITORINFO monitorinfo = {sizeof(MONITORINFO)};
	if( ::GetMonitorInfo( hMonitor, &monitorinfo ) != FALSE ) {
		return monitorinfo.rcMonitor.right - monitorinfo.rcMonitor.left;
	}
	return ::GetSystemMetrics(SM_CXSCREEN);
#endif
	return 2048;
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
	return 2048;
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
	return 0;
}
int tTVPScreen::GetDesktopTop() {
#if 0
	RECT r;
	GetDesktopRect(r);
	return r.top;
#endif
	return 0;
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

