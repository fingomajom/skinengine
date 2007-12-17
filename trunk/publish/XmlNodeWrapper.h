
#pragma once

#import "MSXML3.dll" named_guids 
//using namespace MSXML2;

#include "skinstr.h"

class CXmlNodeWrapper  
{
public:

    CXmlNodeWrapper();
    CXmlNodeWrapper(MSXML2::IXMLDOMNodePtr pNode, BOOL bAutoRelease = TRUE);

    void operator = (MSXML2::IXMLDOMNodePtr pNode);
    virtual ~CXmlNodeWrapper();

public:

    KSE::CString NodeType();

    KSE::CString GetAttribVal (int index);
    KSE::CString GetAttribName(int index);

    int  NumAttributes();
    void ReplaceNode(MSXML2::IXMLDOMNode* pOldNode, MSXML2::IXMLDOMNode* pNewNode);

    KSE::CString GetText();
    BOOL GetText(KSE::CString& strText);
    void SetText(LPCTSTR text);

    MSXML2::IXMLDOMNode* InsertAfter(MSXML2::IXMLDOMNode *refNode, MSXML2::IXMLDOMNode *pNode);
    
    KSE::CString Name();
    BOOL Name(KSE::CString& name);

    MSXML2::IXMLDOMNodeList* FindNodes(LPCTSTR searchStr);
    MSXML2::IXMLDOMNode* Parent();
    void RemoveNodes(LPCTSTR searchStr);

    MSXML2::IXMLDOMNode* InsertAfter(MSXML2::IXMLDOMNode* refNode, LPCTSTR nodeName);
    MSXML2::IXMLDOMNode* InsertBefore(MSXML2::IXMLDOMNode* refNode, LPCTSTR nodeName);
    MSXML2::IXMLDOMNode* AppendNode(LPCTSTR nodeName);

    MSXML2::IXMLDOMNode* Interface();
    MSXML2::IXMLDOMDocument* ParentDocument();
    KSE::CString GetXML();
    MSXML2::IXMLDOMNode* RemoveNode(MSXML2::IXMLDOMNodePtr pNode);
    MSXML2::IXMLDOMNode* InsertNode(int index, LPCTSTR nodeName);
    MSXML2::IXMLDOMNode* InsertNode(int index, MSXML2::IXMLDOMNodePtr pNode);


    MSXML2::IXMLDOMNodeList* GetChildNodes();

    long NumNodes();


    MSXML2::IXMLDOMNode* Detach();
    MSXML2::IXMLDOMNode* GetNode(LPCTSTR nodeName);
    MSXML2::IXMLDOMNode* GetNode(int nodeIndex);
    MSXML2::IXMLDOMNode* FindNode(LPCTSTR searchString);
    MSXML2::IXMLDOMNode* GetPrevSibling();
    MSXML2::IXMLDOMNode* GetNextSibling();

    void SetValue(LPCTSTR valueName, LPCTSTR value);
    void SetValue(LPCTSTR valueName, int     value);
    void SetValue(LPCTSTR valueName, short   value);
    void SetValue(LPCTSTR valueName, double  value);
    void SetValue(LPCTSTR valueName, float   value);
    void SetValue(LPCTSTR valueName, bool    value);

    BOOL IsValid();


    BOOL GetValue(LPCTSTR valueName, KSE::CString& value);
    BOOL GetValue(LPCTSTR valueName, int&    value);
    BOOL GetValue(LPCTSTR valueName, short&  value);
    BOOL GetValue(LPCTSTR valueName, double& value);
    BOOL GetValue(LPCTSTR valueName, float&  value);
    BOOL GetValue(LPCTSTR valueName, bool&   value);

private:
    BOOL m_bAutoRelease;
    MSXML2::IXMLDOMNodePtr m_xmlnode;
};

class CXmlDocumentWrapper
{
public:

    CXmlDocumentWrapper();
    CXmlDocumentWrapper(MSXML2::IXMLDOMDocumentPtr pDoc);
    MSXML2::IXMLDOMNode* AsNode();
    virtual ~CXmlDocumentWrapper();

public:

    KSE::CString GetUrl();
    KSE::CString GetXML();
    BOOL IsValid();
    BOOL Load(LPCTSTR path);
    BOOL LoadXML(LPCTSTR xml);
    BOOL Save(LPCTSTR path = _T(""));
    MSXML2::IXMLDOMDocument* Detach();
    MSXML2::IXMLDOMDocument* Clone();
private:
    MSXML2::IXMLDOMDocumentPtr m_xmldoc;
};

class CXmlNodeListWrapper
{
public:

    CXmlNodeListWrapper();
    CXmlNodeListWrapper(MSXML2::IXMLDOMNodeListPtr pList);

    void operator=(MSXML2::IXMLDOMNodeListPtr pList);
    virtual ~CXmlNodeListWrapper();

public:

    MSXML2::IXMLDOMDocument* AsDocument();
    MSXML2::IXMLDOMNode* Node(int index);
    void Start();
    MSXML2::IXMLDOMNode* Next();
    BOOL IsValid();
    int Count();

private:
    MSXML2::IXMLDOMNodeListPtr m_xmlnodelist;
};
