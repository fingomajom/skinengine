#include "StdAfx.h"
#include "kisGIFImageViewer.h"
#include <math.h>
#include <atlfile.h>


#define IDT_TIMER  1001
#define IDT_DELAY  150


typedef struct {
    unsigned short logx;
    unsigned short logy;
    unsigned short width;
    unsigned short height;
    struct {
        unsigned char d:3;
        unsigned char c:1;
        unsigned char b:3;
        unsigned char a:1;
    }flag;
}gifimageinfo;

kisGIFImageViewer::kisGIFImageViewer(void) :
    m_npaintindex(0),
    m_bpause(false)
{
    m_imagesize.cx = -1;
    m_imagesize.cy = -1;
}

kisGIFImageViewer::~kisGIFImageViewer(void)
{
    DestroyAllPage();
}

void kisGIFImageViewer::Pause(bool bPause /* = true */)
{
    m_bpause = bPause;
}

void kisGIFImageViewer::DoPaint(CDCHandle dc)
{
    HDC hdc = NULL;

    size_t upages = m_vtpages.size();
    if (upages <= 0)
        return;
    
    const onepage& page = m_vtpages[m_npaintindex];

    ATLASSERT(page.pbmp != NULL);
    ATLASSERT(page.pmemdc != NULL);

    BITMAP bmpinfo = {0};

    page.pbmp->GetBitmap(&bmpinfo);

    dc.BitBlt(0, 0, 
        bmpinfo.bmWidth, bmpinfo.bmHeight,
        page.pmemdc->m_hDC,
        0 ,0, SRCCOPY);
}

LRESULT kisGIFImageViewer::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    if (wParam == IDT_TIMER && !m_bpause &&m_vtpages.size() > 0)
    {           
        if (++m_npaintindex >= (int)m_vtpages.size())
            m_npaintindex = 0;

        if (CWindow::IsWindowVisible())
            Invalidate();

        return 0L;
    }

    return DefWindowProc(uMsg, wParam, lParam);
}

LRESULT kisGIFImageViewer::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    KillTimer(IDT_TIMER);

    DestroyAllPage();

    return DefWindowProc(uMsg, wParam, lParam);
}

int kisGIFImageViewer::InitGifPages(UINT uResID, LPCTSTR pszResType)
{
    HINSTANCE hResInstance = _Module.GetResourceInstance();
    if (hResInstance == NULL)
        return -1;

    HRSRC hRes= ::FindResource(hResInstance,
        MAKEINTRESOURCE(uResID),
        pszResType);
    if (hRes == NULL)
        return -1;

    BYTE* pResBuffer = (BYTE*)LoadResource(hResInstance, hRes);
    if (pResBuffer == NULL)
        return -1;

    unsigned long uResLen = ::SizeofResource(hResInstance, hRes);
    if (uResLen == 0)
        return -1;

    unsigned long nbeginpos = 0;
    unsigned long nfirstpos = 0;
    

    while (true)
    {
        unsigned long nfindpos = Find0x20(pResBuffer, uResLen, nbeginpos);
        if (nfindpos == 0)
            break;

        if (nfirstpos == 0)
            nfirstpos = nfindpos;

        
        gifimageinfo* pgifinfo = (gifimageinfo*)&pResBuffer[nfindpos + 1];
        
        if( pgifinfo->flag.a == 0 ) // //a为0时指图象不存在局部调色板
        {
            unsigned long uimagelen = 1 + sizeof(gifimageinfo);

            while(pResBuffer[ nfindpos + uimagelen ] != 0 )
            {
                uimagelen = uimagelen + 
                    (unsigned long)pResBuffer[nfindpos + uimagelen] + 1;
            }                        //算得图象大小

            uimagelen++;                //把最后一个0x00加上

            LoadGifPage(pResBuffer, 
                uResLen,
                nfirstpos,
                nfindpos,
                uimagelen);

            nbeginpos = nfindpos + uimagelen - 1;            //跳过图象

        }
        else
        {
            unsigned long uimagelen = 1 + sizeof(gifimageinfo) +
                3 * (int)floor(pow((float)2, pgifinfo->flag.d));

            while(pResBuffer[ nfindpos + uimagelen ] != 0 )
            {
                uimagelen = uimagelen + 
                    (unsigned long)pResBuffer[nfindpos + uimagelen] + 1;
            }                        //算得图象大小

            uimagelen++;                //把最后一个0x00加上

            LoadGifPage(pResBuffer, 
                uResLen,
                nfirstpos,
                nfindpos,
                uimagelen);

            nbeginpos = nfindpos + uimagelen - 1;            //跳过图象

        }

    }

    AutoSize();

    SetTimer(IDT_TIMER, IDT_DELAY, NULL);

    return 0;
}

