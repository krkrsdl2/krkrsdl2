/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#ifndef __NATIVE_EVENT_QUEUE_H__
#define __NATIVE_EVENT_QUEUE_H__

// 呼び出されるハンドラがシングルスレッドで動作するイベントキュー

#ifndef _WIN32
class NativeEvent;
class NativeEventQueueIntarface {
public:
	virtual void Dispatch(NativeEvent& event) = 0;
};
#endif
class NativeEvent {
public:
#ifdef _WIN32
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

#ifdef _WIN32
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

#ifdef _WIN32
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
#ifdef _WIN32
	HWND window_handle_;
	WNDCLASSEX	wc_;
#endif

	int CreateUtilWindow();
#ifdef _WIN32
	static LRESULT WINAPI WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
#endif

public:
#ifndef _WIN32
	static tjs_uint32 native_event_queue_custom_event_type;
#endif
#ifdef _WIN32
	NativeEventQueueImplement() : window_handle_(NULL) {}
#else
	NativeEventQueueImplement();
#endif

	// デフォルトハンドラ
#ifdef _WIN32
	void HandlerDefault( NativeEvent& event );
#else
	void HandlerDefault( NativeEvent& event ) {}
#endif

	// Queue の生成
#ifdef _WIN32
	void Allocate();
#else
	void Allocate() {}
#endif

	// Queue の削除
#ifdef _WIN32
	void Deallocate();
#else
	void Deallocate() {}
#endif

	void PostEvent( const NativeEvent& event );

#ifdef _WIN32
	HWND GetOwner() { return window_handle_; }
#endif
#ifndef _WIN32
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

#ifndef _WIN32
	T* GetOwner() { return owner_; }
#endif
};

#endif // __NATIVE_EVENT_QUEUE_H__
