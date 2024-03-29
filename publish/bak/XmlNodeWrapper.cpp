
#include "XmlNodeWrapper.h"


CXmlNodeWrapper::CXmlNodeWrapper()
{
}

CXmlNodeWrapper::CXmlNodeWrapper(MSXML2::IXMLDOMNodePtr pNode,BOOL bAutoRelease)
{
    m_xmlnode = pNode;
    m_bAutoRelease = bAutoRelease;
}

void CXmlNodeWrapper::operator=(MSXML2::IXMLDOMNodePtr pNode)
{
    if (IsValid())
        m_xmlnode.Release();

    m_xmlnode = pNode;
}

CXmlNodeWrapper::~CXmlNodeWrapper()
{
    if (!m_bAutoRelease)
        m_xmlnode.Detach();
}


BOOL CXmlNodeWrapper::GetValue(LPCTSTR valueName, KSE::CString& value)
{
    BOOL bResult = FALSE;

    if (!IsValid())
        return bResult;

    MSXML2::IXMLDOMNodePtr attribute = m_xmlnode->Getattributes()->getNamedItem(valueName);
    if (attribute)
    {
        value   = (LPCSTR)attribute->Gettext();

        bResult = TRUE;
    }

    return bResult;
}


BOOL CXmlNodeWrapper::GetValue(LPCTSTR valueName, int& value)
{
    KSE::CString strvalue;

    if (!GetValue(valueName, strvalue))
        return FALSE;
    
    value = _ttoi(strvalue);

    return TRUE;
}


BOOL CXmlNodeWrapper::GetValue(LPCTSTR valueName, short&  value)
{
    KSE::CString strvalue;

    if (!GetValue(valueName, strvalue))
        return FALSE;

    value = _ttoi(strvalue);

    return TRUE;
}

BOOL CXmlNodeWrapper::GetValue(LPCTSTR valueName, double& value)
{
    KSE::CString strvalue;

    if (!GetValue(valueName, strvalue))
        return FALSE;

    value = _tstof(strvalue);

    return TRUE;
}

BOOL CXmlNodeWrapper::GetValue(LPCTSTR valueName, float& value)
{
    KSE::CString strvalue;

    if (!GetValue(valueName, strvalue))
        return FALSE;

    value = (float)_tstof(strvalue);

    return TRUE;
}

BOOL CXmlNodeWrapper::GetValue(LPCTSTR valueName, bool& value)
{
    KSE::CString strvalue;

    if (!GetValue(valueName, strvalue))
        return FALSE;

    value = ( strvalue.CompareNoCase(_T("True")) == 0) ;

    return TRUE;
}

