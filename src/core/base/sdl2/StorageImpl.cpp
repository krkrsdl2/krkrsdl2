//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Universal Storage System
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "MsgIntf.h"

#include "StorageImpl.h"
#include "WindowImpl.h"
#include "SysInitIntf.h"
#include "DebugIntf.h"
#include "Random.h"
#include "XP3Archive.h"
#include "SusieArchive.h"
#include "FileSelector.h"
#include "Random.h"

#include "Application.h"
#include "ApplicationSpecialPath.h"
#include "StringUtil.h"
#include "FilePathUtil.h"
#include "TickCount.h"

#ifndef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

#if defined(__vita__)
#include <psp2/io/devctl.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#endif

#if defined(__ANDROID__)
#include <android/log.h>
#include <android/configuration.h>
#include <android/asset_manager_jni.h>
#include "AndroidAssetManager.h"
#endif

//---------------------------------------------------------------------------
// tTVPFileMedia
//---------------------------------------------------------------------------
class tTVPFileMedia : public iTVPStorageMedia
{
	tjs_uint RefCount;

public:
	tTVPFileMedia()
	{
		RefCount = 1;
	}
	~tTVPFileMedia()
	{
#if defined(__ANDROID__)
		AndroidAssetManager_Destroy_AssetManager();
#endif
	}

	void TJS_INTF_METHOD AddRef() { RefCount ++; }
	void TJS_INTF_METHOD Release()
	{
		if(RefCount == 1)
			delete this;
		else
			RefCount --;
	}

	void TJS_INTF_METHOD GetName(ttstr &name) { name = TJS_W("file"); }

