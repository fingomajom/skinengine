#include "precomp.h"
#pragma hdrstop


DWORD g_dwOsVersion = os_unknown;

knl_os_info_offset g_knl_current_offset = { 0 };

knl_os_info_offset g_knl_os_info_offset[] = 
{
         /* win 2000 */
/*0*/    { 1,    0xE0,   0x2E,   0xa4,   0xf0,   0x9C,  0x1B0 },
    

         /* win xp */
/*1*/    { 1,    0x101,  0x35,   0xba,   0x115,  0x84,  0x1B0 },


         /* win 2003 */
/*2*/    { 1,    0x10A,  0x37,   0xc2,   0x11f,  0x84,  0x190 },
         /* win 2003 sp1 sp2 */
/*3*/    { 1,    0x10A,  0x37,   0xc2,   0x11f,  0x84,  0x1A0 },

         /* win vista */
/*4*/    { 1,    0x152,  0x4E,   0x105,  0x16a,  0x94,  0x188 },
         /* win vista sp 1*/
/*5*/    { 1,    0x14E,  0x4E,   0x105,  0x166,  0x9C,  0x188 }
};

//{ 1,    0xE0,   0x2E,   0xa4,   0xf0,   0x9C,    0x0A0,   0x1C8,   0x1FC,   0x1B0,   -1,      -1,       -1,       0x270,   0x1E0,   0x230,   0x234,   0x22C,   0x240,   0x10,    0x5C,    0x1AA,    0x40,    0, },
//{ 1,    0x101,  0x35,   0xba,   0x115,  0x84,    0x88,    0x14C,   0x174,   0x1B0,   0x248,   0x4,      0x8,      0x190,   0x1EC,   0x224,   0x228,   0x220,   0x22C,   0x10,    0x60,    -1,       0x42,    0, },
//{ 1,    0x10A,  0x37,   0xc2,   0x11f,  0x84,    0x88,    0x128,   0x154,   0x190,   0x248,   0x4,      0x8,      0x170,   0x1F4,   0x22C,   0x230,   0x228,   0x234,   0x10,    0x60,    -1,       -1,      0, },
//{ 1,    0x152,  0x4E,   0x105,  0x16a,  0x94,    0x98,    0x138,   0x164,   0x1A0,   0x240,   0x4,      0x8,      0x180,   0x1E4,   0x21C,   0x220,   0x218,   0x224,   0x10,    0x60,    -1,       -1,      0, },
//{ 1,    0x14E,  0x4E,   0x105,  0x166,  0x9C,    0x0A0,   0x124,   0x14C,   0x188,   0x228,   0x4,      0x8,      0x168,   0x20C,   0x1F8,   0x240,   0x144,   0x248,   0x10,    0x70,    -1,       0x42,    0, }

lp_knl_os_info_offset g_knl_os_info_offset_array[] = 
{

/* os_unkown  */        {   NULL                      },

/* os_2k_sp0  */        {   &g_knl_os_info_offset[0]  },
/* os_2k_sp0  */        {   &g_knl_os_info_offset[0]  },
/* os_2k_sp0  */        {   &g_knl_os_info_offset[0]  },
/* os_2k_sp0  */        {   &g_knl_os_info_offset[0]  },
/* os_2k_sp0  */        {   &g_knl_os_info_offset[0]  },

/* os_xp_sp0  */        {   &g_knl_os_info_offset[1]  },
/* os_xp_sp1  */        {   &g_knl_os_info_offset[1]  },
/* os_xp_sp2  */        {   &g_knl_os_info_offset[1]  },
/* os_xp_sp3  */        {   &g_knl_os_info_offset[1]  },

/* os_2003_sp0  */      {   &g_knl_os_info_offset[2]  },
/* os_2003_sp1  */      {   &g_knl_os_info_offset[3]  },
/* os_2003_sp2  */      {   &g_knl_os_info_offset[3]  },

/* os_vista_sp0  */     {   &g_knl_os_info_offset[4]  },
/* os_vista_sp1  */     {   &g_knl_os_info_offset[5]  }
};



