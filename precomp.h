#pragma warning(disable:4201)  //nonstandard extension used : nameless struct/union
#include <ndis.h>
#include <filteruser.h>
#include "flt_dbg.h"
#include "filter.h"

// {6EC8ECB0-B1A4-4B66-B605-F7CB6E68926C}
//static const GUID << name >> =
//{ 0x6ec8ecb0, 0xb1a4, 0x4b66,{ 0xb6, 0x5, 0xf7, 0xcb, 0x6e, 0x68, 0x92, 0x6c } };

#define WPP_CONTROL_GUIDS                                              \
    WPP_DEFINE_CONTROL_GUID(                                           \
        myDriverTraceGuid, (6EC8ECB0,B1A4,4B66,B605,F7CB6E68926C), \
        WPP_DEFINE_BIT(MYDRIVER_ALL_INFO)        /* bit  0 = 0x00000001 */ \
        WPP_DEFINE_BIT(TRACE_DRIVER)             /* bit  1 = 0x00000002 */ \
        WPP_DEFINE_BIT(TRACE_DEVICE)             /* bit  2 = 0x00000004 */ \
        WPP_DEFINE_BIT(TRACE_QUEUE)              /* bit  3 = 0x00000008 */ \
        )

typedef void(*PFNCLEANUPIRP) (PIRP pIrp, PVOID pContext);
typedef struct _IRPLIST
{
	PIRP pIrp;
	PFNCLEANUPIRP pfnCleanUpIrp;
	PDRIVER_CANCEL pfnCancelRoutine;
	PVOID pContext;
	struct IRPLIST* pNextIrp;
} IRPLIST, *PIRPLIST;

typedef struct _IRPLISTHEAD
{
	PIRPLIST pListFront;
} IRPLISTHEAD, *PIRPLISTHEAD;

extern KSPIN_LOCK kspIrpListLock;
extern PIRPLISTHEAD pIrpList;

//#define _USE_DIRECT_IO_
//#define _USE_NEITHER_IO_
#define _USE_BUFFER_IO_