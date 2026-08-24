/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/IConfigPaZoneGroup.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IConfigPaZoneGroup - This is the interface supported by objects that allow configuration of
  * PaZoneGroups. It provides methods for retrieving, settings, and writing data for an PaZoneGroup object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigPaZoneGroup_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigPaZoneGroup_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <list>
#include <set>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/pa/src/IPaZoneGroup.h"

namespace TA_IRS_Core
{

    class IConfigPaZoneGroup : public virtual TA_Base_Core::IConfigItem, public virtual IPaZoneGroup
    {

    public:
        
        struct AuditInfo
        {
            ta_uint32 entityKey;    /// the key of client entity
            std::string sessionId;      /// current session ID (usually sourced from RunParams)         
        };

        virtual ~IConfigPaZoneGroup() {};

        virtual void setLocationKey(const ta_uint32 locationKey) =0;
        virtual void setLabel(const std::string& label) =0;
        virtual void setIsEventGroup(const ta_uint32 isEventGroup) =0;


        /**
         * getDateCreated
         *
         * Returns the date created for this PaZoneGroup
         *
         * @return The date created for this PaZoneGroup as a time_t.
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
         * Returns the date modified for this PaZoneGroup.
         *
         * @return The date modified for this PaZoneGroup as a time_t.
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
         * Removes this PaZoneGroup from the database. 
         * The calling application MUST then delete this PaZoneGroup object, as it makes no sense
         * to keep it any longer.
         *
         * @param auditData parameters required for transmission of audit messages
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaZoneGroup was initially loaded from the database
         *      OR     - writePaZoneGroupData() has already been called
         *      This PaZoneGroup has not been deleted
         */
        virtual void deleteThisObject(const AuditInfo& auditData) = 0;

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the PaZoneGroup object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * @param auditData parameters required for transmission of audit messages
         *
         * pre: This PaZoneGroup has not been deleted
         */
        virtual void applyChanges(const AuditInfo& auditData) = 0;

        /**
         * destroyExistingPaZoneGroupHelperRecords
         * 
         * Removes all PA_ZONE_GROUP_HELPER records that are
         * associated with this zone group (function made available
         * to effectively allow modifications of what zones a zone
         * group is defined by - by destroying first then recreating)
         *
         * @exception DataException if there already exists
         *          an association between this group and any of the input zones
         *          (meaning a duplicate Group ID / Zone Key entry exists in database)
         *          or this group object doesn't exist (ie have a key)
         *
         * @exception DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */
        virtual void destroyExistingPaZoneGroupHelperRecords() = 0;

        /**
         * createPaZoneGroupHelperRecords
         *
         * Creates a set of Pa Zone Group Helper entries in the
         *  PA_ZONE_GROUP_HELPER table, to associate this
         *  Pa Zone Group with a Zone
         *
         * @param zoneKeys the keys of all the zones to link to this group
         *
         * @exception DataException if there already exists
         *          an association between this group and any of the input zones
         *          (meaning a duplicate Group ID / Zone Key entry exists in database)
         *          or this group object doesn't exist (ie have a key)
         *
         * @exception DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */        
        virtual void createPaZoneGroupHelperRecords(const std::list< ta_uint32 >& zoneKeys) = 0;

        /**
         * deletePaZoneGroupHelperRecords
         *
         * Deletes a set of Pa Zone Group Helper entries in the
         *  PA_ZONE_GROUP_HELPER table
         *
         * @param zoneKeys the keys of all the zones to unlink from this group
         *
         * @exception DataException This group object doesn't exist (ie have a key)
         *
         * @exception DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         *
         */        
        virtual void deletePaZoneGroupHelperRecords(const std::list< ta_uint32 >& zoneKeys) = 0;

        /**
         * queueAdditionOfPaZoneGroupHelperRecords
         * 
         * Unlike createPaZoneGroupHelperRecords, which applies the changes to the database immediately
         *  this function buffers the changes, and they're applied in the function applyChanges
         *  - additionally, hasChanged function will account for differences in the buffer
         * 
         */
        virtual void queueAdditionOfPaZoneGroupHelperRecords(const std::list< ta_uint32 >& zoneKeys) = 0;

        /**
         * queueDeletionOfPaZoneGroupHelperRecords
         * 
         * Unlike deletePaZoneGroupHelperRecords, which applies the changes to the database immediately
         *  this function buffers the changes, and they're applied in the function applyChanges
         *  - additionally, hasChanged function will account for differences in the buffer
         * 
         */
        virtual void queueDeletionOfPaZoneGroupHelperRecords(const std::list< ta_uint32 >& zoneKeys) = 0;

        /**
         * getQueuedAssociatedPaZoneKeys
         *
         * @return an array of keys indexing the PA Zones associated with this group
         *      which factors in calls made to :
         *      queueAdditionOfPaZoneGroupHelperRecords and
         *      queueDeletionOfPaZoneGroupHelperRecords
         */
        virtual const std::set< ta_uint32 > getQueuedAssociatedPaZoneKeys() = 0;

        /**
         * setRecreateOnDeletionFlag
         *
         * Sets a special internal flag that tells this configurable zone group
         *  to recreate itself in the database if it has been deleted, upon a call to applyChanges
         *
         * @param recreateModified - if the zone already exists (isNew() is false), and
         *          there is an attempt to update the existing record, but it is not found
         *          (because an external source has deleted it), then we automatically
         *          regenerate it using our key value
         */
        virtual void setRecreateOnDeletionFlag(bool recreateModified) = 0;   
        
        /**
         * setAuditData (deprecated)
         *
         * Added for the config editor - because cannot pass parameters to apply
         *      changes function, must first set the audit information
         *      up using this function before calling applyChanges
         *
         * @param auditData the audit information to use when sending audit messages
         *
         */
        virtual void setAuditData(const AuditInfo& auditData) = 0;
    };
} //close namespace TA_IRS_Core
#endif // !defined(IConfigPaZoneGroup_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
