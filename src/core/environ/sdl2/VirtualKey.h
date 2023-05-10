/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#ifndef __VIRTUAL_KEY_H__
#define __VIRTUAL_KEY_H__

#if 0
#include <android/keycodes.h>
#endif

#ifdef _WIN32
#include <winuser.h>
#endif

#ifdef VK_LBUTTON
#undef VK_LBUTTON
#endif
#ifdef VK_RBUTTON
#undef VK_RBUTTON
#endif
#ifdef VK_CANCEL
#undef VK_CANCEL
#endif
#ifdef VK_MBUTTON
#undef VK_MBUTTON
#endif
#ifdef VK_XBUTTON1
#undef VK_XBUTTON1
#endif
#ifdef VK_XBUTTON2
#undef VK_XBUTTON2
#endif
#ifdef VK_BACK
#undef VK_BACK
#endif
#ifdef VK_TAB
#undef VK_TAB
#endif
#ifdef VK_CLEAR
#undef VK_CLEAR
#endif
#ifdef VK_RETURN
#undef VK_RETURN
#endif
#ifdef VK_SHIFT
#undef VK_SHIFT
#endif
#ifdef VK_CONTROL
#undef VK_CONTROL
#endif
#ifdef VK_MENU
#undef VK_MENU
#endif
#ifdef VK_PAUSE
#undef VK_PAUSE
#endif
#ifdef VK_CAPITAL
#undef VK_CAPITAL
#endif
#ifdef VK_KANA
#undef VK_KANA
#endif
#ifdef VK_HANGEUL
#undef VK_HANGEUL
#endif
#ifdef VK_HANGUL
#undef VK_HANGUL
#endif
#ifdef VK_JUNJA
#undef VK_JUNJA
#endif
#ifdef VK_FINAL
#undef VK_FINAL
#endif
#ifdef VK_HANJA
#undef VK_HANJA
#endif
#ifdef VK_KANJI
#undef VK_KANJI
#endif
#ifdef VK_ESCAPE
#undef VK_ESCAPE
#endif
#ifdef VK_CONVERT
#undef VK_CONVERT
#endif
#ifdef VK_NONCONVERT
#undef VK_NONCONVERT
#endif
#ifdef VK_ACCEPT
#undef VK_ACCEPT
#endif
#ifdef VK_MODECHANGE
#undef VK_MODECHANGE
#endif
#ifdef VK_SPACE
#undef VK_SPACE
#endif
#ifdef VK_PRIOR
#undef VK_PRIOR
#endif
#ifdef VK_NEXT
#undef VK_NEXT
#endif
#ifdef VK_END
#undef VK_END
#endif
#ifdef VK_HOME
#undef VK_HOME
#endif
#ifdef VK_LEFT
#undef VK_LEFT
#endif
#ifdef VK_UP
#undef VK_UP
#endif
#ifdef VK_RIGHT
#undef VK_RIGHT
#endif
#ifdef VK_DOWN
#undef VK_DOWN
#endif
#ifdef VK_SELECT
#undef VK_SELECT
#endif
#ifdef VK_PRINT
#undef VK_PRINT
#endif
#ifdef VK_EXECUTE
#undef VK_EXECUTE
#endif
#ifdef VK_SNAPSHOT
#undef VK_SNAPSHOT
#endif
#ifdef VK_INSERT
#undef VK_INSERT
#endif
#ifdef VK_DELETE
#undef VK_DELETE
#endif
#ifdef VK_HELP
#undef VK_HELP
#endif
#ifdef VK_0
#undef VK_0
#endif
#ifdef VK_1
#undef VK_1
#endif
#ifdef VK_2
#undef VK_2
#endif
#ifdef VK_3
#undef VK_3
#endif
#ifdef VK_4
#undef VK_4
#endif
#ifdef VK_5
#undef VK_5
#endif
#ifdef VK_6
#undef VK_6
#endif
#ifdef VK_7
#undef VK_7
#endif
#ifdef VK_8
#undef VK_8
#endif
#ifdef VK_9
#undef VK_9
#endif
#ifdef VK_A
#undef VK_A
#endif
#ifdef VK_B
#undef VK_B
#endif
#ifdef VK_C
#undef VK_C
#endif
#ifdef VK_D
#undef VK_D
#endif
#ifdef VK_E
#undef VK_E
#endif
#ifdef VK_F
#undef VK_F
#endif
#ifdef VK_G
#undef VK_G
#endif
#ifdef VK_H
#undef VK_H
#endif
#ifdef VK_I
#undef VK_I
#endif
#ifdef VK_J
#undef VK_J
#endif
#ifdef VK_K
#undef VK_K
#endif
#ifdef VK_L
#undef VK_L
#endif
#ifdef VK_M
#undef VK_M
#endif
#ifdef VK_N
#undef VK_N
#endif
#ifdef VK_O
#undef VK_O
#endif
#ifdef VK_P
#undef VK_P
#endif
#ifdef VK_Q
#undef VK_Q
#endif
#ifdef VK_R
#undef VK_R
#endif
#ifdef VK_S
#undef VK_S
#endif
#ifdef VK_T
#undef VK_T
#endif
#ifdef VK_U
#undef VK_U
#endif
#ifdef VK_V
#undef VK_V
#endif
#ifdef VK_W
#undef VK_W
#endif
#ifdef VK_X
#undef VK_X
#endif
#ifdef VK_Y
#undef VK_Y
#endif
#ifdef VK_Z
#undef VK_Z
#endif
#ifdef VK_LWIN
#undef VK_LWIN
#endif
#ifdef VK_RWIN
#undef VK_RWIN
#endif
#ifdef VK_APPS
#undef VK_APPS
#endif
#ifdef VK_SLEEP
#undef VK_SLEEP
#endif
#ifdef VK_NUMPAD0
#undef VK_NUMPAD0
#endif
#ifdef VK_NUMPAD1
#undef VK_NUMPAD1
#endif
#ifdef VK_NUMPAD2
#undef VK_NUMPAD2
#endif
#ifdef VK_NUMPAD3
#undef VK_NUMPAD3
#endif
#ifdef VK_NUMPAD4
#undef VK_NUMPAD4
#endif
#ifdef VK_NUMPAD5
#undef VK_NUMPAD5
#endif
#ifdef VK_NUMPAD6
#undef VK_NUMPAD6
#endif
#ifdef VK_NUMPAD7
#undef VK_NUMPAD7
#endif
#ifdef VK_NUMPAD8
#undef VK_NUMPAD8
#endif
#ifdef VK_NUMPAD9
#undef VK_NUMPAD9
#endif
#ifdef VK_MULTIPLY
#undef VK_MULTIPLY
#endif
#ifdef VK_ADD
#undef VK_ADD
#endif
#ifdef VK_SEPARATOR
#undef VK_SEPARATOR
#endif
#ifdef VK_SUBTRACT
#undef VK_SUBTRACT
#endif
#ifdef VK_DECIMAL
#undef VK_DECIMAL
#endif
#ifdef VK_DIVIDE
#undef VK_DIVIDE
#endif
#ifdef VK_F1
#undef VK_F1
#endif
#ifdef VK_F2
#undef VK_F2
#endif
#ifdef VK_F3
#undef VK_F3
#endif
#ifdef VK_F4
#undef VK_F4
#endif
#ifdef VK_F5
#undef VK_F5
#endif
#ifdef VK_F6
#undef VK_F6
#endif
#ifdef VK_F7
#undef VK_F7
#endif
#ifdef VK_F8
#undef VK_F8
#endif
#ifdef VK_F9
#undef VK_F9
#endif
#ifdef VK_F10
#undef VK_F10
#endif
#ifdef VK_F11
#undef VK_F11
#endif
#ifdef VK_F12
#undef VK_F12
#endif
#ifdef VK_F13
#undef VK_F13
#endif
#ifdef VK_F14
#undef VK_F14
#endif
#ifdef VK_F15
#undef VK_F15
#endif
#ifdef VK_F16
#undef VK_F16
#endif
#ifdef VK_F17
#undef VK_F17
#endif
#ifdef VK_F18
#undef VK_F18
#endif
#ifdef VK_F19
#undef VK_F19
#endif
#ifdef VK_F20
#undef VK_F20
#endif
#ifdef VK_F21
#undef VK_F21
#endif
#ifdef VK_F22
#undef VK_F22
#endif
#ifdef VK_F23
#undef VK_F23
#endif
#ifdef VK_F24
#undef VK_F24
#endif
#ifdef VK_NUMLOCK
#undef VK_NUMLOCK
#endif
#ifdef VK_SCROLL
#undef VK_SCROLL
#endif
#ifdef VK_LSHIFT
#undef VK_LSHIFT
#endif
#ifdef VK_RSHIFT
#undef VK_RSHIFT
#endif
#ifdef VK_LCONTROL
#undef VK_LCONTROL
#endif
#ifdef VK_RCONTROL
#undef VK_RCONTROL
#endif
#ifdef VK_LMENU
#undef VK_LMENU
#endif
#ifdef VK_RMENU
#undef VK_RMENU
#endif
#ifdef VK_BROWSER_BACK
#undef VK_BROWSER_BACK
#endif
#ifdef VK_BROWSER_FORWARD
#undef VK_BROWSER_FORWARD
#endif
#ifdef VK_BROWSER_REFRESH
#undef VK_BROWSER_REFRESH
#endif
#ifdef VK_BROWSER_STOP
#undef VK_BROWSER_STOP
#endif
#ifdef VK_BROWSER_SEARCH
#undef VK_BROWSER_SEARCH
#endif
#ifdef VK_BROWSER_FAVORITES
#undef VK_BROWSER_FAVORITES
#endif
#ifdef VK_BROWSER_HOME
#undef VK_BROWSER_HOME
#endif
#ifdef VK_VOLUME_MUTE
#undef VK_VOLUME_MUTE
#endif
#ifdef VK_VOLUME_DOWN
#undef VK_VOLUME_DOWN
#endif
#ifdef VK_VOLUME_UP
#undef VK_VOLUME_UP
#endif
#ifdef VK_MEDIA_NEXT_TRACK
#undef VK_MEDIA_NEXT_TRACK
#endif
#ifdef VK_MEDIA_PREV_TRACK
#undef VK_MEDIA_PREV_TRACK
#endif
#ifdef VK_MEDIA_STOP
#undef VK_MEDIA_STOP
#endif
#ifdef VK_MEDIA_PLAY_PAUSE
#undef VK_MEDIA_PLAY_PAUSE
#endif
#ifdef VK_LAUNCH_MAIL
#undef VK_LAUNCH_MAIL
#endif
#ifdef VK_LAUNCH_MEDIA_SELECT
#undef VK_LAUNCH_MEDIA_SELECT
#endif
#ifdef VK_LAUNCH_APP1
#undef VK_LAUNCH_APP1
#endif
#ifdef VK_LAUNCH_APP2
#undef VK_LAUNCH_APP2
#endif
#ifdef VK_OEM_1
#undef VK_OEM_1
#endif
#ifdef VK_OEM_PLUS
#undef VK_OEM_PLUS
#endif
#ifdef VK_OEM_COMMA
#undef VK_OEM_COMMA
#endif
#ifdef VK_OEM_MINUS
#undef VK_OEM_MINUS
#endif
#ifdef VK_OEM_PERIOD
#undef VK_OEM_PERIOD
#endif
#ifdef VK_OEM_2
#undef VK_OEM_2
#endif
#ifdef VK_OEM_3
#undef VK_OEM_3
#endif
#ifdef VK_OEM_4
#undef VK_OEM_4
#endif
#ifdef VK_OEM_5
#undef VK_OEM_5
#endif
#ifdef VK_OEM_6
#undef VK_OEM_6
#endif
#ifdef VK_OEM_7
#undef VK_OEM_7
#endif
#ifdef VK_OEM_8
#undef VK_OEM_8
#endif
#ifdef VK_OEM_102
#undef VK_OEM_102
#endif
#ifdef VK_ICO_HELP
#undef VK_ICO_HELP
#endif
#ifdef VK_ICO_00
#undef VK_ICO_00
#endif
#ifdef VK_PROCESSKEY
#undef VK_PROCESSKEY
#endif
#ifdef VK_ICO_CLEAR
#undef VK_ICO_CLEAR
#endif
#ifdef VK_PACKET
#undef VK_PACKET
#endif
#ifdef VK_OEM_RESET
#undef VK_OEM_RESET
#endif
#ifdef VK_OEM_JUMP
#undef VK_OEM_JUMP
#endif
#ifdef VK_OEM_PA1
#undef VK_OEM_PA1
#endif
#ifdef VK_OEM_PA2
#undef VK_OEM_PA2
#endif
#ifdef VK_OEM_PA3
#undef VK_OEM_PA3
#endif
#ifdef VK_OEM_WSCTRL
#undef VK_OEM_WSCTRL
#endif
#ifdef VK_OEM_CUSEL
#undef VK_OEM_CUSEL
#endif
#ifdef VK_OEM_ATTN
#undef VK_OEM_ATTN
#endif
#ifdef VK_OEM_FINISH
#undef VK_OEM_FINISH
#endif
#ifdef VK_OEM_COPY
#undef VK_OEM_COPY
#endif
#ifdef VK_OEM_AUTO
#undef VK_OEM_AUTO
#endif
#ifdef VK_OEM_ENLW
#undef VK_OEM_ENLW
#endif
#ifdef VK_OEM_BACKTAB
#undef VK_OEM_BACKTAB
#endif
#ifdef VK_ATTN
#undef VK_ATTN
#endif
#ifdef VK_CRSEL
#undef VK_CRSEL
#endif
#ifdef VK_EXSEL
#undef VK_EXSEL
#endif
#ifdef VK_EREOF
#undef VK_EREOF
#endif
#ifdef VK_PLAY
#undef VK_PLAY
#endif
#ifdef VK_ZOOM
#undef VK_ZOOM
#endif
#ifdef VK_NONAME
#undef VK_NONAME
#endif
#ifdef VK_PA1
#undef VK_PA1
#endif
#ifdef VK_OEM_CLEAR
#undef VK_OEM_CLEAR
#endif

