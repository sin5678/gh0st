#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

#pragma comment(lib, "Psapi.lib")

void HideLibrary(HMODULE hModule, LPVOID pCallBackAddr, LPVOID lParam);

typedef struct
{
      HMODULE lpDllBase;
      LPVOID lpNewDllBase;
      PTHREAD_START_ROUTINE pAddress;
      LPVOID lParam;
}UNLOADLIB_CALLBACK, *PUNLOADLIB_CALLBACK;

typedef
LPVOID WINAPI VIRTUALALLOC(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD flAllocationType,
    DWORD flProtect
);

typedef
BOOL WINAPI VIRTUALFREE(
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD dwFreeType
);


typedef
BOOL WINAPI HEAPDESTROY(
    HANDLE hHeap
);

typedef
HMODULE WINAPI LOADLIBRARY(
    LPCTSTR lpFileName
);

typedef
HANDLE WINAPI CREATETHREAD(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
);

typedef void *    __cdecl MEMCPY(void *, const void *, size_t);


BOOL incLibraryCount(HMODULE hMe)
{
      //FreeLibrary后很多系统dll也会free掉，所以将所有已加载的再load一次以增加计数
    
      HANDLE hModsSnap =    CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);

      if(INVALID_HANDLE_VALUE == hModsSnap)
      {
          return FALSE;
      }

      MODULEENTRY32 meModuleEntry;
      meModuleEntry.dwSize = sizeof(MODULEENTRY32);

      if(!Module32First(hModsSnap, &meModuleEntry))
      {
          CloseHandle(hModsSnap);
          return FALSE;
      }
      do
      {
          if(LoadLibrary(meModuleEntry.szModule) == hMe)
              FreeLibrary(hMe);

      } while(Module32Next(hModsSnap, &meModuleEntry));

      CloseHandle(hModsSnap);

      return TRUE;
}

//枚举指定进程的所有线程
DWORD WINAPI EnumAndSetThreadState(LPVOID lParam)
{
      HANDLE hThreadSnap = NULL;
      THREADENTRY32 te32;
      memset(&te32,0,sizeof(te32));
      te32.dwSize = sizeof(THREADENTRY32);
      hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);

      DWORD myThreadId = GetCurrentThreadId();
      DWORD pid = GetCurrentProcessId();

      if (Thread32First(hThreadSnap, &te32))
      {
          do
          {
               if (pid == te32.th32OwnerProcessID)
               {
                   if(myThreadId != te32.th32ThreadID)
                   {
                       HANDLE hThread = OpenThread(
                           THREAD_SUSPEND_RESUME,
                           FALSE,
                           te32.th32ThreadID);

                       if(hThread != NULL)
                       {
                           if((int)lParam)
                               ResumeThread(hThread);
                           else
                               SuspendThread(hThread);

                           CloseHandle(hThread);
                       }
                   }
               }
          }
          while (Thread32Next(hThreadSnap,&te32));
      }
      CloseHandle( hThreadSnap );

      return 0;
}

DWORD WINAPI GotoCallBackAddr(LPVOID lParam)
{
      PUNLOADLIB_CALLBACK cbFunc = (PUNLOADLIB_CALLBACK)lParam;

      DWORD dwThreadId;
      HANDLE hThread;

      if(cbFunc->pAddress)
      {
          hThread = CreateThread(
              NULL,
              0,
              cbFunc->pAddress,
              cbFunc->lParam,
              0,
              &dwThreadId);

          if(hThread)
              CloseHandle(hThread);
      }

      //那份dll的拷贝不需要了，释放~
      VirtualFree(cbFunc->lpNewDllBase, 0, MEM_DECOMMIT);
      delete cbFunc;

      return 0;
}

