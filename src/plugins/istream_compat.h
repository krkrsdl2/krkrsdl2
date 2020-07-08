
/**
 * @file
 * COM IStream source-level compatibility layer for win32 plugins
 */

#ifndef _ISTREAM_COMPAT_H
#define _ISTREAM_COMPAT_H

#include "StorageIntf.h"

#define STDMETHODCALLTYPE
#define ULONG tjs_uint
#define ULONGLONG tjs_uint
#define LONG tjs_int
#define LONGLONG tjs_int
#define HRESULT tjs_error
#define BYTE tjs_uint8
#define DWORD tjs_uint32
#define REFIID void*
#define E_FAIL TJS_E_FAIL
#define E_NOTIMPL TJS_E_FAIL
#define E_INVALIDARG TJS_E_FAIL
#define S_OK TJS_S_OK
#define STREAM_SEEK_SET TJS_BS_SEEK_SET
#define STREAM_SEEK_CUR TJS_BS_SEEK_CUR
#define STREAM_SEEK_END TJS_BS_SEEK_END
#define STATFLAG_NONAME 0
#define STGC_DEFAULT 0
#define FAILED(x) (x != S_OK)

typedef union _ULARGE_INTEGER {
	ULONGLONG QuadPart;
} ULARGE_INTEGER;

typedef union _LARGE_INTEGER {
	LONGLONG QuadPart;
} LARGE_INTEGER;

typedef struct tagSTATSTG {
	ULARGE_INTEGER cbSize;
} STATSTG;

#define IStream tTVPIStreamAdapter

//---------------------------------------------------------------------------
// tTVPIStreamAdapter
//---------------------------------------------------------------------------
/*
	this class provides COM's IStream adapter for tTJSBinaryStream
*/
class tTVPIStreamAdapter
{
private:
	tTJSBinaryStream *Stream;
	ULONG RefCount;

public:
	tTVPIStreamAdapter(tTJSBinaryStream *ref)
	{
		Stream = ref;
		RefCount = 1;
	}
	/*
		the stream passed by argument here is freed by this instance'
		destruction.
	*/

	~tTVPIStreamAdapter()
	{
		delete Stream;
	}


	// IUnknown
	ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return ++ RefCount;
	}
	ULONG STDMETHODCALLTYPE Release(void)
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

	// ISequentialStream
	HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead)
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
	HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten)
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

	// IStream
	HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
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
	HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags)
	{
		return E_NOTIMPL;
	}
	HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag)
	{
		// This method imcompletely fills the target structure, because some
		// informations like access mode or stream name are already lost
		// at this point.

		if(pstatstg)
		{
			memset(pstatstg, 0, sizeof(*pstatstg));

			// cbSize
			pstatstg->cbSize.QuadPart = Stream->GetSize();
		}
		else
		{
			return E_INVALIDARG;
		}

		return S_OK;
	}

	void ClearStream()
	{
		Stream = NULL;
	}
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// IStream creator
//---------------------------------------------------------------------------
inline IStream * TVPCreateIStream(const ttstr &name, tjs_uint32 flags)
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

#endif
