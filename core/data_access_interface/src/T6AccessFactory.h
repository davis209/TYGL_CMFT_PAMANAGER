/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/T6AccessFactory.h $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime:
 * Last modified by:  $Author:
 *
 * T6AccessFactory is a singleton that is used to retrieve Rights(for EventViewer) objects from the table t6.
 *
 */

#if !defined(_T6_ACCESS_FACTORY_H_)
#define _T6_ACCESS_FACTORY_H_

#include "core/types/src/ta_types.h"
#include <map>
#include <set>
#include "core/data_access_interface/src/IDatabase.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
namespace TA_Base_Core {

    class T6AccessFactory
    {
    public:

        //      struct T6ProfileRights
        //      {
        //          int profileId;
        //          int subsystemKey;
        //          int physicalSubsystemKey;
        //          int sesstaId;
        //          int seagroId;
        //          int seactiId;
        //          T6ProfileRights()
        //          {
        //              profileId =0;
        //              subsystemKey = 0;
        //              physicalSubsystemKey =0;
        //              sesstaId =0;
        //              seagroId=0;
        //              seactiId =0;
        //          }
        //      };

        /**
         * getInstance
         *
         * Returns a reference to the only instance of T6AccessFactory.
         */
        static T6AccessFactory& getInstance();

        struct SubSystemKeys
        {
            ta_uint32 subsystemKey;
            ta_int32 physicalSubsystemKey;

            // apply operator< to operands
            bool operator<(const SubSystemKeys& right) const
            {
                if (subsystemKey != right.subsystemKey)
                {
                    return subsystemKey < right.subsystemKey;
                }
                else
                {
                    return physicalSubsystemKey < right.physicalSubsystemKey;
                }
            }
        };

        typedef std::map<SubSystemKeys, std::set<ta_uint32> > Rights;

        /**
         * retrieveRightsByProfileId
         *
         * retrieve rights from t6 table by profile ID
         */
        void retrieveRightsByProfileId(ta_uint32 profileId);

        /*
         * getRightsByProfileId
         *
         * if m_rights has been retrieved from DB. copy it to the 2nd parameter, return true
         * otherwise return false
         *
         */
        bool T6AccessFactory::getRightsByProfileId(ta_uint32 profileId, T6AccessFactory::Rights& rights);

    private:

        static T6AccessFactory* m_instance;
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

        T6AccessFactory::Rights  m_rights;
        ta_uint32 m_currentProfileID;
        bool m_rightsCacheRetrieved;
        T6AccessFactory::T6AccessFactory();
    };
}
#endif // !defined(_T6_ACCESS_FACTORY_H_)
