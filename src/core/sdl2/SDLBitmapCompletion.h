/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#pragma once
#include "tjsCommHead.h"
#include "WindowImpl.h"
#include <SDL.h>

class TVPSDLBitmapCompletion
{
public:
	SDL_Surface* surface;
	tTVPRect update_rect;
	TVPSDLBitmapCompletion();
	~TVPSDLBitmapCompletion();
	void NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const class BitmapInfomation * bmpinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
};


