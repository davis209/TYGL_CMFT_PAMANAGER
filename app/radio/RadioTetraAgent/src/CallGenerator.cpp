/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/CallGenerator.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// allGenerator.cpp: implementation of the CallGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include <time.h>
#include "CallGenerator.h"
#include "RadioTypes.h"
#include "Radio.h"

using namespace TA_IRS_App;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CallGenerator::CallGenerator(Radio& radio) : m_radio(radio)
{
   m_bGenerate = true;
}

CallGenerator::~CallGenerator()
{
	m_bGenerate = false;
}

// Do the work
void CallGenerator::run()
{
	CallDetailsType callDetails;

	while (m_bGenerate)
	{
		// Create a call
		CreateNewCall(callDetails);

		// Add to radio stack
		m_radio.addCallToCallStack(callDetails);

		// Rest
		sleep(5000);
	}

	m_bGenerate = true;
}

// Stop the work
void CallGenerator::terminate()
{
	m_bGenerate = false;
}



// Local function to return a random number >=0 and <iMax 
static unsigned int Random(int iMax)
{
    static bool bSeeded = false;

	int i;

   /* Seed the random-number generator with current time so that
    * the numbers will be different every time we run.
    */
	if (!bSeeded)
	{
       srand( (unsigned)time( NULL ) );
	   bSeeded = true;
	}

   /* Display 10 numbers. */
   i = rand();
   i = i%iMax;
   return i;
}



static std::string getCurrentTime()
{
	char      szBuffer[100];

	time_t    ltime;
	struct tm *today;

	time(&ltime);
	today = localtime(&ltime);

	strftime(szBuffer,sizeof(szBuffer),"%d-%b %H:%M:%S",today);

	return std::string(szBuffer);
}


bool CallGenerator::CreateNewCall(CallDetailsType& callDetails)
{
	char  szTemp[100];

	const char MyTSI[]  = "1234-1-123";
	const char MyName[] = "OCR TCO";

    const char SomeNames[][32] = {"OCR SCO","OCR PSCO","OCR CS","OCR TCO","OCR MCO",
		                          "Mobile Fred_Smith","Mobile Loco_driver","Mobile Bill_Jones",
		                          "DBG SO","MSM SO","CVC SO","MLN SO","NCH SO","BLV SO",
								  "OAR SO","TKT SO","PYL SO","MPS SO","UPL SO","KCD SO",
								  "DSR DCO","DCR PSCO","DSR TCO"};

    const int  MaxSomeNames = sizeof(SomeNames)/sizeof(SomeNames[0]);
    bool  bOther = true;

	// Setup the reference number for the call
    callDetails.callID = Random(10000);
	callDetails.callStatus = Ringing;

	// Set the time for the call
    callDetails.callTime = getCurrentTime().c_str();

	// Make up the source name and numbers
	sprintf(szTemp,"%d-%d-%d",Random(10000),Random(10),Random(1000));
	callDetails.callerTSI = szTemp; 

	sprintf(szTemp,"%s",SomeNames[Random(MaxSomeNames)]);
	callDetails.callerLocation = szTemp;

	// Set up the destination name and numbers
	if (Random(2)==1)
	{
		// One in two calls to us
        //TRACE0("New incoming call created for us\n");
		bOther = false;
	    sprintf(szTemp,"%s",MyTSI);
	    callDetails.calledTSI = szTemp;
		sprintf(szTemp,"%s",MyName);
		callDetails.calledLocation = szTemp;
	}
	else
	{
        //TRACE0("New incoming call created for another operator\n");
	    sprintf(szTemp,"%d-%d-%d",Random(10000),Random(10),Random(1000));
	    callDetails.calledTSI = szTemp;
		sprintf(szTemp,"%s",SomeNames[Random(MaxSomeNames)]);
		callDetails.calledLocation = szTemp;
	}


	// Split the caller name and location
    char* pStr = strstr(callDetails.callerLocation.c_str()," ");
	if (pStr!=NULL)
	{
		callDetails.callerName = pStr+1;
		*pStr = '\0';
    }
    pStr = strstr(callDetails.calledLocation.c_str()," ");
	if (pStr!=NULL)
	{
		callDetails.calledName = pStr+1;
		*pStr = '\0';
    }

	// Determine the call type
	if (Random(6)==1)
	{
        // Call the driver
        //TRACE0("New incoming call converted to train call\n");
		callDetails.callCategory = CallDriver;
	    sprintf(szTemp,"Train%d",Random(100));
		callDetails.callerName = szTemp;
	}
	else
	{
		if (Random(6)==1)
		{
			// SDS Message 
		    callDetails.callCategory = TextMessage;
			callDetails.callText = "Some test text for a message!";
		}
		else if (Random(6)==1)
		{
			// Must be a half duplex voice call
		    callDetails.callCategory = HalfDuplex;
		}
		else
		{
			// Must be a full duplex voice call
		    callDetails.callCategory = FullDuplex;
		}
	}

	callDetails.callSource = IncomingCall;
	callDetails.sessionRef = Random(6);
	//TD 10234
    callDetails.isInvisibleToGUIApps = false;

	return bOther;
}
