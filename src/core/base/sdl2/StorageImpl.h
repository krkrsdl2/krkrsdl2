//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Universal Storage System
//---------------------------------------------------------------------------
#ifndef StorageImplH
#define StorageImplH
//---------------------------------------------------------------------------
#include "StorageIntf.h"
#include "UtilStreams.h"
#include <functional>
#include <SDL.h>

/*[*/
#ifdef _WIN32
#include <objidl.h> // for IStream
#else
#ifndef KRKRSDL2_STORAGEIMPL_NO_COMPAT
#define STDMETHODCALLTYPE
#define ULONG tjs_uint
#define ULONGLONG tjs_uint
#define LONG tjs_int
#define LONGLONG tjs_int
#define HRESULT tjs_error
#define BYTE tjs_uint8
#define DWORD tjs_uint32
#define STG_E_ACCESSDENIED TJS_E_FAIL
#define E_UNEXPECTED TJS_E_FAIL
#define E_NOTIMPL TJS_E_FAIL
#define E_OUTOFMEMORY TJS_E_FAIL
#define E_INVALIDARG TJS_E_FAIL
#define E_NOINTERFACE TJS_E_FAIL
#define E_POINTER TJS_E_FAIL
#define E_HANDLE TJS_E_FAIL
#define E_ABORT TJS_E_FAIL
#define E_FAIL TJS_E_FAIL
#define E_ACCESSDENIED TJS_E_FAIL
#define E_PENDING TJS_E_FAIL
#define E_NOT_SUFFICIENT_BUFFER TJS_E_FAIL
#define S_TRUE TJS_S_TRUE
#define S_FALSE TJS_S_FALSE
#define S_OK TJS_S_OK
#define STREAM_SEEK_SET TJS_BS_SEEK_SET
#define STREAM_SEEK_CUR TJS_BS_SEEK_CUR
#define STREAM_SEEK_END TJS_BS_SEEK_END
#define STATFLAG_NONAME 0
#define STGC_DEFAULT 0
#define STGTY_STORAGE 1
#define STGTY_STREAM 2
#define STGTY_LOCKBYTES 3
#define STGTY_PROPERTY 4
#define LOCK_WRITE 1
#define LOCK_EXCLUSIVE 2
#define LOCK_ONLYONCE 4
#define STGM_DIRECT 0x00000000
#define STGM_TRANSACTED 0x00010000
#define STGM_SIMPLE 0x08000000
#define STGM_READ 0x00000000
#define STGM_WRITE 0x00000001
#define STGM_READWRITE 0x00000002
#define STGM_SHARE_DENY_NONE 0x00000040
#define STGM_SHARE_DENY_READ 0x00000030
#define STGM_SHARE_DENY_WRITE 0x00000020
#define STGM_SHARE_EXCLUSIVE 0x00000010
#define STGM_PRIORITY 0x00040000
#define STGM_DELETEONRELEASE 0x04000000
#define STGM_NOSCRATCH 0x00100000
#define STGM_CREATE 0x00001000
#define STGM_CONVERT 0x00020000
#define STGM_FAILIFTHERE 0x00000000
#define STGM_NOSNAPSHOT 0x00200000
#define STGM_DIRECT_SWMR 0x00400000
#define FAILED(x) (x != S_OK)
#define SUCCEEDED(x) (!(FAILED(x)))

typedef struct _GUID {
	tjs_uint32 Data1;
	tjs_uint16 Data2;
	tjs_uint16 Data3;
	tjs_uint8 Data4[8];
} GUID;

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	__attribute__((unused)) static const GUID name \
		= { l, w1, w2, { b1, b2,  b3, b4, b5, b6, b7, b8 } }

typedef GUID *LPGUID;
typedef const GUID *LPCGUID;
typedef GUID IID;
typedef IID *LPIID;
typedef GUID CLSID;
typedef CLSID *LPCLSID;
typedef GUID FMTID;
typedef FMTID *LPFMTID;
#define REFGUID const GUID &
#define REFIID const IID &
#define REFCLSID const IID &
#define REFFMTID const IID &

