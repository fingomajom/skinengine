
#pragma once

enum { 
    
    // 调用者ID
    CALLERID_UNKNOWN = 0,
    CALLERID_FULL = 1,
    CALLERID_SAFECENTER = 2,
    CALLERID_KIS = 3,
    CALLERID_KOS = 4,
    //...
    //////////////////////////////////////////////////////////////////////////
    //...
    CALLERID_USER = 0xFFF,

    CALLERID_MAX  = 0xFFFE,


    //////////////////////////////////////////////////////////////////////////
    // 提供者ID 也可能是调用者
    //////////////////////////////////////////////////////////////////////////
    MODULEID_UNKNOWN = 0xFFFF,
    MODULEID_FULL    = 0x10000,
    MODULEID_SERVICE = 0x10001,
    MODULEID_SCORE   = 0x10002,
    MODULEID_KAV     = 0x10003,
    MODULEID_KPFW    = 0x10004,
    MODULEID_ANTISPY = 0x10005,
    MODULEID_UPDATE  = 0x10006,

    MODULEID_LOG     = 0x10007,
    MODULEID_SETTING = 0x10008,

    //...
    //////////////////////////////////////////////////////////////////////////
    //...
    MODULEID_USER    = 0xFFFFF,

    MODULEID_MAX     = 0xFFFFFFFF
};

#define IS_CALLERID(id) ( id >= CALLERID_UNKNOWN && id <= CALLERID_MAX )
#define IS_MODULEID(id) ( id >= MODULEID_UNKNOWN && id <= MODULEID_MAX )

