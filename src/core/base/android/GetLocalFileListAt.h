#pragma once
#include <string>
#include <functional>
#include "tjs.h"
#include <dirent.h>

#ifndef S_IFMT
#define S_IFDIR  0x4000 // Directory
#define S_IFREG  0x8000 // Regular
#endif

struct tTVPLocalFileInfo {
	const char * NativeName;
	unsigned short Mode; // S_IFMT
	tjs_uint64         Size;
	time_t         AccessTime;
	time_t         ModifyTime;
	time_t         CreationTime;
};

void TVPGetLocalFileListAt(const ttstr &name, const std::function<void(const ttstr&, tTVPLocalFileInfo*)>& cb);
