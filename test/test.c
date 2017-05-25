#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

typedef struct _ASYNC_READ
{
	OVERLAPPED ov;
	char *pszBuffer;
	UINT uiSize;
	HANDLE hfiler;
} ASYNC_READ, *PASYNC_READ;

VOID CALLBACK ReadCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	PASYNC_READ pAsyncRead = (PASYNC_READ)lpOverlapped;
	unsigned char* ppb = pAsyncRead->pszBuffer;

	UINT8 ethertypeh = ppb[12];
	UINT8 ethertypel = ppb[13];
	if (ethertypeh == 0x08 && ethertypel == 0x00)
	{
		UINT16 protocol = ppb[14 + 9];
		if (protocol == 0x06)
		{
			UINT8 p1 = ppb[26];
			UINT8 p2 = ppb[27];
			UINT8 p3 = ppb[28];
			UINT8 p4 = ppb[29];
//			UINT8 p5 = ppb[30];
//			UINT8 p6 = ppb[31];
//			UINT8 p7 = ppb[32];
//			UINT8 p8 = ppb[33];
			UINT16 srcPort = ppb[14 + 20];
			srcPort = srcPort << 8;
			srcPort += ppb[14 + 20 + 1];
			UINT16 dstPort = ppb[14 + 20 + 2];
			dstPort = dstPort << 8;
			dstPort += ppb[14 + 20 + 3];
			struct servent *pReceiver = 0;
			pReceiver = getservbyport(htons(dstPort), NULL);
			if(pReceiver)
				printf("Inbound=>%s: %d.%d.%d.%d[%d] ", pReceiver->s_name, p1, p2, p3, p4, srcPort);
			else
				printf("Inbound=>%d: %d.%d.%d.%d[%d] ", dstPort, p1, p2, p3, p4, srcPort);
			printf("\r\n");
		}
	}

	memset(pAsyncRead->pszBuffer, 0, 1500);
	ReadFileEx(pAsyncRead->hfiler, pAsyncRead->pszBuffer, pAsyncRead->uiSize - 1, (LPOVERLAPPED)pAsyncRead, ReadCompletionRoutine);
}

int main()
{
	HANDLE hfilter = INVALID_HANDLE_VALUE;
	hfilter = CreateFile("\\\\.\\NDISLWF", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (hfilter != INVALID_HANDLE_VALUE)
	{
		char buffer[1500] = { 0 };
		ASYNC_READ AsyncRead = { 0 };
		AsyncRead.pszBuffer = buffer;
		AsyncRead.hfiler = hfilter;
		AsyncRead.uiSize = sizeof(buffer);
		ReadFileEx(AsyncRead.hfiler, AsyncRead.pszBuffer, AsyncRead.uiSize - 1, (LPOVERLAPPED)&AsyncRead, ReadCompletionRoutine);

		BOOL bDoClientLoop = TRUE;
		while (bDoClientLoop)
		{
			SleepEx(25, TRUE);
//			int cChar = getchar();
//			switch (cChar)
//			{
//			case 27:
//				bDoClientLoop = FALSE;
//				break;
//			default:
//				break;
//			}
		}

		CloseHandle(hfilter);
	}

	return 0;
}
