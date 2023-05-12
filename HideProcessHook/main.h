#include <Windows.h>
#include <winternl.h>
#include <stdio.h>
#include <stdint.h>

typedef struct _SYSTEM_PROCESS_INFO
{
	ULONG                   NextEntryOffset;
	ULONG                   NumberOfThreads;
	LARGE_INTEGER           Reserved[3];
	LARGE_INTEGER           CreateTime;
	LARGE_INTEGER           UserTime;
	LARGE_INTEGER           KernelTime;
	UNICODE_STRING          ImageName;
	ULONG                   BasePriority;
	HANDLE                  ProcessId;
	HANDLE                  InheritedFromProcessId;
}SYSTEM_PROCESS_INFO, *PSYSTEM_PROCESS_INFO;

typedef enum _MEMORY_INFORMATION_CLASS
{
	MemoryBasicInformation, // MEMORY_BASIC_INFORMATION
	MemoryWorkingSetInformation, // MEMORY_WORKING_SET_INFORMATION
	MemoryMappedFilenameInformation, // UNICODE_STRING
	MemoryRegionInformation, // MEMORY_REGION_INFORMATION
	MemoryWorkingSetExInformation, // MEMORY_WORKING_SET_EX_INFORMATION
	MemorySharedCommitInformation // MEMORY_SHARED_COMMIT_INFORMATION
} MEMORY_INFORMATION_CLASS;

typedef NTSTATUS(NTAPI* NtQueryVirtualMemory_t)(
	_In_  HANDLE                    ProcessHandle,
	_In_  PVOID                     BaseAddress,
	_In_  MEMORY_INFORMATION_CLASS  MemoryInformationClass,
	_Out_ PVOID                     Buffer,
	_In_  ULONG                     Length,
	_Out_ PULONG                    ResultLength OPTIONAL
	);