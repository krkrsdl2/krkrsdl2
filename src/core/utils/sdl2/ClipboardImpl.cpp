//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Clipboard Class interface
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#include "MsgIntf.h"
#include "Exception.h"
#include "ClipboardIntf.h"
#include "CharacterSet.h"
#include <SDL.h>

//---------------------------------------------------------------------------
// clipboard related functions
//---------------------------------------------------------------------------
bool TVPClipboardHasFormat(tTVPClipboardFormat format)
{
	if (format == cbfText)
	{
		return SDL_HasClipboardText() == SDL_TRUE;
	}
	return false;
}
//---------------------------------------------------------------------------
void TVPClipboardSetText(const ttstr & text)
{
	tjs_string v_utf16 = text.AsStdString();
	std::string v_utf8;
	if (TVPUtf16ToUtf8(v_utf8, v_utf16))
	{
		SDL_SetClipboardText(v_utf8.c_str());
	}
}
//---------------------------------------------------------------------------
bool TVPClipboardGetText(ttstr & text)
{
	char* src_text = SDL_GetClipboardText();
	if (src_text == nullptr)
	{
		return false;
	}
	tjs_string v_utf16;
	std::string v_utf8 = src_text;
	TVPUtf8ToUtf16( v_utf16, v_utf8 );
	text = ttstr(v_utf16);
	SDL_free(src_text);
	return true;
}
//---------------------------------------------------------------------------

