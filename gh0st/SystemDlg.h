#if !defined(AFX_SystemDlg_H__7A784A33_3CF5_4998_B9A1_1E1C11EF8EB2__INCLUDED_)
#define AFX_SystemDlg_H__7A784A33_3CF5_4998_B9A1_1E1C11EF8EB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SystemDlg.h : header file
//

#define CListCtrl	CCJListCtrl
/////////////////////////////////////////////////////////////////////////////
// CSystemDlg dialog


class CSystemDlg : public CDialog
{
// Construction
public:
	CSystemDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor
	void OnReceiveComplete();
// Dialog Data
	//{{AFX_DATA(CSystemDlg)
	enum { IDD = IDD_SYSTEM };
	CListCtrl	m_list_dialupass;
	CListCtrl	m_list_windows;
	CListCtrl	m_list_process;
	CTabCtrl	m_tab;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystemDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
private:
	void AdjustList();
	void ShowProcessList();
	void ShowWindowsList();
	void ShowDialupassList();
	void ShowSelectWindow();
	void GetProcessList();
	void GetWindowsList();
	void GetDialupassList();
	HICON m_hIcon;
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	// Generated message map functions
	//{{AFX_MSG(CSystemDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillprocess();
	afx_msg void OnRefreshPsList();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SystemDlg_H__7A784A33_3CF5_4998_B9A1_1E1C11EF8EB2__INCLUDED_)
