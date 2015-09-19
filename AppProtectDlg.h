// AppProtectDlg.h : header file
//

#if !defined(AFX_APPPROTECTDLG_H__A78516B5_BF83_4BF1_90EA_3450CB51CE28__INCLUDED_)
#define AFX_APPPROTECTDLG_H__A78516B5_BF83_4BF1_90EA_3450CB51CE28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAppProtectDlg dialog

class CAppProtectDlg : public CDialog
{
// Construction
public:
	CAppProtectDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAppProtectDlg)
	enum { IDD = IDD_APPPROTECT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppProtectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	HANDLE	m_hDevice;//设备句柄
	DWORD	m_dwAppState;//隐藏窗口
	DWORD   m_dwProcess;//自身进场ID
public:	
	BOOL InstallSvc(LPTSTR lpszSvcName, LPTSTR lpszDisplayName, LPTSTR lpszSvcBinaryPath, DWORD dwSvcType, DWORD dwStartType);
	BOOL UnInstallSvc(LPTSTR lpszSvcName);
	BOOL StartSvc(LPTSTR lpszSvcName);
	BOOL StopSvc(LPTSTR lpszSvcName);
	void UninstallDevice();
	void RemoveProtect();
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAppProtectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnProtect();
	afx_msg void OnDestroy();
	LRESULT OnHotkey(WPARAM wParam,LPARAM lParam); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPPROTECTDLG_H__A78516B5_BF83_4BF1_90EA_3450CB51CE28__INCLUDED_)
