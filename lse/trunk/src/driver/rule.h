/********************************************************************
	created:	2008/10/24
	created:	24:10:2008   16:31
	filename: 	d:\lidengwang\lse\trunk\src\driver\rule.h
	file path:	d:\lidengwang\lse\trunk\src\driver
	file base:	rule
	file ext:	h
	author:		lidengwang
	
	purpose:	
*********************************************************************/

#pragma once


#pragma pack(push, 1)

typedef struct tag_RULE_INFO
{
    LIST_ENTRY	ListEntry;
    
    UINT  uContentType;
    UINT  uEnable;

union{
    UINT  uProcessId;
    CHAR  szFileName[MAX_PATH];
    WCHAR wszFileName[MAX_PATH];
    WCHAR wszPath[MAX_PATH];
    WCHAR wszPathFile[MAX_PATH];
    };

}RULE_INFO, *LP_RULE_INFO;


typedef struct tag_RULE_LIST
{
    LIST_ENTRY  RuleList;
    KSPIN_LOCK  RuleLock;

}RULE_LIST, *LP_RULE_LIST;

#pragma pack(pop)


VOID InitRuleList  ( LP_RULE_LIST RuleList );
VOID UninitRuleList( LP_RULE_LIST RuleList );

BOOL AppendRule( LP_RULE_LIST RuleList, LP_RULE_INFO RuleInfo );
VOID RemoveRule( LP_RULE_LIST RuleList, LP_RULE_INFO RuleInfo );

PLIST_ENTRY FindRule( LP_RULE_LIST RuleList, LP_RULE_INFO RuleInfo );

PLIST_ENTRY MatchingRule( 
    LP_RULE_LIST RuleList,
    UINT         uContentType,
    VOID*        pContent);


extern RULE_LIST g_BlackRuleList;
extern RULE_LIST g_WhiteRuleList;
extern RULE_LIST g_ProtectRuleList;

extern RULE_LIST g_RunPIDList;


BOOL AppendRule_I( 
    LP_RULE_LIST RuleList, 
    UINT  uContentType,
    UINT  uEnable,
    VOID* pContent);


BOOL InitDefaultRuleList( UINT uRuleType );
