#include "MainFrame.h"

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

LPCTSTR CMainFrame::GetWindowClassName() const
{
	return _T("MainFrame"); 
}

void CMainFrame::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		if (msg.pSender->GetName() == _T("closebtn"))
			Close();
	}
}

LRESULT CMainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;

	if (uMsg == WM_CREATE)
	{
		m_PaintManager.Init(m_hWnd);

		CControlUI* pBut = new CButtonUI();
		pBut->SetText(_T("Button"));
		pBut->SetName(_T("closebtn"));
		pBut->SetBkColor(0xFF00FF00);

		m_PaintManager.AttachDialog(pBut);
		m_PaintManager.AddNotifier(this);

		return lRes;
	}
	else if (uMsg == WM_DESTROY)
		::PostQuitMessage(0);

	// WM_NCACTIVATE��WM_NCCALCSIZE��WM_NCPAINT��������ϵͳ������
	else if (uMsg == WM_NCACTIVATE)
	{
		if (!::IsIconic(m_hWnd)) //�жϴ����Ƿ�Ϊ��С�� return ��С��Ϊ0�������0
			return (wParam == 0) ? TRUE : FALSE;
	}
	else if (uMsg == WM_NCCALCSIZE)
		return 0;
	else if (uMsg == WM_NCPAINT)
		return 0;

	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes))
		return lRes;

	return __super::HandleMessage(uMsg, wParam, lParam);
}