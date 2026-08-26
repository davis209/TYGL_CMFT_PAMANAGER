/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/Operator.h $
 * @author Nick Jardine
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by: $Author: CM $
 *
 * Operator is an implementation of IOperator. It holds the data specific to an operator entry
 * in the database, and allows read-only access to that data.
 */

#if !defined(Operator_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define Operator_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/src/OperatorHelper.h"
#include "core/data_access_interface/src/IOperator.h"

namespace TA_Base_Core
{
    class Operator : public IOperator
    {
    public:

        /**
         * Operator (constructor)
         *
         * Construct an Operator class based around a key.
         *
         * @param key The key of this Operator in the database
         */
        Operator(const ta_uint32 key);

        /**
         * Operator
         *
         * Construct an Operator class based around given parameters.
         *
         * @param key The key into the OPERATOR table for this Operator
         * @param name The name into the OPERATOR table for this Operator
         * @param description The description into the OPERATOR table for this Operator
         * @param password The password into the OPERATOR table for this Operator
         * @param isSystemOperator The is_system into the OPERATOR table for this Operator
         * @param dateCreated The date_created into the OPERATOR table for this Operator
         * @param dateModified The date_modified into the OPERATOR table for this Operator
         * @param loadProfiles By default the associated profiles are not loaded, but if
         *                     this is set to true, then the associated profiles will be loaded
         * @param loadRegions  By default the associated regions are not loaded, but if
         *                       this is set to true, then the associated regions will be loaded
         */
        Operator(const ta_uint32 key,
                 const std::string& name,
                 const std::string& description,
                 const std::string& password,
                 bool isSystemOperator,
                 time_t dateCreated,
                 time_t dateModified,
                 bool loadProfiles = false,
                 bool loadRegions = false);

        /**
         * Operator
         *
         * Construct an Operator class based around given parameters. it is used for displaying only.
         */

        Operator(const ta_uint32 key,
                 const std::string& name, bool isSystemOperator);

        /**
         * Constructor
         *
         * Construct a Operator class based around dataset
         */
        Operator(const ta_uint32 row, TA_Base_Core::IData& data, std::vector<ta_uint32>& profiles, std::vector<ta_uint32>& regions);

        virtual ~Operator();

    public:

        /**
         * getKey
         *
         * Returns the key for this Operator.
         *
         * @return The key for this operator as an ta_uint32 .
         */
        ta_uint32 getKey();

        /**
         * getName
         *
         * Returns the name of this Operator. If this is the first time data for this operator
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this operator as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

        /**
         * getDescription
         *
         * Returns the description for this Operator.
         *
         * @return The description for this Operator as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDescription();

        /**
         * getPassword
         *
         * Returns the password for this Operator
         *
         * @return The password for the Operator as a string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getPassword(const std::string& options);

        /**
         * setPassword
         *
         * Sets the password for this Operator. This updates the database straight away.
         *
         * @param password The password to give this Operator.
         *
         * pre: This operator has not been deleted
         */
        virtual void setPassword(const std::string password);

        /**
         * isSystemOperator
         *
         * This indicates if this operator is a system operator or not
         *
         * @return bool - True if this operator is a system operator, false otherwise
         */
        virtual bool isSystemOperator();

        /**
         * getAssociatedProfiles
         *
         * Returns the profiles that this operator can use
         *
         * @return The profiles accessible by this operator, as a vector of IProfile*'s
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::vector<IProfile*> getAssociatedProfiles();

        /**
         * getAssociatedProfileKeys
         *
         * Returns the profile keys that are accessible by this operator
         *
         * @return The profiles accessible by this operator, as a vector of keys
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::vector<ta_uint32> getAssociatedProfileKeys();

        /**
         * getAssociatedRegionKeys
         *
         * Returns the regions that this operator can use
         *
         * @return The regions accessible by this operator, as a vector of pkeys.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::vector<ta_uint32> getAssociatedRegionKeys();

        /**
         * invalidate
         *
         * Make the data contained by this operator as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:

        // Assignment operator not used so it is made private
        Operator& operator=(const Operator&);

        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the OperatorHelper
        // has only been written for ConfigLocation objects and will not copy the operator helper
        // for a read-only object (ie this one).
        Operator(const Operator& theOperator);

        TA_Base_Core::OperatorHelper* m_OperatorHelper;
    };
} // closes TA_Base_Core

#endif // !defined(Operator_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
