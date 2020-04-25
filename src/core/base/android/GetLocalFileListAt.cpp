
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "GetLocalFileListAt.h"
#include "CharacterSet.h"

#ifdef st_atime
#undef st_atime
#undef st_ctime
#undef st_mtime
#endif

struct tTVP_stat {
	uint16_t st_mode;
	uint64_t st_size;
	uint64_t st_atime;
	uint64_t st_mtime;
	uint64_t st_ctime;
};

static bool TVP_stat(const char *name, tTVP_stat &s) {
	struct stat t;
	// static_assert(sizeof(t.st_size) == 4, "");
	// static_assert(sizeof(t.st_size) == 8, "");
	bool ret = !stat(name, &t);
	s.st_mode = t.st_mode;
	s.st_size = t.st_size;
#ifdef __MACH__
	s.st_atime = t.st_atimespec.tv_sec;
	s.st_mtime = t.st_mtimespec.tv_sec;
	s.st_ctime = t.st_ctimespec.tv_sec;
#elif defined(__linux__)
	s.st_atime = t.st_atim.tv_sec;
	s.st_mtime = t.st_mtim.tv_sec;
	s.st_ctime = t.st_ctim.tv_sec;
#else
	s.st_atime = t.st_atime;
	s.st_mtime = t.st_mtime;
	s.st_ctime = t.st_ctime;
#endif
	return ret;
}

void TVPGetLocalFileListAt(const ttstr &name, const std::function<void(const ttstr&, tTVPLocalFileInfo*)>& cb) {
	DIR *dirp;
	struct dirent *direntp;
	tTVP_stat stat_buf;
	tjs_string wname(name.AsStdString());
	std::string nname;
	if( TVPUtf16ToUtf8(nname, wname) ) {
		if ((dirp = opendir(nname.c_str())))
		{
			while ((direntp = readdir(dirp)) != NULL)
			{
				if( direntp->d_type == DT_REG ) {
					std::string fullpath = nname + "/" + direntp->d_name;
					if (!TVP_stat(fullpath.c_str(), stat_buf))
						continue;
					ttstr file(direntp->d_name);
					if (file.length() <= 2) {
						if (file == TJS_W(".") || file == TJS_W(".."))
							continue;
					}
					tjs_char *p = file.Independ();
					while (*p)
					{
						// make all characters small
						if (*p >= TJS_W('A') && *p <= TJS_W('Z'))
							*p += TJS_W('a') - TJS_W('A');
						p++;
					}
					tTVPLocalFileInfo info;
					info.NativeName = direntp->d_name;
					info.Mode = stat_buf.st_mode;
					info.Size = stat_buf.st_size;
					info.AccessTime = stat_buf.st_atime;
					info.ModifyTime = stat_buf.st_mtime;
					info.CreationTime = stat_buf.st_ctime;
					cb(file, &info);
				}
			}
			closedir(dirp);
		}
	}
}