#include "SysConsole.h"

CSysConsole::CSysConsole()
{
}


CSysConsole::~CSysConsole()
{
}


BOOL CSysConsole::Init(LPCTSTR szCmdline)
{
    HANDLE hWrite;
    SECURITY_ATTRIBUTES saAttr;

	BYTE sd[SECURITY_DESCRIPTOR_MIN_LENGTH];
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, (PACL)0, FALSE);

    // Set the bInheritHandle flag so pipe handles are inherited. 

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = &sd;

    // Create a pipe for the child process's STDOUT. 

    if (!CreatePipe((PHANDLE)&m_hRead, (PHANDLE)&hWrite, &saAttr, 0))
    {
        ErrorExit(TEXT("StdoutRd CreatePipe"));
        return FALSE;
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.

    if (!SetHandleInformation(m_hRead, HANDLE_FLAG_INHERIT, 0))
    {
        ErrorExit(TEXT("Stdout SetHandleInformation"));
        return FALSE;
    }


    // Create the child process. 
    // Set up members of the PROCESS_INFORMATION structure. 
    ZeroMemory(&m_piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.
    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hWrite;
    siStartInfo.hStdOutput = hWrite;
    siStartInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    // Create the child process. 
	// https://stackoverflow.com/questions/32926272/beginner-c-createprocess-error-2
	wstring str = szCmdline;
	str = _T("cmd /c ") + str;
    BOOL bSuccess = CreateProcess(NULL,
        (LPWSTR)str.c_str(),     // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        CREATE_NO_WINDOW, // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer 
        &m_piProcInfo);  // receives PROCESS_INFORMATION 

    // If an error occurs, exit the application. 
    if (!bSuccess)
    {
        CloseHandle(m_hRead);
        CloseHandle(hWrite);
        ErrorExit(TEXT("CreateProcess Wrong"));
        return FALSE;
    }

    CloseHandle(hWrite);
    return TRUE;
}

BOOL CSysConsole::Read(std::string &str)
{
    DWORD dwRead = 0;
    CHAR chBuf[2048] = { 0 };
    BOOL bSuccess = TRUE;

    if (WaitForSingleObject(m_piProcInfo.hProcess, 0) != WAIT_TIMEOUT)
    {
        bSuccess = ReadFile(m_hRead, chBuf, 2048, &dwRead, NULL);
        if (bSuccess)
            str += chBuf;

        CloseHandle(m_hRead);
        CloseHandle(m_piProcInfo.hProcess);
        CloseHandle(m_piProcInfo.hThread);

        return FALSE;
    }


    if (PeekNamedPipe(m_hRead, NULL, NULL, NULL, &dwRead, NULL) && dwRead > 0)
    {
        bSuccess = ReadFile(m_hRead, chBuf, 2048, &dwRead, NULL);
        if (bSuccess)
            str += chBuf;

    }

    return TRUE;
}

void CSysConsole::ErrorExit(PTSTR lpszFunction)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40)*sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);

    m_sErrInfo = (LPCTSTR)lpDisplayBuf;

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);

}

CDuiString CSysConsole::GetErrInfo()
{
    return m_sErrInfo;
}