/**
 * システム定義のキーコード
 * Windowsと同じ値
 */
enum {
	VK_LBUTTON =0x01,	// Left mouse button
	VK_RBUTTON =0x02,	// Right mouse button
	VK_CANCEL =0x03,	// Control-break processing
	VK_MBUTTON =0x04,	// Middle mouse button (three-button mouse)
	VK_XBUTTON1 =0x05,	// X1 mouse button
	VK_XBUTTON2 =0x06,	// X2 mouse button
	VK_BACK =0x08,		// BACKSPACE key
	VK_TAB =0x09,		// TAB key
	VK_CLEAR =0x0C,		// CLEAR key
	VK_RETURN =0x0D,	// ENTER key
	VK_SHIFT =0x10,		// SHIFT key
	VK_CONTROL =0x11,	// CTRL key
	VK_MENU =0x12,		// ALT key
	VK_PAUSE =0x13,		// PAUSE key
	VK_CAPITAL =0x14,	// CAPS LOCK key
	VK_KANA =0x15,		// IME Kana mode
	VK_HANGEUL =0x15,	// IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
	VK_HANGUL =0x15,	// IME Hangul mode
	VK_JUNJA =0x17,		// IME Junja mode
	VK_FINAL =0x18,		// IME final mode
	VK_HANJA =0x19,		// IME Hanja mode
	VK_KANJI =0x19,		// IME Kanji mode
	VK_ESCAPE =0x1B,	// ESC key
	VK_CONVERT =0x1C,	// IME convert
	VK_NONCONVERT =0x1D,// IME nonconvert
	VK_ACCEPT =0x1E,	// IME accept
	VK_MODECHANGE =0x1F,// IME mode change request
	VK_SPACE =0x20,		// SPACEBAR
	VK_PRIOR =0x21,		// PAGE UP key
	VK_NEXT =0x22,		// PAGE DOWN key
	VK_END =0x23,		// END key
	VK_HOME =0x24,		// HOME key
	VK_LEFT =0x25,		// LEFT ARROW key
	VK_UP =0x26,		// UP ARROW key
	VK_RIGHT =0x27,		// RIGHT ARROW key
	VK_DOWN =0x28,		// DOWN ARROW key
	VK_SELECT =0x29,	// SELECT key
	VK_PRINT =0x2A,		// PRINT key
	VK_EXECUTE =0x2B,	// EXECUTE key
	VK_SNAPSHOT =0x2C,	// PRINT SCREEN key
	VK_INSERT =0x2D,	// INS key
	VK_DELETE =0x2E,	// DEL key
	VK_HELP =0x2F,		// HELP key
	VK_0 =0x30,
	VK_1 =0x31,
	VK_2 =0x32,
	VK_3 =0x33,
	VK_4 =0x34,
	VK_5 =0x35,
	VK_6 =0x36,
	VK_7 =0x37,
	VK_8 =0x38,
	VK_9 =0x39,
	VK_A =0x41,
	VK_B =0x42,
	VK_C =0x43,
	VK_D =0x44,
	VK_E =0x45,
	VK_F =0x46,
	VK_G =0x47,
	VK_H =0x48,
	VK_I =0x49,
	VK_J =0x4A,
	VK_K =0x4B,
	VK_L =0x4C,
	VK_M =0x4D,
	VK_N =0x4E,
	VK_O =0x4F,
	VK_P =0x50,
	VK_Q =0x51,
	VK_R =0x52,
	VK_S =0x53,
	VK_T =0x54,
	VK_U =0x55,
	VK_V =0x56,
	VK_W =0x57,
	VK_X =0x58,
	VK_Y =0x59,
	VK_Z =0x5A,
	VK_LWIN =0x5B,	// Left Windows key (Natural keyboard)
	VK_RWIN =0x5C,	// Right Windows key (Natural keyboard)
	VK_APPS =0x5D,	// Applications key (Natural keyboard) コンテキストメニューを出すキー。右ALtの隣にあることが多い。
	VK_SLEEP =0x5F,	// Computer Sleep key
	VK_NUMPAD0 =0x60,
	VK_NUMPAD1 =0x61,
	VK_NUMPAD2 =0x62,
	VK_NUMPAD3 =0x63,
	VK_NUMPAD4 =0x64,
	VK_NUMPAD5 =0x65,
	VK_NUMPAD6 =0x66,
	VK_NUMPAD7 =0x67,
	VK_NUMPAD8 =0x68,
	VK_NUMPAD9 =0x69,
	VK_MULTIPLY =0x6A,	// Multiply key テンキー[*]
	VK_ADD =0x6B,		// Add key テンキー[+]
	VK_SEPARATOR =0x6C,	// Separator key 区切り記号キー
	VK_SUBTRACT =0x6D,	// Subtract key テンキー[-]
	VK_DECIMAL =0x6E,	// Decimal key テンキー[.]
	VK_DIVIDE =0x6F,	// Divide key テンキー[/]
	VK_F1 =0x70,
	VK_F2 =0x71,
	VK_F3 =0x72,
	VK_F4 =0x73,
	VK_F5 =0x74,
	VK_F6 =0x75,
	VK_F7 =0x76,
	VK_F8 =0x77,
	VK_F9 =0x78,
	VK_F10 =0x79,
	VK_F11 =0x7A,
	VK_F12 =0x7B,
	VK_F13 =0x7C,
	VK_F14 =0x7D,
	VK_F15 =0x7E,
	VK_F16 =0x7F,
	VK_F17 =0x80,
	VK_F18 =0x81,
	VK_F19 =0x82,
	VK_F20 =0x83,
	VK_F21 =0x84,
	VK_F22 =0x85,
	VK_F23 =0x86,
	VK_F24 =0x87,
	VK_NUMLOCK =0x90,	// NUM LOCK key
	VK_SCROLL =0x91,	// SCROLL LOCK key
	VK_LSHIFT =0xA0,	// Left SHIFT key
	VK_RSHIFT =0xA1,	// Right SHIFT key
	VK_LCONTROL =0xA2,	// Left CONTROL key
	VK_RCONTROL =0xA3,	// Right CONTROL key
	VK_LMENU =0xA4,		// Left MENU key
	VK_RMENU =0xA5,		// Right MENU key

