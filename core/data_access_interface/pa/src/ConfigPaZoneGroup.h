/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/ConfigPaZoneGroup.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPaZoneGroup is a concrete implementation of IConfigPaZoneGroup, which is in turn an implementation
  * of IPaZoneGroup. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaZoneGroups.
  */


#if !defined(ConfigPaZoneGroup_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigPaZoneGroup_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <set>

#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/pa/src/IConfigPaZoneGroup.h"

namespace TA_Base_Core
{
    class AuditMessageSender;
    class IData;
}

namespace TA_IRS_Core
{
    class PaZoneGroupHelper;


    class ConfigPaZoneGroup : public IConfigPaZoneGroup
    {

    public:

        /**
         * ConfigPaZoneGroup (constructor)
         *
         * Constructs a new instance of the ConfigPaZoneGroup with no id. This is used when
         * creating a *new* PaZoneGroup - that is, one that does not yet exist in the database.
         */
        ConfigPaZoneGroup();

        /**
         * ConfigPaZoneGroup (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaZoneGroupHelper::PaZoneGroupHelper(IData*)
         *
         */
        ConfigPaZoneGroup(ta_uint32 row, TA_Base_Core::IData& data);


        /**
         * ConfigPaZoneGroup (constructor)
         *
         * Constructs a new instance of ConfigPaZoneGroup with the specified id. This is used 
         * when creating a ConfigPaZoneGroup based around an existing PaZoneGroup in the database.
         *
         * @param key The key to this PaZoneGroup in the database.
         */
        ConfigPaZoneGroup(const ta_uint32 idKey);


        /**
         * ConfigPaZoneGroup (copy constructor)
         *
         * @param ConfigPaZoneGroup& - The PaZoneGroup to make a copy of.
         */
        ConfigPaZoneGroup( const ConfigPaZoneGroup& theConfigPaZoneGroup);

        
        virtual ~ConfigPaZoneGroup();


    public:

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
        void deleteThisObject(const AuditInfo& auditData);

        // Default implementations not used..
        void applyChanges();
        
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
        void applyChanges(const AuditInfo& auditData);

        /**
         * setRecreateOnDeletionFlag (interface implementation)
         *
         * @see IConfigPaZoneGroup::setRecreateOnDeletionFlag for more details
         */
        virtual void setRecreateOnDeletionFlag(bool recreateModified) { m_recreateModified.setValue(recreateModified); }

        /**
         * destroyExistingPaZoneGroupHelperRecords (interface implementation)
         *
         * @see IConfigPaZoneGroup::destroyExistingPaZoneGroupHelperRecords for more details
         */     
        void destroyExistingPaZoneGroupHelperRecords();

        /**
         * createPaZoneGroupHelperRecords (interface implementation)
         *
         * @see IConfigPaZoneGroup::createPaZoneGroupHelperRecords for more details
         */
        virtual void createPaZoneGroupHelperRecords(const std::list< ta_uint32 >& zoneKeys);

        /**
         * deletePaZoneGroupHelperRecords (interface implementation)
         *
         * @see IConfigPaZoneGroup::deletePaZoneGroupHelperRecords for more details
         */
        virtual void deletePaZoneGroupHelperRecords(const std::list< ta_uint32 >& zoneKeys);
        
        /**
         * queueAdditionOfPaZoneGroupHelperRecords (interface implementation)
         *
         * @see IConfigPaZoneGroup::queueAdditionOfPaZoneGroupHelperRecords for more details
         */
        virtual void queueAdditionOfPaZoneGroupHelperRecords(const std::list< ta_uint32 >& zoneKeys);

        /**
         * queueDeletionOfPaZoneGroupHelperRecords (interface implementation)
         *
         * @see IConfigPaZoneGroup::queueDeletionOfPaZoneGroupHelperRecords for more details
         */
        virtual void queueDeletionOfPaZoneGroupHelperRecords(const std::list< ta_uint32 >& zoneKeys);

        /**
         * getQueuedAssociatedPaZoneKeys (interface implementation)
         *
         * @see IConfigPaZoneGroup::getQueuedAssociatedPaZoneKeys for more details
         */
        virtual const std::set< ta_uint32 > getQueuedAssociatedPaZoneKeys();

        /**
         * getKey
         *
         * Returns the key for this PaZoneGroup.
         *
         * @return The key for this PaZoneGroup as an ta_uint32 .
         *
         * pre: Either - this PaZoneGroup was initially loaded from the database
         *      OR     - writePaZoneGroupData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        virtual ta_uint32 getKey();
        virtual ta_uint32 getLocationKey();
        virtual std::string getLabel();
        virtual ta_uint32 getIsEventGroup();

        virtual void setLocationKey(const ta_uint32 locationKey);
        virtual void setLabel(const std::string& label);
        virtual void setIsEventGroup(const ta_uint32 isEventGroup);
        
         /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA Zone Group this is 
         * just the key in a string format e.g. "PA Zone Group 1".
         *
         * @return The key description for this item as a std::string
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
         * setName
         *
         * Not applicable for PA Zone Groups.  Will just assert.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to this zone. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all display changes
         */
        virtual TA_Base_Core::ItemChanges getAllItemChanges()
        {
            return m_paZoneGroupChanges;
        }

