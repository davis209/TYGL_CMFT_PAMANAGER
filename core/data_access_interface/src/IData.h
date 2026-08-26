/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IData.h $
 * @author Nick Jardine
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by: $Author: CM $
 *
 * IData is an interface to an object that provides data. The object represented by this
 * interface will contain code that is specific to a particular database system (e.g. oracle)
 * This object is responsible for interpreting the data return from the database and (if
 * necessary) converting it to the specified data types.
 */

#if !defined(IData_6F37C686_5A1A_4f0e_85DB_C36A508A0467__INCLUDED_)
#define IData_6F37C686_5A1A_4f0e_85DB_C36A508A0467__INCLUDED_

#include "core/types/src/ta_types.h"
#include <boost/iterator/iterator_facade.hpp>
#include <string>
#include <vector>
#include <memory>
#include <sys/timeb.h>

namespace TA_Base_Core
{
    class IData
    {
    public:

        /**
         * getNumRows
         *
         * This function returns the number of rows of data contained by this object.
         *
         * @returns the number of rows of data contained by this IData object in ta_uint32 format.
         */
        virtual size_t getNumRows() = 0;

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
        virtual int getIntegerData(const size_t rowIndex, const std::string& fieldName, const int defaultValue = 0) = 0;
		virtual ta_int64 getInt64Data(const size_t rowIndex, const std::string& fieldName, const ta_int64 defaultValue = 0) = 0;

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
        virtual size_t getUnsignedLongData(const size_t rowIndex, const std::string& fieldName, const size_t defaultValue = 0) = 0;
		virtual ta_uint64 getUnsignedLongLongData(const size_t rowIndex, const std::string& fieldName, const ta_uint64 defaultValue = 0) = 0;
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
        virtual double getRealData(const size_t rowIndex, const std::string& fieldName, const double defaultValue = 0.0) = 0;

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
        virtual bool getBooleanData(const size_t rowIndex, const std::string& fieldName, const bool defaultValue = false) = 0;

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
         * @exception DataException If the data cannot be converted to a string
         * a DataException is thrown.
         *
         * pre: rowIndex <= getNumRows()
         */
        virtual const std::string& getStringData(const size_t rowIndex, const std::string& fieldName) = 0;
        virtual std::string getStringData(const size_t rowIndex, const std::string& fieldName, const std::string& defaultValue) = 0;

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
         * pre: 1) To use this method the date MUST be selected from the database using
         *         the sql statement select format: TO_CHAR([column],'YYYYMMDDHH24MMSS')
         *         from [table];
         *         An assert is used to test that the string is in the YYYYMMDDHH24MMSS
         *         format.
         *         NB: The day/month compatibility will not be tested by this routine,
         *         and the mktime function will convert any invalid date to an equivalent
         *         valid one (e.g. 29/02/03 will become 01/03/03). Note however that dates
         *         greater than 31 will generate an exception.
         *      2) rowIndex <= getNumRows()
         */
        virtual time_t getDateData(const size_t rowIndex, const std::string& fieldName, const time_t defaultValue = 0) = 0;

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
        virtual timeb getTimestampData(const size_t rowIndex, const std::string& fieldName) = 0;

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
        virtual bool isNull(const size_t rowIndex, const std::string& fieldName) = 0;

        /**
         * getNullTime
         *
         * Return the seconds from 01/01/1990 12:00:00
         */
        virtual time_t getNullTime() = 0;

        virtual ~IData() {};

        struct Row
        {
            Row(IData& data, size_t index)
                : m_data(data),
                  m_index(index)
            {
            }

            virtual ~Row() {}

            virtual int getIntegerData(const std::string& fieldName, const int defaultValue = 0)
            {
                return m_data.getIntegerData(m_index, fieldName, defaultValue);
            }

            virtual size_t getUnsignedLongData(const std::string& fieldName, const size_t defaultValue = 0)
            {
                return m_data.getUnsignedLongData(m_index, fieldName, defaultValue);
            }

			virtual unsigned long long getUnsignedLongLongData(const std::string& fieldName, const size_t defaultValue = 0)
			{
				return m_data.getUnsignedLongLongData(m_index, fieldName, defaultValue);
			}

            virtual double getRealData(const std::string& fieldName, const double defaultValue = 0.0)
            {
                return m_data.getRealData(m_index, fieldName, defaultValue);
            }

            virtual bool getBooleanData(const std::string& fieldName, const bool defaultValue = false)
            {
                return m_data.getBooleanData(m_index, fieldName, defaultValue);
            }

            virtual const std::string& getStringData(const std::string& fieldName)
            {
                return m_data.getStringData(m_index, fieldName);
            }

            virtual std::string getStringData(const std::string& fieldName, const std::string& defaultValue)
            {
                return m_data.getStringData(m_index, fieldName, defaultValue);
            }

            virtual time_t getDateData(const std::string& fieldName, const time_t defaultValue = 0)
            {
                return m_data.getDateData(m_index, fieldName, defaultValue);
            }

            virtual timeb getTimestampData(const std::string& fieldName)
            {
                return m_data.getTimestampData(m_index, fieldName);
            }

            virtual bool isNull(const size_t rowIndex, const std::string& fieldName)
            {
                return m_data.isNull(m_index, fieldName);
            }

            virtual time_t getNullTime()
            {
                return m_data.getNullTime();
            }

            IData& m_data;
            size_t m_index;
        };

        struct Iterator : boost::iterator_facade<Iterator, Row, boost::random_access_traversal_tag, Row>
        {
            friend class boost::iterator_core_access;

            Iterator(IData& data, size_t index)
                : m_data(data),
                  m_index(index)
            {
            }

            reference dereference() const
            {
                return Row(m_data, m_index);
            }

            bool equal(const Iterator& rhs) const
            {
                return &m_data == &rhs.m_data && m_index == rhs.m_index;
            }

            void increment()
            {
                m_index++;
            }

            void decrement()
            {
                m_index--;
            }

            void advance(int n)
            {
                m_index += n;
            }

            int distance_to(const Iterator& rhs) const
            {
                return rhs.m_index - m_index;
            }

            size_t m_index;
            IData& m_data;
        };

        Iterator begin()
        {
            return Iterator(*this, 0);
        }

        Iterator end()
        {
            return Iterator(*this, getNumRows());
        }

        using iterator = Iterator;
    };

    using IDataPtr = std::shared_ptr<IData>;
    using IDataPtrList = std::vector<IDataPtr>;
}

#endif // !defined(IData_6F37C686_5A1A_4f0e_85DB_C36A508A0467__INCLUDED_)
