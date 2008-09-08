/********************************************************************
* CreatedOn: 2008-4-15   16:41
* FileName: kis_singleton.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once


template< typename T >
class KISSingleton
{
public:

    static T* getInstance()
    {
        if (ms_Instance.m_p == NULL)
        {
            ms_Instance.Attach(new T());
        }

        ATLASSERT( ms_Instance.m_p != NULL );

        return ms_Instance.m_p;
    }

    static T& Instance()
    {
        return *(getInstance());
    }

    operator T*() const throw()
    {
        return( ms_Instance );
    }

    T* operator->() const throw()
    {
        ATLASSERT( ms_Instance.m_p != NULL );
        return( ms_Instance );
    }

private:

    KISSingleton()
    { }

    static ATL::CAutoPtr<T> ms_Instance;

};

template< typename T >
ATL::CAutoPtr<T> KISSingleton<T>::ms_Instance;