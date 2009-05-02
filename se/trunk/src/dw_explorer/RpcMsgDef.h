/********************************************************************
	created:	2009/05/02
	created:	2:5:2009   10:52
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\RpcMsgDef.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	RpcMsgDef
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/

#pragma once

#define Rpc_Svr_EPoint_Name  _T("C40A5CFA-07A4-4c87-9983-4B8551B7B643")
#define Rpc_Clt_EPoint_Name  _T("5503D213-0664-4c11-84FF-FB029F50FC9C")


//////////////////////////////////////////////////////////////////////////
//  server -> client
//////////////////////////////////////////////////////////////////////////

#define s2c_create_webwnd           (100)

//////////////////////////////////////////////////////////////////////////
//  client -> server
//////////////////////////////////////////////////////////////////////////

#define c2s_get_pid                 (100)