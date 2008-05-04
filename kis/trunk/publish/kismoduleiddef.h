
#pragma once

enum { 
    
    // 调用者ID
    CALLERID_UNKNOWN = 0,
    CALLERID_FULL = 1,
    CALLERID_SAFECENTER = 2,
    CALLERID_KIS = 3,
    CALLERID_KOS = 4,
    //...
    CALLERID_USER = 0xFF,
    CALLERID_MAX  = 0xFFFE,


    // 提供者ID 也可能是调用者
    MODULEID_UNKNOWN = 0xFFFF,
    MODULEID_FULL    = 0x10000,
    MODULEID_SERVICE = 0x10001,
    MODULEID_SCORE   = 0x10002,
    MODULEID_

};

