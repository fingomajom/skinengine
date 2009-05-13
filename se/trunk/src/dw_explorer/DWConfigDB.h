

#pragma once

class CDWConfigDB : public CDWSingleton<CDWConfigDB>
{
    friend class CDWSingleton<CDWConfigDB>;
    CDWConfigDB() {}

public:
    ~CDWConfigDB() {}
};