int kisGIFImageViewer::InitGifPages( LPCTSTR pszFile )
{
    BYTE*     pResBuffer = NULL;
    ULONGLONG uResLen    = 0 ;

    CAtlFile file;

    HRESULT hr = file.Create(pszFile, 
        GENERIC_READ, 
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        OPEN_EXISTING);
    if (hr != S_OK)
        return 0;

    hr = file.GetSize( uResLen );
    if (hr != S_OK)
        return 0;

    pResBuffer = new BYTE[(size_t)uResLen];
    if (pResBuffer == NULL)
        return 0;

    hr = file.Read( pResBuffer, (DWORD)uResLen );
    if (pResBuffer == NULL)
    {
        delete []pResBuffer;

        return 0;
    }


    unsigned long nbeginpos = 0;
    unsigned long nfirstpos = 0;


    while (true)
    {
        unsigned long nfindpos = Find0x20(pResBuffer, (ULONG)uResLen, nbeginpos);
        if (nfindpos == 0)
            break;

        if (nfirstpos == 0)
            nfirstpos = nfindpos;


        gifimageinfo* pgifinfo = (gifimageinfo*)&pResBuffer[nfindpos + 1];

        if( pgifinfo->flag.a == 0 ) // //a为0时指图象不存在局部调色板
        {
            unsigned long uimagelen = 1 + sizeof(gifimageinfo);

            while(pResBuffer[ nfindpos + uimagelen ] != 0 )
            {
                uimagelen = uimagelen + 
                    (unsigned long)pResBuffer[nfindpos + uimagelen] + 1;
            }                        //算得图象大小

            uimagelen++;                //把最后一个0x00加上

            LoadGifPage(pResBuffer, 
                (ULONG)uResLen,
                nfirstpos,
                nfindpos,
                uimagelen);

            nbeginpos = nfindpos + uimagelen - 1;            //跳过图象

        }
        else
        {
            unsigned long uimagelen = 1 + sizeof(gifimageinfo) +
                3 * (int)floor(pow((float)2, pgifinfo->flag.d));

            while(pResBuffer[ nfindpos + uimagelen ] != 0 )
            {
                uimagelen = uimagelen + 
                    (unsigned long)pResBuffer[nfindpos + uimagelen] + 1;
            }                        //算得图象大小

            uimagelen++;                //把最后一个0x00加上

            LoadGifPage(pResBuffer, 
                (ULONG)uResLen,
                nfirstpos,
                nfindpos,
                uimagelen);

            nbeginpos = nfindpos + uimagelen - 1;            //跳过图象

        }

    }

    delete []pResBuffer;

    AutoSize();

    SetTimer(IDT_TIMER, IDT_DELAY, NULL);

    return 0;
}

