#include "ncbind/ncbind.hpp"
#include "istream_compat.h"
#include <string>
#include <vector>
using namespace std;

#ifndef _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

NCB_TYPECONV_CAST_INTEGER(tjs_uint64);


/**
 * メソッド追加用
 */
class StoragesFstat {

	/**
	 * パスをローカル化する＆末尾の\を削除
	 * @param path パス名
	 */
	static void getLocalName(ttstr &path) {
		TVPGetLocalName(path);
		if (path.GetLastChar() == TJS_W('\\')) {
			tjs_int i,len = path.length();
			tjs_char* tmp = new tjs_char[len];
			const tjs_char* dp = path.c_str();
			for (i=0,len--; i<len; i++) tmp[i] = dp[i];
			tmp[i] = 0;
			path = tmp;
			delete[] tmp;
		}
	}
	/**
	 * ローカルパスの有無判定
	 * @param in  path  パス名
	 * @param out local ローカルパス
	 * @return ローカルパスがある場合はtrue
	 */
	static bool getLocallyAccessibleName(const ttstr &path, ttstr *local = NULL) {
		bool r = false;
		if (local) {
			*local = TVPGetLocallyAccessibleName(path);
			r = ! local->IsEmpty();
		} else {
			ttstr local(TVPGetLocallyAccessibleName(path));
			r = ! local.IsEmpty();
		}
		return r;
	}

public:
	StoragesFstat(){};

	static void clearStorageCaches() {
		TVPClearStorageCaches();
	}
	
	/**
	 * 指定されたファイルの情報を取得する
	 * @param filename ファイル名
	 * @return サイズ・時刻辞書
	 * ※アーカイブ内ファイルはサイズのみ返す
	 */
	static tjs_error TJS_INTF_METHOD fstat(tTJSVariant *result,
										   tjs_int numparams,
										   tTJSVariant **param,
										   iTJSDispatch2 *objthis) {
		if (numparams < 1) return TJS_E_BADPARAMCOUNT;

		ttstr filename = TVPGetPlacedPath(*param[0]);
		if (filename.length() > 0) {
			// アーカイブ内ファイル
			IStream *in = TVPCreateIStream(filename, TJS_BS_READ);
			if (in) {
				STATSTG stat;
				in->Stat(&stat, STATFLAG_NONAME);
				tTJSVariant size((tjs_int64)stat.cbSize.QuadPart);
				if (result) {
					iTJSDispatch2 *dict;
					if ((dict = TJSCreateDictionaryObject()) != NULL) {
						dict->PropSet(TJS_MEMBERENSURE, TJS_W("size"),  NULL, &size, dict);
						*result = dict;
						dict->Release();
					}
				}
				in->Release();
				return TJS_S_OK;
			}
		}
		TVPThrowExceptionMessage((ttstr(TJS_W("cannot open : ")) + filename).c_str());
		return TJS_S_OK;
	}

	/**
	 * 吉里吉里のストレージ空間中のファイルを抽出する
	 * @param src 保存元ファイル
	 * @param dest 保存先ファイル
	 */
	static void exportFile(ttstr filename, ttstr storename) {
		IStream *in = TVPCreateIStream(filename, TJS_BS_READ);
		if (in) {
			IStream *out = TVPCreateIStream(storename, TJS_BS_WRITE);
			if (out) {
				BYTE buffer[1024*16];
				DWORD size;
				while (in->Read(buffer, sizeof buffer, &size) == S_OK && size > 0) {			
					out->Write(buffer, size, &size);
				}
				out->Release();
				in->Release();
			} else {
				in->Release();
				TVPThrowExceptionMessage((ttstr(TJS_W("cannot open storefile: ")) + storename).c_str());
			}
		} else {
			TVPThrowExceptionMessage((ttstr(TJS_W("cannot open readfile: ")) + filename).c_str());
		}
	}

	/**
	 * 吉里吉里のストレージ空間中の指定ファイルを削除する。
	 * @param file 削除対象ファイル
	 * @return 実際に削除されたら true
	 * 実ファイルがある場合のみ削除されます
	 */
	static bool deleteFile(const tjs_char *file) {
		bool r = false;
		ttstr filename(TVPGetLocallyAccessibleName(TVPGetPlacedPath(file)));
		if (filename.length()) {
			std::string nfilename;
			TVPUtf16ToUtf8( nfilename, filename.AsStdString() );
			r = remove(nfilename.c_str()) == 0;
			if (r)
			{
				TVPClearStorageCaches();
			}
		}
		return r;
	}

