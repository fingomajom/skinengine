#include "stdafx.h"
#include "iTreeSurf.h"
#include "GlobalSpace.h"


///{ static global vars
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_PROCESS,
	ID_INDICATOR_TOOLS,
	ID_INDICATOR_SECU,
	ID_INDICATOR_IP,
	ID_INDICATOR_BP,
	ID_INDICATOR_RES,
	ID_INDICATOR_WINS,
};

static UINT toolbarbuttons[]=
{
	ID_NEWWINDOW,
	0,
	ID_GO_BACK,
	ID_GO_FORWARD,
	ID_VIEW_STOP,
	ID_VIEW_REFRESH,
	ID_GO_START_PAGE,
	0,
	ID_VIEW_EXPLOREBAR_SEARCH,
	ID_FAVORITES_DROPDOWN,
	ID_VIEW_HIS,
	ID_VIEW_FOLDER,
	ID_VIEW_EXPLOREBAR_RESOURCE,
	0,
	ID_BMODE_DROPDOWN,
	ID_FILE_PRINT,
	ID_TOOLS_EMAIL,
	ID_FONT_DROPDOWN,
	ID_ENCODE_DROPDOWN,
	ID_OPTIONS_LOADIMAGE,
	ID_VIEW_FULLSCREEN,
	ID_WINDOW_TILE_VERT,
	ID_WINDOW_TILE_HORZ,
	ID_OPTIONS_USE_WEBP,
	ID_OPTIONS_USE_PROXY,
	ID_FILE_WORKOFFLINE,
	0,
	ID_FILE_OPENF,
	ID_EDIT_EDIT,
	ID_FILE_STOP_OPENALL,
	ID_TOOLS_AUTOSCROLL_USEAUTOSCROLLING,
	ID_FILE_SAVEAS,
	ID_EDIT_UNDOCLOSE,
	ID_EDIT_COPY,
	ID_OPTIONS_ITREESURFOPTIONS,
	ID_VIEW_STOP_ALL,
	ID_VIEW_REFRESHALL,
	ID_TOOLS_SETBG,
	ID_GROUP,
	ID_EDIT_FIND,
	ID_OPTIONS_NEWTAB,
	ID_FILE_PRINTPREVIEW,
	ID_TOOLS_ALL,
	ID_OPTIONS_SET_PROXY,
};

static UINT toolbarlabels[]=
{
	IDS_NEW_WINDOW,
	0,
	IDS_BACK,
	IDS_FORWARD,
	IDS_STOP,
	IDS_REFRESH,
	IDS_HOME,
	0,
	IDS_SEARCH,
	IDS_FAVORITES,
	IDS_TITLE_HIS,
	IDS_TITLE_SHELL,
	IDS_TITLE_RESOURCE,
	0,
	IDS_ROWSE_MODE,
	IDS_PRINT,
	ID_TOOLS_EMAIL,
	IDS_FONT,
	IDS_ENCODE,
	IDS_IMAGE,
	ID_VIEW_FULLSCREEN,
	IDS_VTILE,
	IDS_HTILE,
	IDS_WEB_PROXY,
	IDS_PROXY,
	ID_FILE_WORKOFFLINE,
	0,
	ID_FILE_OPENF,
	ID_EDIT_EDIT,
	ID_FILE_STOP_OPENALL,
	IDS_AUTO_SCROLL,
	IDS_SAVE_AS,
	IDS_UNDO_CLOSE,
	ID_EDIT_COPY,
	ID_OPTIONS_ITREESURFOPTIONS,
	ID_VIEW_STOP_ALL,
	ID_VIEW_REFRESHALL,
	ID_TOOLS_SETBG,
	ID_GROUP,
	ID_EDIT_FIND,
	ID_OPTIONS_NEWTAB,
	ID_FILE_PRINTPREVIEW,
	ID_TOOLS_ALL,
	ID_OPTIONS_SET_PROXY,
};

static UINT toolbarbuttonstyles[] = 
{
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBBS_SEPARATOR,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBBS_SEPARATOR,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBBS_SEPARATOR,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON,
	TBBS_SEPARATOR,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON,
	TBSTYLE_BUTTON | TBSTYLE_DROPDOWN,
	TBSTYLE_BUTTON,
};

static UINT toolbarbuttonimgs[] = 
{
	9,0,0,1,2,3,4,0,5,6,12,23,24,0,25,7,13,8,14,10,11,15,16,17,18,19,0,21,20,22,26,28,27,29,30,31,32,33,34,35,36,37,38,39,
};

static UINT toolbarselectivelabels[]=
{
	0,0,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

static char menu_order_key[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MenuOrder\\Favorites\\";
///}

////////////////////////////////////////////////////////////////////////////
// CGlobalSpace Contruction
CGlobalSpace::CGlobalSpace()
{
	_iPercent = -1;

	CString str="yes";
	_bsYes = str.AllocSysString();


	_iaIndicators = indicators;
	_iaToolbarButtons = toolbarbuttons;
	_iaToolbarLabels = toolbarlabels;
	_iaToolbarButtonStyles = toolbarbuttonstyles;
	_iaToolbarButtonImgs = toolbarbuttonimgs;
	_iaToolbarSelectiveLabels = toolbarselectivelabels;
	_lpstrMenuOrderKey = menu_order_key;

	_iIndicatorsCount = sizeof( indicators )/sizeof(UINT);
	_iToolbarButtonsCount = sizeof( toolbarbuttons )/sizeof(UINT);
	_iToolbarLabelsCount = sizeof( toolbarlabels )/sizeof(UINT);
	_iToolbarButtonStylesCount = sizeof( toolbarbuttonstyles )/sizeof(UINT);
	_iToolbarButtonImgsCount = sizeof( toolbarbuttonimgs )/sizeof(UINT);
	_iToolbarSelectiveLabelsCount = sizeof( toolbarselectivelabels )/sizeof(UINT);

}

CGlobalSpace::~CGlobalSpace()
{
	SysFreeString( _bsYes );
}

