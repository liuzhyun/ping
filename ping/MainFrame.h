#pragma once
#include "StdAfx.h"
#include "WinBase.h"

class CMainFrame : public WinBase
{
public:
	CMainFrame();
	~CMainFrame();

    virtual void InitWindow();
    virtual void OnClick(TNotifyUI& msg);
    virtual void Notify(TNotifyUI& msg);
    virtual CDuiString GetSkinFile();
    virtual LPCTSTR GetWindowClassName(void) const;

private:
    bool    m_bIsFirstClickEdit = true;
};

