// CpuUsage.h: interface for the CCpuUsage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPUUSAGE_H__60CF4F03_9F01_41E8_A9FB_51F065D5F3C2__INCLUDED_)
#define AFX_CPUUSAGE_H__60CF4F03_9F01_41E8_A9FB_51F065D5F3C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <pdh.h>
#include <pdhmsg.h>

#pragma comment(lib,"PDH.lib")


#define MAX_RAW_VALUES          20

const char szCounterName[] = "\\Processor(_Total)\\% Processor Time";

typedef struct _tag_PDHCounterStruct {
    HCOUNTER hCounter;      // Handle to the counter - given to use by PDH Library
    int nNextIndex;         // element to get the next raw value
    int nOldestIndex;       // element containing the oldes raw value
    int nRawCount;          // number of elements containing raw values
    PDH_RAW_COUNTER a_RawValue[MAX_RAW_VALUES]; // Ring buffer to contain raw values
} PDHCOUNTERSTRUCT, *PPDHCOUNTERSTRUCT;


class CCpuUsage  
{
public:
	CCpuUsage();
	virtual ~CCpuUsage();
	BOOL	Init();
	int GetUsage();

protected:

	PPDHCOUNTERSTRUCT			m_pCounterStruct;
	HQUERY						m_hQuery;


};

#endif // !defined(AFX_CPUUSAGE_H__60CF4F03_9F01_41E8_A9FB_51F065D5F3C2__INCLUDED_)
