#ifndef __IDATABASE_WRITABLE_DATA_H_INCLUDED__
#define __IDATABASE_WRITABLE_DATA_H_INCLUDED__

#pragma once

#include "boost//shared_ptr.hpp"
#include <vector>

namespace TA_Base_Core
{
    class IDbWritableData
    {
    public:

        virtual ~IDbWritableData() {};

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a
         *            DataException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges() = 0;
    };

    typedef boost::shared_ptr<IDbWritableData> IDbWritableDataPtr;
    typedef std::vector<IDbWritableDataPtr> T_IDbWritableDataList;
} //!TA_Base_Core

#endif // !__IDATABASE_WRITABLE_DATA_H_INCLUDED__
