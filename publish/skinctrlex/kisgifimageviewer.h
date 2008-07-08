/********************************************************************
* CreatedOn: 2006-10-27   20:36
* FileName: kisGIFImageViewer.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include <atlframe.h>
#include <vector>


class kisGIFImageViewer : 
    public CWindowImpl<kisGIFImageViewer, WTL::CStatic>
{
    typedef struct {
        CDC *pmemdc;
        CBitmap *pbmp;
        HBITMAP holdbmp;
    }onepage;
public:
    kisGIFImageViewer(void);
    ~kisGIFImageViewer(void);
    
    int InitGifPages(UINT uResID, LPCTSTR pszResType);
    int InitGifPages( LPCTSTR pszFile );

    void Pause(bool bPause = true);
    
    BEGIN_MSG_MAP(kisGIFImageViewer)

        MESSAGE_HANDLER( WM_PAINT  , OnPaint)
        MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBackground)

        MESSAGE_HANDLER( WM_TIMER  , OnTimer   )
        MESSAGE_HANDLER( WM_DESTROY, OnDestroy )

    END_MSG_MAP()
    
    LRESULT OnTimer  (UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    void DoPaint(CDCHandle dc);


protected:
    unsigned long Find0x20(unsigned char* pBuffer, 
        unsigned long usize, 
        unsigned long nbeginpos = 0);

    void InitPage(onepage& page, int cx, int cy, HDC hdc = NULL);
    void UnInitPage(onepage& page);
    
    void DestroyAllPage();

    int LoadGifPage(unsigned char* pBuffer, 
        unsigned long usize,
        unsigned long ucopytopos,
        unsigned long ucopypos,
        unsigned long ucopylen);

    void AutoSize();
private:
    std::vector<onepage> m_vtpages;
    int  m_npaintindex;

    SIZE m_imagesize;

    bool m_bpause;
};
