

#pragma once

class CDWCrashMgt
{
    //CDWCrashMgt( const CDWCrashMgt& );

public:
    CDWCrashMgt(void);
    ~CDWCrashMgt(void);

    static CDWCrashMgt& Instance();

};