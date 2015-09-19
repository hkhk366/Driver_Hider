// AppProtect.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AppProtect.h"
#include "AppProtectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAppProtectApp

BEGIN_MESSAGE_MAP(CAppProtectApp, CWinApp)
	//{{AFX_MSG_MAP(CAppProtectApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppProtectApp construction

CAppProtectApp::CAppProtectApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAppProtectApp object

CAppProtectApp theApp;


BOOL AdjustPrivileges()
{
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES tp = {0};
	TOKEN_PRIVILEGES oldtp = {0};
	DWORD dwSize = sizeof(TOKEN_PRIVILEGES);
	LUID luid = {0};

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		if (GetLastError()==ERROR_CALL_NOT_IMPLEMENTED)
			return TRUE;
		else
			return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount=1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	/* Adjust Token Privileges */
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
		CloseHandle(hToken);
		return FALSE;
	}

	// close handles
	CloseHandle(hToken);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAppProtectApp initialization

BOOL CAppProtectApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	static HANDLE hMutex = CreateMutex(NULL,FALSE,"CAppProtectAppDemo");
	if(ERROR_ALREADY_EXISTS == GetLastError())
	{
		AfxMessageBox("程序已经启动!");
		ExitProcess(0);
	}
	//AdjustPrivileges();

	CAppProtectDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
