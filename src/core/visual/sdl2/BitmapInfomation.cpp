/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#include "tjsCommHead.h"
#include "BitmapInfomation.h"
#include "MsgIntf.h"

BitmapInfomation::BitmapInfomation( tjs_uint width, tjs_uint height, int bpp, bool unpadding ) {
#ifdef _WIN32
	BitmapInfoSize = sizeof(BITMAPINFOHEADER) + ((bpp==8)?sizeof(RGBQUAD)*256 : 0);
	BitmapInfo = (BITMAPINFO*)GlobalAlloc(GPTR, BitmapInfoSize);
#else
	BitmapInfoSize = sizeof(TVPBITMAPINFOHEADER) + ((bpp == 8) ? sizeof(TVPRGBQUAD) * 256 : 0);
	BitmapInfo = (TVPBITMAPINFO*)malloc(BitmapInfoSize);
#endif
	if(!BitmapInfo) TVPThrowExceptionMessage(TVPCannotAllocateBitmapBits,
		TJS_W("allocating BITMAPINFOHEADER"), ttstr((tjs_int)BitmapInfoSize));

	tjs_int PitchBytes;
	tjs_uint bitmap_width = width;
	if( unpadding == false ) {
		// note that the allocated bitmap size can be bigger than the
		// original size because the horizontal pitch of the bitmap
		// is aligned to a paragraph (16bytes)
		if( bpp == 8 ) {
			bitmap_width = (((bitmap_width-1) / 16)+1) *16; // align to a paragraph
			PitchBytes = (((bitmap_width-1) >> 2)+1) <<2;
		} else {
			bitmap_width = (((bitmap_width-1) / 4)+1) *4; // align to a paragraph
			PitchBytes = bitmap_width * 4;
		}
	} else {
		if( bpp == 8 ) {
			PitchBytes = bitmap_width;
		} else {
			PitchBytes = bitmap_width * 4;
		}
	}
#ifdef _WIN32
	BitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
#else
	BitmapInfo->bmiHeader.biSize = sizeof(BitmapInfo->bmiHeader);
#endif
	BitmapInfo->bmiHeader.biWidth = bitmap_width;
	BitmapInfo->bmiHeader.biHeight = height;
	BitmapInfo->bmiHeader.biPlanes = 1;
	BitmapInfo->bmiHeader.biBitCount = bpp;
#ifdef _WIN32
	BitmapInfo->bmiHeader.biCompression = BI_RGB;
#else
	BitmapInfo->bmiHeader.biCompression = /*BI_RGB*/0;
#endif
	BitmapInfo->bmiHeader.biSizeImage = PitchBytes * height;
	BitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	BitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	BitmapInfo->bmiHeader.biClrUsed = 0;
	BitmapInfo->bmiHeader.biClrImportant = 0;

	// create grayscale palette
	if(bpp == 8) {
#ifdef _WIN32
		RGBQUAD *pal = (RGBQUAD*)((tjs_uint8*)BitmapInfo + sizeof(BITMAPINFOHEADER));
#else
		TVPRGBQUAD *pal = (TVPRGBQUAD*)((tjs_uint8*)BitmapInfo + sizeof(TVPBITMAPINFOHEADER));
#endif
		for( tjs_int i=0; i<256; i++ ) {
			pal[i].rgbBlue = pal[i].rgbGreen = pal[i].rgbRed = (tjs_uint8)i;
			pal[i].rgbReserved = 0;
		}
	}
}

BitmapInfomation::~BitmapInfomation() {
#ifdef _WIN32
	::GlobalFree((HGLOBAL)BitmapInfo);
#else
	free(BitmapInfo);
#endif
	BitmapInfo = NULL;
}

