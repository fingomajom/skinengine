// AnimateIcon.h : interface of the CAnimateIcon class
// written by Rajesh Parikh (MCSD)
// email : rparikh@usa.net
// Not protected by any copyright, use it freely
//
////////////////////////////////////////////////////////////////////////////

#if !defined( _ITREESURF__ANIMATEICON_H__INCLUDED_ )
#define _ITREESURF__ANIMATEICON_H__INCLUDED_

class CAnimateIcon
{
protected:
	// variable to hold the image list containing a series
	// of icons
	CImageList  m_imgList;
	// keep track of which image is current
	int m_iImageCounter;
	// store the max nos of images
	int m_iMaxNoOfImages;
	HICON hIcon;
	HICON hPrevIcon ;
		
public:
	BOOL ShowFirstImage();
	CAnimateIcon();
	~CAnimateIcon();
	BOOL SetImageList(int IDOfImgListResource,int numberOfImages,COLORREF transparentColor);
	BOOL ShowNextImage();
private:
	HICON m_hAppIcon;
};

#endif //#define _ITREESURF__ANIMATEICON_H__INCLUDED_
