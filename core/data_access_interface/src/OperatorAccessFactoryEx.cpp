/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/OperatorAccessFactoryEx.cpp $
 * @author Nick Jardine
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * OperatorAccessFactory is a singleton that is used to retrieve operator objects either from the
 * database or newly created. All operator objects returned will adhear to the IOperator interface.
 */

#include "OperatorAccessFactoryEx.h"
#include "core/utilities/src/StdUtil.h"
#include "core/utilities/src/TAVector.h"
#include <mutex>

namespace TA_Base_Core
{
    OperatorAccessFactory& OperatorAccessFactoryEx::instance()
    {
        static auto& s_instance = OperatorAccessFactory::getInstanceNoInit();
        static std::once_flag s_once;
        std::call_once(s_once, [&] { boost::async([] { s_instance.init(); }); });
        return s_instance;
    }

    IOperatorPtrList& OperatorAccessFactoryEx::getAllOperators()
    {
        static auto s_all = stdutil::to_shared(instance().getAllOperators());
        return s_all;
    }

    IOperatorPtrList& OperatorAccessFactoryEx::getAllSystemOperators()
    {
        static IOperatorPtrList s_all = getAllOperatorsIf([](auto & o) { return o->isSystemOperator(); });
        return s_all;
    }

    IOperatorPtrList& OperatorAccessFactoryEx::getAllNonSystemOperators()
    {
        static IOperatorPtrList s_all = getAllOperatorsIf([](auto & o) { return !o->isSystemOperator(); });
        return s_all;
    }

    IOperatorPtrList OperatorAccessFactoryEx::getAllOperatorsIf(std::function<bool(const IOperatorPtr&)> pred)
    {
        return stdutil::filter_copy(getAllOperators(), pred);
    }

    IOperatorPtr OperatorAccessFactoryEx::getOperator(const std::string& name)
    {
        return getOperatorIf([&](auto o) { return o->getName() == name; });
    }

    IOperatorPtr OperatorAccessFactoryEx::getOperator(size_t key)
    {
        return getOperatorIf([&](auto o) { return o->getKey() == key; });
    }

    IOperatorPtr OperatorAccessFactoryEx::getOperatorIf(std::function<bool(const IOperatorPtr&)> pred)
    {
        return stdutil::get_if(getAllOperators(), pred);
    }

    std::string OperatorAccessFactoryEx::toName(size_t key, const std::string& defaultName)
    {
        return stdutil::no_throw_or(defaultName, [&]
        {
            auto o = getOperator(key);
            return o ? o->getName() : defaultName;
        });
    }

    bool OperatorAccessFactoryEx::getOperatorSaltValue(const ta_uint32 key, std::string& saltValue)
    {
        // [aqiu] for now, read the DB for every request.
        return instance().getOperatorSaltValue(key, saltValue);
    }
}