	VK_BROWSER_BACK =0xA6,		// Browser Back key
	VK_BROWSER_FORWARD =0xA7,	// Browser Forward key
	VK_BROWSER_REFRESH =0xA8,	// Browser Refresh key
	VK_BROWSER_STOP =0xA9,		// Browser Stop key
	VK_BROWSER_SEARCH =0xAA,	// Browser Search key
	VK_BROWSER_FAVORITES =0xAB,	// Browser Favorites key
	VK_BROWSER_HOME =0xAC,		// Browser Start and Home key
	VK_VOLUME_MUTE =0xAD,		// Volume Mute key
	VK_VOLUME_DOWN =0xAE,		// Volume Down key
	VK_VOLUME_UP =0xAF,			// Volume Up key
	VK_MEDIA_NEXT_TRACK =0xB0,	// Next Track key
	VK_MEDIA_PREV_TRACK =0xB1,	// Previous Track key
	VK_MEDIA_STOP =0xB2,		// Stop Media key
	VK_MEDIA_PLAY_PAUSE =0xB3,	// Play/Pause Media key
	VK_LAUNCH_MAIL =0xB4,		// Start Mail key
	VK_LAUNCH_MEDIA_SELECT =0xB5,// Select Media key
	VK_LAUNCH_APP1 =0xB6,		// Start Application 1 key
	VK_LAUNCH_APP2 =0xB7,		// Start Application 2 key

