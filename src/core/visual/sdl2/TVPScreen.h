
#ifndef __TVP_SCREEN_H__
#define __TVP_SCREEN_H__

class tTVPScreen {
public:
	tTVPScreen();
	static int GetWidth();
	static int GetHeight();
#if 0
	static void GetDesktopRect( RECT& r );
#endif
	static int GetDesktopLeft();
	static int GetDesktopTop();
	static int GetDesktopWidth();
	static int GetDesktopHeight();
};
#if 0
extern void TVPDumpDisplayDevices();
#endif
#endif // __TVP_SCREEN_H__