	void TJS_INTF_METHOD NormalizeDomainName(ttstr &name);
	void TJS_INTF_METHOD NormalizePathName(ttstr &name);
	bool TJS_INTF_METHOD CheckExistentStorage(const ttstr &name);
	tTJSBinaryStream * TJS_INTF_METHOD Open(const ttstr & name, tjs_uint32 flags);
	void TJS_INTF_METHOD GetListAt(const ttstr &name, iTVPStorageLister *lister);
	void TJS_INTF_METHOD GetLocallyAccessibleName(ttstr &name);

public:
	void TJS_INTF_METHOD GetLocalName(ttstr &name);
};
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPFileMedia::NormalizeDomainName(ttstr &name)
{
	// normalize domain name
	// make all characters small
#ifdef KRKRZ_CASE_INSENSITIVE
	tjs_char *p = name.Independ();
	while(*p)
	{
		if(*p >= TJS_W('A') && *p <= TJS_W('Z'))
			*p += TJS_W('a') - TJS_W('A');
		p++;
	}
#endif
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPFileMedia::NormalizePathName(ttstr &name)
{
	// 非Windows環境では大文字小文字区別する実装の方が良いか？
	// normalize path name
	// make all characters small
#ifdef KRKRZ_CASE_INSENSITIVE
	tjs_char *p = name.Independ();
	while(*p)
	{
		if(*p >= TJS_W('A') && *p <= TJS_W('Z'))
			*p += TJS_W('a') - TJS_W('A');
		p++;
	}
#endif
}
//---------------------------------------------------------------------------
bool TJS_INTF_METHOD tTVPFileMedia::CheckExistentStorage(const ttstr &name)
{
	if(name.IsEmpty()) return false;

	ttstr _name(name);
	GetLocalName(_name);

	return TVPCheckExistentLocalFile(_name);
}
//---------------------------------------------------------------------------
tTJSBinaryStream * TJS_INTF_METHOD tTVPFileMedia::Open(const ttstr & name, tjs_uint32 flags)
{
	// open storage named "name".
	// currently only local/network(by OS) storage systems are supported.
	if(name.IsEmpty())
		TVPThrowExceptionMessage(TVPCannotOpenStorage, TJS_W("\"\""));

	ttstr origname = name;
	ttstr _name(name);
	GetLocalName(_name);

	return new tTVPLocalFileStream(origname, _name, flags);
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPFileMedia::GetListAt(const ttstr &_name, iTVPStorageLister *lister)
{
	ttstr name(_name);
	GetLocalName(name);
#ifdef _WIN32
	name += TJS_W("*.*");

	// perform UNICODE operation
	WIN32_FIND_DATAW ffd;
	HANDLE handle = ::FindFirstFile(name.c_str(), &ffd);
	if(handle != INVALID_HANDLE_VALUE)
	{
		BOOL cont;
		do
		{
			if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				ttstr file( reinterpret_cast<tjs_char*>(ffd.cFileName) );
				tjs_char *p = file.Independ();
				while(*p)
				{
					// make all characters small
					if(*p >= TJS_W('A') && *p <= TJS_W('Z'))
						*p += TJS_W('a') - TJS_W('A');
					p++;
				}
				lister->Add(file);
			}

			cont = ::FindNextFile(handle, &ffd);
		} while(cont);
		FindClose(handle);
	}
#else
	tjs_string wname(name.AsStdString());
	std::string nname;
	if( TVPUtf16ToUtf8(nname, wname) ) {
#if defined(__ANDROID__)
		AAssetManager *asset_manager = AndroidAssetManager_Get_AssetManager();
		AAssetDir* android_dr;
#endif
#if defined(__vita__)
		SceUID dr;
		if( ( dr = sceIoDopen(nname.c_str()) ) >= 0 )
#else
		DIR* dr;
		if( ( dr = opendir(nname.c_str()) ) != nullptr )
#endif
		{
#if defined(__vita__)
			SceIoDirent entry;
			while( sceIoDread( dr, &entry ) > 0 )
#else
			struct dirent* entry;
			while( ( entry = readdir( dr ) ) != nullptr )
#endif
			{
#if defined(__vita__)
				if (SCE_S_ISREG(entry.d_stat.st_mode))
#else
				if( entry->d_type == DT_REG )
#endif
				{
					tjs_char fname[256];
#if defined(__vita__)
					tjs_int count = TVPUtf8ToWideCharString( entry.d_name, fname );
#else
					tjs_int count = TVPUtf8ToWideCharString( entry->d_name, fname );
#endif
					fname[count] = TJS_W('\0');
					ttstr file(fname);
#ifdef KRKRZ_CASE_INSENSITIVE
					tjs_char *p = file.Independ();
					while(*p) {
						// make all characters small
						if(*p >= TJS_W('A') && *p <= TJS_W('Z'))
							*p += TJS_W('a') - TJS_W('A');
						p++;
					}
#endif
					lister->Add(file);
				}
				// entry->d_type == DT_UNKNOWN
			}
#if defined(__vita__)
			sceIoDclose( dr );
#else
			closedir( dr );
#endif
		}
#if defined(__ANDROID__)
		// Skip the leading slash.
		else if ( asset_manager != NULL && nname.length() > 0 && nname[0] == '/' && (AndroidAssetManager_Check_Directory_Existent(asset_manager, nname.c_str() + 1)) && (android_dr = AAssetManager_openDir( asset_manager, nname.c_str() + 1 )) )
		{
			const char* filename = nullptr;
			do {
				filename = AAssetDir_getNextFileName( android_dr );
				if( filename ) {
					tjs_char fname[256];
					tjs_int count = TVPUtf8ToWideCharString( filename, fname );
					fname[count] = TJS_W('\0');
					ttstr file( fname );
#ifdef KRKRZ_CASE_INSENSITIVE
					tjs_char *p = file.Independ();
					while(*p) {
						// make all characters small
						if(*p >= TJS_W('A') && *p <= TJS_W('Z'))
							*p += TJS_W('a') - TJS_W('A');
						p++;
					}
#endif
					lister->Add( file );
				}
			} while( filename );
			AAssetDir_close( android_dr );
		}
#endif
	}
#endif
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPFileMedia::GetLocallyAccessibleName(ttstr &name)
{
#ifdef _WIN32
	ttstr newname;

	const tjs_char *ptr = name.c_str();

	if(TJS_strncmp(ptr, TJS_W("./"), 2))
	{
		// differs from "./",
		// this may be a UNC file name.
		// UNC first two chars must be "\\\\" ?
		// AFAIK 32-bit version of Windows assumes that '/' can be used as a path
		// delimiter. Can UNC "\\\\" be replaced by "//" though ?

		newname = ttstr(TJS_W("\\\\")) + ptr;
	}
	else
	{
		ptr += 2;  // skip "./"
		if(!*ptr) {
			newname = TJS_W("");
		} else {
			tjs_char dch = *ptr;
			// dch not in 'a'-'z', 'A'-'Z'
			if (!(dch >= TJS_W('a') && dch <= TJS_W('z')) && !(dch >= TJS_W('A') && dch <= TJS_W('Z'))) {
				newname = TJS_W("");
			} else {
				ptr++;
				if(*ptr != TJS_W('/')) {
					newname = TJS_W("");
				} else {
					newname = ttstr(dch) + TJS_W(":") + ptr;
				}
			}
		}
	}

	// change path delimiter to '\\'
    if (!newname.IsEmpty()) {
        tjs_char *pp = newname.Independ();
        while (*pp) {
            if (*pp == TJS_W('/')) *pp = TJS_W('\\');
            pp++;
        }
    }
	name = newname;
#else
	tjs_string wname(name.AsStdString());
	std::string nname;
	if (!TVPUtf16ToUtf8(nname, wname))
	{
		name.Clear();
		return;
	}

#if 0
	ttstr newname;

	const tjs_char *ptr = name.c_str();
	if( *ptr == TJS_W('.') ) ptr++;
	while( (*ptr == TJS_W('/') || *ptr == TJS_W('\\')) && (ptr[1] == TJS_W('/') || ptr[1] == TJS_W('\\')) ) ptr++;
	newname = ttstr(ptr);
	// change path delimiter to '/'
	if (!newname.IsEmpty()) {
		tjs_char *pp = newname.Independ();
		while(*pp)
		{
			if(*pp == TJS_W('\\')) *pp = TJS_W('/');
			pp++;
		}
	}
	name = newname;
#endif

#if defined(__ANDROID__)
	AAssetManager *asset_manager = AndroidAssetManager_Get_AssetManager();
#endif

	std::string nnewname;
	const char *ptr = nname.c_str();
	tjs_int path_entries = 0;
	std::string domain_name;

	while (*ptr)
	{
		const char *ptr_end = ptr;
		while (*ptr_end && (*ptr_end != '/' && *ptr_end != '\\'))
		{
			ptr_end += 1;
		}
		if (ptr_end == ptr)
		{
			break;
		}
		const char *ptr_cur = ptr;
		std::string nwalker(ptr, ptr_end - ptr);
		while (*ptr_end && (*ptr_end == '/' || *ptr_end == '\\'))
		{
			ptr_end += 1;
		}
		ptr = ptr_end;
		if (path_entries == 0)
		{
			domain_name = nwalker;
		}
		path_entries += 1;
		if (nwalker == "." || (path_entries == 1 && nwalker == "?"))
		{
			continue;
		}

		DIR *dirp;
#if defined(__ANDROID__)
		AAssetDir* android_dr;
#endif
		struct dirent *direntp;
		if ((path_entries != 2) || (path_entries == 2 && domain_name != "?"))
		{
			nnewname += "/";
		}
		else
		{
			nnewname += "./";
		}

		if ((dirp = opendir(nnewname.c_str())))
		{
			bool found = false;
			bool is_directory = false;
			while (found == false && (direntp = readdir(dirp)) != nullptr)
			{
				if (!SDL_strcasecmp(nwalker.c_str(), direntp->d_name))
				{
					nnewname += direntp->d_name;
					found = true;
					break;
				}
			}
			closedir(dirp);
			if (!found)
			{
				nnewname += ptr_cur;
				break;
			}
		}
#if defined(__ANDROID__)
		// Skip the leading slash.
		else if ( asset_manager != NULL && nnewname.length() > 0 && nnewname[0] == '/' && (AndroidAssetManager_Check_Directory_Existent(asset_manager, nnewname.c_str() + 1)) && (android_dr = AAssetManager_openDir( asset_manager, nnewname.c_str() + 1 )) )
		{
			const char* filename = nullptr;
			bool found = false;
			bool is_directory = false;
			while (found == false && (filename = AAssetDir_getNextFileName( android_dr )) != nullptr)
			{
				if (!SDL_strcasecmp(nwalker.c_str(), filename))
				{
					nnewname += filename;
					found = true;
					break;
				}
			}
			AAssetDir_close( android_dr );
			if (!found)
			{
				nnewname += ptr_cur;
				break;
			}
		}
#endif
		else
		{
			if (errno == EPERM || errno == EACCES) // Most likely inside the iOS sandbox, so just append the name
			{
				nnewname += nwalker;
			}
			else if (errno == ENOENT || errno == ENOTDIR)
			{
				nnewname += ptr_cur;
				break;
			}
			else // Other error, probably can't access anyway
			{
				nnewname += ptr_cur;
				break;
			}
		}
	}

	if (path_entries == 1 && domain_name == "?")
	{
		name = ttstr(TJS_W("./"));
		return;
	}

	tjs_string wnewname;
	if (!TVPUtf8ToUtf16(wnewname, nnewname))
	{
		name.Clear();
		return;
	}

	name = ttstr(wnewname);
#endif

}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPFileMedia::GetLocalName(ttstr &name)
{
	ttstr tmp = name;
	GetLocallyAccessibleName(tmp);
	if(tmp.IsEmpty()) TVPThrowExceptionMessage(TVPCannotGetLocalName, name);
	name = tmp;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
iTVPStorageMedia * TVPCreateFileMedia()
{
	return new tTVPFileMedia;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// TVPPreNormalizeStorageName
//---------------------------------------------------------------------------
void TVPPreNormalizeStorageName(ttstr &name)
{
	// if the name is an OS's native expression, change it according with the
	// TVP storage system naming rule.
	tjs_int namelen = name.GetLen();
	if(namelen == 0) return;

#if defined(__SWITCH__) || defined(__vita__)
	// HACK for Switch and Vita: colon in filesystem causes a conflict
	if ((TJS_strstr(name.c_str(), TJS_W("file:")) == nullptr) && (TJS_strchr(name.c_str(), TJS_W(':')) != nullptr))
	{
		ttstr newname(TJS_W("file://?/"));
		newname += name;
		name = newname;
	}
#endif

#ifdef _WIN32
	if(namelen >= 2)
	{
		if((name[0] >= TJS_W('a') && name[0]<=TJS_W('z') ||
			name[0] >= TJS_W('A') && name[0]<=TJS_W('Z') ) &&
			name[1] == TJS_W(':'))
		{
			// Windows drive:path expression
			ttstr newname(TJS_W("file://./"));
			newname += name[0];
			newname += (name.c_str()+2);
            name = newname;
			return;
		}
	}

	if(namelen>=3)
	{
		if(name[0] == TJS_W('\\') && name[1] == TJS_W('\\') ||
			name[0] == TJS_W('/') && name[1] == TJS_W('/'))
		{
			// unc expression
			name = ttstr(TJS_W("file:")) + name;
			return;
		}
	}
#else
	if (namelen >= 1)
	{
		if (name[0] == TJS_W('/'))
		{
			// POSIX absolute path
			name = ttstr(TJS_W("file://./")) + name;
			return;
		}
	}
#endif
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPGetTemporaryName
//---------------------------------------------------------------------------
static tjs_int TVPTempUniqueNum = 0;
static tTJSCriticalSection TVPTempUniqueNumCS;
static ttstr TVPTempPath;
bool TVPTempPathInit = false;
static tjs_int TVPProcessID;
ttstr TVPGetTemporaryName()
{
	tjs_int num;

	{
		tTJSCriticalSectionHolder holder(TVPTempUniqueNumCS);

		if(!TVPTempPathInit)
		{
			//tjs_char tmp[MAX_PATH+1];
			//TVPUtf8ToWideCharString( Application->GetCachePath()->c_str(), static_cast<tjs_char*>(tmp) );
#if 0
			TVPTempPath = ttstr( Application->GetCachePath()->c_str() );
#endif
#if defined(__SWITCH__)
			TVPTempPath = ttstr( "sdmc:/tmp/" );
#elif defined(__vita__)
			TVPTempPath = ttstr( "./tmp/" );
#elif defined(_WIN32)
			tjs_char tmp[MAX_PATH+1];
			::GetTempPath(MAX_PATH, tmp);
			TVPTempPath = tmp;
#else
			TVPTempPath = ttstr( "/tmp/" );
#endif

#ifdef _WIN32
			if(TVPTempPath.GetLastChar() != TJS_W('/') && TVPTempPath.GetLastChar() != TJS_W('\\')) TVPTempPath += TJS_W("\\");
#else
			if(TVPTempPath.GetLastChar() != TJS_W('/')) TVPTempPath += TJS_W("/");
#endif
#ifdef _WIN32
			TVPProcessID = static_cast<tjs_int>( GetCurrentProcessId() );
#else
			TVPProcessID = static_cast<tjs_int>( getpid() );
#endif
			TVPTempUniqueNum = static_cast<tjs_int>( TVPGetRoughTickCount32() );
			TVPTempPathInit = true;
		}
		num = TVPTempUniqueNum ++;
	}

	unsigned char buf[16];
	TVPGetRandomBits128(buf);
	tjs_char random[128];
	TJS_snprintf(random, sizeof(random)/sizeof(tjs_char), TJS_W("%02x%02x%02x%02x%02x%02x"),
		buf[0], buf[1], buf[2], buf[3],
		buf[4], buf[5]);

	return TVPTempPath + TJS_W("krkr_") + ttstr(random) +
		TJS_W("_") + ttstr(num) + TJS_W("_") + ttstr(TVPProcessID);
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// TVPRemoveFile
//---------------------------------------------------------------------------
bool TVPRemoveFile(const ttstr &name)
{
#ifdef _WIN32
	return 0!=::DeleteFile(name.c_str());
#else
	std::string filename;
	if( TVPUtf16ToUtf8( filename, name.AsStdString() ) ) {
#if defined(__vita__)
		bool res = 0 == sceIoRemove(filename.c_str());
#else
		bool res = 0 == remove(filename.c_str());
#endif
		if (res)
		{
			Application->SyncSavedata();
		}
		return res;
	} else {
		return false;
	}
#endif
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TVPRemoveFolder
//---------------------------------------------------------------------------
bool TVPRemoveFolder(const ttstr &name)
{
#ifdef _WIN32
	return 0!=RemoveDirectory(name.c_str());
#else
	std::string filename;
	if( TVPUtf16ToUtf8( filename, name.AsStdString() ) ) {
#if defined(__vita__)
		bool res = 0==sceIoRmdir(filename.c_str());
#else
		bool res = 0==rmdir(filename.c_str());
#endif
		if (res)
		{
			Application->SyncSavedata();
		}
		return res;
	} else {
		return false;
	}
#endif
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPGetAppPath
//---------------------------------------------------------------------------
ttstr TVPGetAppPath()
{
	static ttstr exepath(TVPExtractStoragePath(TVPNormalizeStorageName(ExePath())));
	return exepath;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// TVPOpenStream
//---------------------------------------------------------------------------
tTJSBinaryStream * TVPOpenStream(const ttstr & _name, tjs_uint32 flags)
{
	// open storage named "name".
	// currently only local/network(by OS) storage systems are supported.
	if(_name.IsEmpty())
		TVPThrowExceptionMessage(TVPCannotOpenStorage, TJS_W("\"\""));

	ttstr origname = _name;
	ttstr name(_name);
	TVPGetLocalName(name);

	return new tTVPLocalFileStream(origname, name, flags);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// TVPCheckExistantLocalFile
//---------------------------------------------------------------------------
bool TVPCheckExistentLocalFile(const ttstr &name)
{
#ifdef _WIN32
	DWORD attrib = ::GetFileAttributes(name.c_str());
	if(attrib == 0xffffffff || (attrib & FILE_ATTRIBUTE_DIRECTORY))
		return false; // not a file
	else
		return true; // a file
#else
	std::string filename;
	if( TVPUtf16ToUtf8( filename, name.AsStdString() ) ) {
#if defined(__vita__)
		SceIoStat st;
		if( sceIoGetstat( filename.c_str(), &st) >= 0)
			if( SCE_S_ISREG(st.st_mode) )
#else
		struct stat st;
		if( stat( filename.c_str(), &st) == 0)
			if( S_ISREG(st.st_mode) )
#endif
				return true;
#if defined(__ANDROID__)
		AAssetManager *asset_manager = AndroidAssetManager_Get_AssetManager();
		// Skip the leading slash.
		if (asset_manager != NULL && filename.length() > 0 && filename[0] == '/')
		{
			AAsset* asset = AAssetManager_open( asset_manager, filename.c_str() + 1, AASSET_MODE_UNKNOWN);
			bool result = asset != NULL;
			if ( result )
			{
				AAsset_close( asset );
				return result;
			}
		}
#endif
	}
	return false;
#endif
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPCheckExistantLocalFolder
//---------------------------------------------------------------------------
bool TVPCheckExistentLocalFolder(const ttstr &name)
{
#ifdef _WIN32
	DWORD attrib = GetFileAttributes(name.c_str());
	if(attrib != 0xffffffff && (attrib & FILE_ATTRIBUTE_DIRECTORY))
		return true; // a folder
	else
		return false; // not a folder
#else
	std::string filename;
	if( TVPUtf16ToUtf8( filename, name.AsStdString() ) ) {
#if defined(__vita__)
		SceIoStat st;
		if( sceIoGetstat( filename.c_str(), &st) >= 0)
			if( SCE_S_ISDIR(st.st_mode) )
#else
		struct stat st;
		if( stat( filename.c_str(), &st) == 0)
			if( S_ISDIR(st.st_mode) )
#endif
				return true;
#if defined(__ANDROID__)
		AAssetManager *asset_manager = AndroidAssetManager_Get_AssetManager();
		// Skip the leading slash.
		if (asset_manager != NULL && filename.length() > 0 && filename[0] == '/')
		{
			AAssetDir* asset = AAssetManager_openDir( asset_manager, filename.c_str() + 1 );
			bool result = AndroidAssetManager_Check_Directory_Existent(asset_manager, filename.c_str() + 1);
			if ( result )
			{
				return result;
			}
		}
#endif
	}
	return false;
#endif
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// TVPOpenArchive
//---------------------------------------------------------------------------
tTVPArchive * TVPOpenArchive(const ttstr & name)
{
	tTVPArchive * archive = TVPOpenSusieArchive(name); // in SusieArchive.h
	if(!archive) return new tTVPXP3Archive(name); else return archive;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPLocalExtrectFilePath
//---------------------------------------------------------------------------
ttstr TVPLocalExtractFilePath(const ttstr & name)
{
	// this extracts given name's path under local filename rule
	const tjs_char *p = name.c_str();
	tjs_int i = name.GetLen() -1;
	for(; i >= 0; i--)
	{
		if(p[i] == TJS_W(':') || p[i] == TJS_W('/') ||
			p[i] == TJS_W('\\'))
			break;
	}
	return ttstr(p, i + 1);
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPCreateFolders
//---------------------------------------------------------------------------
static bool _TVPCreateFolders(const ttstr &folder)
{
	// create directories along with "folder"
	if(folder.IsEmpty()) return true;

	if(TVPCheckExistentLocalFolder(folder))
		return true; // already created

	const tjs_char *p = folder.c_str();
	tjs_int i = folder.GetLen() - 1;

	if(p[i] == TJS_W(':')) return true;

	while(i >= 0 && (p[i] == TJS_W('/') || p[i] == TJS_W('\\'))) i--;

	if(i >= 0 && p[i] == TJS_W(':')) return true;

	for(; i >= 0; i--)
	{
		if(p[i] == TJS_W(':') || p[i] == TJS_W('/') ||
			p[i] == TJS_W('\\'))
			break;
	}

	ttstr parent(p, i + 1);

	if(!_TVPCreateFolders(parent)) return false;

#ifdef _WIN32
	BOOL res = ::CreateDirectory(folder.c_str(), NULL);
	return 0!=res;
#else
	std::string filename;
	int res = -1;
	if( TVPUtf16ToUtf8( filename, folder.AsStdString() ) ) {
#if defined(__vita__)
		res = sceIoMkdir( filename.c_str(), 0777 );
#else
		res = mkdir( filename.c_str(), 0777 );
#endif
	}
	return 0 == res;
#endif
}

bool TVPCreateFolders(const ttstr &folder)
{
	if(folder.IsEmpty()) return true;

	const tjs_char *p = folder.c_str();
	tjs_int i = folder.GetLen() - 1;

	if(p[i] == TJS_W(':')) return true;

	if(p[i] == TJS_W('/') || p[i] == TJS_W('\\')) i--;

	return _TVPCreateFolders(ttstr(p, i+1));
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTVPLocalFileStream
//---------------------------------------------------------------------------
tTVPLocalFileStream::tTVPLocalFileStream(const ttstr &origname,
	const ttstr &localname, tjs_uint32 flag)
{
	tjs_uint32 access = flag & TJS_BS_ACCESS_MASK;
	io_handle = NULL;
	written = false;
	const char* mode = "rb";
	switch(access)
	{
	case TJS_BS_READ:
		mode = "rb";		break;
	case TJS_BS_WRITE:
		mode = "wb";		break;
	case TJS_BS_APPEND:
		mode = "ab";		break;
	case TJS_BS_UPDATE:
		mode = "rb+";		break;
	}

	tjs_int trycount = 0;
	std::string filename;
	TVPUtf16ToUtf8( filename, localname.AsStdString() );

retry:
	io_handle = SDL_RWFromFile(filename.c_str(), mode);
	if(io_handle == nullptr)
	{
		if(trycount == 0 && access == TJS_BS_WRITE)
		{
			trycount++;

			// retry after creating the folder
			if(TVPCreateFolders(TVPLocalExtractFilePath(localname)))
				goto retry;
		}
#ifdef __ANDROID__
		// Special case for Android and reading the asset filesystem from AAssetManager
		if (trycount == 0 && access == TJS_BS_READ && filename.length() > 0 && filename[0] == '/')
		{
			trycount++;

			// retry after removing the leading slash
			filename.erase(0, 1);
			goto retry;
		}
#endif
		TVPThrowExceptionMessage(TVPCannotOpenStorage, origname);
	}

	if (access == TJS_BS_APPEND) // move the file pointer to last
	{
		SetEndOfStorage();
	}

	// push current tick as an environment noise
	tjs_uint32 tick = TVPGetRoughTickCount32();
	TVPPushEnvironNoise(&tick, sizeof(tick));
}
//---------------------------------------------------------------------------
tTVPLocalFileStream::~tTVPLocalFileStream()
{
	if (io_handle != nullptr)
	{
		SDL_RWclose(io_handle);
	}

	// push current tick as an environment noise
	// (timing information from file accesses may be good noises)
	tjs_uint32 tick = TVPGetRoughTickCount32();
	TVPPushEnvironNoise(&tick, sizeof(tick));

	if (written)
	{
		Application->SyncSavedata();
	}
}
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTVPLocalFileStream::Seek(tjs_int64 offset, tjs_int whence)
{
	int dwmm;
	switch(whence)
	{
	case TJS_BS_SEEK_SET:	dwmm = RW_SEEK_SET;	break;
	case TJS_BS_SEEK_CUR:	dwmm = RW_SEEK_CUR;	break;
	case TJS_BS_SEEK_END:	dwmm = RW_SEEK_END;	break;
	default:				dwmm = RW_SEEK_SET;	break; // may be enough
	}

	Sint64 low = SDL_RWseek(io_handle, offset, dwmm);
	if (low < 0)
	{
		TVPThrowExceptionMessage(TVPSeekError);
	}
	return (tjs_uint64)low;
}
//---------------------------------------------------------------------------
tjs_uint TJS_INTF_METHOD tTVPLocalFileStream::Read(void *buffer, tjs_uint read_size)
{
	size_t ret = SDL_RWread(io_handle, buffer, 1, read_size);
	return (tjs_uint)ret;
}
//---------------------------------------------------------------------------
tjs_uint TJS_INTF_METHOD tTVPLocalFileStream::Write(const void *buffer, tjs_uint write_size)
{
	written = true;
	size_t ret = SDL_RWwrite(io_handle, buffer, 1, write_size);
	return (tjs_uint)ret;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPLocalFileStream::SetEndOfStorage()
{
	Seek(0, TJS_BS_SEEK_END);
}
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTVPLocalFileStream::GetSize()
{
#if 0
	tjs_uint64 oldpos = Seek(0, TJS_BS_SEEK_CUR);
	tjs_uint64 retpos = Seek(0, TJS_BS_SEEK_END);
	Seek(oldpos, TJS_BS_SEEK_SET);
	return retpos;
#endif
	Sint64 low = SDL_RWsize(io_handle);
	if (low < 0)
	{
		TVPThrowExceptionMessage(TVPSeekError);
	}
	return (tjs_uint64)low;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTVPIStreamAdapter
//---------------------------------------------------------------------------
/*
	this class provides adapter for COM's IStream
*/
tTVPIStreamAdapter::tTVPIStreamAdapter(tTJSBinaryStream *ref)
{
	Stream = ref;
	RefCount = 1;
}
//---------------------------------------------------------------------------
tTVPIStreamAdapter::~tTVPIStreamAdapter()
{
	delete Stream;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::QueryInterface(REFIID riid,
		void **ppvObject)
{
	if(!ppvObject) return E_INVALIDARG;

	*ppvObject=NULL;
	if(!memcmp(&riid,&IID_IUnknown,16))
		*ppvObject=(IUnknown*)this;
	else if(!memcmp(&riid,&IID_ISequentialStream,16))
		*ppvObject=(ISequentialStream*)this;
	else if(!memcmp(&riid,&IID_IStream,16))
		*ppvObject=(IStream*)this;

	if(*ppvObject)
	{
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE tTVPIStreamAdapter::AddRef(void)
{
	return ++ RefCount;
}
//---------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE tTVPIStreamAdapter::Release(void)
{
	if(RefCount == 1)
	{
		delete this;
		return 0;
	}
	else
	{
		return --RefCount;
	}
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::Read(void *pv, ULONG cb, ULONG *pcbRead)
{
	try
	{
		ULONG read;
		read = Stream->Read(pv, cb);
		if(pcbRead) *pcbRead = read;
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::Write(const void *pv, ULONG cb,
		ULONG *pcbWritten)
{
	try
	{
		ULONG written;
		written = Stream->Write(pv, cb);
		if(pcbWritten) *pcbWritten = written;
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::Seek(LARGE_INTEGER dlibMove,
	DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
{
	try
	{
		switch(dwOrigin)
		{
		case STREAM_SEEK_SET:
			if(plibNewPosition)
				(*plibNewPosition).QuadPart =
					Stream->Seek(dlibMove.QuadPart, TJS_BS_SEEK_SET);
			else
					Stream->Seek(dlibMove.QuadPart, TJS_BS_SEEK_SET);
			break;
		case STREAM_SEEK_CUR:
			if(plibNewPosition)
				(*plibNewPosition).QuadPart =
					Stream->Seek(dlibMove.QuadPart, TJS_BS_SEEK_CUR);
			else
					Stream->Seek(dlibMove.QuadPart, TJS_BS_SEEK_CUR);
			break;
		case STREAM_SEEK_END:
			if(plibNewPosition)
				(*plibNewPosition).QuadPart =
					Stream->Seek(dlibMove.QuadPart, TJS_BS_SEEK_END);
			else
					Stream->Seek(dlibMove.QuadPart, TJS_BS_SEEK_END);
			break;
		default:
			return E_FAIL;
		}
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::SetSize(ULARGE_INTEGER libNewSize)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::CopyTo(IStream *pstm, ULARGE_INTEGER cb,
	ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::Commit(DWORD grfCommitFlags)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::Revert(void)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::LockRegion(ULARGE_INTEGER libOffset,
	ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::UnlockRegion(ULARGE_INTEGER libOffset,
	ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::Stat(STATSTG *pstatstg, DWORD grfStatFlag)
{
	// This method imcompletely fills the target structure, because some
	// informations like access mode or stream name are already lost
	// at this point.

	if(pstatstg)
	{
#ifdef _WIN32
		ZeroMemory(pstatstg, sizeof(*pstatstg));
#else
		memset(pstatstg, 0, sizeof(*pstatstg));
#endif

#ifdef _WIN32
		// pwcsName
		// this object's storage pointer does not have a name ...
		if(!(grfStatFlag &  STATFLAG_NONAME))
		{
			// anyway returns an empty string
			LPWSTR str = (LPWSTR)CoTaskMemAlloc(sizeof(*str));
			if(str == NULL) return E_OUTOFMEMORY;
			*str = TJS_W('\0');
			pstatstg->pwcsName = str;
		}
#endif

		// type
		pstatstg->type = STGTY_STREAM;

		// cbSize
		pstatstg->cbSize.QuadPart = Stream->GetSize();

		// mtime, ctime, atime unknown

		// grfMode unknown
		pstatstg->grfMode = STGM_DIRECT | STGM_READWRITE | STGM_SHARE_DENY_WRITE ;
			// Note that this method always returns flags above, regardless of the
			// actual mode.
			// In the return value, the stream is to be indicated that the
			// stream can be written, but of cource, the Write method will fail
			// if the stream is read-only.

		// grfLockSuppoted
		pstatstg->grfLocksSupported = 0;

		// grfStatBits unknown
	}
	else
	{
		return E_INVALIDARG;
	}

	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::Clone(IStream **ppstm)
{
	return E_NOTIMPL;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// IStream creator
//---------------------------------------------------------------------------
IStream * TVPCreateIStream(const ttstr &name, tjs_uint32 flags)
{
	// convert tTJSBinaryStream to IStream thru TStream

	tTJSBinaryStream *stream0 = NULL;
	try
	{
		stream0 = TVPCreateStream(name, flags);
	}
	catch(...)
	{
		if(stream0) delete stream0;
		return NULL;
	}

	IStream *istream = new tTVPIStreamAdapter(stream0);

	return istream;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// tTVPBinaryStreamAdapter
//---------------------------------------------------------------------------
tTVPBinaryStreamAdapter::tTVPBinaryStreamAdapter(IStream *ref)
{
	Stream = ref;
	Stream->AddRef();
}
//---------------------------------------------------------------------------
tTVPBinaryStreamAdapter::~tTVPBinaryStreamAdapter()
{
	Stream->Release();
}
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTVPBinaryStreamAdapter::Seek(tjs_int64 offset, tjs_int whence)
{
	DWORD origin;

	switch(whence)
	{
	case TJS_BS_SEEK_SET:			origin = STREAM_SEEK_SET;		break;
	case TJS_BS_SEEK_CUR:			origin = STREAM_SEEK_CUR;		break;
	case TJS_BS_SEEK_END:			origin = STREAM_SEEK_END;		break;
	default:						origin = STREAM_SEEK_SET;		break;
	}

	LARGE_INTEGER ofs;
	ULARGE_INTEGER newpos;

	ofs.QuadPart = 0;
	HRESULT hr = Stream->Seek(ofs, STREAM_SEEK_CUR, &newpos);
	bool orgpossaved;
	LARGE_INTEGER orgpos;
	if(FAILED(hr))
	{
		orgpossaved = false;
	}
	else
	{
		orgpossaved = true;
		*(LARGE_INTEGER*)&orgpos = *(LARGE_INTEGER*)&newpos;
	}

	ofs.QuadPart = offset;

	hr = Stream->Seek(ofs, origin, &newpos);
	if(FAILED(hr))
	{
		if(orgpossaved)
		{
			Stream->Seek(orgpos, STREAM_SEEK_SET, &newpos);
		}
		else
		{
			TVPThrowExceptionMessage(TVPSeekError);
		}
	}

	return newpos.QuadPart;
}
//---------------------------------------------------------------------------
tjs_uint TJS_INTF_METHOD tTVPBinaryStreamAdapter::Read(void *buffer, tjs_uint read_size)
{
	ULONG cb = read_size;
	ULONG read;
	HRESULT hr = Stream->Read(buffer, cb, &read);
	if(FAILED(hr)) read = 0;
	return read;
}
//---------------------------------------------------------------------------
tjs_uint TJS_INTF_METHOD tTVPBinaryStreamAdapter::Write(const void *buffer, tjs_uint write_size)
{
	ULONG cb = write_size;
	ULONG written;
	HRESULT hr = Stream->Write(buffer, cb, &written);
	if(FAILED(hr)) written = 0;
	return written;
}
//---------------------------------------------------------------------------
tjs_uint64 TJS_INTF_METHOD tTVPBinaryStreamAdapter::GetSize()
{
	HRESULT hr;
	STATSTG stg;

	hr = Stream->Stat(&stg, STATFLAG_NONAME);
	if(FAILED(hr))
	{
		return inherited::GetSize(); // use default routine
	}

	return stg.cbSize.QuadPart;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTVPBinaryStreamAdapter::SetEndOfStorage()
{
	ULARGE_INTEGER pos;
	pos.QuadPart = GetPosition();
	HRESULT hr = Stream->SetSize(pos);
	if(FAILED(hr)) TVPThrowExceptionMessage(TVPTruncateError);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TVPCreateBinaryStreamAdapter
//---------------------------------------------------------------------------
tTJSBinaryStream * TVPCreateBinaryStreamAdapter(IStream *refstream)
{
	return new  tTVPBinaryStreamAdapter(refstream);
}
//---------------------------------------------------------------------------









//---------------------------------------------------------------------------
// Plugin location related
//---------------------------------------------------------------------------
static tjs_int TVPPluginSearchPathOptionsGeneration = 0;
static tjs_string TVPPluginSearchPath = TJS_W("");
//---------------------------------------------------------------------------
static void TVPInitPluginSearchPathOptions()
{
	if (TVPPluginSearchPathOptionsGeneration == TVPGetCommandLineArgumentGeneration()) return;
	TVPPluginSearchPathOptionsGeneration = TVPGetCommandLineArgumentGeneration();

	tTJSVariant val;
	tjs_string searchpath;
	char *searchpath_cstr = SDL_getenv("KRKRSDL2_PATH");
	if (searchpath_cstr != NULL)
	{
		std::string searchpath_utf8 = searchpath_cstr;
		TVPUtf8ToUtf16(searchpath, searchpath_utf8);
	}
	else
	{
		searchpath = TJS_W("${ORIGIN}:${ORIGIN}/system:${ORIGIN}/plugin");
	}
	if (TVPGetCommandLine(TJS_W("-krkrsdl2_pluginsearchpath"), &val))
	{
		ttstr str(val);
		searchpath = str.c_str();
	}
	tjs_string exename = ExePath();
	tjs_string exepath = ExcludeTrailingSlash(ExtractFileDir(exename));
	searchpath = ApplicationSpecialPath::ReplaceStringAll(searchpath, TJS_W("${ORIGIN}"), exepath);
	searchpath = ApplicationSpecialPath::ReplaceStringAll(searchpath, TJS_W("$ORIGIN"), exepath);
	TVPPluginSearchPath = searchpath;
}
//---------------------------------------------------------------------------
static void TVPLocatePlugin(const ttstr &aname, ttstr &fpname, ttstr &flname)
{
	if (TJS_strstr(TJS_W(":"), aname.c_str()) != NULL)
	{
		// Absolute path; use this path only
		ttstr place(TVPGetPlacedPath(aname));
		if (!place.IsEmpty())
		{
			fpname = place;
		}
	}
	else
	{
		// Relative path; try various ways to find the plugin
		tjs_string cur_name = TVPExtractStorageName(aname).c_str();
#ifdef _WIN32
		ttstr place(TVPGetPlacedPath(cur_name));
		if (!place.IsEmpty())
		{
			fpname = place;
		}
#endif
		if (fpname.GetLen() == 0)
		{
#ifndef _WIN32
			{
				tjs_int len = cur_name.length();
				if (len > 4 && cur_name[len - 1] == TJS_W('l') && cur_name[len - 2] == TJS_W('l') && cur_name[len - 3] == TJS_W('d') && cur_name[len - 4] == TJS_W('.'))
				{
					tjs_string extso = ChangeFileExt(cur_name, TJS_W("so"));
					cur_name = extso;
				}
			}
#endif
			TVPInitPluginSearchPathOptions();
			tjs_string searchpath = TVPPluginSearchPath;
			if (!searchpath.empty())
			{
				size_t searchpath_pos = 0;
				tjs_string searchpath_single;
				tjs_string colon = TJS_W(":");
				while (true)
				{
					searchpath_pos = searchpath.find(colon);
					if (searchpath_pos == tjs_string::npos)
					{
						searchpath_single = searchpath;
					}
					else
					{
						searchpath_single = searchpath.substr(0, searchpath_pos);
					}
					{
						if (searchpath_single.empty())
						{
							searchpath_single = TJS_W(".");
						}
						searchpath_single = IncludeTrailingSlash(searchpath_single);
						searchpath_single += cur_name;
						if (TVPCheckExistentLocalFile(searchpath_single))
						{
							flname = searchpath_single;
							break;
						}
					}
					if (searchpath_pos == tjs_string::npos)
					{
						break;
					}
					searchpath.erase(0, searchpath_pos + colon.length());
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
// tTVPPluginHolder
//---------------------------------------------------------------------------
tTVPPluginHolder::tTVPPluginHolder(const ttstr &aname)
: LocalTempStorageHolder(nullptr)
{
#if 0
	// /data/data/(パッケージ名)/lib/
	tjs_string sopath = tjs_string(TJS_W("file://")) + tjs_string(Application->GetSoPath()) + aname.AsStdString();
	//tjs_string sopath = tjs_string(TJS_W("/data/data/")) + tjs_string(Application->GetPackageName()) + tjs_string(TJS_W("/lib/")) + aname.AsStdString();
	ttstr place( sopath.c_str() );
	LocalTempStorageHolder = new tTVPLocalTempStorageHolder(place);
#endif
	LocalTempStorageHolder = NULL;

	ttstr fpname;
	ttstr flname;
	TVPLocatePlugin(aname, fpname, flname);
	if (fpname.GetLen() != 0)
	{
		LocalTempStorageHolder = new tTVPLocalTempStorageHolder(fpname);
	}
	else if (flname.GetLen() != 0)
	{
		LocalPath = flname;
	}
}
//---------------------------------------------------------------------------
tTVPPluginHolder::~tTVPPluginHolder()
{
	if(LocalTempStorageHolder)
	{
		delete LocalTempStorageHolder;
	}
}
//---------------------------------------------------------------------------
const ttstr & tTVPPluginHolder::GetLocalName() const
{
	if(LocalTempStorageHolder) return LocalTempStorageHolder->GetLocalName();
	return LocalPath;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TVPCreateNativeClass_Storages
//---------------------------------------------------------------------------
tTJSNativeClass * TVPCreateNativeClass_Storages()
{
	tTJSNC_Storages *cls = new tTJSNC_Storages();


	// setup some platform-specific members
//----------------------------------------------------------------------

//-- methods

//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/searchCD)
{
	return TJS_E_NOTIMPL;
}
TJS_END_NATIVE_STATIC_METHOD_DECL_OUTER(/*object to register*/cls,
	/*func. name*/searchCD)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/getLocalName)
{
	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		ttstr str(TVPNormalizeStorageName(*param[0]));
		TVPGetLocalName(str);
		*result = str;
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_METHOD_DECL_OUTER(/*object to register*/cls,
	/*func. name*/getLocalName)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/selectFile)
{
	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	iTJSDispatch2 * dsp =  param[0]->AsObjectNoAddRef();

	bool res = TVPSelectFile(dsp);

	if(result) *result = (tjs_int)res;

	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_METHOD_DECL_OUTER(/*object to register*/cls,
	/*func. name*/selectFile)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/isExistentPlugin)
{
	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	ttstr path = *param[0];

	tjs_int res = 0;
	ttstr fpname;
	ttstr flname;
	TVPLocatePlugin(path, fpname, flname);
	if (fpname.GetLen() > 0 || flname.GetLen() > 0)
	{
		res = 1;
	}

	if(result)
		*result = res;

	return TJS_S_OK;
}
TJS_END_NATIVE_STATIC_METHOD_DECL_OUTER(/*object to register*/cls,
	/*func. name*/isExistentPlugin)
//----------------------------------------------------------------------


	return cls;

}
//---------------------------------------------------------------------------

