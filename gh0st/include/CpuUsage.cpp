// CpuUsage.cpp: implementation of the CCpuUsage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CpuUsage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCpuUsage::CCpuUsage()
{
	m_hQuery = NULL;
	m_pCounterStruct = NULL;

}

CCpuUsage::~CCpuUsage()
{

	PdhCloseQuery(m_hQuery);
	delete m_pCounterStruct;
}


BOOL CCpuUsage::Init()
{
	if (ERROR_SUCCESS != PdhOpenQuery(NULL, 1, &m_hQuery))
		return FALSE;

    m_pCounterStruct = (PPDHCOUNTERSTRUCT) new PDHCOUNTERSTRUCT;

	PDH_STATUS pdh_status = PdhAddCounter(m_hQuery, szCounterName, (DWORD) m_pCounterStruct, &(m_pCounterStruct->hCounter));
    if (ERROR_SUCCESS != pdh_status) 
	{
		return FALSE;
	}



	return TRUE;
}


int CCpuUsage::GetUsage()
{
    PDH_FMT_COUNTERVALUE pdhFormattedValue;

	PdhCollectQueryData(m_hQuery);

    if (ERROR_SUCCESS != PdhGetFormattedCounterValue( 
                                    m_pCounterStruct->hCounter,
                                    PDH_FMT_LONG,
                                    NULL,
                                    &pdhFormattedValue )) 


	{
		return 0;
	}

	return pdhFormattedValue.longValue;
}