	/**
	 * 吉里吉里のストレージ空間中の指定ファイルのサイズを変更する(切り捨てる)
	 * @param file ファイル
	 * @param size 指定サイズ
	 * @return サイズ変更できたら true
	 * 実ファイルがある場合のみ処理されます
	 */
	static bool truncateFile(const tjs_char *file, tjs_int size) {
		bool r = false;
		ttstr filename(TVPGetLocallyAccessibleName(TVPGetPlacedPath(file)));
		if (filename.length()) {
			std::string nfilename;
			TVPUtf16ToUtf8( nfilename, filename.AsStdString() );
			r = truncate(nfilename.c_str(), size) == 0;
		}
		return r;
	}
	
	/**
	 * 指定ファイルを移動する。
	 * @param fromFile 移動対象ファイル
	 * @param toFile 移動先パス
	 * @return 実際に移動されたら true
	 * 移動対象ファイルが実在し、移動先パスにファイルが無い場合のみ移動されます
	 */
	static bool moveFile(const tjs_char *from, const tjs_char *to) {
		bool r = false;
		ttstr fromFile(TVPGetLocallyAccessibleName(from));
		ttstr   toFile(TVPGetLocallyAccessibleName(to));
		if (fromFile.length()
			&& toFile.length())
		{
			std::string nfromFile;
			std::string   ntoFile;
			TVPUtf16ToUtf8( nfromFile, fromFile.AsStdString() );
			TVPUtf16ToUtf8( ntoFile, toFile.AsStdString() );
			r = rename(nfromFile.c_str(), ntoFile.c_str()) == 0;
			if (r)
			{
				TVPClearStorageCaches();
			}
		}
		return r;
	}

	/**
	 * 指定ディレクトリのファイル一覧を取得する
	 * @param dir ディレクトリ名
	 * @return ファイル名一覧が格納された配列
	 */
	static tTJSVariant dirlist(ttstr dir) {
		// OSネイティブな表現に変換
		dir = TVPNormalizeStorageName(dir);
		if (dir.GetLastChar() != TJS_W('/')) {
			TVPThrowExceptionMessage(TJS_W("'/' must be specified at the end of given directory name."));
		}
		TVPGetLocalName(dir);

		// Array クラスのオブジェクトを作成
		iTJSDispatch2 * array = TJSCreateArrayObject();
		tTJSVariant result;

		tjs_string wname(dir.AsStdString());
		std::string nname;
		tjs_int icount = 0;
		if( TVPUtf16ToUtf8(nname, wname) ) {
			DIR* dr;
			if( ( dr = opendir(nname.c_str()) ) != nullptr ) {
				struct dirent* entry;
				while( ( entry = readdir( dr ) ) != nullptr ) {
					if( entry->d_type == DT_REG ) {
						tjs_char fname[256];
						tjs_int count = TVPUtf8ToWideCharString( entry->d_name, fname );
						fname[count] = TJS_W('\0');
						tTJSVariant val = ttstr(fname);
						array->PropSetByNum(TJS_MEMBERENSURE, icount, &val, array);
						icount += 1;
					}
					else if( entry->d_type == DT_DIR )
					{
						tjs_char fname[256];
						tjs_int count = TVPUtf8ToWideCharString( entry->d_name, fname );
						fname[count] = TJS_W('\0');
						ttstr ttname(fname);
						ttname += TJS_W("/");
						tTJSVariant val = ttname;
						array->PropSetByNum(TJS_MEMBERENSURE, icount, &val, array);
						icount += 1;
					}
					// entry->d_type == DT_UNKNOWN
				}
				closedir( dr );
			}
			else {
				array->Release();
				TVPThrowExceptionMessage(TJS_W("Directory not found."));
			}
			result = tTJSVariant(array, array);
			array->Release();
		}

		return result;
	}

public:

