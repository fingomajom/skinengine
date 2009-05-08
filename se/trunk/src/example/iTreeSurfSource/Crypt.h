// Crypt.h: interface for the CCrypt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined( _ITREESURF__CRYPT_H__INCLUDED_ )
#define _ITREESURF__CRYPT_H__INCLUDED_

#if _MSC_VER > 1000         
#pragma once                    
#endif // _MSC_VER > 1000                  


void Encoding(LPCSTR src, CString& dest);
void Decoding(LPCSTR src, CString& dest);
/*
class CCrypt  
{
public:
	CCrypt();
	virtual ~CCrypt();

};
*/
#endif // !defined( _ITREESURF__CRYPT_H__INCLUDED_ )
