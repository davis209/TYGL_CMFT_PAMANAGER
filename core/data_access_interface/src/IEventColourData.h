/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IEventColourData.h $
 * @author:  Ripple
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * IEventColourData is an interface to a Event Colour object. It allows the event colour object implementation
 * to be changed without changing the code that uses this interface.
 */

// IEventColourData.h: interface for the IEventColourData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IEVENTCOLOURDATA_H__30F9C74A_B90E_48FB_B8EE_FA134E586A1C__INCLUDED_)
#define AFX_IEVENTCOLOURDATA_H__30F9C74A_B90E_48FB_B8EE_FA134E586A1C__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "core/types/src/ta_types.h"
#include <string>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IEventColourData : public IItem
    {
    public:

        enum EEventColourType
        {
            OPEN_ACKED_FG1,
            OPEN_ACKED_BG1,
            OPEN_UNACKED_FG1,
            OPEN_UNACKED_BG1,
            CLOSED_UNACKED_FG1,
            CLOSED_UNACKED_BG1,
            CLOSED_ACKED_FG1,
            CLOSED_ACKED_BG1,
        };

        /**
         * getEventColourKey
         *
         * Returns the severity RGB level of this event type.
         *
         * @param EEventColourType - Which colour to retrieve. This defaults to OPEN_ACKED which was the
         *                      behaviour when only one event colour was available.
         *
         * @return The severity RGB level of the event type as an unsigned int.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. persist_message should be
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        virtual ta_uint32 getEventColourKey(EEventColourType type = OPEN_ACKED_FG1) = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this event as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() = 0;

        /**
         * getDateCreated
         *
         * Returns the date created for this event colour type.
         *
         * @return The date created for this event colour type as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual time_t getDateCreated() = 0;

        /**
         * getDateModified
         *
         * Returns the date modified for this event colour type.
         *
         * @return The date modified for this event colour type as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual time_t getDateModified() = 0;

        virtual ~IEventColourData() {};
    };

} // close namespace
#endif // !defined(AFX_IEVENTCOLOURDATA_H__30F9C74A_B90E_48FB_B8EE_FA134E586A1C__INCLUDED_)
