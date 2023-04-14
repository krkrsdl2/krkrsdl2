
#include "tjsCommHead.h"
#include "istream_compat.h"
#include "MsgIntf.h"


#ifndef _WIN32
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
HRESULT STDMETHODCALLTYPE tTVPIStreamAdapter::QueryInterface(REFIID riid, void **ppvObject)
{
	if(!ppvObject) return E_INVALIDARG;

#if 0
	*ppvObject=NULL;
	if(!memcmp(&riid,&IID_IUnknown,16))
		*ppvObject=(IUnknown*)this;
	else if(!memcmp(&riid,&IID_ISequentialStream,16))
		*ppvObject=(ISequentialStream*)this;
	else if(!memcmp(&riid,&IID_IStream,16))
		*ppvObject=(IStream*)this;
#endif
	*ppvObject = (void *)this;

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
	// This method imcompletely fills the target structure, because some
	// informations like access mode or stream name are already lost
	// at this point.

	if(pstatstg)
	{
#if 0
		ZeroMemory(pstatstg, sizeof(*pstatstg));
#else
		memset(pstatstg, 0, sizeof(*pstatstg));
#endif

#if 0
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

		// type
		pstatstg->type = STGTY_STREAM;
#endif

		// cbSize
		pstatstg->cbSize.QuadPart = Stream->GetSize();

#if 0
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
#endif
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
#endif

#ifndef _WIN32
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
#endif

