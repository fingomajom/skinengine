

#pragma once

class CDWCrashMgt : public CDWSingleton<CDWCrashMgt>
{
    //CDWCrashMgt( const CDWCrashMgt& );

    friend class CDWSingleton<CDWCrashMgt>;
public:
    CDWCrashMgt(void);
    ~CDWCrashMgt(void);
};