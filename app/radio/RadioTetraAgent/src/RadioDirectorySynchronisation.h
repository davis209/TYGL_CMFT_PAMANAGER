/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioDirectorySynchronisation.h $
 * @author:  Nick Jardine
 * @version: $Revision: #4 $
 * Last modification: $DateTime: 2010/05/10 14:05:07 $
 * Last modified by:  $Author: builder $
 *
 * This class is responsible for handling the periodic synchronisation of the subscriber database with
 * the TransActive database.
 */

#if !defined(AFX_RADIODIRECTORYSYNCHRONISATION_H)
#define AFX_RADIODIRECTORYSYNCHRONISATION_H

#include "core/types/src/ta_types.h"
#include <ctime>
#include <string>
#include <map>

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSearchSubscribers.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodChangeReference.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodQueryReference.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocol.h"

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioDirectoryCorbaDef.h"

#include "core/data_access_interface/radio/src/IRASubscribers.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "app/radio/RadioTetraAgent/src/RadioSynchronisationTypes.h"
#include "core/data_access_interface/entity_access/src/RadioDirectoryEntityData.h"

namespace TA_IRS_App
{
    class Radio;        // Forward declarations
    class RadioEvent;   // ********************
    class RadioMethod;  // ********************
	class IRadioDirectoryCallback; // *********

    class RadioDirectorySynchronisation  :  virtual public TA_Base_Core::Thread
    {
		public:

			RadioDirectorySynchronisation(	time_t synchronisationFrequency,
											TA_IRS_App::Radio& radio,
											TA_Base_Core::RadioDirectoryEntityDataPtr entityData,
											IRadioDirectoryCallback& directoryCallback);

			virtual ~RadioDirectorySynchronisation();

			/**
			 * updateSynchronisationFrequency
			 *
			 * Changes the current synch frequency for the radio directory. Used when an update on the parameter is 
			 * received.
			 *
			 * @param synchronisationFrequency The new synch frequency to set for the agent.
			 */
			void updateSynchronisationFrequency(time_t synchronisationFrequency);

			// ***************************
			// Thread Interface Methods  *
			// ***************************
        
			/**
			 * run
			 *
			 * Called by the thread framework when the thread is started
			 */
			virtual void run();

			/**
			 * terminate
			 *
			 * Called by the thread framework when the thread is to be terminated
			 */
			virtual void terminate();

			/**
			 * Triggers the main thread into synchronising immediately. Called
			 * by the DirectoryServant when the TCPServer connection is
			 * restored.
			 */
			void triggerSynchronisation();

			/**
			 * Prevents the main thread from synchronising. Used while not
			 * connected to the TCPServer
			 */
			void preventSynchronisation();

			/**
			 * Retrieves the most recent time of synchronisation
			 */
			ta_int32 getLastSynchroniseTime();

		private:

			struct SubscriberDetails
			{
				std::string name;
				std::string type;
				std::string tsi;
				std::string location;
			};

			/**
			 *
			 */
			void synchronise();

			/**
			 *
			 */
			TA_Base_Core::IRASubscribers::SubscriberType getSubscriberType(const std::string& typeString) const;

			/**
			 *
			 */
			const ta_uint32 getLocationKey(const std::string& locationName) const;

			/**
			 *
			 */
			const std::string getRadioTimeString(bool doGetZero=false) const;

			/**
			 *
			 */
			std::vector<RadioEvent*> searchSubscribers(
											const std::string& dbTime,
											const RadioMethodSearchSubscribers::EView searchView,
											std::vector<std::string>& fields,
											const std::string & extraSearchCriteria ="") const;

			/**
			 *
			 */
			const void processIndividualEvents(std::vector<RadioEvent*> events) const;

			/**
			 *
			 */
			const void processGroupEvents(	std::vector<RadioEvent*> events,const std::string& dbTime,
											std::vector<std::string>& fields) const;

			/**
			 *
			 */
			const void processGroupMembershipEvents(std::vector<RadioEvent*> events, const std::string & gitsi, TA_Base_Core::IRASubscribers * group) const;

			/**
			 *
			 */
			const void processPatchEvents(	std::vector<RadioEvent*> events,const std::string& dbTime,
											std::vector<std::string>& fields) const;

			/**
			 *
			 */
            bool changeSearchType(RadioMethodChangeReference::ESearchType searchType) const;

			/**
			 *
			 */
            std::string queryReference(RadioReference queryRef, RadioMethodQueryReference::EKeys key) const;

			/**
			 *
			 */
			void addPosition(const std::string& positionName,const unsigned int position);

			/**
			 *
			 */
			void removePosition(const std::string& positionName);

			/**
			 *
			 */
			unsigned int getPosition(const std::string& positionName) const;

			/**
			 *
			 */
			void clearPositions();

			/**
			 *
			 */
			unsigned int getNumPositions() const;
			// /////////////////////////////////////////////////

			///
			///
			volatile time_t m_synchronisationFrequency;

			///
			///
			volatile time_t m_elapsedTime;

			///
			///
			volatile bool m_isTerminated;
			
			///
			/// Used when the connection to the radio goes down.
			/// Has the same effect as setting the synchronisation frequency
			/// to zero, however does not affect that variable.
			volatile bool m_doSynchronise;

			///
			///
			typedef std::map< std::string, unsigned int > FieldPositionMap;


			///
			///
			FieldPositionMap m_positionMap;

			///
			///
			Radio& m_radio;

			///
			///
			TA_Base_Core::RadioDirectoryEntityDataPtr m_entityData;

			///
			///
			IRadioDirectoryCallback& m_directoryCallback;

			///
			/// Used to prevent getLastSynchroniseTime() being called during
			/// synchronise().  Shouldn't really be necessary since the former
			/// will usually only be called after the latter has completed, but
			/// the safety rules OK (spot the risk, assess the danger, make the
			/// changes)
			TA_Base_Core::NonReEntrantThreadLockable m_synchroniseLock;
			ESessionState m_dbSynchSessionState;

			static const std::string TEXT_REFERENCE;
			static const std::string NAME_POSITION;
			static const std::string TYPE_POSITION;
			static const std::string ITSI_POSITION;
			static const std::string DELETED_POSITION;
			static const std::string REGION_POSITION;
			static const std::string PATCH_ID_POSITION;
			static const std::string PATCH_OWNER_POSITION;

			ta_uint32 m_svrTimeOffset;
	};
}
// vim:noexpandtab:shiftwidth=4:tabstop=4:
#endif // !defined(AFX_RADIODIRECTORYSYNCHRONISATION_H)
