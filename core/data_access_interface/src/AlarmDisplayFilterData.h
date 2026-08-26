/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/AlarmDisplayFilterData.h $
 * @author:  Noel R. Tajanlangit
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * AlarmDisplayFilterData is an interface to the Alarm_Display_Filter table. It provides both read and write access
 * for agents and user applicaitons.
 */

#ifndef __ALARM_DISPLAY_FILTER_DATA_H_INCLUDED__
#define __ALARM_DISPLAY_FILTER_DATA_H_INCLUDED__

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/IAlarmDisplayFilterData.h"
#include <string>

namespace TA_Base_Core
{
    class AlarmDisplayFilterData
        : public IAlarmDisplayFilterData
    {
    public: //Methods

        /*
        * Constructor
        */
        AlarmDisplayFilterData()
            : m_isValidData(false)
            , m_pkey(0)
            , m_isNew(true)
            , m_isLocationKeyModified(false)
            , m_isAlarmLocationKeyModified(false)
            , m_isAlarmTypeModified(false)
            , m_isSubSystemKeyModified(false)
            , m_isDisplayConditionModified(false)
            , m_isDeletedModified(false)
        {}

        /*
        * Constructor
        */
        AlarmDisplayFilterData(ta_uint32 _pkey)
            : m_isValidData(false)
            , m_pkey(_pkey)
            , m_isNew(false)
            , m_isLocationKeyModified(false)
            , m_isAlarmLocationKeyModified(false)
            , m_isAlarmTypeModified(false)
            , m_isSubSystemKeyModified(false)
            , m_isDisplayConditionModified(false)
            , m_isDeletedModified(false)
        {};

        /*
        * Constructor
        */
        AlarmDisplayFilterData(ta_uint32 _pkey,
                               ta_uint32 _locationKey,
                               ta_uint32 _alarmType,
                               ta_uint32 _subSystemKey,
                               ta_uint32 _alarmLocationKey,
                               ta_uint32 _displayCondition,
                               ta_uint32 _deleted);

        /*
        * Destructor
        */
        virtual ~AlarmDisplayFilterData() {};

        /**
         * getKey
         */
        virtual ta_uint32 getKey();

        /**
         * getLocationKey
         */
        virtual ta_uint32 getLocationKey();

        /**
         * setLocationKey
         */
        virtual void setLocationKey(const int& _nLocationKey);

        /**
         * setAlarmLocationKey
         */
        virtual ta_uint32 getAlarmLocationKey();

        /**
         * getAlarmLocationKey
         */
        virtual void setAlarmLocationKey(const ta_uint32& _nLocationKey);

        /**
         * getAlarmType
         */
        virtual ta_uint32 getAlarmType();

        /**
         * setAlarmType
         */
        virtual void setAlarmType(const ta_uint32& _nAlarmType);

        /**
         * getSubSystemKey
         */
        virtual ta_uint32 getSubSystemKey();

        /**
         * setSubSystemKey
         */
        virtual void setSubSystemKey(const ta_uint32& _nSubSystemKey);

        /**
         * getAlarmLocationKey
         */
        virtual IAlarmDisplayFilterData::DISPLAY_CONDITION getDisplayCondition();

        /**
         * setDisplayCondition
         */
        virtual void setDisplayCondition(const DISPLAY_CONDITION& _nDisplayCondition);

        /**
         * getDeleted
         */
        virtual ta_uint32 getDeleted();

        /**
         * setDeleted
         */
        virtual void setDeleted(const ta_uint32& _isDeleted);

        /**
         * applyChanges
         */
        void applyChanges();

        /**
         * invalidate
         */
        void invalidate();

    private: // Methods
        void _reload();
        void _update();
        void _createNew();
        std::string getLocalDatabaseName();

    private: //Variables
        ta_uint32 m_pkey;
        ta_uint32 m_locationKey;
        ta_uint32 m_alarmType;
        ta_uint32 m_alarmLocationKey;
        ta_uint32 m_subSystemKey;
        DISPLAY_CONDITION m_displayCondition;
        ta_uint32 m_deleted;

        bool m_isLocationKeyModified;
        bool m_isAlarmLocationKeyModified;
        bool m_isSubSystemKeyModified;
        bool m_isDisplayConditionModified;
        bool m_isDeletedModified;
        bool m_isAlarmTypeModified;

        bool m_isValidData;
        bool m_isNew;
        std::string m_localDatabase;
    };

}//close namespace TA_Base_Core

#endif // __ALARM_DISPLAY_FILTER_DATA_H_INCLUDED__
