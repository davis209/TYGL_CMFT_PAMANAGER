/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/GetGroupDetailsEventReader.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API GetGroupDetails event.
  *
  */
///////////////////////////////////////////////////////////
//  GetGroupDetailsEventReader.cpp
//  Implementation of the Class GetGroupDetailsEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "GetGroupDetailsEventReader.h"
#include "RadioEvent.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    const unsigned int GetGroupDetailsEventReader::REFERENCE_PARAMETER_POSITION  =   0;
    const unsigned int GetGroupDetailsEventReader::RESULT_TABLE_START_POSITION   =   4;
    const unsigned int GetGroupDetailsEventReader::RESULT_TABLE_STRING_POSITION   =   8;

    static void LogRow(const GroupDetailsRow & row, int rowNum);
    static void LogTable(const GroupDetailsTable & table);

    GetGroupDetailsEventReader::GetGroupDetailsEventReader(RadioEvent& event) : 
    RadioEventReader(event,EVENT_GET_GROUP_DETAILS)
    {
    }



    GetGroupDetailsEventReader::~GetGroupDetailsEventReader()
    {
    }


    const ta_uint32 GetGroupDetailsEventReader::getReference() const
    {
        return getParameterLong(REFERENCE_PARAMETER_POSITION);
    }

    int GetGroupDetailsEventReader::getDetailsTable(GroupDetailsTable & table) const
    {
        // Get the length of the string
        ta_uint32 stringLength = getParameterLong(RESULT_TABLE_START_POSITION);
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
             "getDetailsTable() - stringLength = %lu", stringLength);

        GroupDetailsRow row;
        std::string element("");

        // The outer loop goes until the end of the string
        //std::string::iterator iter;
        //for (iter = resultString.begin();iter != resultString.end();iter++)

		// RvH 20050115 Radio-Pre-iFAT
		// The first row is the column labels
        // RvH 20050128 Pre-Radio-iFAT
        // The first row of EVERY PACKET that has arrived is the column labels.
        // We decide what is a data row based on the first field
        int rowNum = 0;
		// The first column is the Subscriber Textual Refernce
		// (K_TEXT_REFERENCE) which we choose to ignore. HOWEVER due to an
        // interesting off-by-one collusion between this class and
        // RadioMethodGetGroupDetails, this should not be removed from the table
        int colNum = 0;
        unsigned int maxRowSize=0;
        unsigned int minRowSize=0xFFFFFFFF;
        for (ta_uint32 charPos = 0;charPos < stringLength;charPos++)
        {
			unsigned char currentChar = getParameterChar(RESULT_TABLE_STRING_POSITION + charPos);

            if ( (currentChar == 0x0D) || (currentChar > 0x7F) || (currentChar == '\0') )
            {
                // Dump these.
                // The first only seems to appear with 0x0A, so is safe to dump.
                // The second is obviously incorrect
                // The last is unnecessary
				continue;
            }
            else if (currentChar == 0x0A)
            {
                // the end of the line. Add the element, but only if it isn't empty
                // RvH 20050129 Pre-Radio-iFAT add the element even if its empty
                // BUT MAYBE NOT
                if (!element.empty())
                {
                    row.push_back(element);
					element = "";
                }

                // RvH 20050128 Pre-Radio-iFAT
                // The first row of EVERY PACKET that has arrived is the column labels
                if ( (!row.empty()) && (row[0]!="K_TEXT_REFERENCE") )
                {
                    table.push_back(row);
                }
                //
                // Track rowsize minima and maxima
                if ( row.size() > maxRowSize )
                {
                    maxRowSize = row.size();
                }
                if ( row.size() < minRowSize )
                {
                    minRowSize = row.size();
                }

                LogRow(row, rowNum);

                row.clear();

                rowNum++;
                colNum = 0;
            }
            else if (currentChar == 0x01)
            {
				// RvH 20050128 Pre-Radio-iFAT
				// need to insert blank fields
				row.push_back(element);
                element = "";
                colNum++;
            }
            else
            {
                // Character
                element += currentChar;
            }
        } // for loop 

		// Ensure final incomplete row added
        // if there is a final element, add it
		if (element.size()>0)
		{
           row.push_back(element);
		}

		if (row.size()>0)
		{
		   table.push_back(row);
		}

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
             "getDetailsTable() -  table.size()=%lu minRowSize=%lu maxRowSize=%lu)",
             table.size(), minRowSize, maxRowSize);

        return table.size();
    }

    void LogTable(const GroupDetailsTable & table)
    {
        int currentRowNum = 0;
        for ( GroupDetailsTable::const_iterator titer = table.begin() ;
                titer != table.end() ;
                titer++,currentRowNum++ )
        {
            const GroupDetailsRow & currentRow = *titer;
            LogRow(currentRow, currentRowNum);
        }
    }


    void LogRow(const GroupDetailsRow & row, int rowNum)
    {
        char elementstr[64] = {0};
        int colNum = 0;
        std::string rowText = "";
        for ( GroupDetailsRow::const_iterator riter = row.begin() ;
                riter != row.end() ;
                riter++,colNum++ )
        {
            const std::string & currentText = (*riter);
            sprintf(elementstr, "[R%d,C%d]=\"%s\" ", rowNum, colNum, currentText.c_str());
            rowText.append(elementstr);
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
             "%s",rowText.c_str());
    }
} // namespace TA_IRS_App

