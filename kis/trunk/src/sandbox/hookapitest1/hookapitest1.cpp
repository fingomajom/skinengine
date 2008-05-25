// hookapitest1.cpp : main source file for hookapitest1.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "resource.h"

#include "MainDlg.h"

CAppModule _Module;

static HMODULE s_hInst = NULL;
static HMODULE s_hDetouredDll = NULL;
static CHAR s_szDllPath[MAX_PATH];
static CHAR s_szDetouredDllPath[MAX_PATH];

static BOOL s_bLog = 1;
static LONG s_nTlsIndent = -1;
static LONG s_nTlsThread = -1;
static LONG s_nThreadCnt = 0;

void Syelog( LPCTSTR format, ... )
{
    TCHAR szBuffer[1024] = { 0 };

    va_list arglist;
    va_start(arglist, format);

    _stprintf_s(szBuffer, format, arglist);

    OutputDebugStr(szBuffer);
}


PCHAR DetRealName(PCHAR psz)
{
    PCHAR pszBeg = psz;
    // Move to end of name.
    while (*psz) {
        psz++;
    }
    // Move back through A-Za-z0-9 names.
    while (psz > pszBeg &&
        ((psz[-1] >= 'A' && psz[-1] <= 'Z') ||
        (psz[-1] >= 'a' && psz[-1] <= 'z') ||
        (psz[-1] >= '0' && psz[-1] <= '9'))) {
            psz--;
    }
    return psz;
}


VOID DetAttach(PVOID *ppbReal, PVOID pbMine, PCHAR psz)
{
    LONG l = DetourAttach(ppbReal, pbMine);
    if (l != 0) {
        Syelog(L"Attach failed: `%s': error %d\n", DetRealName(psz), l);
    }
}

VOID DetDetach(PVOID *ppbReal, PVOID pbMine, PCHAR psz)
{
    LONG l = DetourDetach(ppbReal, pbMine);
    if (l != 0) {
        Syelog(L"Detach failed: `%s': error %d\n", DetRealName(psz), l);
    }
}

#define ATTACH(x,y)   DetAttach(x,y,#x)
#define DETACH(x,y)   DetDetach(x,y,#x)


extern "C" {

    int (WINAPI *
        Real_MessageBoxA)(
        HWND hWnd,
        LPCSTR lpText,
        LPCSTR lpCaption,
        UINT uType)
        = MessageBoxA;

    int (WINAPI *
        Real_MessageBoxW)(
        HWND hWnd,
        LPCWSTR lpText,
        LPCWSTR lpCaption,
        UINT uType)
        = MessageBoxW;
};


int WINAPI Mine_MessageBoxA(
     HWND hWnd,
     LPCSTR lpText,
     LPCSTR lpCaption,
     UINT uType)
{
    return Real_MessageBoxA(hWnd, lpText, lpCaption, uType);
}

int WINAPI Mine_MessageBoxW(
    HWND hWnd,
    LPCWSTR lpText,
    LPCWSTR lpCaption,
    UINT uType)
{
    return Real_MessageBoxW(hWnd, lpText, lpCaption, uType);
}


LONG AttachDetours(VOID)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    ATTACH(&(PVOID&)Real_MessageBoxA, Mine_MessageBoxA);
    ATTACH(&(PVOID&)Real_MessageBoxW, Mine_MessageBoxW);

    return DetourTransactionCommit();
}

LONG DetachDetours(VOID)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DETACH(&(PVOID&)Real_MessageBoxA, Mine_MessageBoxA);
    DETACH(&(PVOID&)Real_MessageBoxW, Mine_MessageBoxW);

    return DetourTransactionCommit();
}


PIMAGE_NT_HEADERS NtHeadersForInstance(HINSTANCE hInst)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hInst;
    __try {
        if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            SetLastError(ERROR_BAD_EXE_FORMAT);
            return NULL;
        }

        PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)pDosHeader +
            pDosHeader->e_lfanew);
        if (pNtHeader->Signature != IMAGE_NT_SIGNATURE) {
            SetLastError(ERROR_INVALID_EXE_SIGNATURE);
            return NULL;
        }
        if (pNtHeader->FileHeader.SizeOfOptionalHeader == 0) {
            SetLastError(ERROR_EXE_MARKED_INVALID);
            return NULL;
        }
        return pNtHeader;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
    }
    SetLastError(ERROR_EXE_MARKED_INVALID);

    return NULL;
}

BOOL InstanceEnumerate(HINSTANCE hInst)
{
    WCHAR wzDllName[MAX_PATH];

    PIMAGE_NT_HEADERS pinh = NtHeadersForInstance(hInst);
    if (pinh && GetModuleFileNameW(hInst, wzDllName, MAX_PATH)) {
        Syelog(L"### %08lx: %-43.43ls %08x\n",
            hInst, wzDllName, pinh->OptionalHeader.CheckSum);
        return TRUE;
    }
    return FALSE;
}

BOOL ProcessEnumerate()
{
    Syelog(L"######################################################### Binaries\n");
    for (HINSTANCE hInst = NULL; (hInst = DetourEnumerateModules(hInst)) != NULL;) {
        InstanceEnumerate(hInst);
    }
    return TRUE;
}

BOOL ThreadAttach(HMODULE hDll)
{
    (void)hDll;

    if (s_nTlsIndent >= 0) {
        TlsSetValue(s_nTlsIndent, (PVOID)0);
    }
    if (s_nTlsThread >= 0) {
        LONG nThread = InterlockedIncrement(&s_nThreadCnt);
        TlsSetValue(s_nTlsThread, (PVOID)(LONG_PTR)nThread);
    }
    return TRUE;
}

BOOL ThreadDetach(HMODULE hDll)
{
    (void)hDll;

    if (s_nTlsIndent >= 0) {
        TlsSetValue(s_nTlsIndent, (PVOID)0);
    }
    if (s_nTlsThread >= 0) {
        TlsSetValue(s_nTlsThread, (PVOID)0);
    }
    return TRUE;
}


BOOL ProcessAttach(HMODULE hDll)
{
    s_nTlsIndent = TlsAlloc();
    s_nTlsThread = TlsAlloc();

    s_hInst = hDll;
#if 0
    s_hDetouredDll = DetourGetDetouredMarker();
    Real_GetModuleFileNameA(s_hDetouredDll, s_szDetouredDllPath, ARRAYOF(s_szDetouredDllPath));
    Real_GetModuleFileNameA(s_hInst, s_szDllPath, ARRAYOF(s_szDllPath));
#endif

    ProcessEnumerate();

    LONG error = AttachDetours();
    if (error != NO_ERROR) {
        Syelog(L"### Error attaching detours: %d\n", error);
    }

    ThreadAttach(hDll);

    s_bLog = TRUE;
    return TRUE;
}

BOOL ProcessDetach(HMODULE hDll)
{
    ThreadDetach(hDll);
    s_bLog = FALSE;

    LONG error = DetachDetours();
    if (error != NO_ERROR) {
        Syelog(L"### Error detaching detours: %d\n", error);
    }

    Syelog(L"### Closing.\n");
    //SyelogClose(FALSE);

    if (s_nTlsIndent >= 0) {
        TlsFree(s_nTlsIndent);
    }
    if (s_nTlsThread >= 0) {
        TlsFree(s_nTlsThread);
    }
    return TRUE;
}



int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

    Sleep(50);
    DetourRestoreAfterWith();

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = 0;
	// BLOCK: Run application
	{
        ProcessAttach(hInstance);

		CMainDlg dlgMain;
		nRet = dlgMain.DoModal();

        ProcessDetach(hInstance);
	}

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
