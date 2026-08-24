/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/IConfigPaDvaMessage.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IConfigPaDvaMessage - This is the interface supported by objects that allow configuration of
  * PaDvaMessages. It provides methods for retrieving, settings, and writing data for an PaDvaMessage object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigPaDvaMessage_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigPaDvaMessage_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/pa/src/IPaDvaMessage.h"

namespace TA_IRS_Core
{

    class IConfigPaDvaMessage : public virtual TA_Base_Core::IConfigItem, public virtual IPaDvaMessage
    {

    public:

        virtual ~IConfigPaDvaMessage() {};

        virtual void setLocationKey(const ta_uint32 locationKey) =0;
        virtual void setId(const ta_uint32 id) =0;
        virtual void setLabel(const std::string& label) =0;
        virtual void setType(const std::string& type) =0;
        virtual void setTisMessageTag(const ta_uint32 tisMessageTag) = 0;
        virtual void setTisLibrarySection(const ta_uint32 tisLibrarySection) = 0;
        virtual void setTisLibraryVersion(const ta_uint32 version) =0;


        /**
         * getDateCreated
         *
         * Returns the date created for this PaDvaMessage
         *
         * @return The date created for this PaDvaMessage as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this PaDvaMessage.
         *
         * @return The date modified for this PaDvaMessage as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;

        
        
        /**
         * deleteThisObject
         *
         * Removes this PaDvaMessage from the database. 
         * The calling application MUST then delete this PaDvaMessage object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaDvaMessage was initially loaded from the database
         *      OR     - writePaDvaMessageData() has already been called
         *      This PaDvaMessage has not been deleted
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

    };
} //close namespace TA_IRS_Core
#endif // !defined(IConfigPaDvaMessage_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