static inline bool IsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
	return (
		((tjs_uint32 *)&rguid1)[0] == ((tjs_uint32 *)&rguid2)[0] &&
		((tjs_uint32 *)&rguid1)[1] == ((tjs_uint32 *)&rguid2)[1] &&
		((tjs_uint32 *)&rguid1)[2] == ((tjs_uint32 *)&rguid2)[2] &&
		((tjs_uint32 *)&rguid1)[3] == ((tjs_uint32 *)&rguid2)[3]
	);
}

#define IsEqualIID(riid1, riid2) IsEqualGUID(riid1, riid2)
#define IsEqualCLSID(rclsid1, rclsid2) IsEqualGUID(rclsid1, rclsid2)

static inline bool operator==(REFGUID rguid1, REFGUID rguid2)
{
	return IsEqualGUID(rguid1, rguid2);
}

static inline bool operator!=(REFGUID rguid1, REFGUID rguid2)
{
	return !(rguid1 == rguid2);
}

typedef union _ULARGE_INTEGER {
	ULONGLONG QuadPart;
} ULARGE_INTEGER;

typedef union _LARGE_INTEGER {
	LONGLONG QuadPart;
} LARGE_INTEGER;

typedef tjs_char *LPOLESTR;

typedef struct _FILETIME {
#ifdef WORDS_BIGENDIAN
	DWORD  dwHighDateTime;
	DWORD  dwLowDateTime;
#else
	DWORD  dwLowDateTime;
	DWORD  dwHighDateTime;
#endif
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct tagSTATSTG {
	LPOLESTR pwcsName;
	DWORD type;
	ULARGE_INTEGER cbSize;
	FILETIME mtime;
	FILETIME ctime;
	FILETIME atime;
	DWORD grfMode;
	DWORD grfLocksSupported;
	CLSID clsid;
	DWORD grfStateBits;
	DWORD reserved;
} STATSTG;

DEFINE_GUID(IID_IUnknown, 0x00000000, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
class IUnknown
{
public:
	// Virtual destructor implemented to avoid UB
	virtual ~IUnknown() {};

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) = 0;
	virtual ULONG STDMETHODCALLTYPE AddRef() = 0;
	virtual ULONG STDMETHODCALLTYPE Release() = 0;
};

DEFINE_GUID(IID_ISequentialStream, 0x0c733a30, 0x2a1c, 0x11ce, 0xad, 0xe5, 0x00, 0xaa, 0x00, 0x44, 0x77, 0x3d);
class ISequentialStream : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead) = 0;
	virtual HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten) = 0;
};

DEFINE_GUID(IID_IStream, 0x0000000c, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
class IStream : public ISequentialStream
{
public:
	virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) = 0;
	virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) = 0;
	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) = 0;
	virtual HRESULT STDMETHODCALLTYPE Revert() = 0;
    virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) = 0;
	virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) = 0;
	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag) = 0;
	virtual HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm) = 0;
};
#endif
#endif
/*]*/

//---------------------------------------------------------------------------
// Susie plug-in related
//---------------------------------------------------------------------------
// Also defined in SusieArchive.h
#ifdef _WIN32
void TVPLoadArchiveSPI(void *inst);
void TVPUnloadArchiveSPI(void *inst);
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// tTVPLocalFileStream
//---------------------------------------------------------------------------
class tTVPLocalFileStream : public tTJSBinaryStream
{
private:
	SDL_RWops * io_handle;
	bool written;

public:
	tTVPLocalFileStream(const ttstr &origname, const ttstr & localname,
		tjs_uint32 flag);
	~tTVPLocalFileStream();

	tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 offset, tjs_int whence);

	tjs_uint TJS_INTF_METHOD Read(void *buffer, tjs_uint read_size);
	tjs_uint TJS_INTF_METHOD Write(const void *buffer, tjs_uint write_size);

	void TJS_INTF_METHOD SetEndOfStorage();

	tjs_uint64 TJS_INTF_METHOD GetSize();

	SDL_RWops * GetHandle() const { return io_handle; }
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(bool, TVPCheckExistentLocalFolder, (const ttstr &name));
	/* name must be an OS's NATIVE folder name */

TJS_EXP_FUNC_DEF(bool, TVPCheckExistentLocalFile, (const ttstr &name));
	/* name must be an OS's NATIVE file name */

TJS_EXP_FUNC_DEF(bool, TVPCreateFolders, (const ttstr &folder));
	/* make folders recursively, like mkdir -p. folder must be OS NATIVE folder name */
//---------------------------------------------------------------------------




#ifndef KRKRSDL2_STORAGEIMPL_NO_COMPAT
//---------------------------------------------------------------------------
// tTVPIStreamAdapter
//---------------------------------------------------------------------------
/*
	this class provides COM's IStream adapter for tTJSBinaryStream
*/
class tTVPIStreamAdapter : public IStream
{
private:
	tTJSBinaryStream *Stream;
	ULONG RefCount;

public:
	tTVPIStreamAdapter(tTJSBinaryStream *ref);
	/*
		the stream passed by argument here is freed by this instance'
		destruction.
	*/

	~tTVPIStreamAdapter();


	// IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
		void **ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(void);
	ULONG STDMETHODCALLTYPE Release(void);

	// ISequentialStream
	HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead);
	HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb,
		ULONG *pcbWritten);

	// IStream
	HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove,
		DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
	HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize);
	HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm, ULARGE_INTEGER cb,
		ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
	HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);
	HRESULT STDMETHODCALLTYPE Revert(void);
	HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb, DWORD dwLockType);
	HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset,
		ULARGE_INTEGER cb, DWORD dwLockType);
	HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag);
	HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm);

	void ClearStream() {
		Stream = NULL;
	}
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// IStream creator
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(IStream *, TVPCreateIStream, (const ttstr &name, tjs_uint32 flags));
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTVPBinaryStreamAdapter
//---------------------------------------------------------------------------
/*
	this class provides tTJSBinaryStream adapter for IStream
*/
class tTVPBinaryStreamAdapter : public tTJSBinaryStream
{
	typedef tTJSBinaryStream inherited;

private:
	IStream *Stream;

public:
	tTVPBinaryStreamAdapter(IStream *ref);
	/*
		the stream passed by argument here is freed by this instance'
		destruction.
	*/

	~tTVPBinaryStreamAdapter();

	tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 offset, tjs_int whence);
	tjs_uint TJS_INTF_METHOD Read(void *buffer, tjs_uint read_size);
	tjs_uint TJS_INTF_METHOD Write(const void *buffer, tjs_uint write_size);
	tjs_uint64 TJS_INTF_METHOD GetSize();
	void TJS_INTF_METHOD SetEndOfStorage();
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTVPBinaryStreamAdapter creator
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(tTJSBinaryStream *, TVPCreateBinaryStreamAdapter, (IStream *refstream));
//---------------------------------------------------------------------------
#endif



//---------------------------------------------------------------------------
// tTVPPluginHolder
//---------------------------------------------------------------------------
/*
	tTVPPluginHolder holds plug-in. if the plug-in is not a local storage,
	plug-in is to be extracted to temporary directory and be held until
	the program done using it.
*/
class tTVPPluginHolder
{
private:
	tTVPLocalTempStorageHolder * LocalTempStorageHolder;
	ttstr LocalPath;

public:
	tTVPPluginHolder(const ttstr &aname);
	~tTVPPluginHolder();

	const ttstr & GetLocalName() const;
};
//---------------------------------------------------------------------------

#endif
