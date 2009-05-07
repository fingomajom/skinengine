/********************************************************************
	created:	2009/05/04
	created:	4:5:2009   12:10
	filename: 	d:\lidengwang\se\trunk\src\dw_explorer\CookieMgt.h
	file path:	d:\lidengwang\se\trunk\src\dw_explorer
	file base:	CookieMgt
	file ext:	h
	author:		lidengwang@s-zone.com
	
	purpose:	
*********************************************************************/
#pragma once

class CCookieMgt
{
    CCookieMgt( const CCookieMgt& );

    CCookieMgt(void);
public:
    ~CCookieMgt(void);

    static CCookieMgt& Instance();


};