	VK_OEM_1 =0xBA,		// Used for miscellaneous characters; it can vary by keyboard.For the US standard keyboard, the ';:' key
	VK_OEM_PLUS =0xBB,	// For any country/region, the '+' key
	VK_OEM_COMMA =0xBC,	// For any country/region, the ',' key
	VK_OEM_MINUS =0xBD,	// For any country/region, the '-' key
	VK_OEM_PERIOD =0xBE,// For any country/region, the '.' key
	VK_OEM_2 =0xBF,		// Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
	VK_OEM_3 =0xC0,		// Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
	VK_OEM_4 =0xDB,		// Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
	VK_OEM_5 =0xDC,		// Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
	VK_OEM_6 =0xDD,		// Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
	VK_OEM_7 =0xDE,		// Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
	VK_OEM_8 =0xDF,		// Used for miscellaneous characters; it can vary by keyboard.
	VK_OEM_102 =0xE2,	// Either the angle bracket key or the backslash key on the RT 102-key keyboard
	VK_ICO_HELP =0xE3,
	VK_ICO_00 =0xE4,

	VK_PROCESSKEY =0xE5,// IME PROCESS key

	VK_ICO_CLEAR =0xE6,
	VK_PACKET =0xE7,	// Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
	VK_OEM_RESET =0xE9,
	VK_OEM_JUMP =0xEA,
	VK_OEM_PA1 =0xEB,
	VK_OEM_PA2 =0xEC,
	VK_OEM_PA3 =0xED,
	VK_OEM_WSCTRL =0xEE,
	VK_OEM_CUSEL =0xEF,
	VK_OEM_ATTN =0xF0,	// [英数 CapsLock]
	VK_OEM_FINISH =0xF1,
	VK_OEM_COPY =0xF2,	// [カタカナ ひらがな]
	VK_OEM_AUTO =0xF3,	// [半角／全角]
	VK_OEM_ENLW =0xF4,
	VK_OEM_BACKTAB =0xF5,// [Alt]+[カタカナ ひらがな]

	VK_ATTN =0xF6,		// Attn key
	VK_CRSEL =0xF7,		// CrSel key
	VK_EXSEL =0xF8,		// ExSel key
	VK_EREOF =0xF9,		// Erase EOF key
	VK_PLAY =0xFA,		// Play key
	VK_ZOOM =0xFB,		// Zoom key
	VK_NONAME =0xFC,
	VK_PA1 =0xFD,		// PA1 key
	VK_OEM_CLEAR =0xFE	// Clear key
};


#endif // __VIRTUAL_KEY_H__