BOOL CXmlNodeWrapper::IsValid()
{
    if (m_xmlnode == NULL)
        return FALSE;
    if (m_xmlnode.GetInterfacePtr() == NULL)
        return FALSE;
    return TRUE;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetPrevSibling()
{
    if (!IsValid())
        return NULL;
    return m_xmlnode->GetpreviousSibling().Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetNextSibling()
{
    if (!IsValid())
        return NULL;
    return m_xmlnode->GetnextSibling().Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetNode(LPCTSTR nodeName)
{
    if (!IsValid())
        return NULL;
    try{
        return m_xmlnode->selectSingleNode(nodeName).Detach();
    }
    catch (_com_error e)
    {
        KSE::CString err = e.ErrorMessage();
    }
    return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetNode(int nodeIndex)
{
    if (!IsValid())
        return NULL;
    return m_xmlnode->GetchildNodes()->Getitem(nodeIndex).Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::FindNode(LPCTSTR searchString)
{
    if (!IsValid())
        return NULL;
    try{
        return m_xmlnode->selectSingleNode(searchString).Detach();
    }
    catch (_com_error e)
    {
        KSE::CString err = e.ErrorMessage();
    }
    return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::Detach()
{
    if (IsValid())
    {
        return m_xmlnode.Detach();
    }
    else
        return NULL;
}

long CXmlNodeWrapper::NumNodes()
{
    if (IsValid())
    {
        return m_xmlnode->GetchildNodes()->Getlength();
    }
    else
        return 0;
}

MSXML2::IXMLDOMNodeList* CXmlNodeWrapper::GetChildNodes()
{
    if (IsValid())
    {
        return m_xmlnode->GetchildNodes().Detach();
    }
    else
        return NULL;
}


void CXmlNodeWrapper::SetValue(LPCTSTR valueName,LPCTSTR value)
{
    MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
    if (xmlDocument)
    {
        MSXML2::IXMLDOMNamedNodeMapPtr attributes = m_xmlnode->Getattributes();
        if (attributes)
        {
            MSXML2::IXMLDOMAttributePtr attribute = xmlDocument->createAttribute(valueName);
            if (attribute)
            {
                attribute->Puttext(value);
                attributes->setNamedItem(attribute);
            }
        }
    }
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName,int value)
{
    KSE::CString str;
    str.Format(_T("%ld"),value);
    SetValue(valueName,str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName,short value)
{
    KSE::CString str;
    str.Format(_T("%hd"),value);
    SetValue(valueName,str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName,double value)
{
    KSE::CString str;
    str.Format(_T("%f"),value);
    SetValue(valueName,str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName,float value)
{
    KSE::CString str;
    str.Format(_T("%f"),value);
    SetValue(valueName,str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName,bool value)
{
    KSE::CString str;
    if (value)
        str = _T("True");
    else
        str = _T("False");

    SetValue(valueName,str);
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertNode(int index,LPCTSTR nodeName)
{
    MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
    if (xmlDocument)
    {
        MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT),nodeName,"");
        MSXML2::IXMLDOMNode* refNode = GetNode(index);
        if (refNode)
            newNode = m_xmlnode->insertBefore(newNode.Detach(),refNode);
        else
            newNode = m_xmlnode->appendChild(newNode.Detach());
        return newNode.Detach();
    }
    return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertNode(int index,MSXML2::IXMLDOMNodePtr pNode)
{
    MSXML2::IXMLDOMNodePtr newNode = pNode->cloneNode(VARIANT_TRUE);
    if (newNode)
    {
        MSXML2::IXMLDOMNode* refNode = GetNode(index);
        if (refNode)
            newNode = m_xmlnode->insertBefore(newNode.Detach(),refNode);
        else
            newNode = m_xmlnode->appendChild(newNode.Detach());
        return newNode.Detach();
    }
    else
        return NULL;
}

KSE::CString CXmlNodeWrapper::GetXML()
{
    if (IsValid())
        return (LPCSTR)m_xmlnode->Getxml();
    else
        return _T("");
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::RemoveNode(MSXML2::IXMLDOMNodePtr pNode)
{
    if (!IsValid())
        return NULL;
    return m_xmlnode->removeChild(pNode).Detach();
}

/* ********************************************************************************************************* */
CXmlDocumentWrapper::CXmlDocumentWrapper()
{
    m_xmldoc.CreateInstance(MSXML2::CLSID_DOMDocument);
}

CXmlDocumentWrapper::CXmlDocumentWrapper(MSXML2::IXMLDOMDocumentPtr pDoc)
{
    m_xmldoc = pDoc;
}

CXmlDocumentWrapper::~CXmlDocumentWrapper()
{
}

BOOL CXmlDocumentWrapper::IsValid()
{
    if (m_xmldoc == NULL)
        return FALSE;
    if (m_xmldoc.GetInterfacePtr() == NULL)
        return FALSE;
    return TRUE;
}

MSXML2::IXMLDOMDocument* CXmlDocumentWrapper::Detach()
{
    if (!IsValid())
        return NULL;
    return m_xmldoc.Detach();
}

MSXML2::IXMLDOMDocument* CXmlDocumentWrapper::Clone()
{
    if (!IsValid())
        return NULL;
    MSXML2::IXMLDOMDocumentPtr xmldoc;
    xmldoc.CreateInstance(MSXML2::CLSID_DOMDocument);
    _variant_t v(xmldoc.GetInterfacePtr());
    m_xmldoc->save(v);
    return xmldoc.Detach();
}

BOOL CXmlDocumentWrapper::Load(LPCTSTR path)
{
    if (!IsValid())
        return FALSE;

    _variant_t v(path);
    m_xmldoc->put_async(VARIANT_FALSE);
    VARIANT_BOOL success = m_xmldoc->load(v);
    if (success == VARIANT_TRUE)
        return TRUE;
    else
        return FALSE;
}

BOOL CXmlDocumentWrapper::LoadXML(LPCTSTR xml)
{
    if (!IsValid())
        return FALSE;
    VARIANT_BOOL success = m_xmldoc->loadXML(xml);
    if (success == VARIANT_TRUE)
        return TRUE;
    else
        return FALSE;
}

BOOL CXmlDocumentWrapper::Save(LPCTSTR path)
{
    try
    {
        if (!IsValid())
            return FALSE;
        KSE::CString szPath(path);
        if (szPath == "")
        {
            _bstr_t curPath = m_xmldoc->Geturl();
            szPath = (LPSTR)curPath;
        }
        _variant_t v(szPath);
        if (FAILED(m_xmldoc->save(v)))
            return FALSE;
        else
            return TRUE;
    }
    catch(...)
    {
        return FALSE;
    }
}

MSXML2::IXMLDOMNode* CXmlDocumentWrapper::AsNode()
{
    if (!IsValid())
        return NULL;
    return m_xmldoc->GetdocumentElement().Detach();
}

KSE::CString CXmlDocumentWrapper::GetXML()
{
    if (IsValid())
        return (LPCSTR)m_xmldoc->Getxml();
    else
        return "";
}

KSE::CString CXmlDocumentWrapper::GetUrl()
{
    return (LPSTR)m_xmldoc->Geturl();
}

MSXML2::IXMLDOMDocument* CXmlNodeWrapper::ParentDocument()
{
    return m_xmlnode->GetownerDocument().Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::Interface()
{
    if (IsValid())
        return m_xmlnode;
    return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertBefore(MSXML2::IXMLDOMNode *refNode, LPCTSTR nodeName)
{
    MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
    if (xmlDocument)
    {
        MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT),nodeName,"");
        newNode = m_xmlnode->insertBefore(newNode.Detach(),(IUnknown*)refNode);
        return newNode.Detach();
    }
    return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertAfter(MSXML2::IXMLDOMNode *refNode, LPCTSTR nodeName)
{
    MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
    if (xmlDocument)
    {
        MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT),nodeName,"");
        MSXML2::IXMLDOMNodePtr nextNode = refNode->GetnextSibling();
        if (nextNode.GetInterfacePtr() != NULL)
            newNode = m_xmlnode->insertBefore(newNode.Detach(),(IUnknown*)nextNode);
        else
            newNode = m_xmlnode->appendChild(newNode.Detach());
        return newNode.Detach();
    }
    return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::AppendNode(LPCTSTR nodeName)
{
    MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();

    if (xmlDocument)
    {
        try
        {
            MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createElement(nodeName);
            //MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT), nodeName,"");

            newNode = m_xmlnode->appendChild(newNode.Detach());

            return newNode.Detach();

        }
        catch (_com_error er)
        {
            MessageBox(NULL, er.Description(), er.ErrorMessage(), MB_OK);
                
            return NULL;
        }

    }
    return NULL;
}


void CXmlNodeWrapper::RemoveNodes(LPCTSTR searchStr)
{
    if (!IsValid())
        return;

    MSXML2::IXMLDOMNodeListPtr nodeList = m_xmlnode->selectNodes(searchStr);

    for (int i = 0; i < nodeList->Getlength(); i++)
    {
        try
        {
            MSXML2::IXMLDOMNode* pNode = nodeList->Getitem(i).Detach();

            pNode->GetparentNode()->removeChild(pNode);
        }
        catch (_com_error er)
        {
        }
    }
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::Parent()
{
    if (IsValid())
        return m_xmlnode->GetparentNode().Detach();
    return NULL;
}

CXmlNodeListWrapper::CXmlNodeListWrapper()
{
}
CXmlNodeListWrapper::CXmlNodeListWrapper(MSXML2::IXMLDOMNodeListPtr pList)
{
    m_xmlnodelist = pList;
}

void CXmlNodeListWrapper::operator=(MSXML2::IXMLDOMNodeListPtr pList)
{
    if (IsValid())
        m_xmlnodelist.Release();
    m_xmlnodelist = pList;
}

CXmlNodeListWrapper::~CXmlNodeListWrapper()
{

}

int CXmlNodeListWrapper::Count()
{
    if (IsValid())
        return m_xmlnodelist->Getlength();
    else
        return 0;

}

BOOL CXmlNodeListWrapper::IsValid()
{
    if (m_xmlnodelist == NULL)
        return FALSE;
    if (m_xmlnodelist.GetInterfacePtr() == NULL)
        return FALSE;
    return TRUE;
}

MSXML2::IXMLDOMNode* CXmlNodeListWrapper::Next()
{
    if (IsValid())
        return m_xmlnodelist->nextNode().Detach();
    else
        return NULL;
}

void CXmlNodeListWrapper::Start()
{
    if (IsValid())
        m_xmlnodelist->reset();
}

MSXML2::IXMLDOMNode* CXmlNodeListWrapper::Node(int index)
{
    if (IsValid())
        return m_xmlnodelist->Getitem(index).Detach();
    else
        return NULL;
}

MSXML2::IXMLDOMDocument* CXmlNodeListWrapper::AsDocument()
{
    if (IsValid())
    {
        CXmlDocumentWrapper doc;
        doc.LoadXML(_T("<NodeList></NodeList>"));
        CXmlNodeWrapper root(doc.AsNode());

        for (int i = 0; i < m_xmlnodelist->Getlength(); i++)
        {
            root.InsertNode(root.NumNodes(),m_xmlnodelist->Getitem(i)->cloneNode(VARIANT_TRUE));
        }
        return doc.Detach();
    }
    else 
        return NULL;
}

MSXML2::IXMLDOMNodeList* CXmlNodeWrapper::FindNodes(LPCTSTR searchStr)
{
    if(IsValid())
    {
        try{
            return m_xmlnode->selectNodes(searchStr).Detach();
        }
        catch (_com_error e)
        {
            KSE::CString err = e.ErrorMessage();
            return NULL;
        }
    }
    else
        return NULL;
}

KSE::CString CXmlNodeWrapper::Name()
{
    if (IsValid())
        return (LPCSTR)m_xmlnode->GetbaseName();
    return "";
}

BOOL CXmlNodeWrapper::Name(KSE::CString& name)
{
    if (!IsValid())
        return FALSE;

    name = (LPCSTR)m_xmlnode->GetbaseName();

    return TRUE;
}


MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertAfter(MSXML2::IXMLDOMNode *refNode, MSXML2::IXMLDOMNode *pNode)
{
    MSXML2::IXMLDOMNodePtr nextNode = refNode->GetnextSibling();
    MSXML2::IXMLDOMNodePtr newNode;
    if (nextNode.GetInterfacePtr() != NULL)
        newNode = m_xmlnode->insertBefore(pNode,(IUnknown*)nextNode);
    else
        newNode = m_xmlnode->appendChild(pNode);
    return newNode.Detach();
}

void CXmlNodeWrapper::SetText(LPCTSTR text)
{
    if (IsValid())
        m_xmlnode->Puttext(text);
}

KSE::CString CXmlNodeWrapper::GetText()
{
    if (IsValid())
        return (LPCSTR)m_xmlnode->Gettext();
    else
        return _T("");
}

BOOL CXmlNodeWrapper::GetText(KSE::CString& strText)
{
    if (!IsValid())
        return FALSE;

    strText = (LPCSTR)m_xmlnode->Gettext();
    
    return TRUE;

}

void CXmlNodeWrapper::ReplaceNode(MSXML2::IXMLDOMNode *pOldNode, MSXML2::IXMLDOMNode *pNewNode)
{
    if (IsValid())
    {
        m_xmlnode->replaceChild(pNewNode,pOldNode);
    }
}

int CXmlNodeWrapper::NumAttributes()
{
    if (IsValid())
    {
        MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();
        if (attribs)
            return attribs->Getlength();
    }

    return 0;
}

KSE::CString CXmlNodeWrapper::GetAttribName(int index)
{
    if (IsValid())
    {
        MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();
        if (attribs)
        {
            MSXML2::IXMLDOMAttributePtr attrib = attribs->Getitem(index);
            if (attrib)
                return (LPCSTR)attrib->Getname();
        }
    }

    return "";
}

KSE::CString CXmlNodeWrapper::GetAttribVal(int index)
{
    if (IsValid())
    {
        MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();
        if (attribs)
        {
            MSXML2::IXMLDOMAttributePtr attrib = attribs->Getitem(index);
            if (attrib)
                return (LPCSTR)attrib->Gettext();
        }
    }


    return "";
}

KSE::CString CXmlNodeWrapper::NodeType()
{
    if (IsValid())
        return (LPCSTR)m_xmlnode->GetnodeTypeString();
    return "";
}
