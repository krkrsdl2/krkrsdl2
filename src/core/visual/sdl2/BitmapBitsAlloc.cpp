#include "tjsCommHead.h"
//---------------------------------------------------------------------------
#include "tjsUtils.h"
#include "MsgIntf.h"
#include "BitmapBitsAlloc.h"
#include "SysInitIntf.h"
#include "EventIntf.h"
#include "DebugIntf.h"

#if defined(__APPLE__) || defined(__linux__) || defined(ANDROID) || defined(__ANDROID__)
#define USE_MMAP_FOR_ALLOCATION
#endif

#ifdef USE_MMAP_FOR_ALLOCATION
#include <sys/mman.h>
#endif

class BasicAllocator : public iTVPMemoryAllocator
{
public:
	BasicAllocator() {}
	void* allocate(size_t size)
	{
#ifdef USE_MMAP_FOR_ALLOCATION
		return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#else
		return malloc(size);
#endif
	}
	void free(void* mem, size_t size)
	{
#ifdef USE_MMAP_FOR_ALLOCATION
		munmap(mem, size);
#else
		::free( mem );
#endif
	}
};

iTVPMemoryAllocator* tTVPBitmapBitsAlloc::Allocator = NULL;
tTJSCriticalSection tTVPBitmapBitsAlloc::AllocCS;

void tTVPBitmapBitsAlloc::InitializeAllocator() {
	if (Allocator == NULL)
	{
		Allocator = new BasicAllocator();
	}
}
void tTVPBitmapBitsAlloc::FreeAllocator() {
	if( Allocator ) delete Allocator;
	Allocator = NULL;
}
static tTVPAtExit
	TVPUninitMessageLoad(TVP_ATEXIT_PRI_CLEANUP, tTVPBitmapBitsAlloc::FreeAllocator);

void* tTVPBitmapBitsAlloc::Alloc( tjs_uint size, tjs_uint width, tjs_uint height ) {
	if(size == 0) return NULL;
	tTJSCriticalSectionHolder Lock(AllocCS);	// Lock

	InitializeAllocator();
	tjs_uint8 * ptrorg, * ptr;
	tjs_uint allocbytes = 16 + size + sizeof(tTVPLayerBitmapMemoryRecord) + sizeof(tjs_uint32)*2;

	ptr = ptrorg = (tjs_uint8*)Allocator->allocate(allocbytes);
	if(!ptr) {
		// Do GC
		TVPDeliverCompactEvent(TVP_COMPACT_LEVEL_MAX);
		ptr = ptrorg = (tjs_uint8*)Allocator->allocate(allocbytes);
		if(!ptr) {
			TVPThrowExceptionMessage(TVPCannotAllocateBitmapBits,
				TJS_W("at TVPAllocBitmapBits"), ttstr((tjs_int)allocbytes) + TJS_W("(") +
				ttstr((int)width) + TJS_W("x") + ttstr((int)height) + TJS_W(")"));
		}
	}
	// align to a paragraph ( 16-bytes )
	ptr += 16 + sizeof(tTVPLayerBitmapMemoryRecord);
	*reinterpret_cast<tTJSPointerSizedInteger*>(&ptr) >>= 4;
	*reinterpret_cast<tTJSPointerSizedInteger*>(&ptr) <<= 4;

	tTVPLayerBitmapMemoryRecord * record =
		(tTVPLayerBitmapMemoryRecord*)
		(ptr - sizeof(tTVPLayerBitmapMemoryRecord) - sizeof(tjs_uint32));

	// fill memory allocation record
	record->alloc_ptr = (void *)ptrorg;
	record->orig_size = allocbytes;
	record->size = size;
	record->sentinel_backup1 = rand() + (rand() << 16);
	record->sentinel_backup2 = rand() + (rand() << 16);

	// set sentinel
	*(tjs_uint32*)(ptr - sizeof(tjs_uint32)) = ~record->sentinel_backup1;
	*(tjs_uint32*)(ptr + size              ) = ~record->sentinel_backup2;
		// Stored sentinels are nagated, to avoid that the sentinel backups in
		// tTVPLayerBitmapMemoryRecord becomes the same value as the sentinels.
		// This trick will make the detection of the memory corruption easier.
		// Because on some occasions, running memory writing will write the same
		// values at first sentinel and the tTVPLayerBitmapMemoryRecord.

	// return buffer pointer
	return ptr;
}
void tTVPBitmapBitsAlloc::Free( void* ptr ) {
	if(ptr)
	{
#if 0
		tTJSCriticalSectionHolder Lock(AllocCS);	// Lock
#endif

		// get memory allocation record pointer
		tjs_uint8 *bptr = (tjs_uint8*)ptr;
		tTVPLayerBitmapMemoryRecord * record =
			(tTVPLayerBitmapMemoryRecord*)
			(bptr - sizeof(tTVPLayerBitmapMemoryRecord) - sizeof(tjs_uint32));

		// check sentinel
		if(~(*(tjs_uint32*)(bptr - sizeof(tjs_uint32))) != record->sentinel_backup1)
			TVPAddLog( ttstr(TVPLayerBitmapBufferUnderrunDetectedCheckYourDrawingCode) );
		if(~(*(tjs_uint32*)(bptr + record->size      )) != record->sentinel_backup2)
			TVPAddLog( ttstr(TVPLayerBitmapBufferOverrunDetectedCheckYourDrawingCode) );

		Allocator->free( record->alloc_ptr, record->orig_size );
	}
}

