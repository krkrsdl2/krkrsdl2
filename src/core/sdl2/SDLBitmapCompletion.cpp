
#include "SDLBitmapCompletion.h"
#include "DebugIntf.h"

TVPSDLBitmapCompletion::TVPSDLBitmapCompletion(SDL_Renderer* in_renderer, SDL_Texture* in_framebuffer, SDL_Surface* in_surface)
{
	renderer = in_renderer;
	framebuffer = in_framebuffer;
	surface = in_surface;
	if (framebuffer)
	{
		SDL_LockTexture(framebuffer, nullptr, &TextureBuffer, &TexturePitch);
	}
}

void TVPSDLBitmapCompletion::NotifyBitmapCompleted(iTVPLayerManager * manager,
	tjs_int x, tjs_int y, const void * bits, const class BitmapInfomation * bmpinfo,
	const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity) {
	const TVPBITMAPINFO *bitmapinfo = bmpinfo->GetBITMAPINFO();
	tjs_int w = 0;
	tjs_int h = 0;
	if(!manager) return;
	if(!manager->GetPrimaryLayerSize(w, h))
	{
		w = 0;
		h = 0;
	}
	if(
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
		long dest_y      = y;
		long dest_x      = x;
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

		if (framebuffer)
		{
			for(; src_y < src_y_limit; src_y ++, dest_y ++)
			{
				const void *srcp = src_p + src_pitch * src_y + src_x * 4;
				void *destp = (tjs_uint8*)TextureBuffer + TexturePitch * dest_y + dest_x * 4;
				memcpy(destp, srcp, width_bytes);
			}
		}
		else if (surface)
		{
			SDL_Rect dstrect;
			dstrect.x = x;
			dstrect.y = y;
			dstrect.w = cliprect.get_width();
			dstrect.h = cliprect.get_height();
			dstrect.h = 1;
			SDL_Surface* clip_surface = SDL_CreateRGBSurfaceFrom((void *)src_p, cliprect.get_width(), 1, 32, cliprect.get_width() * 4, 0x00ff0000, 0x0000ff00, 0x000000ff, 0);
			if (clip_surface == nullptr)
			{
				TVPAddLog(ttstr("Cannot create clip surface: ") + ttstr(SDL_GetError()));
				return;
			}
			for(; src_y < src_y_limit; src_y ++)
			{
				const void *srcp = src_p + src_pitch * src_y + src_x * 4;
				SDL_LockSurface(clip_surface);
				clip_surface->pixels = (void *)srcp;
				SDL_UnlockSurface(clip_surface);
				int blit_result = SDL_BlitSurface(clip_surface, nullptr, surface, &dstrect);
				if (blit_result < 0)
				{
					TVPAddLog(ttstr("Cannot blit onto window surface: ") + ttstr(SDL_GetError()));
				}
				dstrect.y += 1;
			}
			SDL_FreeSurface(clip_surface);
		}

	}
}

TVPSDLBitmapCompletion::~TVPSDLBitmapCompletion()
{
	if (framebuffer)
	{
		SDL_UnlockTexture(framebuffer);
	}
	if (renderer)
	{
		SDL_RenderFillRect(renderer, NULL);
		if (framebuffer)
		{
			SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
		}
	}
}
