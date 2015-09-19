// AppProtectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AppProtect.h"
#include "AppProtectDlg.h"
#include "afxcmn.h"
#include "inc.h"
#include <winsvc.h>
#include <Tlhelp32.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAppProtectDlg dialog

CAppProtectDlg::CAppProtectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppProtectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAppProtectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDevice = NULL;
	m_dwAppState = -1;
	m_dwProcess = 0;
}

void CAppProtectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppProtectDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAppProtectDlg, CDialog)
	//{{AFX_MSG_MAP(CAppProtectDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PROTECT, OnProtect)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_HOTKEY,OnHotkey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAppProtectDlg message handlers

BOOL CAppProtectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	RegisterHotKey(m_hWnd,1001,MOD_CONTROL|MOD_SHIFT,VK_F8); 
	CString strPath = GetExeDir();
	strPath=strPath+szSysPath;
	char chPath[MAX_PATH];
	strcpy(chPath,strPath.GetBuffer(strPath.GetLength()));
	strPath.ReleaseBuffer();
	BOOL bResult = InstallSvc((LPTSTR)szSvcName,(LPTSTR)szSvcName,chPath,SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START);
	if(FALSE == bResult)
	{
		MessageBox(_TEXT(" Install driver failed and exit."), _TEXT("Error"), MB_OK | MB_ICONSTOP);
		UninstallDevice();
		bResult = InstallSvc((LPTSTR)szSvcName,(LPTSTR)szSvcName,chPath,SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START);
		if(FALSE == bResult)
		{
			PostMessage(WM_CLOSE);		
			return FALSE;
		}
	}
	if(bResult)
	{
		bResult = StartSvc((LPTSTR)szSvcName);
		if(FALSE == bResult)
		{
			MessageBox(_TEXT("Run driver Failed and Exit.  "), _TEXT("Error"), MB_OK | MB_ICONSTOP);
			PostMessage(WM_CLOSE);
			return FALSE;
		}

		m_hDevice = CreateFile((LPTSTR)szSymLinkName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(INVALID_HANDLE_VALUE == m_hDevice)
		{
			MessageBox(_TEXT(" Open driver failed and exit. "), _TEXT("Error"), MB_OK | MB_ICONSTOP);
			PostMessage(WM_CLOSE);
			return FALSE;
		}
	}
	m_dwProcess = GetCurrentProcessId();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAppProtectDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAppProtectDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAppProtectDlg::OnProtect() 
{
	// TODO: Add your control notification handler code here
	DWORD dwOutput;
	CHAR inBuffer[10];
	CHAR outBuffer[10];
	memset(inBuffer, 0, 10);
	memset(outBuffer, 0, 10);
	_ultoa(m_dwProcess, inBuffer, 10);
	BOOL bRet=DeviceIoControl(m_hDevice, IO_INSERT_HIDE_PROCESS, inBuffer, 10, &outBuffer, 10, &dwOutput, NULL);
	if (bRet)
	{
		bRet=DeviceIoControl(m_hDevice, IO_INSERT_PROTECT_PROCESS, inBuffer, 10, &outBuffer, 10, &dwOutput, NULL);
		if (bRet)
		{
			m_dwAppState = 0;
			ShowWindow(SW_HIDE);
		}
	}
	else
	{
		AfxMessageBox(_T("Failed."));
	}
}



BOOL CAppProtectDlg::InstallSvc(LPTSTR lpszSvcName, LPTSTR lpszDisplayName, LPTSTR lpszSvcBinaryPath, DWORD dwSvcType, DWORD dwStartType)
{
	SC_HANDLE hSCM=NULL;
	SC_HANDLE hSvc=NULL;
	hSCM=OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if(NULL == hSCM)
	{
		DWORD dw=GetLastError();
		return FALSE;
	}

	for(int i = 0; i < 3 && (NULL == hSvc); i++)
	{
		hSvc = CreateService(hSCM, lpszSvcName, lpszDisplayName, SERVICE_ALL_ACCESS, 
			dwSvcType, dwStartType, SERVICE_ERROR_NORMAL, 
			lpszSvcBinaryPath, NULL, NULL, NULL, NULL, NULL);
		if(NULL != hSvc)
		{
			if(NULL != hSvc)
			{
				CloseServiceHandle(hSvc);
			}
			CloseServiceHandle(hSCM);
			return TRUE;
		}
	}

	CloseServiceHandle(hSCM);

	return FALSE;
}


BOOL CAppProtectDlg::UnInstallSvc(LPTSTR lpszSvcName)
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hSvc = NULL;
	BOOL rtResult = FALSE;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(NULL == hSCM)
	{
		return FALSE;
	}

	hSvc = OpenService(hSCM, lpszSvcName, SERVICE_ALL_ACCESS);
	if(NULL == hSvc)
	{
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	rtResult = DeleteService(hSvc);

	CloseServiceHandle(hSvc);
	CloseServiceHandle(hSCM);

	return rtResult;
}



BOOL CAppProtectDlg::StartSvc(LPTSTR lpszSvcName)
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hSvc = NULL;
	BOOL rtResult = FALSE;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(NULL == hSCM)
	{
		return FALSE;
	}

	hSvc = OpenService(hSCM, lpszSvcName, SERVICE_ALL_ACCESS);
	if(NULL == hSvc)
	{
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	rtResult = StartService(hSvc, NULL, NULL);
	DWORD dw=GetLastError();
	CloseServiceHandle(hSvc);
	CloseServiceHandle(hSCM);

	if(FALSE == rtResult)
	{
		if(ERROR_SERVICE_ALREADY_RUNNING == GetLastError())
		{
			AfxMessageBox(_T("服务已经运行！"));
			return TRUE;
		}
		else
		{
			CString str;
			str.Format(_T("%d"),GetLastError());
			AfxMessageBox(str);
			return FALSE;
		}
	}
	else
	{
		return TRUE;
	}
}



BOOL CAppProtectDlg::StopSvc(LPTSTR lpszSvcName)
{
	SC_HANDLE hSCM = NULL;
	SC_HANDLE hSvc = NULL;
	BOOL rtResult = FALSE;

	SERVICE_STATUS svcStatus;

	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(NULL == hSCM)
	{
		return FALSE;
	}

	hSvc = OpenService(hSCM, lpszSvcName, SERVICE_ALL_ACCESS);
	if(NULL == hSvc)
	{		
		CloseServiceHandle(hSCM);
		return FALSE;
	}

	rtResult = ControlService(hSvc, SERVICE_CONTROL_STOP, &svcStatus);
	if(rtResult == FALSE)
	{
		//
	}
	CloseServiceHandle(hSvc);
	CloseServiceHandle(hSCM);

	return rtResult;
}

void CAppProtectDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	UninstallDevice();
}