DWORD WINAPI UnLoadLibrary(LPVOID lParam)
{
      BYTE HeapDestroy_HookCode_bak[4];
      BYTE HeapDestroy_HookCode[4] = "\xC2\x04\x00";//RETN 0004
      MODULEINFO modinfo;
      DWORD oldProtect;

      PUNLOADLIB_CALLBACK cbFunc = (PUNLOADLIB_CALLBACK)lParam;

      HMODULE hDllInstance = cbFunc->lpDllBase;
      char dllpath_bak[MAX_PATH];

      GetModuleFileName(hDllInstance, dllpath_bak, sizeof(dllpath_bak));
      GetModuleInformation(GetCurrentProcess(), hDllInstance, &modinfo, sizeof(MODULEINFO));

      //给所有dll(除了自己)增加计数,防止FreeLibrary的时候那些dll给系统卸载掉
      incLibraryCount(hDllInstance);

      //保险起见，挂起其他线程，搞定后再恢复
      EnumAndSetThreadState((LPVOID)FALSE);

      //FreeLibrary之后原来存放api地址的内存也会被释放，
      //但是FreeLibrary之后还有些动作，趁现在还没free，关键API记下来
      VIRTUALALLOC *_VirtualAlloc = (VIRTUALALLOC*)
          GetProcAddress(GetModuleHandle("kernel32.dll"), "VirtualAlloc");
      LOADLIBRARY    *_LoadLibrary    = (LOADLIBRARY*)
          GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
      CREATETHREAD *_CreateThread = (CREATETHREAD*)
          GetProcAddress(GetModuleHandle("kernel32.dll"), "CreateThread");
      MEMCPY         *_memcpy         = (MEMCPY*)
          GetProcAddress(GetModuleHandle("ntdll.dll"), "memcpy");

      //这个很关键，并不是我要调用，是 FreeLibrary 时系统会调用，我要hook它,
      //不能给系统破坏这个heap，否则之后的dll貌似能工作，
      //但却不能用new或malloc申请内存, VirtualAlloc可以代替之，
      //但如果改写好多代码是划不来的，况且一些代码不好改,如list<T>的push内部的new

      HEAPDESTROY *_HeapDestroy    = (HEAPDESTROY*)
          GetProcAddress(GetModuleHandle("kernel32.dll"), "HeapDestroy");

      VirtualProtect(_HeapDestroy, 3, PAGE_EXECUTE_READWRITE, &oldProtect);

      //修改第一条指令为直接返回
      _memcpy(HeapDestroy_HookCode_bak, _HeapDestroy, 3);
      _memcpy(_HeapDestroy, HeapDestroy_HookCode, 3);


      //Sleep(100);
      //终于到这里了~~~^_^!
      FreeLibrary(hDllInstance);//释放

      //修复刚hook的函数
      _memcpy(_HeapDestroy, HeapDestroy_HookCode_bak, 3);
      //_memcpy(_RtlFreeHeap, RtlFreeHeap_HookCode_bak, 3);

      //在原来的dll基址申请同样大小的内存，并把之前的那份dll拷贝还原回去
      if(_VirtualAlloc(hDllInstance,
          modinfo.SizeOfImage,
          MEM_COMMIT|MEM_RESERVE,
          PAGE_EXECUTE_READWRITE) == NULL
          )
      {
          //失败，加载原来dll, 以正常方式工作
          //注意，不宜在dllmain中调用HideLibrary，LoadLibrary将导致dllmain再次被调用,导致死循环啦
          HMODULE hDll = _LoadLibrary(dllpath_bak);

          //重新计算回调函数在hDll地址空间的地址
          cbFunc->pAddress = (LPTHREAD_START_ROUTINE)
              ((DWORD)cbFunc->pAddress - (DWORD)hDllInstance + (DWORD)hDll);

          LPTHREAD_START_ROUTINE pFunc1 = (LPTHREAD_START_ROUTINE)
              ((DWORD)EnumAndSetThreadState - (DWORD)hDllInstance + (DWORD)hDll);

          //恢复被挂起的线程
          _CreateThread(0, 0, pFunc1, (LPVOID)TRUE, 0, 0);

          //调用回调函数
          if(cbFunc->pAddress)
              _CreateThread(0, 0, cbFunc->pAddress, cbFunc->lParam, 0, 0);

          return 0;
      }

      _memcpy(hDllInstance, cbFunc->lpNewDllBase, modinfo.SizeOfImage);

      //恢复被挂起的线程
      EnumAndSetThreadState((LPVOID)TRUE);

      //跳回原dll地址空间的GotoCallBackAddr，由它来释放这边VirtualAlloc申请的指针
      _CreateThread(0, 0, GotoCallBackAddr, cbFunc, 0, 0);

      return 0;
}

DWORD WINAPI HideLibrary02(LPVOID lParam)
{
      //__asm INT 3

      PUNLOADLIB_CALLBACK cbFunc = (PUNLOADLIB_CALLBACK)lParam;

      MODULEINFO modinfo;

      GetModuleInformation(GetCurrentProcess(), cbFunc->lpDllBase, &modinfo, sizeof(MODULEINFO));

      //申请一块和当前dll同样大小的内存
      cbFunc->lpNewDllBase = VirtualAlloc(NULL, modinfo.SizeOfImage, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);

      if(cbFunc->lpNewDllBase == NULL)
          return FALSE;

      //给当前dll做份拷贝，复制所有数据到刚申请的内存，
      memcpy(cbFunc->lpNewDllBase, modinfo.lpBaseOfDll, modinfo.SizeOfImage);

      //计算在copy中UnLoadLibrary的地址,并另起线程到该地址执行
      void *pNewUnLoadLibrary = LPVOID((DWORD)cbFunc->lpNewDllBase + (DWORD)UnLoadLibrary - (DWORD)modinfo.lpBaseOfDll);

      DWORD ThreadId;
      HANDLE hThread = CreateThread(0,0,
          (LPTHREAD_START_ROUTINE)pNewUnLoadLibrary, (LPVOID)cbFunc, CREATE_SUSPENDED, &ThreadId);

      if(hThread == NULL)
      {
          VirtualFree(cbFunc->lpNewDllBase, 0, MEM_DECOMMIT);
          delete cbFunc;

          return FALSE;
      }

      ResumeThread(hThread);
      CloseHandle(hThread);

      return TRUE;
}


void HideLibrary(HMODULE hModule, LPVOID pCallBackAddr, LPVOID lParam)
{

      PUNLOADLIB_CALLBACK lparam = new UNLOADLIB_CALLBACK;

      lparam->lpDllBase      = hModule;
      lparam->lpNewDllBase = NULL;
      lparam->pAddress       = (PTHREAD_START_ROUTINE)pCallBackAddr;
      lparam->lParam         = lParam;

      HANDLE hThread = CreateThread(0,0,
          HideLibrary02, (LPVOID)lparam, 0, NULL);

      if(hThread == NULL)
      {
          delete lparam;
          return;
      }

      CloseHandle(hThread);

      return;
}

