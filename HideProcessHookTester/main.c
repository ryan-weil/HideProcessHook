#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#include "main.h"

#if _WIN32 || _WIN64
	#if _WIN64
		#define ENVIRONMENT64
	#else
		#define ENVIRONMENT32
	#endif
#endif

int main()
{
	// Make sure to input your own path to the DLL below, or it will not work!
#ifdef ENVIRONMENT64
	LoadLibrary(L"C:\\Users\\Admin\\source\\repos\\HideProcessHook\\x64\\Debug\\HideProcessHook.dll");
#endif
#ifdef ENVIRONMENT32
	LoadLibrary(L"C:\\Users\\Admin\\source\\repos\\HideProcessHook\\Win32\\Debug\\HideProcessHook.dll");
#endif

	NTSTATUS status;
	PVOID buffer;
	PSYSTEM_PROCESS_INFO spi;

	buffer = VirtualAlloc(NULL, 0x100000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!buffer)
		return -1;

	spi = (PSYSTEM_PROCESS_INFO)buffer;

	if (!NT_SUCCESS(status = NtQuerySystemInformation(SystemProcessInformation, spi, 1024 * 1024, NULL)))
	{
		VirtualFree(buffer, 0, MEM_RELEASE);
		return -1;
	}

	while (spi->NextEntryOffset)
	{
		printf("\nProcess name: %ws | Process ID: %d\n", spi->ImageName.Buffer, spi->ProcessId);
		spi = (PSYSTEM_PROCESS_INFO)((LPBYTE)spi + spi->NextEntryOffset);
	}

	getchar();

	VirtualFree(buffer, 0, MEM_RELEASE);
	return 0;
}