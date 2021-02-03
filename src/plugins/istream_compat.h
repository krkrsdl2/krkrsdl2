
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
#define STG_E_ACCESSDENIED TJS_E_FAIL
#define E_FAIL TJS_E_FAIL
#define E_NOTIMPL TJS_E_FAIL
#define E_INVALIDARG TJS_E_FAIL
#define S_TRUE TJS_S_TRUE
#define S_FALSE TJS_S_FALSE
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

class IStream
{
public:

	// IUnknown
	virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
	virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

	// ISequentialStream
	virtual HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead) = 0;
	virtual HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten) = 0;

	// IStream
	virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition) = 0;
	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) = 0;
	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag) = 0;

};

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
	tTVPBinaryStreamAdapter(IStream *ref)
	{
		Stream = ref;
		Stream->AddRef();
	}
	/*
		the stream passed by argument here is freed by this instance'
		destruction.
	*/

	~tTVPBinaryStreamAdapter()
	{
		Stream->Release();
	}

	tjs_uint64 TJS_INTF_METHOD Seek(tjs_int64 offset, tjs_int whence)
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
		}

		return newpos.QuadPart;
	}

	tjs_uint TJS_INTF_METHOD Read(void *buffer, tjs_uint read_size)
	{
		ULONG cb = read_size;
		ULONG read;
		HRESULT hr = Stream->Read(buffer, cb, &read);
		if(FAILED(hr)) read = 0;
		return read;
	}

	tjs_uint TJS_INTF_METHOD Write(const void *buffer, tjs_uint write_size)
	{
		ULONG cb = write_size;
		ULONG written;
		HRESULT hr = Stream->Write(buffer, cb, &written);
		if(FAILED(hr)) written = 0;
		return written;
	}

	tjs_uint64 TJS_INTF_METHOD GetSize()
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
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TVPCreateBinaryStreamAdapter
//---------------------------------------------------------------------------
inline tTJSBinaryStream * TVPCreateBinaryStreamAdapter(IStream *refstream)
{
	return new  tTVPBinaryStreamAdapter(refstream);
}
//---------------------------------------------------------------------------

#endif
