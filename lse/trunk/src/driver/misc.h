/********************************************************************
	created:	2008/10/18
	created:	18:10:2008   23:39
	filename: 	d:\lidengwang\lse\trunk\src\driver\misc.h
	file path:	d:\lidengwang\lse\trunk\src\driver
	file base:	misc
	file ext:	h
	author:		lidengwang
	
	purpose:	

*********************************************************************/

#pragma once


UINT GetWindowsDirectory(
    LPWSTR lpBuffer,
    UINT   uSize);

BOOL GetProcessFullPath(
    PEPROCESS       pPeProcess,
    PUNICODE_STRING pUniCodeFullPath);


BOOL GetProcessFullPathByID(
    UINT uPID,
    PUNICODE_STRING pUniCodeFullPath);

PEPROCESS GetEProcessByHandle( HANDLE hProcess );