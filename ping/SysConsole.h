#pragma once
#include "StdAfx.h"
#include <strsafe.h>

class CSysConsole
{
public:
    CSysConsole();
    ~CSysConsole();

    BOOL Init(LPCTSTR);
    BOOL Read(std::string&);
    void ErrorExit(PTSTR);
    CDuiString GetErrInfo();

protected:
    HANDLE m_hRead;
    PROCESS_INFORMATION m_piProcInfo;
    CDuiString m_sErrInfo;
};

