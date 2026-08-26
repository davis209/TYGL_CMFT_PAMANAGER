/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IConfigOnlinePrinter.h $
 * @author Nick Jardine
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by: $Author: CM $
 *
 * IConfigOnlinePrinter - This is the interface supported by objects that allow configuration of
 * OnlinePrinters. It provides methods for retrieving, settings, and writing data for an OnlinePrinter object.
 * The set methods in this interface do not set the values in the database
 * straight away. They only set the values in the object after validating that
 * they are correct.
 */

#if !defined(IConfigOnlinePrinter_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigOnlinePrinter_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/src/IOnlinePrinter.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_Base_Core
{
    class IConfigOnlinePrinter : public virtual IConfigItem, public virtual IOnlinePrinter
    {
    public:

        virtual ~IConfigOnlinePrinter() {};

        virtual void setPrinter(const std::string& pPrinter) = 0;
        virtual void setIsAlarm(bool pIsAlarm) = 0;
        virtual void setSubsystem(ta_uint32 pSubsystem) = 0;
        virtual void setLocation(ta_uint32 pLocation) = 0;

        /**
         * deleteThisObject
         *
         * Remove this OnlinePrinter from the database. Once this method has been executed, the
         * OnlinePrinterHelper object should be destroyed.
         *
         * @exception DatabaseException
         *            - if there is a problem writing the data to the database.
         *            - if references to this record exists in AT_FIXED_BLOCK_MAP table.
         *              These can be removed by specifying the cascade option.
         *
         * @param cascade
         *        - force the removal of this record by removing all references to this record
         *          in other tables (AT_FIXED_BLOCK_MAP).  This should be set to true to prevent the
         *          DatabaseException from being thrown.
         *
         * pre: - this OnlinePrinter was initially loaded from the database
         *      - writeOnlinePrinterData() has already been called
         */
        virtual void deleteThisObject(bool cascade = false) = 0;
    };

    typedef std::vector<IConfigOnlinePrinter*> IConfigOnlinePrinters;
    typedef IConfigOnlinePrinters::iterator    IConfigOnlinePrintersIt;
} //close namespace TA_IRS_Core
#endif // !defined(IConfigOnlinePrinter_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
