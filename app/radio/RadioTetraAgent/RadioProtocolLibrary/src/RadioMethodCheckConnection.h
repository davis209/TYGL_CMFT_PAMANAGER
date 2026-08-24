// RadioMethodCheckConnection.h: interface for the RadioMethodCheckConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIOMETHODCHECKCONNECTION_H__C8DFE98C_9841_434E_9ECE_50D496AEF08D__INCLUDED_)
#define AFX_RADIOMETHODCHECKCONNECTION_H__C8DFE98C_9841_434E_9ECE_50D496AEF08D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/types/src/ta_types.h"
#include "RadioMethod.h"

class RadioMethodCheckConnection : public TA_IRS_App::RadioMethod
{
public:
	RadioMethodCheckConnection( bool enabled = true );
	virtual ~RadioMethodCheckConnection();

	void enableCheckConnection(bool enable);
	void setCheckInterval(ta_uint32 interval);
};

#endif // !defined(AFX_RADIOMETHODCHECKCONNECTION_H__C8DFE98C_9841_434E_9ECE_50D496AEF08D__INCLUDED_)
