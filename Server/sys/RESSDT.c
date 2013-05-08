#include <ntddk.h>
#include <devioctl.h>

#define IOCTL_SETPROC  (ULONG)CTL_CODE(FILE_DEVICE_UNKNOWN, 0x852, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA )

NTSTATUS DisPatchCreateClose(PDEVICE_OBJECT pDriverObj,PIRP pIrp);
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT  DeviceObject,IN PIRP  pIrp);
void DriverUnload(PDRIVER_OBJECT pDriverObj);

typedef struct _tagSSDT {
    PVOID pvSSDTBase;
    PVOID pvServiceCounterTable;
    ULONG ulNumberOfServices;
    PVOID pvParamTableBase;
} SSDT, *PSSDT;

UNICODE_STRING DerName,DerName2;
PDEVICE_OBJECT	pDevObj;

extern PSSDT    KeServiceDescriptorTable;


//StartService时调用
NTSTATUS DriverEntry( IN PDRIVER_OBJECT theDriverObject, IN PUNICODE_STRING theRegistryPath )
{
	NTSTATUS status=STATUS_SUCCESS;
	ULONG i;
	__asm
	{
		pushad
		xor eax, ebx
		sub ebx, ecx
		add ecx, 0x989898
		sub ecx, eax
		xor ebx, 0x87
		popad
	}

	for(i= 0;i<IRP_MJ_MAXIMUM_FUNCTION;++i)
		theDriverObject->MajorFunction[i] = DisPatchCreateClose;

	theDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]=DispatchDeviceControl;
	theDriverObject->DriverUnload=DriverUnload;

	RtlInitUnicodeString(&DerName,L"\\Device\\RESSDT");
	status=IoCreateDevice(theDriverObject,0,&DerName,FILE_DEVICE_UNKNOWN,0,FALSE,&pDevObj);
	if(!NT_SUCCESS(status))
	{
		//DbgPrint("IoCreateDevice Fail!");
		return status;
	}

	RtlInitUnicodeString(&DerName2,L"\\??\\RESSDTDOS");
	status=IoCreateSymbolicLink(&DerName2,&DerName);
//	if(!NT_SUCCESS(status))
//		DbgPrint("IoCreateSymbolicLink fail!");

	return status;
}
NTSTATUS DisPatchCreateClose(PDEVICE_OBJECT pDriverObj,PIRP pIrp)
{
//	DbgPrint("DisPatchCreate!");
	pIrp->IoStatus.Status=STATUS_SUCCESS;
	IoCompleteRequest(pIrp,IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}
//服务停止时执行
void DriverUnload(PDRIVER_OBJECT pDriverObj)
{
	if(IoDeleteSymbolicLink(&DerName2)!=STATUS_SUCCESS)
	{
		//DbgPrint("DeleteSymbolicLink Fail!");
	}
	IoDeleteDevice(pDriverObj->DeviceObject);
}
//DeviceIoControl 时执行 
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT  DeviceObject,IN PIRP  pIrp)
{
	NTSTATUS status=STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION pIrpStack=IoGetCurrentIrpStackLocation(pIrp);

	ULONG uIoControlCode=pIrpStack->Parameters.DeviceIoControl.IoControlCode;
	PVOID pInputBuffer= pIrpStack->Parameters.DeviceIoControl.Type3InputBuffer;
	PVOID pOutputBuffer=pIrp->UserBuffer;
	ULONG uInsize=pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG uOutsize=pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

	//DbgPrint("DispatchDeviceControl  Code:%X",uIoControlCode);
	switch(uIoControlCode)
	{
		case IOCTL_SETPROC:
		{
			ULONG uIndex = 0;
			PULONG pBase = NULL;

			__try
			{
				ProbeForRead( pInputBuffer, sizeof( ULONG ), sizeof( ULONG ) );
				ProbeForWrite( pOutputBuffer, sizeof( ULONG ), sizeof( ULONG ) );
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
				status= GetExceptionCode();
				break;
			}


			uIndex = *(PULONG)pInputBuffer;
			if ( KeServiceDescriptorTable->ulNumberOfServices <= uIndex )
			{
				status= STATUS_INVALID_PARAMETER;
				break;
			}
			
			pBase  = KeServiceDescriptorTable->pvSSDTBase;

		//	DbgPrint("0x%x 0x%x",uIndex,*((PULONG)pOutputBuffer));
			__asm
			{//关中断
            		cli
				mov eax,cr0
				and eax,~0x10000
				mov cr0,eax
			}
			*( pBase + uIndex )=*((PULONG)pOutputBuffer);
			__asm
			{//开中断
				mov  eax,cr0
				or   eax,0x10000
				mov  cr0,eax
				sti
			}
			
			status=STATUS_SUCCESS;
		}
		break;
		default:
			break;
	}
	if(status==STATUS_SUCCESS)
		pIrp->IoStatus.Information=uOutsize;
	else
		pIrp->IoStatus.Information=0;
	
	pIrp->IoStatus.Status=status;
	IoCompleteRequest(pIrp,IO_NO_INCREMENT);
	return status;	
}