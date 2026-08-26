/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/OperatorAccessFactoryEx.h $
 * @author Nick Jardine
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * OperatorAccessFactory is a singleton that is used to retrieve operator objects either from the
 * database or newly created. All operator objects returned will adhear to the IOperator interface.
 */

#pragma once
#include "OperatorAccessFactory.h"

namespace TA_Base_Core
{
    struct OperatorAccessFactoryEx
    {
        static OperatorAccessFactory& instance();
        static IOperatorPtrList& getAllOperators();
        static IOperatorPtrList& getAllSystemOperators();
        static IOperatorPtrList& getAllNonSystemOperators();
        static IOperatorPtrList getAllOperatorsIf(std::function<bool(const IOperatorPtr&)> pred);
        static IOperatorPtr getOperator(const std::string& name);
        static IOperatorPtr getOperator(size_t key);
        static IOperatorPtr getOperatorIf(std::function<bool(const IOperatorPtr&)> pred);

        static std::string toName(size_t key, const std::string& defaultName = "");

        static bool getOperatorSaltValue(const ta_uint32 key, std::string& saltValue);

#if 0
        static IOperatorPtr getOperatorFromSession(const std::string& sessionId);
        std::string getDeletedOperatorName(ta_uint32 key);
        bool getDeletedOperatorIsSystemOperator(ta_uint32 key);
        std::vector<IOperator*> getAllOperators(const bool readWrite = false);
        std::vector<IOperator*> getAllOperatorsInfo();
        std::vector<IOperator*> getAllOperatorsAndRegions(const bool readWrite = false);
        std::vector<IOperator*> getAllOperatorsAndAttributes();
        std::vector<IOperator*> getAllNonSystemOperators(const bool readWrite = false,
                                                         const bool getProfiles = true,
                                                         const bool getRegions = false);
        void getProfileByOperator(ta_uint32 operatorId, std::vector<ta_uint32>& profiles);
        void getRegionByOperator(ta_uint32 operatorId, std::vector<ta_uint32>& regions);
#endif
    };
}
