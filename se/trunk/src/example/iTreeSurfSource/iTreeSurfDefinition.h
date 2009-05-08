
#ifndef _ITREE_SURF_DEFINITION_H
#define _ITREE_SURF_DEFINITION_H
////

//for debug "dead some seconds when surfing"
#define _ITREE_SURF_LOG_TO_FILE_FOR_DEBUG //when release, mark this line


// Version strings
#ifdef _ITREE_SURF_DYNAMIC
#define _ITREE_SURF_VERSION		"3.6.1D" //dynamic version
#else
#define _ITREE_SURF_VERSION		"3.6.1" //static version
#endif

#define _ITREE_SURF_VERSION_INT		36156
#define _ITREE_SURF_APP_NAME		"iTreeSurf "

#define _ITREE_SURF_BUILD_NUMBER	" (Build 056)"

#define _ITREE_SURF_MOST_FAVOR_FILE	"itreesurffolder.mic"
#define _ITREE_SURF_MAP_FILE_NAME	"ITREE_SURF_MAP_FILE"
// URLs
#define _ITREE_SURF_HOME			"http://iTreeSurf.126.com/"
#define _ITREE_SURF_BBS				"http://iTreeSurf.yeah.net/"
#define _ITREE_SURF_CO_DEV			"http://iTreeSurfDev.126.com/"

// Files
#define _ITREE_SURF_INI_FILE_		"iTreeSurf.ini"
#define _LANGUAGE_INI_FILE_			"LTLanguage.ini"
#define _QUICK_SEARCH_INI_FILE_		"LTQuickSearch.ini"
#define _URL_FILTER_INI_FILE_		"LTUrlFilter.ini"
#define _FORM_DATA_INI_FILE_		"LTFormData.ini"
#define _SAVE_TEXT_TXT_FILE_		"SaveText.txt"
#define _URL_LIST_HTM_FILE_			"urllist.htm"
#define _ITREE_SURF_HELP_FILE_		"help.htm"

// Folders
#define _RELATIVE_LANGUAGE_ROOT_	"languages\\"
#define _RELATIVE_SKIN_ROOT_		"skins\\"
#define _RELATIVE_GROUP_ROOT_		"groups\\"
#define _RELATIVE_BMP_ROOT_			"bitmaps\\"

// _LANGUAGE_INI_FILE_ {
#define _INI_SECTION_LANG			"Language"
#define _INI_VAR_AUTO				"AutoSelect"
// Current
#define _INI_SECTION_CURRENT		"Current"
#define _INI_VAR_CURRENT_LANG		"CurrentLang"
#define _INI_VAR_CURRENT_NAME		"CurrentName"
#define _INI_VAR_CURRENT_FOLDER		"CurrentFolder"
#define _INI_VAR_CURRENT_DLL		"CurrentDll"
// Each language
#define _INI_VAR_LANG_NAME			"Name"
#define _INI_VAR_LANG_FOLDER		"Folder"
#define _INI_VAR_LANG_DLL			"Dll"
// _LANGUAGE_INI_FILE_ }

// _ITREE_SURF_INI_FILE_ {
#define _INI_SECTION_SETTING				"Settings"

#define _INI_VAR_ONE_INSTANCE				"OneInstance"
#define _INI_VAR_ONE_INSTANCE__D			false

#define _INI_VAR_GROUP_PATH					"GroupPath"
#define _INI_VAR_GROUP_PATH__D				".\\groups"

#define _INI_VAR_SKIN_PATH					"SkinPath"
#define _INI_VAR_SKIN_PATH__D				".\\skins"

#define _INI_VAR_TOOLBAR_BMP_PATH			"ImagePath"
#define _INI_VAR_TOOLBAR_BMP_PATH__D		".\\bitmaps"

#define _INI_VAR_SAVE_PATH					"DefaultDir"
#define _INI_VAR_SAVE_PATH__D				"\\"

#define _INI_VAR_FAVORITE_PATH				"StartFolder"
#define _INI_VAR_FAVORITE_PATH__D			"\\"

#define _INI_VAR_TAB_POSITION				"TabPos"
#define _INI_VAR_TAB_POSITION__D			0

#define _INI_VAR_OPEN_TYPE					"goHome"
#define _INI_VAR_OPEN_TYPE__D				false

//max number of windows when open windows "at the same time",
// NOT the max number of all windows you can open.
#define _INI_VAR_MAX_WINDOWS_NUMBER			"MaxWins"
#define _INI_VAR_MAX_WINDOWS_NUMBER__D		100

#define _INI_VAR_RUNNING					"Running"
#define _INI_VAR_RUNNING__D					false

#define _INI_VAR_LAST_VISIT					"LastVisit"

#define _INI_VAR_FULL_SCREEN				"FullScreen"
#define _INI_VAR_FULL_SCREEN__D				false

#define _INI_SECTION_TOOLBAR				"ToolBar"
#define _INI_VAR_TOOLBAR_ID__D				-5

//About proxy
#define _INI_VAR_ENABLE_PROXY				"EnableProxy"
#define _INI_VAR_ENABLE_PROXY__D			-1

// _ITREE_SURF_INI_FILE_ }

// some values used
#define __LT_PATH_EMPTY__		1001
#define __LT_PATH_NOT_EXIST__	1002



////
#endif //_ITREE_SURF_DEFINITION_H