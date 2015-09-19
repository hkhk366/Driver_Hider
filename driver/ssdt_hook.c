#ifdef __cplusplus
extern "C" 
{
#endif
#include "ssdt_hook.h"
#ifdef __cplusplus
}
#endif

VOID DisableWriteProtect(ULONG oldAttr)
{
	_asm
	{
		mov eax, oldAttr
		mov cr0, eax
		sti;
	}
}



VOID EnableWriteProtect(PULONG pOldAttr)
{
	ULONG uAttr; 

	_asm 
	{ 
		cli;
		mov  eax, cr0; 
		mov  uAttr, eax; 
		and  eax, 0FFFEFFFFh; 
		mov  cr0, eax; 
	}; 

	*pOldAttr = uAttr; 
}


VOID BackupSysServicesTable()
{
	ULONG i;

	for(i = 0; (i < KeServiceDescriptorTable->ntoskrnl.NumberOfService) && (i < MAX_SYSTEM_SERVICE_NUMBER); i++)
	{
		oldSysServiceAddr[i] = KeServiceDescriptorTable->ntoskrnl.ServiceTableBase[i];

		KdPrint(("\nBackupSysServicesTable - Function Information { Number: 0x%04X , Address: %08X}", i, oldSysServiceAddr[i]));
	}
}


NTSTATUS InstallSSDTHook(ULONG oldService, ULONG newService)
{
	ULONG uOldAttr = 0;
	EnableWriteProtect(&uOldAttr);
	SYSCALL_FUNCTION(oldService) = newService;
	DisableWriteProtect(uOldAttr);
	return STATUS_SUCCESS;
}


NTSTATUS UnInstallSSDTHook(ULONG oldService)
{
	ULONG uOldAttr = 0;
	EnableWriteProtect(&uOldAttr);
	SYSCALL_FUNCTION(oldService) = oldSysServiceAddr[SYSCALL_INDEX(oldService)];
	DisableWriteProtect(uOldAttr);
	return STATUS_SUCCESS;
}
