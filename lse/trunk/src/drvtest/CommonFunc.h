#pragma once

BOOL InstallDriver( LPCTSTR pszDriverName );
BOOL StartDriver( LPCTSTR pszDriverName );
BOOL OpenDevice(LPCTSTR DriverName, HANDLE *lphDevice);
BOOL StopDriver( LPCTSTR DriverName);
BOOL RemoveDriver( LPCTSTR DriverName );
