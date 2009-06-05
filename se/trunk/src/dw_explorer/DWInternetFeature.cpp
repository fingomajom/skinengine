#include "StdAfx.h"
#include "DWInternetFeature.h"

typedef HRESULT (_stdcall * pfnCoInternetSetFeatureEnabled)(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags, BOOL fEnable);
typedef HRESULT (_stdcall * pfnCoInternetIsFeatureEnabled )(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags);

pfnCoInternetSetFeatureEnabled g_pfnCoInternetSetFeatureEnabled = NULL;
pfnCoInternetIsFeatureEnabled  g_pfnCoInternetIsFeatureEnabled  = NULL;


CDWInternetFeature::CDWInternetFeature(void)
{
    m_hMod = LoadLibrary(_T("urlmon.dll"));
    if (m_hMod)
    {
        g_pfnCoInternetSetFeatureEnabled = (pfnCoInternetSetFeatureEnabled)GetProcAddress(m_hMod, "CoInternetSetFeatureEnabled");
        g_pfnCoInternetIsFeatureEnabled  = (pfnCoInternetIsFeatureEnabled)GetProcAddress(m_hMod, "CoInternetIsFeatureEnabled");
        if (g_pfnCoInternetSetFeatureEnabled && g_pfnCoInternetIsFeatureEnabled)
        {
            // 这个是本地文件中带有控件或脚本的情况！
            // 暂时开启这个，因为ie自带的对于本地弹窗是没有限制的。
            SetFeatureEnabled(FEATURE_LOCALMACHINE_LOCKDOWN, SET_FEATURE_ON_PROCESS, TRUE);
        }
    }
}

CDWInternetFeature::~CDWInternetFeature(void)
{
    FreeLibrary(m_hMod);
}


BOOL CDWInternetFeature::EnableSecurityBand(BOOL bEnable)
{
    return SetFeatureEnabled(FEATURE_SECURITYBAND, SET_FEATURE_ON_PROCESS, bEnable);
}

BOOL CDWInternetFeature::RestrictActivexInstall(BOOL bEnable)
{
    if (bEnable)
        EnableSecurityBand(bEnable);
    return SetFeatureEnabled(FEATURE_RESTRICT_ACTIVEXINSTALL, SET_FEATURE_ON_PROCESS, bEnable);
}

BOOL CDWInternetFeature::IsActivexInstallEnabled()
{
    return IsFeatureEnabled(FEATURE_RESTRICT_ACTIVEXINSTALL, SET_FEATURE_ON_PROCESS);
}

BOOL CDWInternetFeature::WebOcPopupManagement(BOOL bEnable)
{
    if (bEnable)
        EnableSecurityBand(bEnable);
    return SetFeatureEnabled(FEATURE_WEBOC_POPUPMANAGEMENT, SET_FEATURE_ON_PROCESS, bEnable);
}

BOOL CDWInternetFeature::IsWebPopupManagementEnabled()
{
    return IsFeatureEnabled(FEATURE_WEBOC_POPUPMANAGEMENT, SET_FEATURE_ON_PROCESS);
}


BOOL CDWInternetFeature::SetFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags, BOOL fEnable)
{
    BOOL bRet = FALSE;
    if (g_pfnCoInternetSetFeatureEnabled)
        bRet = SUCCEEDED(g_pfnCoInternetSetFeatureEnabled(FeatureEntry, dwFlags, fEnable));
    return bRet;
}
BOOL CDWInternetFeature::IsFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags)
{
    BOOL bRet = FALSE;
    if (g_pfnCoInternetIsFeatureEnabled)
        bRet = SUCCEEDED(g_pfnCoInternetIsFeatureEnabled(FeatureEntry, dwFlags));
    return bRet;
}
