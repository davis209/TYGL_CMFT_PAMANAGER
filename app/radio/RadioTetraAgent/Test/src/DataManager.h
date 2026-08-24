/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/DataManager.h $
  * @author: Kent Yip
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "RadioCircuitModeManager.h"

class DataManager
{
public:
    static DataManager& getDataManager();
    void setLastExpectedPacket( const std::string callID, const Data& data );
    void setLastReceivedPacket( const std::string callID, const Data& data );
    bool didPacketsMatch( const std::string callID );
    TA_IRS_App::References getCallReference( const std::string callID );
private:
    DataManager();
    ~DataManager();
    static DataManager* dataManager;

    std::map<const std::string, Data>                   m_lastReceived;
    std::map<const std::string, Data>                   m_lastExpected;
    std::map<const std::string, TA_IRS_App::References*> m_callMap;
    int                                                 m_callCount;
};

#endif // DATAMANAGER_H