BOOL InitKernelVersion()
{
    BOOL    bResult = FALSE;
    ULONG   ulMajor = 0;
    ULONG   ulMinor = 0;
    ULONG   ulBuilderNumber = 0;

    UNICODE_STRING CSDVersion;

    NTSTATUS ntRetCode = STATUS_INVALID_PARAMETER;
    ULONG    ulType    = 0;
    ULONG    ulReturnLength = 0;

    WCHAR    sCSDVersion[PROC_NAME_LEN];

    g_dwOsVersion = os_unknown;

    PsGetVersion(&ulMajor, &ulMinor, &ulBuilderNumber, &CSDVersion);

    
    ntRetCode = DrvGetValueKey(
        REG_CURRENT_VERSION_PATH, 
        CSD_VERSION, 
        &ulType, 
        sCSDVersion, 
        PROC_NAME_LEN, 
        &ulReturnLength);

    if (ulReturnLength == 0)
    {
        if (CSDVersion.Length != 0 && CSDVersion.Length < PROC_NAME_LEN)
        {
            RtlCopyMemory(sCSDVersion, CSDVersion.Buffer, CSDVersion.Length);

            ulReturnLength = CSDVersion.Length;
        }
    }

    if (ulMajor != 5 && ulMajor != 6)
        goto Exit0;

    if (ulMajor == 5)
    {
        if (ulMinor == 0)
        {
            if (ulReturnLength == 0)
            {
                g_dwOsVersion = os_2k_sp0;
            }
            else if (!_wcsnicmp(sCSDVersion, L"Service Pack 1", ulReturnLength / 2))
            {
                g_dwOsVersion = os_2k_sp1;
            }
            else if (!_wcsnicmp(sCSDVersion, L"Service Pack 2", ulReturnLength / 2))
            {
                g_dwOsVersion = os_2k_sp2;
            }
            else if (!_wcsnicmp(sCSDVersion, L"Service Pack 3", ulReturnLength / 2))
            {
                g_dwOsVersion = os_2k_sp3;
            }
            else if (!_wcsnicmp(sCSDVersion, L"Service Pack 4", ulReturnLength / 2))
            {
                g_dwOsVersion = os_2k_sp4;
            }
            else
            {
                g_dwOsVersion = os_unknown;
                goto Exit0;
            }

        }
        else if (ulMinor == 1)
        {
            if ((ulReturnLength / 2) < wcslen(L"Service Pack 1"))
            {
                g_dwOsVersion = os_xp_sp0;
            }
            else if (!_wcsnicmp(sCSDVersion, L"Service Pack 1", ulReturnLength / 2))
            {
                g_dwOsVersion = os_xp_sp1;
            }
            else if (!_wcsnicmp(sCSDVersion, L"Service Pack 2", ulReturnLength / 2))
            {
                g_dwOsVersion = os_xp_sp2;
            }
            else if (!_wcsnicmp(sCSDVersion, L"Service Pack 3", wcslen(L"Service Pack 3")))
            {
                g_dwOsVersion = os_xp_sp3;
            }
            else
            {
                g_dwOsVersion = os_unknown;
                goto Exit0;
            }
        }
        else if(ulMinor == 2)
        {
            if (ulReturnLength == 0)
            {
                g_dwOsVersion = os_2003_sp0;
            }
            else if (!_wcsnicmp(sCSDVersion, L"Service Pack 1", ulReturnLength / 2))
            {
                g_dwOsVersion = os_2003_sp1;
            }
            else if (!_wcsnicmp(sCSDVersion, L"Service Pack 2", ulReturnLength / 2))
            {
                g_dwOsVersion = os_2003_sp2;
            }
            else
            {
                g_dwOsVersion = os_unknown;
                goto Exit0;
            }
        }
    }
    else if (ulMajor == 6)//vista
    {
        if (ulMinor == 0)
        {
            if (ulReturnLength == 0)
            {
                g_dwOsVersion = os_vista_sp0;
            }
            else if (!_wcsnicmp(sCSDVersion, L"Service Pack 1", ulReturnLength / 2))
            {
                g_dwOsVersion = os_vista_sp1;
            }
            else
            {
                g_dwOsVersion = os_unknown;
                goto Exit0;
            }
        }
        else
        {
            g_dwOsVersion = os_unknown;
            goto Exit0;
        }
    }
    else
    {
        g_dwOsVersion = os_unknown;
        goto Exit0;
    }

    bResult = TRUE;

    g_knl_current_offset = *g_knl_os_info_offset_array[ g_dwOsVersion ];

Exit0:

    return bResult;
}
