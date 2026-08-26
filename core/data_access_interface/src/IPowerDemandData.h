/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IPowerDemandData.h $
 * @author:  Wenching
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 *
 */

#if !defined(IPowerDemandData_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IPowerDemandData_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

namespace TA_Base_Core
{
    class IPowerDemandData
    {
    public:

        virtual ~IPowerDemandData() {};

        /**
         * getMeterName
         *
         * Returns the name of the power demand data
         *
         * @return The name for this entity as string.
         */
        virtual std::string getMeterName() const = 0;

        /**
         * getInterval
         *
         * Returns the 30 minute period of the power demand
         *
         * @return The interval as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. PKEY should be an
         *             ta_uint32 ), or if the wrong amount of data is retrieved.
         */
        virtual time_t getInterval() const = 0;

        /**
         * getRecordedValue
         *
         * Returns the recorded value of power demand
         *
         * @return The recorded value of the entity as a ta_uint32 .
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. PKEY should be an
         *             ta_uint32 ), or if the wrong amount of data is retrieved.
         */
        virtual ta_uint32 getRecordedValue() const = 0;

        /**
         * getUpdatedValue
         *
         * Returns the updated value of power demand
         *
         * @return The updated value of the entity as a ta_uint32 .
         *
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. PKEY should be an
         *             ta_uint32 ), or if the wrong amount of data is retrieved.
         */
        virtual ta_uint32 getUpdatedValue() const = 0;

        /**
         * setRecordedValue
         *
         * Set the recorded value of power demand
         *
         * @
         *
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. PKEY should be an
         *             ta_uint32 ), or if the wrong amount of data is retrieved.
         */
        virtual void setRecordedValue(ta_uint32 recordedValue) = 0;

        /**
         * setUpdatedValue
         *
         * Set the updated value of power demand
         *
         * @
         *
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. PKEY should be an
         *             ta_uint32 ), or if the wrong amount of data is retrieved.
         */
        virtual void setUpdatedValue(ta_uint32 updatedValue) = 0;
    };
} //close namespace TA_Base_Core

#endif // !defined(IPowerDemandEditor_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
