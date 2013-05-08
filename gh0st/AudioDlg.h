#if !defined(AFX_AUDIODLG_H__996CA19E_64E4_4C8B_9A82_F5DED1FC66AB__INCLUDED_)
#define AFX_AUDIODLG_H__996CA19E_64E4_4C8B_9A82_F5DED1FC66AB__INCLUDED_

#include "Audio.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AudioDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAudioDlg dialog

class CAudioDlg : public CDialog
{
// Construction
public:
	CAudioDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor

	void OnReceiveComplete();
	void OnReceive();
	bool m_bIsWorking;
	CAudio m_Audio;
// Dialog Data
	//{{AFX_DATA(CAudioDlg)
	enum { IDD = IDD_AUDIO };
	BOOL	m_bIsSendLocalAudio;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAudioDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSendLocalaudio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:

	UINT m_nTotalRecvBytes;
	HICON m_hIcon;
	HANDLE	m_hWorkThread;
	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	CString m_IPAddress;

	static	DWORD WINAPI WorkThread(LPVOID lparam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIODLG_H__996CA19E_64E4_4C8B_9A82_F5DED1FC66AB__INCLUDED_)
