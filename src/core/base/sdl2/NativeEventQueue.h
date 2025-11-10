/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#ifndef __NATIVE_EVENT_QUEUE_H__
#define __NATIVE_EVENT_QUEUE_H__

// 呼び出されるハンドラがシングルスレッドで動作するイベントキュー

#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
#else
class NativeEvent;
class NativeEventQueueIntarface {
public:
	virtual void Dispatch(NativeEvent& event) = 0;
};
#endif
class NativeEvent {
public:
#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
	LRESULT Result;
	HWND HWnd;
	UINT Message;
	WPARAM WParam;
	LPARAM LParam;
#else
 	unsigned int Message;
	intptr_t WParam;
	intptr_t LParam;
	NativeEventQueueIntarface * queue;
#endif

#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
	NativeEvent(){}
	NativeEvent( int mes ) : Result(0), HWnd(NULL), Message(mes), WParam(0), LParam(0) {}
#else
	NativeEvent( int mes ) : /*Result(0), HWnd(NULL),*/ Message(mes), WParam(0), LParam(0) {}
	void SetQueue(NativeEventQueueIntarface * tmp_queue)
	{
		queue = tmp_queue;
	}
	void HandleEvent()
	{
		NativeEvent _this = *this;
		queue->Dispatch(_this);
		delete this;
	}
#endif
};

#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
class NativeEventQueueIntarface {
public:
	// デフォルトハンドラ
	virtual void HandlerDefault( class NativeEvent& event ) = 0;

	// Queue の生成
	virtual void Allocate() = 0;

	// Queue の削除
	virtual void Deallocate() = 0;

	virtual void Dispatch( class NativeEvent& event ) = 0;

	virtual void PostEvent( const NativeEvent& event ) = 0;
};
#endif
class NativeEventQueueImplement : public NativeEventQueueIntarface {
#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
	HWND window_handle_;
	WNDCLASSEX	wc_;
#endif

	int CreateUtilWindow();
#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
	static LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
#endif

public:
#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
#else
	static tjs_uint32 native_event_queue_custom_event_type;
#endif
#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
	NativeEventQueueImplement() : window_handle_(NULL) {}
#else
	NativeEventQueueImplement();
#endif

	// デフォルトハンドラ
#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
	void HandlerDefault( NativeEvent& event );
#else
	void HandlerDefault( NativeEvent& event ) {}
#endif

	// Queue の生成
#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
	void Allocate();
#else
	void Allocate() {}
#endif

	// Queue の削除
#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
	void Deallocate();
#else
	void Deallocate() {}
#endif

	void PostEvent( const NativeEvent& event );

#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
	HWND GetOwner() { return window_handle_; }
#endif
#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
#else
	void Dispatch(NativeEvent& event) {}
#endif
};


template<typename T>
class NativeEventQueue : public NativeEventQueueImplement {
	void (T::*handler_)(NativeEvent&);
	T* owner_;

public:
	NativeEventQueue( T* owner, void (T::*Handler)(NativeEvent&) ) : owner_(owner), handler_(Handler) {}

	void Dispatch( NativeEvent &ev ) {
		(owner_->*handler_)(ev);
	}

#if defined(_WIN32) && defined(KRKRSDL2_USE_WIN32_EVENT_QUEUE)
#else
	T* GetOwner() { return owner_; }
#endif
};

#endif // __NATIVE_EVENT_QUEUE_H__
