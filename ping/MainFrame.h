#pragma once
#include "StdAfx.h"

class CMainFrame : public CWindowWnd, public INotifyUI
{
public:
	CMainFrame();
	~CMainFrame();

	virtual LPCTSTR GetWindowClassName() const;
	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	CPaintManagerUI m_PaintManager;
};

