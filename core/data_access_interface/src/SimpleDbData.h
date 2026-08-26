/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SimpleDbData.h $
 * @author Nick Jardine
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by: Nick Jardine
 *
 * SimpleDbData is an implementation of IData. It is used to process data returned from the
 * database into C++ specific data-types. The code in here is specific to SimpleDb, and may
 * not work for any other data library (even other data libraries for Oracle).
 *
 */

#pragma once

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/IData.h"
#include "core/database/src/SimpleDb.h"
#include <vector>
#include <map>
#include <string>

namespace TA_Base_Core
{
    class SimpleDbData : public IData
    {
    public:

        /**
         * SimpleDbData - constructor
         *
         * This is the only valid constructor - there is no "default" constructor, as this class
         * does not make sense without data
         *
         * @param columnNames The vector of column names. Each name MUST be unique. This is tested
         * by an assert
         * @param data The double vector of data retrieved from the database. The number of columns
         * in this Buffer MUST be equal to the number of column names. This is tested by
         * an assert
         *
         * pre: each entry in columnNames is unique
         *      columnNames.size() == data[0].size()
         */
        SimpleDbData(Table&& table);
        SimpleDbData(const std::vector<std::string>& columnNames, Table&& table);

        SimpleDbData(const SimpleDbData& theSimpleDbData) = delete;
        SimpleDbData& operator=(const SimpleDbData&) = delete;

        /**
         * getNumRows
         *
         * This function returns the number of rows of data contained by this object.
         *
         * @returns the number of rows of data contained by this IData object in ta_uint32 format.
         */
        size_t getNumRows();

        /**
         * getIntegerData
         *
         * This function returns the data at the specified location as an integer.
         *
         * @param rowIndex The row number from which to retrieve the data. This MUST be less than or
         * equal to m_numRows. This is tested by an assert.
         * @param fieldName The name of the column (field) from which to retrieve the data.
         *
         * @return The data is returned as an integer
         *
         * @exception DataException If the data cannot be converted to an integer, a DataException
         * is thrown. NB: The string retrieved by the sql statement MUST NOT contain any leading spaces, or
         * the number will be considered a non-integer and an exception will be thrown.
         *
         * pre: rowIndex <= getNumRows()
         */
        int getIntegerData(const size_t rowIndex, const std::string& fieldName, const int defaultValue = 0);
        ta_int64 getInt64Data(const size_t rowIndex, const std::string& fieldName, const ta_int64 defaultValue = 0);

        /**
         * getUnsignedLongData
         *
         * This function returns the data at the specified location as an ta_uint32
         *
         * @param rowIndex The row number from which to retrieve the data. This MUST be less than or
         * equal to m_numRows. This is tested by an assert.
         * @param fieldName The name of the column (field) from which to retrieve the data.
         *
         * @return The data is retunred as an ta_uint32
         *
         * @exception DataException If the data cannot be converted to an unsigned
         * long, a DataException is thrown. NB: The string retrieved by the sql statement
         * MUST NOT contain any leading spaces, or the number will be considered
         * invalid and an exception will be thrown.
         *
         * pre: rowIndex <= getNumRows()
         */
        size_t getUnsignedLongData(const size_t rowIndex, const std::string& fieldName, const size_t defaultValue = 0);
        ta_uint64 getUnsignedLongLongData(const size_t rowIndex, const std::string& fieldName, const ta_uint64 defaultValue = 0);

        /**
         * getRealData
         *
         * This function returns the data at the specified location as a double
         *
         * @param rowIndex The row number from which to retrieve the data. This MUST be less than or
         * equal to m_numRows. This is tested by an assert.
         * @param fieldName The name of the column (field) from which to retrieve the data.
         *
         * @return The data is retunred as a dobule
         *
         * @exception DataException If the data cannot be converted to a double
         * a DataException is thrown. NB: 1) The string retrieved by the sql statement
         * MUST NOT contain any leading spaces, or the number will be considered
         * invalid and an exception will be thrown. 2) The format of the real data retrived by
         * the sql statement must be [-](0123456789)n[.(0123456789)n]. That is, a "-", if present,
         * must be in the first location, there must always be at least one numeric chracter, and
         * the decimal point, if present, must always be trailed by at least one numeric character.
         * If the number differs from these specifications, an exception will be thrown.
         *
         * pre: rowIndex <= getNumRows()
         */
        double getRealData(const size_t rowIndex, const std::string& fieldName, const double defaultValue = 0.0);

