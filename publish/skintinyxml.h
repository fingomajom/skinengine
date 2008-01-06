
#pragma once

#include <tinyxml.h>
#include <atlconv.h>
#include <skinstr.h>

#ifdef _DEBUG
    #pragma comment(lib, "tinyxmld.lib")
#else
    #pragma comment(lib, "tinyxml.lib")
#endif

namespace KSG{

//typedef TiXmlNode           SkinXmlNode;
//typedef TiXmlDocument       SkinXmlDocument;
//typedef TiXmlElement        SkinXmlElement;
//typedef TiXmlComment        SkinXmlComment;
//typedef TiXmlUnknown        SkinXmlUnknown;
//typedef TiXmlAttribute      SkinXmlAttribute;
//typedef TiXmlText           SkinXmlText;
//typedef TiXmlDeclaration    SkinXmlDeclaration;
//typedef TiXmlParsingData    SkinXmlParsingData;

#define CP_DEFAULT  CP_UTF8

class SkinXmlElement;
class SkinXmlDocument;

class SkinXmlElement
{
public:

    SkinXmlElement()
    {
        m_bowner = false;
        m_tixmlelement = 0;
    }

    SkinXmlElement (LPCTSTR in_value)
    {
        CT2AEX<> Value(in_value, CP_DEFAULT);

        m_bowner = true;
        m_tixmlelement = new TiXmlElement(Value.m_psz);
    }

    SkinXmlElement( const SkinXmlElement& ref )
    {
        m_tixmlelement = 0;

        *this = ref;
    }

    void operator = ( const SkinXmlElement& ref )
    {
        ClearThis();

        m_bowner = ref.m_bowner;
        m_tixmlelement = ref.m_tixmlelement;
        
        if (m_bowner && m_tixmlelement)
            m_tixmlelement = new TiXmlElement(*ref.m_tixmlelement);        
    }

    virtual ~SkinXmlElement()
    {
        ClearThis();
    }

    void CopyFrom( const SkinXmlElement& ref )
    {
        ClearThis();

        m_bowner = true;
        m_tixmlelement = 0;

        if ( ref.m_tixmlelement )
            m_tixmlelement = new TiXmlElement(*ref.m_tixmlelement);        
    }

public:

    KSG::CString Name()
    {
        if (m_tixmlelement == 0)
            return KSG::CString();

        CA2TEX<> Value(m_tixmlelement->Value(), CP_DEFAULT);

        return KSG::CString(Value.m_psz);
    }


    BOOL Name(KSG::CString& name)
    {
        if (m_tixmlelement == 0)
            return FALSE;

        CA2TEX<> Value(m_tixmlelement->Value(), CP_DEFAULT);

        name = Value.m_psz;

        return TRUE;
    }

    BOOL SetName(const KSG::CString& name)
    {
        if (m_tixmlelement == 0)
            return FALSE;

        CT2AEX<> Value(name, CP_DEFAULT);
        
        m_tixmlelement->SetValue(Value.m_psz);

        return TRUE;
    }


    void SetValue(LPCTSTR valueName, LPCTSTR value)
    {
        if (m_tixmlelement == 0)    
            return;

        CT2AEX<> Key(valueName, CP_DEFAULT);
        CT2AEX<> Value(value, CP_DEFAULT);

        m_tixmlelement->SetAttribute(Key.m_psz, Value.m_psz);
    }

    void SetValue(LPCTSTR valueName, int value)
    {
        KSG::CString str;
        str.Format(_T("%ld"),value);
        SetValue(valueName, str);
    }

    void SetValue(LPCTSTR valueName, float   value)
    {
        KSG::CString str;
        str.Format(_T("%f"),value);
        SetValue(valueName,str);
    }

    BOOL GetValue(LPCTSTR valueName, KSG::CString& value)
    {
        if (m_tixmlelement == 0)    
            return FALSE;

        CT2AEX<> Key(valueName, CP_DEFAULT);

        const char * pszValue = m_tixmlelement->Attribute(Key.m_psz);
        if (pszValue == 0)
            return FALSE;
        
        CA2TEX<> RetValue(pszValue , CP_DEFAULT);
            
        value = RetValue.m_psz;

        return TRUE;
    }

