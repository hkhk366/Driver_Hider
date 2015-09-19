#pragma once

#ifdef __cplusplus
extern "C" 
{
#endif
#include <NTDDK.h>
#include <stdlib.h>
#include "ssdt_hook.c"
#ifdef __cplusplus
}
#endif




#define PAGEDCODE code_seg("PAGE")
#define LOCKEDCODE code_seg()
#define INITCODE code_seg("INIT")

#define PAGEDDATA data_seg("PAGE")
#define LOCKEDDATA data_seg()
#define INITDATA data_seg("INIT")

#define arraysize(p) (sizeof(p)/sizeof((p)[0]))


#define MAX_PROCESS_ARRARY_LENGTH		1024


NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation (
	__in SYSTEM_INFORMATION_CLASS SystemInformationClass,
	__out_bcount_opt(SystemInformationLength) PVOID SystemInformation,
	__in ULONG SystemInformationLength,
	__out_opt PULONG ReturnLength
	);

typedef NTSTATUS (* NTQUERYSYSTEMINFORMATION)(
	__in SYSTEM_INFORMATION_CLASS SystemInformationClass,
	__out_bcount_opt(SystemInformationLength) PVOID SystemInformation,
	__in ULONG SystemInformationLength,
	__out_opt PULONG ReturnLength
	);

NTSTATUS  MyNtQuerySystemInformation (
									 __in SYSTEM_INFORMATION_CLASS SystemInformationClass,
									 __out_bcount_opt(SystemInformationLength) PVOID SystemInformation,
									 __in ULONG SystemInformationLength,
									 __out_opt PULONG ReturnLength
									 );

NTQUERYSYSTEMINFORMATION pOldNtQuerySystemInformation;


typedef NTSTATUS (* NTTERMINATEPROCESS)(
										__in_opt HANDLE ProcessHandle,
										__in NTSTATUS ExitStatus
										);

NTSTATUS MyNtTerminateProcess(
							  __in_opt HANDLE ProcessHandle,
							  __in NTSTATUS ExitStatus
							  );

NTTERMINATEPROCESS pOldNtTerminateProcess;


PUCHAR PsGetProcessImageFileName(__in PEPROCESS Process);


ULONG g_PIDHideArray[MAX_PROCESS_ARRARY_LENGTH];
ULONG g_PIDProtectArray[MAX_PROCESS_ARRARY_LENGTH];
ULONG g_currHideArrayLen = 0;
ULONG g_currProtectArrayLen = 0;


ULONG ValidateProcessNeedHide(ULONG uPID);

ULONG ValidateProcessNeedProtect(ULONG uPID);

ULONG InsertHideProcess(ULONG uPID);

ULONG RemoveHideProcess(ULONG uPID);

ULONG InsertProtectProcess(ULONG uPID);

ULONG RemoveProtectProcess(ULONG uPID);