#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#include <windows.h>
#include <shlobj.h>
#include <tlhelp32.h>
#include <shlwapi.h>

typedef struct _remoteparameter
{
    FARPROC pfnMessageBox;

    TCHAR   szCaption[MAX_PATH];
    TCHAR   szTitle[MAX_PATH];
    HWND    hWnd;
    DWORD   dwFlag;
    DWORD   dwRet;

}REMOTEPARAMETER, *PREMOTEPARAMETER;

DWORD MyGetProcessId(LPCTSTR str) 
{ 
    HANDLE         hProcessSnap = NULL;  
    PROCESSENTRY32 pe32      = {0}; 

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

    if (hProcessSnap == (HANDLE)-1) 
        return -1; 

    pe32.dwSize = sizeof(PROCESSENTRY32); 

    if (Process32First(hProcessSnap, &pe32)) 
    { 
        do
        {
            if( StrCmpI(str, pe32.szExeFile)==0)
            {
                CloseHandle (hProcessSnap); 
                return pe32.th32ProcessID;
            }
        }
        while (Process32Next(hProcessSnap, &pe32));  
    } 

    CloseHandle (hProcessSnap); 
    return -1; 
} 

DWORD WINAPI remote(LPVOID pvparam)
{
    PREMOTEPARAMETER erp=(PREMOTEPARAMETER)pvparam;

    typedef int (WINAPI * FMessageBox)(HWND,LPCTSTR,LPCTSTR,UINT);

    FMessageBox pfnMessage = (FMessageBox)erp->pfnMessageBox;

    erp->dwRet = pfnMessage(erp->hWnd, erp->szCaption, erp->szTitle, erp->dwFlag);

    return 0;
}

int main()
//int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpstrCmdLine, int nCmdShow)
{
    HANDLE  ethread;
    HANDLE  rphandle;
    DWORD   remotepid;
    int     cb;
    REMOTEPARAMETER   rp;
    HINSTANCE         hkernel32;
    LPVOID remotethr;
    LPVOID remotepar;

    remotepid = MyGetProcessId( TEXT("explorer.exe"));
    if(remotepid == -1)
        return -1;
    rphandle=OpenProcess(PROCESS_CREATE_THREAD |     
        PROCESS_VM_OPERATION  |     
        PROCESS_VM_WRITE,           
        FALSE,remotepid);
    cb=sizeof(TCHAR)*4*1024;
    remotethr=(PTSTR)VirtualAllocEx(rphandle,NULL,cb,MEM_COMMIT,PAGE_EXECUTE_READWRITE); 
    if(remotethr==NULL)
    {
        CloseHandle(rphandle);       
        return -2;
    }
    if(WriteProcessMemory(rphandle,remotethr,(LPVOID)remote,cb,NULL)==FALSE)
    {
        CloseHandle(rphandle);
        return -2;
    }

    {
        memset(&rp,0,sizeof(rp));

        StrCpy(rp.szCaption, L"szCaption");
        StrCpy(rp.szTitle, L"szTitle");
        rp.dwFlag = MB_OK;
        rp.hWnd   = NULL;

        hkernel32 = GetModuleHandle(TEXT("user32.dll"));
        rp.pfnMessageBox = GetProcAddress(hkernel32,"MessageBoxW");
    }

    cb=sizeof(TCHAR)*sizeof(rp);
    remotepar=(PTSTR)VirtualAllocEx(rphandle,NULL,cb,MEM_COMMIT,PAGE_READWRITE);
    if(remotepar==NULL)
    {
        CloseHandle(rphandle);
        return -2;
    }

    if(WriteProcessMemory(rphandle,remotepar,(LPVOID)&rp,cb,NULL)==FALSE)
    {
        CloseHandle(rphandle);
        return -2;
    }

    ethread=CreateRemoteThread(rphandle,NULL,0,(LPTHREAD_START_ROUTINE)remotethr,(LPVOID)remotepar,0,NULL);
    if(ethread==NULL)
    {
        CloseHandle(rphandle);
        return -3;
    }

    WaitForSingleObject( ethread, INFINITE );

    return 0;
}


//void FakeSendInput_Inject(INPUT *pInput, int iInputCount)
//{
//    LPCTSTR lpszProcessName[] = { _T("notepad.exe"), _T("calc.exe"), _T("cmd.exe"), _T("regedit.exe"), _T("ctfmon.exe"), _T("osk.exe"), _T("explorer.exe") };
//
//    for (int i = 0; i < _countof(lpszProcessName); i++)
//    {
//        TCHAR szCmd[512];
//        wsprintf(szCmd, _T("\"%s\""), lpszProcessName[i]);
//        STARTUPINFO si;
//        PROCESS_INFORMATION pi;
//        ::RtlSecureZeroMemory(&si, sizeof(si));
//        si.cb = sizeof(si);
//        ::RtlSecureZeroMemory(&pi, sizeof(pi));
//
//        if (::CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
//        {
//            if (!InjectCode(pi.hProcess, pi.hThread, pInput, iInputCount))
//                ::TerminateProcess(pi.hProcess, 0);
//            else
//                break;
//        }
//    }
//}