       /**
         * getUniqueIdentifier
         *
         * This retrieves the unique identifier for this operator. We cannot use keys for identification
         * in the configuration editor because new operators do not have keys. Therefore we use this
         * method for identification
         *
         * @return ta_uint32 - The unique identifier for this operator
         */
        virtual ta_uint32 getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        }

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
         *            converted to the required format (e.g. TYPEKEY should be an 
         *             ta_uint32 ), or if the wrong amount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated();


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
         *            converted to the required format (e.g. TYPEKEY should be an 
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the PaZoneGroup has been changed by the user.
         *
         * @return bool - This will return true if the PaZoneGroup does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged();

        /**
         * isNew
         *
         * This should be called to determine if this PaZoneGroup is new. A new PaZoneGroup is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the PaZoneGroup has not been applied to the database.
         */
        virtual bool isNew();


        /**
         * getAssociatedPaZones
         *
         * Returns the PaZones associated with this Pa Zone Group
         *
         * @return The PaZones in this group
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */ 
		virtual std::vector<IPaZone*> getAssociatedPaZones();


        /**
         * invalidate
         *
         * Make the data contained by this PaZoneGroup as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();

        /**
         * setAuditData (interface implementation, see IConfigPaZoneGroup for more details)
         */
        virtual void setAuditData(const AuditInfo& auditData);

    private:
        
        /**
         * refreshZoneAssociationSets
         *
         * Refreshes the original and modified zone key sets from the database
         *  provided they're not already set (only a once off operation)
         *
         */
        void refreshZoneAssociationSets();

        /**
         * writeZoneAssociationChanges
         *
         * Called to write all queued updates stored by queueZoneGroupHelperRecordUpdate
         * to the database - subsequently clears contents of change queue       
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
        void writeZoneAssociationChanges();

        /**
         * queueZoneGroupHelperRecordUpdate
         *
         * Helper function to share functionality between calling functions
         *
         * @see queueAdditionOfPaZoneGroupHelperRecords
         * @see queueDeletionOfPaZoneGroupHelperRecords
         */
        void queueZoneGroupHelperRecordUpdate(const std::list< ta_uint32 >& zoneKeys, bool add);

	    /**
         * updatePaZoneGroupChanges
         *
         * This updates the map recording the changes to the PaZoneGroup. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updatePaZoneGroupChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
	    /**
         * updatePaZoneGroupChanges
         *
         * This updates the map recording the changes to the PaZoneGroup. Whenever a call to set is made in this
         * class then this method must be called to store the changes.  The input values are converted to
         * string representations before being added to the map
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const ta_uint32 - The old value that was stored for this attribute
         * @param const ta_uint32 - The new value to be stored for this attribute
         */
        virtual void updatePaZoneGroupChanges(const std::string& name, const ta_uint32 oldValue, const ta_uint32 newValue);
		
        enum EAuditUpdate
        {
            ZONE_GROUP_ADDED,
            ZONE_GROUP_REMOVED,
            ZONE_GROUP_MODIFIED
        };

        /**
         * submitAuditMessage 
         * 
         * Used when a zone group is deleted, created or modified
         *
         * @param zoneGroupKey the primary key of the zone group being added/removed/modified
         *
         * @param zoneGroupName the name of the group being added/removed/modified
         *
         * @param updateType the type of update that had been performed
         *
         * @auditData additional information required for audit message
         *
         */
        void submitAuditMessage(ta_uint32 zoneGroupKey,    
                                const std::string& zoneGroupName, 
                                EAuditUpdate updateType,
                                const AuditInfo& auditData);

        // Assignment PaZoneGroup not used so it is made private
		ConfigPaZoneGroup& operator=(const ConfigPaZoneGroup&);


        ta_uint32 m_uniqueIdentifier; // We cannot use keys for identification of PaZoneGroups in the
                                          // Configuration Editor because new PaZoneGroups do not have keys.
                                          // Therefore we use this method for identification.

        static ta_int32 s_nextAvailableIdentifier; // The next unique identifier available


        PaZoneGroupHelper* m_paZoneGroupHelper;

        TA_Base_Core::AuditMessageSender*            m_auditSender;

        TA_Base_Core::ItemChanges m_paZoneGroupChanges; // This is a map of all changes made to the PaZoneGroup so that they are
                                       // recorded and can be sent in audit or online update messages.        

        // The original set of zones belonging in this group
        TA_Base_Core::PrimitiveWrapper< std::set< ta_uint32 > >    m_originalZoneAssociations;
        // The modified set of zones in this group
        std::set< ta_uint32 >    m_modifiedZoneAssociations;

        TA_Base_Core::PrimitiveWrapper<bool> m_recreateModified;

        // Used when operator makes a raw call, without supplying audit parameters
        // (can be set up in advance)
        TA_Base_Core::PrimitiveWrapper<AuditInfo> m_auditData;
    };
} // closes TA_IRS_Core

#endif // !defined(ConfigPaZoneGroup_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
