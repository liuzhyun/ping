#include "MainFrame.h"

const TCHAR* const kEditControlName = _T("domain");
const TCHAR* const kPingButtonControlName = _T("ping");

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

CDuiString CMainFrame::GetSkinFile()
{
    return _T("skin.xml");
}

LPCTSTR CMainFrame::GetWindowClassName() const
{
	return _T("MainFrame"); 
}

void CMainFrame::InitWindow()
{
    
}

void CMainFrame::OnClick(TNotifyUI& msg)
{
    if (msg.pSender->GetName() == kPingButtonControlName)
    {
        CControlUI* pEdit = static_cast<CControlUI*>(m_PaintManager.FindControl(kEditControlName));
        if (!pEdit) return;

        if(pEdit->GetText().IsEmpty())
            ::MessageBox(GetHWND(), _T("请输入你想ping的域名！"), _T("警告"), 0);

        return;
    }

    __super::OnClick(msg);
}

void CMainFrame::Notify(TNotifyUI& msg)
{
    if (msg.sType == DUI_MSGTYPE_SETFOCUS)
    {
        if (msg.pSender->GetName() == kEditControlName && m_bIsFirstClickEdit)
        {
            m_bIsFirstClickEdit = false;
            DWORD dColor = ((CEditUI*)msg.pSender)->GetTextColor();
            ((CEditUI*)msg.pSender)->SetTextColor(((CEditUI*)msg.pSender)->GetDisabledTextColor());
            ((CEditUI*)msg.pSender)->SetDisabledTextColor(dColor);
            msg.pSender->SetText(_T(""));
        }
    }

    __super::Notify(msg);
}