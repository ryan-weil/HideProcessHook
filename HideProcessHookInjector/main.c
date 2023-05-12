#include <Windows.h>
#include <stdio.h>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

#define ERROR(x)\
printf(x);\
getchar();\
getchar();\
return -1;\

int main()
{
	LPVOID loadlibrary = (LPVOID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");

	if (!PathFileExistsW(L"HideProcessHook.dll"))
	{
		ERROR("Failed to find HideProcessHook.dll in the current directory\n");
	}

	WCHAR buffer[MAX_PATH];
	BOOL result = GetFullPathNameW(L"HideProcessHook.dll", MAX_PATH, buffer, NULL);

	printf("Please input the selected pid to inject into: \n");

	UINT32 pid;
	scanf_s("%d", &pid);

	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (!handle)
	{
		ERROR("Failed to get a handle to pid\n");
	}

	LPVOID address = (LPVOID)VirtualAllocEx(handle, NULL, (wcslen(buffer) + 1) * sizeof(WCHAR), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (!address)
	{
		ERROR("Failed to allocate string in remote process\n");
	}

	if (!WriteProcessMemory(handle, address, buffer, (wcslen(buffer) + 1) * sizeof(WCHAR), NULL))
	{
		ERROR("Failed to write string to remote process\n");
	}

	HANDLE thread = CreateRemoteThread(handle, NULL, 0, (LPTHREAD_START_ROUTINE)loadlibrary, address, 0, NULL);

	if (!thread)
	{
		ERROR("Failed to create remote thread\n");
	}

	if (WaitForSingleObject(thread, INFINITE) == WAIT_FAILED)
	{
		ERROR("Waiting for thread failed\n");
	}
		
	DWORD ret = 0;
	if (!GetExitCodeThread(thread, &ret)) 
	{
		ERROR("Failed to get thread exit code\n");
	}

	if (!ret)
	{
		ERROR("Bad thread exit code\n");
	}
		
	CloseHandle(thread);

	CloseHandle(handle);

	return 0;
}