        /**
         * getBooleanData
         *
         * This function returns the data at the specified location as a boolean
         *
         * @param rowIndex The row number from which to retrieve the data. This MUST be less than or
         * equal to m_numRows. This is tested by an assert.
         * @param fieldName The name of the column (field) from which to retrieve the data.
         *
         * @return The data is retunred as a boolean
         *
         * @exception DataException If the data cannot be converted to a boolean
         * a DataException is thrown. NB: 1) The string retrieved by the sql statement
         * MUST NOT contain any leading spaces, or the number will be considered
         * invalid and an exception will be thrown.
         * If the number differs from these specifications, an exception will be thrown.
         *
         * pre: rowIndex <= getNumRows()
         */
        virtual bool getBooleanData(const size_t rowIndex, const std::string& fieldName, const bool defaultValue = false);

        /**
         * getStringData
         *
         * This function returns the data at the specified location as an ta_uint32
         *
         * @param rowIndex The row number from which to retrieve the data. This MUST be less than or
         * equal to m_numRows. This is tested by an assert.
         * @param fieldName The name of the column (field) from which to retrieve the data.
         *
         * @return The data is retunred as a string
         *
         * NB: No DataException will be thrown from this method for SimpleDb, as it retrieves all
         * data as strings.
         *
         * pre: rowIndex <= getNumRows()
         */
        const std::string& getStringData(const size_t rowIndex, const std::string& fieldName);
        std::string getStringData(const size_t rowIndex, const std::string& fieldName, const std::string& defaultValue);

        /**
         * getDateData
         *
         * This function returns the data at the specified location as a time_t
         *
         * @param rowIndex The row number from which to retrieve the data. This MUST be less than or
         * equal to m_numRows. This is tested by an assert.
         * @param fieldName The name of the column (field) from which to retrieve the data.
         *
         * @return The data is retunred as a time_t
         *
         * @exception DataException If the data cannot be converted to a time_t
         * a DataException is thrown.
         *
         * pre: 1) To use this method the date MUST be selected from the database using the sql
         * statement select format: TO_CHAR([column],'YYYYMMDDHH24MMSS') from [table];
         * An assert is used to test that the string is in the YYYYMMDDHH24MMSS format.
         * NB: The day/month compatibility will not be tested by this routine, and the mktime
         * function will convert any invalid date to an equivalent valid one (e.g. 29/02/03 will
         * become 01/03/03). Note however that dates greater than 31 will generate an exception.
         *      2) rowIndex <= getNumRows()
         */
        time_t getDateData(const size_t rowIndex, const std::string& fieldName, const time_t defaultValue = 0);

        /**
         * getTimestampData
         *
         * This function returns the data at the specified location as a timeb
         *
         * @param rowIndex The row number from which to retrieve the data. This MUST be less than or
         * equal to m_numRows. This is tested by an assert.
         * @param fieldName The name of the column (field) from which to retrieve the data.
         *
         * @return The data is retunred as a timeb
         *
         * @exception DataException If the data cannot be converted to a timeb
         * a DataException is thrown.
         *
         * pre: 1) To use this method the date MUST be selected from the database using the sql
         * statement select format: TO_CHAR([column],'YYYYMMDDHH24MMSSFF3') from [table];
         * An assert is used to test that the string is in the YYYYMMDDHH24MMSSFF3 format.
         * NB: The day/month compatibility will not be tested by this routine, and the mktime
         * function will convert any invalid date to an equivalent valid one (e.g. 29/02/03 will
         * become 01/03/03). Note however that dates greater than 31 will generate an exception.
         *      2) rowIndex <= getNumRows()
         */
        timeb getTimestampData(const size_t rowIndex, const std::string& fieldName);

        /**
         * isNull
         *
         * This function checks if the data at the specified location is NULL or an empty string.
         *
         * @param rowIndex The row number from which to retrieve the data. This MUST be less than or
         * equal to m_numRows. This is tested by an assert.
         * @param fieldName The name of the column (field) from which to retrieve the data.
         *
         * @return true if the field is null, and false otherwise.
         *
         * pre: rowIndex <= getNumRows()
         */
        virtual bool isNull(const size_t rowIndex, const std::string& fieldName);

        /**
         * getNullTime
         *
         * Return the seconds from 01/01/1990 12:00:00
         */
        virtual time_t getNullTime();

        bool isInitialDate(const std::string& dataStr);

    private:

        const std::string& getData(const size_t rowIndex, const std::string& fieldName);

        Table m_table;
        ta_uint32 m_numRows;
        size_t m_firstRowIndex;
        std::map<std::string, size_t> m_columnNameIndexMap;

        static const int JANUARY;
        static const int FEBRUARY;
        static const int MARCH;
        static const int APRIL;
        static const int MAY;
        static const int JUNE;
        static const int JULY;
        static const int AUGUST;
        static const int SEPTEMBER;
        static const int OCTOBER;
        static const int NOVEMBER;
        static const int DECEMBER;
    };
}
