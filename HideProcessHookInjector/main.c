#include <Windows.h>
#include <stdio.h>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

int main()
{
	LPVOID loadlibrary = (LPVOID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");

	if (!PathFileExistsW(L"HideProcessHook.dll"))
	{
		printf("Failed to find HideProcessHook.dll in the current directory\n");
		getchar();
		getchar();
		return -1;
	}

	WCHAR buffer[MAX_PATH];
	BOOL result = GetFullPathNameW(L"HideProcessHook.dll", MAX_PATH, buffer, NULL);

	printf("Please input the selected pid to inject into: \n");

	UINT32 pid;
	scanf_s("%d", &pid);

	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (!handle)
	{
		printf("Failed to get a handle to pid %d\n", pid);
		getchar();
		getchar();
		return -1;
	}

	LPVOID address = (LPVOID)VirtualAllocEx(handle, NULL, (wcslen(buffer) + 1) * sizeof(WCHAR), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (!address)
	{
		printf("Failed to allocate string in remote process\n");
		getchar();
		getchar();
		return -1;
	}

	if (!WriteProcessMemory(handle, address, buffer, (wcslen(buffer) + 1) * sizeof(WCHAR), NULL))
	{
		printf("Failed to write string to remote process\n");
		getchar();
		getchar();
		return -1;
	}

	HANDLE thread = CreateRemoteThread(handle, NULL, 0, (LPTHREAD_START_ROUTINE)loadlibrary, address, 0, NULL);

	if(!thread)
	{
		printf("Failed to create remote thread\n");
		getchar();
		getchar();
		return -1;
	}

	if (handle)
		CloseHandle(handle);

	return 0;
}