/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/ConfigPaDvaMessage.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPaDvaMessage is a concrete implementation of IConfigPaDvaMessage, which is in turn an implementation
  * of IPaDvaMessage. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaDvaMessages.
  */


#if !defined(ConfigPaDvaMessage_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigPaDvaMessage_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <set>

#include "core/data_access_interface/pa/src/IConfigPaDvaMessage.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_IRS_Core
{
    class PaDvaMessageHelper;

    class ConfigPaDvaMessage : public IConfigPaDvaMessage
    {

    public:
        /**
         * ConfigPaDvaMessage (constructor)
         *
         * Constructs a new instance of the ConfigPaDvaMessage with no id. This is used when
         * creating a *new* PaDvaMessage - that is, one that does not yet exist in the database.
         */
        ConfigPaDvaMessage();

        /**
         * ConfigPaDvaMessage (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaDvaMessageHelper::PaDvaMessageHelper(IData*)
         *
         */
        ConfigPaDvaMessage(ta_uint32 row, TA_Base_Core::IData& data);

        /**
         * ConfigPaDvaMessage (constructor)
         *
         * Constructs a new instance of ConfigPaDvaMessage with the specified id. This is used 
         * when creating a ConfigPaDvaMessage based around an existing PaDvaMessage in the database.
         *
         * @param key The key to this PaDvaMessage in the database.
         */
        ConfigPaDvaMessage(const ta_uint32 idKey);


        /**
         * ConfigPaDvaMessage (copy constructor)
         *
         * @param ConfigPaDvaMessage& - The PaDvaMessage to make a copy of.
         */
        ConfigPaDvaMessage( const ConfigPaDvaMessage& theConfigPaDvaMessage);

        
        virtual ~ConfigPaDvaMessage();


    public:

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
        void deleteThisObject(bool cascade=false);


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the PaDvaMessage object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This PaDvaMessage has not been deleted
         */
        void applyChanges();
        

        /**
         * getKey
         *
         * Returns the key for this PaDvaMessage.
         *
         * @return The key for this PaDvaMessage as an ta_uint32 .
         *
         * pre: Either - this PaDvaMessage was initially loaded from the database
         *      OR     - writePaDvaMessageData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        virtual ta_uint32 getKey();
        virtual ta_uint32 getLocationKey();
        virtual ta_uint32 getId();
        virtual std::string getLabel();
        virtual std::string getType();
        virtual ta_uint32 getTisMessageTag();
        virtual ta_uint32 getTisLibrarySection();
        virtual ta_uint32 getTisLibraryVersion();

        virtual void setLocationKey(const ta_uint32 locationKey);
        virtual void setId(const ta_uint32 id);
        virtual void setLabel(const std::string& label);
        virtual void setType(const std::string& type);
        virtual void setTisMessageTag(const ta_uint32 tisMessageTag);
        virtual void setTisLibrarySection(const ta_uint32 tisLibrarySection);
        virtual void setTisLibraryVersion(const ta_uint32 version);


         /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA DVA Message this is 
         * just the key in a string format e.g. "PA Dva Message 1".
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
         * Not applicable for PA DVA Messages.  Will just assert.
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
            return m_paDvaMessageChanges;
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
        };

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
         *            converted to the required format (e.g. TYPEKEY should be an 
         *             ta_uint32 ), or if the wrong amount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated();


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
         *            converted to the required format (e.g. TYPEKEY should be an 
         *             ta_uint32 ), or if the wrong amount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the PaDvaMessage has been changed by the user.
         *
         * @return bool - This will return true if the PaDvaMessage does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the PaDvaMessage changes is empty then nothing has changed
            return !m_paDvaMessageChanges.empty();
        }


        /**
         * isNew
         *
         * This should be called to determine if this PaDvaMessage is new. A new PaDvaMessage is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the PaDvaMessage has not been applied to the database.
         */
        virtual bool isNew();


        /**
         * invalidate
         *
         * Make the data contained by this PaDvaMessage as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updatePaDvaMessageChanges
         *
         * This updates the map recording the changes to the PaDvaMessage. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updatePaDvaMessageChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
	    /**
         * updatePaDvaMessageChanges
         *
         * This updates the map recording the changes to the PaDvaMessage. Whenever a call to set is made in this
         * class then this method must be called to store the changes.  The input values are converted to
         * string representations before being added to the map
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const ta_uint32 - The old value that was stored for this attribute
         * @param const ta_uint32 - The new value to be stored for this attribute
         */
        virtual void updatePaDvaMessageChanges(const std::string& name, const ta_uint32 oldValue, const ta_uint32 newValue);
		
        // Assignment PaDvaMessage not used so it is made private
		ConfigPaDvaMessage& operator=(const ConfigPaDvaMessage&);


        ta_uint32 m_uniqueIdentifier; // We cannot use keys for identification of PaDvaMessages in the
                                          // Configuration Editor because new PaDvaMessages do not have keys.
                                          // Therefore we use this method for identification.

        static ta_int32 s_nextAvailableIdentifier; // The next unique identifier available


        PaDvaMessageHelper* m_paDvaMessageHelper;


        TA_Base_Core::ItemChanges m_paDvaMessageChanges; // This is a map of all changes made to the PaDvaMessage so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_IRS_Core

#endif // !defined(ConfigPaDvaMessage_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
