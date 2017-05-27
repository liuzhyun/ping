#pragma once
#include "StdAfx.h"
#include "WinBase.h"
#include "SysConsole.h"

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
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	CDuiString StringToCDuiString(std::string&);
    void WriteStringToFile(CDuiString& str);
    HRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    CDuiString      m_strFilePath;
    HANDLE          m_hFile;
    CSysConsole     m_SysCon;
    bool    m_bIsFirstClickEdit = true;
    CListUI*    m_pList;
    CProgressUI* m_pProgress;
};

