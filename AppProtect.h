// AppProtect.h : main header file for the APPPROTECT application
//

#if !defined(AFX_APPPROTECT_H__71125694_1FE8_417D_A4F6_6F7AB92F72E1__INCLUDED_)
#define AFX_APPPROTECT_H__71125694_1FE8_417D_A4F6_6F7AB92F72E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAppProtectApp:
// See AppProtect.cpp for the implementation of this class
//

class CAppProtectApp : public CWinApp
{
public:
	CAppProtectApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppProtectApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAppProtectApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


static CString GetExeDir()
{
	CString strPath;
	GetModuleFileName(AfxGetInstanceHandle(), strPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	strPath.ReleaseBuffer();
	int idx = strPath.ReverseFind('\\');
	strPath = strPath.Left(idx);
	return strPath;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPPROTECT_H__71125694_1FE8_417D_A4F6_6F7AB92F72E1__INCLUDED_)