	/**
	 * 指定ディレクトリを削除する
	 * @param dir ディレクトリ名
	 * @return 実際に削除されたら true
	 * 中にファイルが無い場合のみ削除されます
	 */
	static bool removeDirectory(ttstr dir) {

		if (dir.GetLastChar() != TJS_W('/')) {
			TVPThrowExceptionMessage(TJS_W("'/' must be specified at the end of given directory name."));
		}

		// OSネイティブな表現に変換
		dir = TVPNormalizeStorageName(dir);
		TVPGetLocalName(dir);

		std::string ndir;
		if ( TVPUtf16ToUtf8( ndir, dir.AsStdString() ) )
		{
			return 0 == rmdir(ndir.c_str());
		}
		return false;
	}

	/**
	 * ディレクトリの作成
	 * @param dir ディレクトリ名
	 * @return 実際に作成できたら true
	 */
	static bool createDirectory(ttstr dir)
	{
		if(dir.GetLastChar() != TJS_W('/'))
		{
			TVPThrowExceptionMessage(TJS_W("'/' must be specified at the end of given directory name."));
		}
		dir	= TVPNormalizeStorageName(dir);
		TVPGetLocalName(dir);
		return TVPCreateFolders(dir);
	}

	/**
	 * ディレクトリの作成
	 * @param dir ディレクトリ名
	 * @return 実際に作成できたら true
	 */
	static bool createDirectoryNoNormalize(ttstr dir)
	{
		if(dir.GetLastChar() != TJS_W('/'))
		{
			TVPThrowExceptionMessage(TJS_W("'/' must be specified at the end of given directory name."));
		}
		TVPGetLocalName(dir);
		return TVPCreateFolders(dir);
	}

	/**
	 * ディレクトリの存在チェック
	 * @param directory ディレクトリ名
	 * @return ディレクトリが存在すれば true/存在しなければ -1/ディレクトリでなければ false
	 */
	static int isExistentDirectory(ttstr dir)
	{
		if(dir.GetLastChar() != TJS_W('/'))
		{
			TVPThrowExceptionMessage(TJS_W("'/' must be specified at the end of given directory name."));
		}
		dir	= TVPNormalizeStorageName(dir);
		TVPGetLocalName(dir);
		return TVPCheckExistentLocalFolder(dir);
	}

	/**
	 * パスの正規化を行なわず、autoPathからの検索も行なわずに
	 * ファイルの存在確認を行う
	 * @param fileame ファイルパス
	 * @return ファイルが存在したらtrue
	 */
	static bool isExistentStorageNoSearchNoNormalize(ttstr filename) 
	{
		return TVPIsExistentStorageNoSearchNoNormalize(filename);
	}
};

NCB_ATTACH_CLASS(StoragesFstat, Storages) {
	NCB_METHOD(clearStorageCaches);
	RawCallback("fstat",               &Class::fstat,               TJS_STATICMEMBER);
#if 0
	RawCallback("getTime",             &Class::getTime,             TJS_STATICMEMBER);
	RawCallback("setTime",             &Class::setTime,             TJS_STATICMEMBER);
	NCB_METHOD(getLastModifiedFileTime);
	NCB_METHOD(setLastModifiedFileTime);
#endif
	NCB_METHOD(exportFile);
	NCB_METHOD(deleteFile);
	NCB_METHOD(truncateFile);
	NCB_METHOD(moveFile);
	NCB_METHOD(dirlist);
#if 0
	NCB_METHOD(dirlistEx);
#endif
	NCB_METHOD(removeDirectory);
	NCB_METHOD(createDirectory);
	NCB_METHOD(createDirectoryNoNormalize);
#if 0
	NCB_METHOD(changeDirectory);
	NCB_METHOD(setFileAttributes);
	NCB_METHOD(resetFileAttributes);
	NCB_METHOD(getFileAttributes);
	RawCallback("selectDirectory",     &Class::selectDirectory,     TJS_STATICMEMBER);
#endif
	NCB_METHOD(isExistentDirectory);
#if 0
	NCB_METHOD(copyFile);
	NCB_METHOD(copyFileNoNormalize);
#endif
	NCB_METHOD(isExistentStorageNoSearchNoNormalize);
#if 0
	NCB_METHOD(getDisplayName);
	RawCallback("getMD5HashString",    &Class::getMD5HashString,    TJS_STATICMEMBER);
	RawCallback("searchPath",          &Class::searchPath,          TJS_STATICMEMBER);
	Property("currentPath", &Class::getCurrentPath, &Class::setCurrentPath);
#endif
	Method(TJS_W("getTemporaryName"), &TVPGetTemporaryName);
};
