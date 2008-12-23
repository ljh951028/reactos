#ifndef MEMTRACK_H
#define MEMTRACK_H

#include <pool.h>

#ifndef FOURCC
#define FOURCC(a,b,c,d) (((a)<<24)|((b)<<16)|((c)<<8)|(d))
#endif

#define FBSD_MALLOC FOURCC('d','s','b','f')
#define EXALLOC_TAG FOURCC('E','x','A','l')
#define IRP_TAG     FOURCC('P','I','R','P')
#define NPLOOK_TAG  FOURCC('N','P','L','A')

#define AllocatePacketWithBuffer(x,y,z) AllocatePacketWithBufferX(x,y,z,__FILE__,__LINE__)
#define FreeNdisPacket(x) FreeNdisPacketX(x,__FILE__,__LINE__)

#define MTMARK() TrackDumpFL(__FILE__, __LINE__)
#define exAllocatePool(x,y) ExAllocatePoolX(x,y,__FILE__,__LINE__)
#define exAllocatePoolWithTag(x,y,z) ExAllocatePoolX(x,y,__FILE__,__LINE__)
#define exFreePool(x) ExFreePoolX(x,__FILE__,__LINE__)
#define exAllocateFromNPagedLookasideList(x) ExAllocateFromNPagedLookasideListX(x,__FILE__,__LINE__)
#define exFreeToNPagedLookasideList(x,y) ExFreeToNPagedLookasideListX(x,y,__FILE__,__LINE__)

typedef struct _ALLOCATION_TRACKER {
    LIST_ENTRY Entry;
    DWORD Tag;
    PVOID Thing;
    PCHAR FileName;
    DWORD LineNo;
} ALLOCATION_TRACKER, *PALLOCATION_TRACKER;

VOID TrackingInit();
VOID TrackWithTag( DWORD Tag, PVOID Thing, PCHAR File, DWORD Line );
#define Track(Tag,Thing) TrackWithTag(Tag,Thing,__FILE__,__LINE__)
VOID UntrackFL( PCHAR File, DWORD Line, PVOID Thing, DWORD Tag );
#define Untrack(Thing) UntrackFL(__FILE__,__LINE__,Thing)
VOID TrackDumpFL( PCHAR File, DWORD Line );
#define TrackDump() TrackDumpFL(__FILE__,__LINE__)
VOID TrackTag( DWORD Tag );

static __inline PVOID ExAllocateFromNPagedLookasideListX( PNPAGED_LOOKASIDE_LIST List, PCHAR File, ULONG Line ) {
    PVOID Out = ExAllocateFromNPagedLookasideList( List );
    if( Out ) TrackWithTag( NPLOOK_TAG, Out, File, Line );
    return Out;
}

static __inline VOID ExFreeToNPagedLookasideListX( PNPAGED_LOOKASIDE_LIST List, PVOID Thing, PCHAR File, ULONG Line ) {
    UntrackFL(File, Line, Thing, NPLOOK_TAG);
    ExFreeToNPagedLookasideList( List, Thing );
}

static __inline PVOID ExAllocatePoolX( POOL_TYPE type, SIZE_T size, PCHAR File, ULONG Line ) {
    PVOID Out = ExAllocatePool( type, size );
    if( Out ) TrackWithTag( EXALLOC_TAG, Out, File, Line );
    return Out;
}

static __inline VOID ExFreePoolX( PVOID Data, PCHAR File, ULONG Line ) {
    UntrackFL(File, Line, Data, EXALLOC_TAG);
    ExFreePool( Data );
}

#define MEMTRACK_MAX_TAGS_TO_TRACK 64

#endif/*MEMMTRAC_H*/
