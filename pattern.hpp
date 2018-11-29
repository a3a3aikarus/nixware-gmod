#pragma once
#include <cstdint>
#include <Windows.h>
#include <assert.h>
#include <algorithm>
#include <array>
#include <Psapi.h>
#pragma comment(lib,"psapi")
#define INRANGE(x,a,b)    (x >= a && x <= b)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

#ifdef __clang__
#define FORCE_INLINE __attribute__((always_inline))
#else
#define FORCE_INLINE __forceinline
#endif

namespace signature
{
	uint64_t find_signature(const char* szModule, const char* szSignature)
	{
		MODULEINFO modInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &modInfo, sizeof(MODULEINFO));
		DWORD startAddress = (DWORD)modInfo.lpBaseOfDll;
		DWORD endAddress = startAddress + modInfo.SizeOfImage;
		const char* pat = szSignature;
		DWORD firstMatch = 0;
		for (DWORD pCur = startAddress; pCur < endAddress; pCur++) {
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
				if (!firstMatch) firstMatch = pCur;
				if (!pat[2]) return firstMatch;
				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2;    //one ?
			}
			else {
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return NULL;
	}

	DWORD find_pattern(DWORD start_offset, DWORD size, BYTE* pattern, char mask[])
	{
		DWORD pos = 0;
		int searchLen = strlen(mask) - 1;

		for (DWORD retAddress = start_offset; retAddress < start_offset + size; retAddress++)
		{
			if (*(BYTE*)retAddress == pattern[pos] || mask[pos] == '?') {
				if (mask[pos + 1] == '\0')
					return (retAddress - searchLen);
				pos++;
			}
			else
				pos = 0;
		}
		return NULL;
	}
}