#pragma once

class CDWInternetFeature
{
public:
    CDWInternetFeature(void);
    ~CDWInternetFeature(void);

    /** 开启/关闭信息栏提示 */
    BOOL EnableSecurityBand(BOOL bEnable);

    /** 开启/关闭 ActiveX 控件安装 */
    BOOL RestrictActivexInstall(BOOL bEnable);

    /** 是否启用 ActiveX 控件安装 */
    BOOL IsActivexInstallEnabled();

    /** 开启/关闭弹出窗口控件安装 */
    BOOL WebOcPopupManagement(BOOL bEnable);

    /** 是否启用弹出窗口控件安装 */
    BOOL IsWebPopupManagementEnabled();

protected:

    BOOL SetFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags, BOOL fEnable);
    BOOL IsFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags);

private:
    HMODULE m_hMod;
};
