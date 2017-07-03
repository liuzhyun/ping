#include "WinBase.h"

WinBase::WinBase()
{
}

WinBase::~WinBase()
{
}

CDuiString WinBase::GetSkinFolder()
{
    return _T("");
}

LRESULT WinBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
    styleValue &= ~WS_CAPTION;
    ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    RECT rcClient;
    ::GetClientRect(*this, &rcClient);
    ::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
        rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

    m_PaintManager.Init(m_hWnd);
    m_PaintManager.AddPreMessageFilter(this);

    CDialogBuilder builder;

    switch (GetResourceType())
    {
    case UILIB_ZIP:
        m_PaintManager.SetResourceZip(GetZIPFileName().GetData(), true);
        break;
    case UILIB_ZIPRESOURCE:
    {
        HRSRC hResource = ::FindResource(m_PaintManager.GetResourceDll(), GetResourceID(), _T("ZIPRES"));
        if (hResource == NULL)
            return 0L;
        DWORD dwSize = 0;
        HGLOBAL hGlobal = ::LoadResource(m_PaintManager.GetResourceDll(), hResource);
        if (hGlobal == NULL)
        {
#if defined(WIN32) && !defined(UNDER_CE)
            ::FreeResource(hResource);
#endif
            return 0L;
        }
        dwSize = ::SizeofResource(m_PaintManager.GetResourceDll(), hResource);
        if (dwSize == 0)
            return 0L;
        m_lpResourceZIPBuffer = new BYTE[dwSize];
        if (m_lpResourceZIPBuffer != NULL)
        {
            ::CopyMemory(m_lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);
        }
#if defined(WIN32) && !defined(UNDER_CE)
        ::FreeResource(hResource);
#endif
        m_PaintManager.SetResourceZip(m_lpResourceZIPBuffer, dwSize);
    }
    break;
    }

    CControlUI* pRoot = NULL;
    if (GetResourceType() == UILIB_RESOURCE)
    {
        STRINGorID xml(_ttoi(GetSkinFile().GetData()));
        pRoot = builder.Create(xml, _T("xml"), this, &m_PaintManager);
    }
    else
        pRoot = builder.Create(GetSkinFile().GetData(), (UINT)0, this, &m_PaintManager);
    ASSERT(pRoot);
    if (pRoot == NULL)
    {
        MessageBox(NULL, _T("加载资源文件失败"), _T("Duilib"), MB_OK | MB_ICONERROR);
        ExitProcess(1);
        return 0;
    }
    m_PaintManager.AttachDialog(pRoot);
    m_PaintManager.AddNotifier(this);

    InitWindow();
    return 0;
}

LRESULT WinBase::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    PostQuitMessage(0L);

    bHandled = FALSE;
    return 0;
}

LRESULT WinBase::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
    BOOL bZoomed = ::IsZoomed(*this);
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    if (::IsZoomed(*this) != bZoomed)
    {
        if (!bZoomed)
        {
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("maxbtn")));
            if (pControl) pControl->SetVisible(false);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("restorebtn")));
            if (pControl) pControl->SetVisible(true);
        }
        else
        {
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("maxbtn")));
            if (pControl) pControl->SetVisible(true);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("restorebtn")));
            if (pControl) pControl->SetVisible(false);
        }
    }
#else
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
#endif
    return lRes;
}