/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/DataManager.cpp $
  * @author: Kent Yip
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */

#include "DataManager.h"

DataManager* DataManager::dataManager = NULL;

DataManager& DataManager::getDataManager()
{
    if(NULL == dataManager)
    {
        dataManager = new DataManager();
    }
    return *dataManager;
}

DataManager::DataManager()
    : m_lastReceived()
    , m_lastExpected()
    , m_callMap()
    , m_callCount( 0 )
{
}

DataManager::~DataManager()
{
    for( std::map<const std::string, TA_IRS_App::References*>::iterator it = this->m_callMap.begin();
         it != this->m_callMap.end();
         it++ )
    {
        delete it->second;
    }
}

TA_IRS_App::References DataManager::getCallReference( const std::string callID )
{
    for( std::map<const std::string, TA_IRS_App::References*>::iterator it = this->m_callMap.begin();
         it != this->m_callMap.end();
         it++ )
    {
        if( it->first == callID )
            return *(it->second);
    }
    
    int callRef = this->m_callCount++;
    this->m_callMap[ callID ] = new TA_IRS_App::References( callRef, callRef );
    return *(this->m_callMap[ callID ]);
}

void DataManager::setLastExpectedPacket( const std::string callID, const Data& data )
{
    this->m_lastExpected[callID] = data;
}

void DataManager::setLastReceivedPacket( const std::string callID, const Data& data )
{
    this->m_lastReceived[callID] = data;
}

bool DataManager::didPacketsMatch( const std::string callID )
{
    if( this->m_lastExpected[callID].size() != this->m_lastReceived[callID].size() )
        return false;

    for( unsigned int i = 0; i < this->m_lastExpected[callID].size(); i++ )
    {
        unsigned char char1 = this->m_lastExpected[callID][i];
        unsigned char char2 = this->m_lastReceived[callID][i];
        if( char1 != char2 )
            return false;
    }

    return true;
}