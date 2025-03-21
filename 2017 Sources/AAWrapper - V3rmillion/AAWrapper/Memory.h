#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <WinInet.h>
#include <fstream>
#include <istream>
#include <iterator>
#include <algorithm>
#include <string>
#include <Psapi.h>
#include <tlhelp32.h>

#define aslr(x)(x - 0x400000 + (DWORD)GetModuleHandleA(0))

class Memory
{
public:
	static void WriteBytes(DWORD addr, BYTE* bytes, int len);
	static DWORD Hook(BYTE* addr, DWORD dst, int len);
	static bool Compare(const BYTE *pData, const BYTE *bMask, const char *szMask);
	static DWORD FindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char *szMask);
	static int Scan(char * content, char* mask);
	int ScanNoAlign(char *toFind, int bcount, int reachsz = 1024, int start = 0, int endd = 0x50000000, int align = 1);
	int ScanNoAlignW(char* toFind, int bcount, int reachsz = 1024, int start = 0, int endd = 0x50000000, int align = 1);
	static DWORD scan2(const char *AoB, char *Mask);
};


namespace SkId {
	static bool Skidpare(const char* pskid, const char* bskid, const char* szskid)
	{
		while (*szskid) {
			if (*szskid != '?') {
				if (*pskid != *bskid) return 0;
			}
			++szskid, ++pskid, ++bskid;
		}
		return 1;
	}

	static DWORD ScanforSkid(const char* vfskid)
	{
		MEMORY_BASIC_INFORMATION SKIDI = { 0 };
		SYSTEM_INFO SkId = { 0 };
		GetSystemInfo(&SkId);
		DWORD StartSkId = (DWORD)SkId.lpMinimumApplicationAddress;
		DWORD EndSkId = (DWORD)SkId.lpMaximumApplicationAddress;
		do
		{
			while (VirtualQuery((void*)StartSkId, &SKIDI, sizeof(SKIDI))) {
				if ((SKIDI.Protect & PAGE_READWRITE) && !(SKIDI.Protect & PAGE_GUARD))
				{
					for (DWORD i = (DWORD)(SKIDI.BaseAddress); i - (DWORD)(SKIDI.BaseAddress) < SKIDI.RegionSize; ++i)
					{
						if (Skidpare((const char*)i, vfskid, "xxxx"))
							return i;
					}
				}
				StartSkId += SKIDI.RegionSize;
			}
		} while (StartSkId < EndSkId);
		return 0;
	}
}

static DWORD Memcheck_Addr = aslr(0x59D5F0); /* Sniped. */

static CRITICAL_SECTION CS;
static LONG ChunkCurrentIndex = -1;
static DWORD ChunkHolder, oldProtection;

static LONG WINAPI MemoryCheck_ExceptionHandler(PEXCEPTION_POINTERS ExceptionPointers) 
																			  
{
	if (ExceptionPointers->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
		return EXCEPTION_EXECUTE_HANDLER;
	return EXCEPTION_CONTINUE_EXECUTION;
}
																				
static BOOL MemoryCheck_NextChunk(DWORD Max, DWORD& ChunkLocation) /* This function is used to check for the next chunks generated by the Memory Check. */
{
	DWORD CurrentChunk = (DWORD)GetModuleHandle("RobloxPlayerBeta.exe") - 2048;
	CurrentChunk += 2048;
	ChunkLocation = CurrentChunk;
	ChunkCurrentIndex += 1;
	if (CurrentChunk >= Max)
		return FALSE;
	return TRUE;
}

static void MemoryCheck_Disable()
{
	HMODULE RobloxPlayerBeta = GetModuleHandle("RobloxPlayerBeta.exe");
	//FreeModule(RobloxPlayerBeta); /* Frees the Module. This is used to prevent any threads in ROBLOX that aren't being used up. */
	//InitializeCriticalSection(&CS);


	printf("Trying to JMP The bypass.\n");
	MODULEINFO RobloxPlayerBetaInfo;
	GetModuleInformation(GetCurrentProcess(), RobloxPlayerBeta, &RobloxPlayerBetaInfo, sizeof(MODULEINFO));
	PVOID ModuleCopy = VirtualAlloc(NULL, RobloxPlayerBetaInfo.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	
	printf("Got Memcheck_NextChunk.\n");

	WriteProcessMemory(GetCurrentProcess(), *(LPVOID*)Memcheck_Addr, "\x90", 9, NULL); /* NOP */
	//LeaveCriticalSection(&CS);
}