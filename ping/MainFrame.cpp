#include "MainFrame.h"
#include <ShlObj.h>

const TCHAR* const kEditControlName = _T("domain");
const TCHAR* const kPingButtonControlName = _T("ping");
const TCHAR* const kProgressControlName = _T("progress");
const TCHAR* const kOutListControlName = _T("outList");

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
    if (m_hFile)
        CloseHandle(m_hFile);
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
    m_pList = static_cast<CListUI*>(m_PaintManager.FindControl(kOutListControlName));
    if (!m_pList) return;
    m_pProgress = static_cast<CProgressUI*>(m_PaintManager.FindControl(kProgressControlName));
    if (!m_pProgress) return;

    TCHAR path[MAX_PATH + 2];
    ::SHGetSpecialFolderPath(NULL, path, CSIDL_DESKTOPDIRECTORY, FALSE);
    m_strFilePath = path;
    m_strFilePath += _T("\\net.log");
}

void CMainFrame::OnClick(TNotifyUI& msg)
{
    if (msg.pSender->GetName() == kPingButtonControlName)
    {
        CControlUI* pEdit = static_cast<CControlUI*>(m_PaintManager.FindControl(kEditControlName));
        if (!pEdit) return;

        if (pEdit->GetText().IsEmpty()) 
        {
            ::MessageBox(GetHWND(), _T("请输入你想ping的域名！"), _T("警告"), 0);
            return;
        }

        m_hFile = ::CreateFile((LPCTSTR)m_strFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_hFile == INVALID_HANDLE_VALUE)
        {
            ::MessageBox(GetHWND(), _T("新建或者打开net.log出错！请重试"), _T("警告"), 0);
            return;
        }

        SetFilePointer(m_hFile, 0, NULL, FILE_END);

        SYSTEMTIME sys;
        GetLocalTime(&sys);
        TCHAR strTemp[50];
        wsprintf(strTemp, _T("%d-%02d-%02d %02d:%02d:%02d"), sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
        CDuiString str = _T("\r\n\r\n\r\n*****************  ");
        str += strTemp;
        str += _T("  ********************\r\n\r\n");
        WriteStringToFile(str);

        str = _T("ping ");
        str += pEdit->GetText();
		if (!m_SysCon.Init(str.GetData()))
		{
			::MessageBox(GetHWND(), m_SysCon.GetErrInfo(), _T("错误"), 0);
			return;
		}
        SetTimer(m_hWnd, 1, 100, NULL);

        CListTextElementUI* pListElement = new CListTextElementUI;
        pListElement->SetOwner(m_pList);
        pListElement->SetText(1, pEdit->GetText());
        if (pListElement) m_pList->Add(pListElement);

        m_pProgress->SetValue(10);

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

HRESULT CMainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = FALSE;
    if (uMsg == WM_TIMER)
        lRes = OnTimer(uMsg, wParam, lParam, bHandled);

    if (bHandled) return lRes;
        
    return __super::HandleMessage(uMsg, wParam, lParam);
}

HRESULT CMainFrame::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    std::string str;
    BOOL bKillTime = m_SysCon.Read(str);

	CDuiString strTemp = StringToCDuiString(str);
    if (!strTemp.IsEmpty())
        WriteStringToFile(strTemp);

	if (!bKillTime) 
	{
		KillTimer(m_hWnd, 1);
		CListTextElementUI* pListElement = (CListTextElementUI*)m_pList->GetItemAt(0);
		pListElement->SetText(2, _T("成功"));
		m_pProgress->SetValue(100);
		::MessageBox(GetHWND(), _T("测试完成，请检查桌面net.log文件！"), _T("提示"), 0);
	}

    bHandled = TRUE;
    return 0;
}

CDuiString CMainFrame::StringToCDuiString(std::string& str)
{
	int  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	CDuiString  rt;
	rt = (wchar_t*)pUnicode;
	delete  pUnicode;

	return  rt;
}

void CMainFrame::WriteStringToFile(CDuiString& str)
{
    DWORD dwLen = WideCharToMultiByte(CP_ACP, NULL, str.GetData(), -1, NULL, NULL, NULL, FALSE);
    char* pchBuffer = new char[dwLen + 1];
    WideCharToMultiByte(CP_ACP, NULL, str.GetData(), -1, pchBuffer, dwLen, NULL, FALSE);
    DWORD dw = 0;
    ::WriteFile(m_hFile, pchBuffer, dwLen - 1, &dw, NULL);
}