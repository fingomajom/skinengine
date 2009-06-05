#pragma once

class CDWInternetFeature
{
public:
    CDWInternetFeature(void);
    ~CDWInternetFeature(void);

    /** ����/�ر���Ϣ����ʾ */
    BOOL EnableSecurityBand(BOOL bEnable);

    /** ����/�ر� ActiveX �ؼ���װ */
    BOOL RestrictActivexInstall(BOOL bEnable);

    /** �Ƿ����� ActiveX �ؼ���װ */
    BOOL IsActivexInstallEnabled();

    /** ����/�رյ������ڿؼ���װ */
    BOOL WebOcPopupManagement(BOOL bEnable);

    /** �Ƿ����õ������ڿؼ���װ */
    BOOL IsWebPopupManagementEnabled();

protected:

    BOOL SetFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags, BOOL fEnable);
    BOOL IsFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD dwFlags);

private:
    HMODULE m_hMod;
};