LRESULT CAppProtectDlg::OnHotkey(WPARAM wParam,LPARAM lParam)
{
	
	if(wParam == 1001)
	{
		HWND hwnd = ::FindWindow(NULL,("Protection"));
		if(hwnd && m_hDevice)
		{
			if(m_dwAppState == 0)
			{
				RemoveProtect();
			}
			else if(m_dwAppState == 1)
			{
				OnProtect();
			}
		}
	}
	return FALSE;
}

void CAppProtectDlg::UninstallDevice()
{
	if(m_hDevice)
	{
		CloseHandle(m_hDevice);
	}	

	StopSvc((LPTSTR)szSvcName);
	UnInstallSvc((LPTSTR)szSvcName);
}

void CAppProtectDlg::RemoveProtect()
{
	DWORD dwOutput;
	CHAR inBuffer[10];
	CHAR outBuffer[10];
	memset(inBuffer, 0, 10);
	memset(outBuffer, 0, 10);
	_ultoa(m_dwProcess, inBuffer, 10);
	BOOL bRet=DeviceIoControl(m_hDevice, IO_REMOVE_PROTECT_PROCESS, inBuffer, 10, &outBuffer, 10, &dwOutput, NULL);
	if (bRet)
	{
		bRet=DeviceIoControl(m_hDevice, IO_REMOVE_HIDE_PROCESS, inBuffer, 10, &outBuffer, 10, &dwOutput, NULL);		
		if(bRet)
		{
			ShowWindow(SW_SHOW);
			SetForegroundWindow();
			SetActiveWindow();
			m_dwAppState = 1;
		}
	}
}