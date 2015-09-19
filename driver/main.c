#ifdef __cplusplus
extern "C" 
{
#endif
#include "inc.c"
#ifdef __cplusplus
}
#endif

#define DEVICE_NAME_PROCESS				L"\\Device\\SSDTProcess"
#define SYMBOLINK_NAME_PROCESS			L"\\??\\SSDTProcess"

NTSTATUS SSDTGeneralDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);
NTSTATUS SSDTCreateDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);
NTSTATUS SSDTCloseDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);
NTSTATUS SSDTReadDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);
NTSTATUS SSDTWriteDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);
NTSTATUS SSDTDeviceIoControlDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp);

VOID SSDTDriverUnload(IN PDRIVER_OBJECT pDriverObject);


#define	IO_INSERT_HIDE_PROCESS			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IO_REMOVE_HIDE_PROCESS			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IO_INSERT_PROTECT_PROCESS		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IO_REMOVE_PROTECT_PROCESS		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)

NTSTATUS DriverEntry (
			IN PDRIVER_OBJECT pDriverObject,
			IN PUNICODE_STRING pRegistryPath	) 
{
	ULONG i;
	NTSTATUS status;
	UNICODE_STRING strDeviceName;
	UNICODE_STRING strSymbolLinkName;
	PDEVICE_OBJECT pDeviceObject;

	pDeviceObject = NULL;

	RtlInitUnicodeString(&strDeviceName, DEVICE_NAME_PROCESS);
	RtlInitUnicodeString(&strSymbolLinkName, SYMBOLINK_NAME_PROCESS);

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriverObject->MajorFunction[i] = SSDTGeneralDispatcher;
	}

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = SSDTCreateDispatcher;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = SSDTCloseDispatcher;
	pDriverObject->MajorFunction[IRP_MJ_READ] = SSDTReadDispatcher;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = SSDTWriteDispatcher;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = SSDTDeviceIoControlDispatcher;

	pDriverObject->DriverUnload = SSDTDriverUnload;

	status = IoCreateDevice(pDriverObject, 0, &strDeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDeviceObject);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	if (!pDeviceObject)
	{
		return STATUS_UNEXPECTED_IO_ERROR;
	}


	pDeviceObject->Flags |= DO_DIRECT_IO;
	pDeviceObject->AlignmentRequirement = FILE_WORD_ALIGNMENT;
	status = IoCreateSymbolicLink(&strSymbolLinkName, &strDeviceName);

	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;


	BackupSysServicesTable();


	InstallSSDTHook((ULONG)ZwQuerySystemInformation, (ULONG)MyNtQuerySystemInformation);
	InstallSSDTHook((ULONG)ZwTerminateProcess, (ULONG)MyNtTerminateProcess);
	return STATUS_SUCCESS;
}

NTSTATUS SSDTGeneralDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return pIrp->IoStatus.Status;
}
NTSTATUS SSDTCreateDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}
NTSTATUS SSDTCloseDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}
NTSTATUS SSDTReadDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	NTSTATUS rtStatus;

	rtStatus = STATUS_NOT_SUPPORTED;

	return rtStatus;
}
NTSTATUS SSDTWriteDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	NTSTATUS rtStatus;

	rtStatus = STATUS_NOT_SUPPORTED;

	return rtStatus;
}
NTSTATUS SSDTDeviceIoControlDispatcher(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp)
{
	NTSTATUS rtStatus;

	ULONG uPID;
	ULONG uInLen;
	ULONG uOutLen;
	ULONG uCtrlCode;

	PCHAR pInBuffer;

	PIO_STACK_LOCATION pStack;	

	uPID = 0;
	rtStatus = STATUS_SUCCESS;
	pStack = IoGetCurrentIrpStackLocation(pIrp);

	uInLen = pStack->Parameters.DeviceIoControl.InputBufferLength;
	uOutLen = pStack->Parameters.DeviceIoControl.OutputBufferLength;
	uCtrlCode = pStack->Parameters.DeviceIoControl.IoControlCode;


	pInBuffer = (PCHAR)pIrp->AssociatedIrp.SystemBuffer;

	if(uInLen >= 4)
	{

		uPID = atol(pInBuffer);

		switch(uCtrlCode)
		{
		case IO_INSERT_PROTECT_PROCESS:
			{
				if(InsertProtectProcess(uPID) == FALSE)
				{
					rtStatus = STATUS_PROCESS_IS_TERMINATING;
				}
				break;
			}
		case IO_REMOVE_PROTECT_PROCESS:
			{
				if(RemoveProtectProcess(uPID) == FALSE)
				{
					rtStatus = STATUS_PROCESS_IS_TERMINATING;
				}
				break;
			}
		case IO_INSERT_HIDE_PROCESS:
			{
				if(InsertHideProcess(uPID) == FALSE)
				{
					rtStatus = STATUS_PROCESS_IS_TERMINATING;
				}
				break;
			}
		case IO_REMOVE_HIDE_PROCESS:
			{
				if(RemoveHideProcess(uPID) == FALSE)
				{
					rtStatus = STATUS_PROCESS_IS_TERMINATING;
				}
				break;
			}
		default:
			{
				rtStatus = STATUS_INVALID_VARIANT;
				break;
			}
		}
	}
	else
	{
		rtStatus = STATUS_INVALID_PARAMETER;
	}


	pIrp->IoStatus.Status = rtStatus;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return rtStatus;
}
VOID SSDTDriverUnload(IN PDRIVER_OBJECT pDriverObject)
{
	UNICODE_STRING strSymbolLinkName;

	DbgPrint("In SSDTDriverUnload !");

	RtlInitUnicodeString(&strSymbolLinkName, SYMBOLINK_NAME_PROCESS);
	IoDeleteSymbolicLink(&strSymbolLinkName);
	IoDeleteDevice(pDriverObject->DeviceObject);


	UnInstallSSDTHook((ULONG)ZwQuerySystemInformation);


	UnInstallSSDTHook((ULONG)ZwTerminateProcess);

	DbgPrint("Out SSDT01DriverUnload !");
}