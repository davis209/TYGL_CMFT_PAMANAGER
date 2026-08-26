/**
 * The source code in this file is the property of
 * ST Engineering Electronics and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/C830G_TIP/TA_BASE/transactive/core/data_access_interface/global_status/src/GlobalStatusAccessFactory.h $
 * @author:  Anton QIU
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2020/08/27 14:52:41 $
 * Last modified by:  $Author: anton.qiu $
 *
 * GlobalStatusAccessFactory is a singleton that is used to retrieve
 * Global Status objects.
 */

#if !defined(GLOBAL_STATUS_ACCESS_FACTORY_H_INCLUDED)
#define GLOBAL_STATUS_ACCESS_FACTORY_H_INCLUDED

#include "core/global_status/src/GlobalStatusData.h"
#include "core/naming/src/INamedObject.h"
#include <vector>
#include <string>

namespace TA_Base_Core
{
    //agents = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
    //  TA_Base_Core::GlobalStatusAgentEntityData::getStaticType(), m_location, true);

    class GlobalStatusAccessFactory
    {
    public:

        virtual ~GlobalStatusAccessFactory();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of a ComponentAccessFactory object.
         */
        static GlobalStatusAccessFactory& getInstance()
        {
            static GlobalStatusAccessFactory fact;
            return fact;
        }

        /**
         * getAllStatus
         *
         * Retrieves All global Status variables from database
         *
         * @return A vector of pointers to GlobalStatusDataCorbaDef objects. The caller must delete
         *         these objects.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if the data retrieved
         *            is of the wrong type.
         */
        std::vector<GlobalStatusData*> getAllStatus();

        /**
         * getStatusByName
         *
         * Retrieves global Status variable from database by its name.
         *
         * @param  status variable name string. This name is case sensitive.
         *
         * @return A pointer to GlobalStatusDataCorbaDef objects. The caller must delete
         *         these objects.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *            4) The result is empty or more than one row returned (not unique). Most likely caused by incorrect database configuration.
         * @exception DataException A DataException will be thrown if the data retrieved
         *            is of the wrong type.
         */
        TA_Base_Core::GlobalStatusData* getStatusByName(std::string statusName);

        /**
         * insertStatus
         *
         * insert global Status variable to database. The variable name is unique.
         *     If the variable name already exists, the data will not be inserted (no exception generated).
         *
         * @param  A pointer to GlobalStatusDataCorbaDef object containing the variable information.
         *          The caller is responsible to set the variable information in the GlobalStatusDataCorbaDef object.
         * @return none.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if the data retrieved
         *            is of the wrong type.
         */
        void insertStatus(const GlobalStatusData& status);

        /**
         * updateStatus
         *
         * update global Status variable to database.
         *
         * @param  A pointer to GlobalStatusDataCorbaDef object containing the variable information.
         *          The caller is responsible to set the variable information in the GlobalStatusDataCorbaDef object.
         * @return none.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if the data retrieved
         *            is of the wrong type.
         */
        void updateStatus(const GlobalStatusData& status);

        /**
         * getGSACorbaNameAtLocation
         *
         * get Corba Name of GSA at the location.
         *
         * @param  location key.
         * @return CorbaName.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception TransactiveException thrown if the entity for the global status agent at the location
         *            is not properly configured.
         */
        TA_Base_Core::CorbaName getGSACorbaNameAtLocation(ta_uint32 locationkey);

        /**
         * getGSAAtLocation
         *
         * get GlobalStatusNamedObjectPtr of GSA at the location.
         *
         * @param  location key.
         * @return A pointer to GlobalStatusNamedObject.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception TransactiveException thrown if the entity for the global status agent at the location
         *            is not properly configured.
         */
        // TA_Base_Core::GlobalStatusNamedObjectPtr getGSAAtLocation(ta_uint32 locationkey);

        ///**
        // * updateToGSA
        // * updateToGSAAtLocation
        // * This function use CORBA Call to Global Status Agent to update a Global Status variable.
        // * updateToGSAAtLocation will call getGSAAtLocation() before calling updateToGSA().
        // *
        // * The caller is responsible to catch the exception that may arises.
        // *
        // * @param  A pointer to GlobalStatusNamedObject (for updateToGSA()).
        // *         location key (for updateToGSAAtLocation)
        // *         session_id,
        // *         id of the variable (variable name)
        // *         value of the variable
        // *         name of the updater
        // *         time in millisecond since epoch.
        // * @return true if update successful.
        // *
        // * @exception DatabaseException A DatabaseException will be thrown if there are
        // *            any problems in communicating with the database. The possilble
        // *            reasons are:
        // *            1) Invalid connection string (as stored in RunParams)
        // *            2) Database is not/cannot be opened
        // *            3) Error while attempting to execute a database query
        // * @exception CORBA exception may be thrown by Global Status Agent.
        // */
        //bool updateToGSA(TA_Base_Core::GlobalStatusNamedObjectPtr p_globalStatusAgent,
        //    std::string session_id, std::string id, std::string value, std::string update_by, ta_uint64 time_millisecond);
        //bool updateToGSAAtLocation(ta_uint32 locationkey,
        //    std::string session_id, std::string id, std::string value, std::string update_by, ta_uint64 time_millisecond);

        ///**
        // * readFromGSA
        // * readFromGSAAtLocation
        // * This function use CORBA Call to Global Status Agent to update a Global Status variable.
        // * readFromGSAAtLocation will call getGSAAtLocation() before calling readFromGSA().
        // *
        // * The caller is responsible to catch the exception that may arises.
        // * The caller is responsible to delete the GlobalStatusDataCorbaDef object if no longer needed.
        // *
        // * @param  A pointer to GlobalStatusNamedObject (for readFromGSA()).
        // *         location key (for readFromGSAAtLocation)
        // *         id of the variable (variable name)
        // * @return pointer to GlobalStatusDataCorbaDef object.
        // *
        // * @exception DatabaseException A DatabaseException will be thrown if there are
        // *            any problems in communicating with the database. The possilble
        // *            reasons are:
        // *            1) Invalid connection string (as stored in RunParams)
        // *            2) Database is not/cannot be opened
        // *            3) Error while attempting to execute a database query
        // * @exception CORBA exception may be thrown by Global Status Agent.
        // */
        //TA_Base_Core::GlobalStatusDataCorbaDef * readFromGSA(TA_Base_Core::GlobalStatusNamedObjectPtr p_globalStatusAgent, std::string id);
        //TA_Base_Core::GlobalStatusDataCorbaDef * readFromGSAAtLocation(ta_uint32 locationkey, std::string id);

    private:

        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        GlobalStatusAccessFactory() {};
        GlobalStatusAccessFactory& operator=(const GlobalStatusAccessFactory&) = delete;
        GlobalStatusAccessFactory(const GlobalStatusAccessFactory&) = delete;
    };
} //end namespace TA_Base_Core

#endif  // !GLOBAL_STATUS_ACCESS_FACTORY_H_INCLUDED