    BOOL GetValue(LPCTSTR valueName, int& value)
    {
        KSG::CString strvalue;

        if (!GetValue(valueName, strvalue))
            return FALSE;

        value = _ttoi(strvalue);

        return TRUE;
    }

    BOOL GetValue(LPCTSTR valueName, float&  value)
    {
        KSG::CString strvalue;

        if (!GetValue(valueName, strvalue))
            return FALSE;

        value = (float)_tstof(strvalue);

        return TRUE;
    }


    BOOL IsValid()
    {
        return m_tixmlelement != 0;
    }


public:

    SkinXmlElement FirstChildElement()	
    { 
        if (m_tixmlelement == 0)    
            return SkinXmlElement();

        return  SkinXmlElement(m_tixmlelement->FirstChildElement());
    }

    SkinXmlElement FirstChildElement( LPCTSTR pszValue)	
    { 
        if (m_tixmlelement == 0)    
            return SkinXmlElement();

        CT2AEX<> Value(pszValue, CP_DEFAULT);

        return  SkinXmlElement(m_tixmlelement->FirstChildElement(Value.m_psz));
    }

    SkinXmlElement NextSiblingElement()	
    { 
        if (m_tixmlelement == 0)    
            return SkinXmlElement();

        return  SkinXmlElement(m_tixmlelement->NextSiblingElement());
    }

    SkinXmlElement NextSiblingElement( LPCTSTR pszValue)	
    { 
        if (m_tixmlelement == 0)    
            return SkinXmlElement();

        CT2AEX<> Value(pszValue, CP_DEFAULT);

        return  SkinXmlElement(m_tixmlelement->NextSiblingElement(Value.m_psz));
    }


    bool RemoveElement( SkinXmlElement& remove )
    {
        if (m_tixmlelement == 0)    
            return false;
            
        return m_tixmlelement->RemoveChild(remove.m_tixmlelement);
    }
    
    SkinXmlElement AppendElement( SkinXmlElement& Element )
    {
        if (m_tixmlelement == 0 || !Element.IsValid())
            return SkinXmlElement();
        
        return SkinXmlElement(m_tixmlelement->InsertEndChild(
            *Element.m_tixmlelement)->ToElement());
    }
    
    SkinXmlElement AppendElement( LPCTSTR pszValue )
    {
        if (m_tixmlelement == 0 || pszValue == NULL)
            return SkinXmlElement();
        
        SkinXmlElement Ele(pszValue);

        return AppendElement(Ele);
    }


protected:

    void ClearThis()
    {
        if (m_bowner && m_tixmlelement != 0)
            delete m_tixmlelement;

        m_tixmlelement = 0;
    }

    SkinXmlElement( TiXmlElement* ref )
    {
        m_tixmlelement = ref;
        m_bowner = false;
    }

    void operator = ( TiXmlElement* ref )
    {
        ClearThis();

        m_bowner = false;

        m_tixmlelement = ref;
    }

protected:

    TiXmlElement* m_tixmlelement;
    bool m_bowner;

    friend class SkinXmlDocument;

};


const TiXmlEncoding SKINXML_DEFAULT_ENCODIN = TIXML_ENCODING_UTF8;

class SkinXmlDocument
{
public:
    SkinXmlDocument()
    {
    }

    bool LoadFile( LPCTSTR filename )
    {
        CT2AEX<> Value(filename, CP_DEFAULT);

        return m_tixmldoc.LoadFile(Value.m_psz, SKINXML_DEFAULT_ENCODIN);
    }

    bool SaveFile( LPCTSTR filename )
    {
        CT2AEX<> Value(filename, CP_DEFAULT);

        return m_tixmldoc.SaveFile(Value.m_psz);
    }

    bool LoadXML( LPCTSTR xml )
    {
        CT2AEX<> Value(xml, CP_DEFAULT);

        return m_tixmldoc.Parse(Value.m_psz, 0, SKINXML_DEFAULT_ENCODIN) != 0;
    }

public:

    //const SkinXmlElement RootElement() const		
    //{ return SkinXmlElement(m_tixmldoc.FirstChildElement()); }
    SkinXmlElement RootElement()					
    { return SkinXmlElement( m_tixmldoc.FirstChildElement()); }


protected:
    TiXmlDocument m_tixmldoc;
};



};

