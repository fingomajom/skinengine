
#if !defined( _ITREESURF__GLOBAL_SPACE_H__INCLUDED_ )
#define _ITREESURF__GLOBAL_SPACE_H__INCLUDED_

#include "MainFrm.h"

////////////////////////////////////////////////////////////////////////////
// CGlobalSpace

class CGlobalSpace
{
public:
	CGlobalSpace();
	~CGlobalSpace();

	int _iPercent;
	int _iMax;
	int _nTrayIconID;

	bool _bVerbose;
	bool _bRTab;

	CMainFrame* _pMainFrame;
	CStringArray _csaLinks;
	CStringArray _csaFavors;
	char _lpFiles[1024];

	UINT* _iaIndicators;
	UINT* _iaToolbarButtons;
	UINT* _iaToolbarLabels;
	UINT* _iaToolbarButtonStyles;
	UINT* _iaToolbarButtonImgs;
	UINT* _iaToolbarSelectiveLabels;
	char* _lpstrMenuOrderKey;

	int _iIndicatorsCount;
	int _iToolbarButtonsCount;
	int _iToolbarLabelsCount;
	int _iToolbarButtonStylesCount;
	int _iToolbarButtonImgsCount;
	int _iToolbarSelectiveLabelsCount;

	CMapStringToPtr _mapUrlFilter;
	CStringArray _csaUrlPattern;

	BSTR _bsYes;
};



#endif // !defined( _ITREESURF__GLOBAL_SPACE_H__INCLUDED_ )