int kisGIFImageViewer::LoadGifPage(unsigned char* pBuffer, 
    unsigned long usize,
    unsigned long ucopytopos,
    unsigned long ucopypos,
    unsigned long ucopylen)
{
    HGLOBAL hglobal = NULL;
    

    hglobal = GlobalAlloc(0, ucopytopos + ucopylen);
    if (hglobal == NULL)
        return -1;

    memcpy((void*)hglobal, pBuffer, ucopytopos);

    if ( ucopylen > 0 )
    {
        memcpy((unsigned char*)hglobal + ucopytopos, pBuffer + ucopypos, ucopylen);
    }
   
    CComPtr<IStream>    piStream;
    CComQIPtr<IPicture> piPicture;

    HRESULT hr = CreateStreamOnHGlobal(hglobal, 
        TRUE, &piStream);

    if (FAILED(hr))
        return -1;      

    hr = ::OleLoadPicture(piStream, 0, false,
        IID_IPicture,(void**)&piPicture);

    if (FAILED(hr))
        return -1;
    
    gifimageinfo* pgifinfo = (gifimageinfo*)&((unsigned char*)hglobal)[ucopytopos + 1];

    onepage page = { 0 };
    
    long cx = 0;
    long cy = 0;

    CDCHandle dc = GetDC();

    piPicture->get_Width (&cx);
    piPicture->get_Height(&cy);
    
    SIZE size = {cx, cy};
    dc.HIMETRICtoDP(&size);

    if (m_imagesize.cx <= 0)
        m_imagesize = size;

    InitPage(page, m_imagesize.cx, m_imagesize.cy, dc);

    RECT rect = {0};

    size_t upagecount = m_vtpages.size();

    if (upagecount > 0)
    {
        page.pmemdc->BitBlt(0, 0, 
            m_imagesize.cx, m_imagesize.cy,
            m_vtpages[upagecount - 1].pmemdc->m_hDC,
            0, 0, SRCCOPY);
    }

    piPicture->Render(page.pmemdc->m_hDC,
        pgifinfo->logx, pgifinfo->logy,
        size.cx,size.cy,
        0 , +cy, 
        cx, -cy,
        &rect);
    
    ReleaseDC(dc);

    m_vtpages.push_back(page);

    return 0;
}

unsigned long kisGIFImageViewer::Find0x20(unsigned char* pBuffer,
    unsigned long usize, 
    unsigned long nbeginpos /* = 0 */)
{
    while (nbeginpos < usize)
    {
        if (pBuffer[nbeginpos] == 0 &&
            pBuffer[nbeginpos + 1] == 0x2C)
            return nbeginpos + 1;

        nbeginpos++;
    }

    return 0;
}

void kisGIFImageViewer::InitPage(onepage& page, int cx, int cy, HDC hdc /* = NULL */)
{
    UnInitPage(page);

    page.pmemdc = new CDC();
    page.pbmp   = new CBitmap();

    if (page.pmemdc == NULL || page.pbmp == NULL)
    {
        UnInitPage(page);
        return;
    }
    
    page.pmemdc->CreateCompatibleDC(hdc);
    page.pbmp->CreateCompatibleBitmap(hdc, cx, cy);

    if (page.pbmp->m_hBitmap == NULL || page.pmemdc->m_hDC == NULL)
    {
        UnInitPage(page);
        return;
    }

    page.holdbmp = page.pmemdc->SelectBitmap(page.pbmp->m_hBitmap);

}

void kisGIFImageViewer::UnInitPage(onepage& page)
{
    if (page.pmemdc != NULL && page.pbmp != NULL)
        page.pmemdc->SelectBitmap(page.holdbmp);

    if (page.pmemdc != NULL)
        delete page.pmemdc;
    if (page.pbmp)
        delete page.pbmp;

    page.holdbmp = NULL;
    page.pbmp    = NULL;
    page.pmemdc  = NULL;
}

void kisGIFImageViewer::DestroyAllPage()
{
    for (size_t idx = 0; idx < m_vtpages.size(); idx++)
    {
        UnInitPage(m_vtpages[idx]);
    }

    m_vtpages.clear();
}


void kisGIFImageViewer::AutoSize()
{
    RECT moveto = {0};

    GetWindowRect(&moveto);
        
    POINT pt = {moveto.left, moveto.top};
    ::ScreenToClient(GetParent(), &pt);

    moveto.left = pt.x;
    moveto.top  = pt.y;

    moveto.right  = moveto.left + m_imagesize.cx ;
    moveto.bottom = moveto.top  + m_imagesize.cy ;

    MoveWindow(&moveto);
}


LRESULT kisGIFImageViewer::OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 1;   // no background painting needed
}

LRESULT kisGIFImageViewer::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    if(wParam != NULL)
    {
        DoPaint((HDC)wParam);
    }
    else
    {
        CPaintDC dc(m_hWnd);
        DoPaint(dc.m_hDC);
    }

    return 0;
};
