

#pragma once

class CDWCrashMgt
{
    CDWCrashMgt( const CDWCrashMgt& );

    CDWCrashMgt(void);
public:
    ~CDWCrashMgt(void);

    static CDWCrashMgt& Instance();

};