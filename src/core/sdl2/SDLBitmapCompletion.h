#pragma once
#include "tjsCommHead.h"
#include "WindowImpl.h"
#include <SDL.h>

class TVPSDLBitmapCompletion
{
	SDL_Renderer* renderer;
	SDL_Texture* framebuffer;
	SDL_Surface* surface;
	void* TextureBuffer;
	int TexturePitch;
public:
	TVPSDLBitmapCompletion(SDL_Renderer* in_renderer, SDL_Texture* in_framebuffer, SDL_Surface* in_surface);
	~TVPSDLBitmapCompletion();
	void NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const class BitmapInfomation * bmpinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
};


