// Rasspdmon.h : interface of the RAS speed monitor
//
////////////////////////////////////////////////////////////////////////////

#if !defined( _ITREESURF__RASSPDMON_H__INCLUDED_ )
#define _ITREESURF__RASSPDMON_H__INCLUDED_


#ifndef __RASSPDMON_H_
#define __RASSPDMON_H_

extern BOOL glo_fInitialized, glo_fInitializing;

void InitSpeedMonitor(void);
void ClearSpeedMonitor(void);
void GetRASSpeed(/*DWORD* pTPS,DWORD* pRPS,DWORD* pTT,*/ DWORD* pRT);
////参数：每秒发送，每秒接收，总发送，总接受
UINT _cdecl TInitialMonitor(LPVOID con);


#endif



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( _ITREESURF__RASSPDMON_H__INCLUDED_ )
