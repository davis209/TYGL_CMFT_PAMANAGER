/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/OnlinePrintCfg.h $
 * @author Nick Jardine
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by: $Author: CM $
 *
 * OnlinePrintCfg is an implementation of IOnlinePrintCfg. It holds the data specific to an OnlinePrintCfg entry
 * in the database, and allows read-only access to that data.
 */

#if !defined(OnlinePrintCfg_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define OnlinePrintCfg_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/src/OnlinePrintCfgHelper.h"
#include "core/data_access_interface/src/IOnlinePrintCfg.h"

namespace TA_Base_Core
{
    class OnlinePrintCfg : public IOnlinePrintCfg
    {
    public:

        /**
         * OnlinePrintCfg (constructor)
         *
         * Construct an OnlinePrintCfg class based around a key.
         *
         * @param key The key of this OnlinePrintCfg in the database
         */
        OnlinePrintCfg(const ta_uint32 idKey);

        /**
         * OnlinePrintCfg (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see OnlinePrintCfgHelper::OnlinePrintCfgHelper(IData*)
         *
         */
        OnlinePrintCfg(ta_uint32 row, TA_Base_Core::IData& data);

        virtual ~OnlinePrintCfg();

        virtual ta_uint32 getKey();

        virtual std::string getDefaultPrinter();
        virtual ta_uint32 getLocation();
        virtual std::string getLocationName();
        virtual std::string getLocationDisplayName();
        virtual void invalidate();

        /**
         * getName
         *
         * Returns the name of this item. For alarm plan associations this is a name built up from the
         * entity or entity type and the alarm type.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

    private:

        // Assignment OnlinePrintCfg not used so it is made private
        OnlinePrintCfg& operator=(const OnlinePrintCfg&);

        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the OnlinePrintCfgHelper
        // has only been written for ConfigLocation objects and will not copy the OnlinePrintCfg helper
        // for a read-only object (ie this one).
        OnlinePrintCfg(const OnlinePrintCfg& theOnlinePrintCfg);

        OnlinePrintCfgHelper* m_OnlinePrintCfgHelper;
    };
} // closes TA_Core

#endif // !defined(OnlinePrintCfg_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
