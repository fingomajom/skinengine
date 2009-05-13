
#pragma once


template<class T>
class CDWSingleton
{
public:
    
    static T* InstancePtr()
    {
        if ( m_p == NULL )
            m_p = new T;

        ATLASSERT(m_p != NULL);

        return m_p;
    }
    
    static T& Instance()
    {
        return *InstancePtr();
    }

    static void DeleteInstance()
    {
        if ( m_p )
            delete m_p;
        m_p = NULL;
    }

private:
    static T* m_p;
};


template<class T>
class CDWSingleton2
{
public:

    static T* InstancePtr()
    {
        return &Instance(); 
    }

    static T& Instance()
    {
        static T gs_instance;
        return gs_instance;
    }
};

template<class T> __declspec(selectany) T* CDWSingleton<T>::m_p;
