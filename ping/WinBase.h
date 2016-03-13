#pragma once
#include "StdAfx.h"
class WinBase : public WindowImplBase
{
public:
    WinBase();
    ~WinBase();

    virtual CDuiString GetSkinFile() = 0;
    virtual CDuiString GetSkinFolder();
    virtual LPCTSTR GetWindowClassName(void) const = 0;

    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

