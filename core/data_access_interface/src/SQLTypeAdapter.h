//////////////////////////////////////////////////////////////////////
///     @file       SQLTypeAdapter.h
///     @author     zhilin,ouyang
///     @date       2010-11-29 14:44:41
///
///     @brief      to converts many different data types to strings
///                 suitable for use in SQL queries. This class provides
///                 implicit conversion between many C++ types and
///                 SQL-formatted string representations of that data
///                 without losing important type information.
//////////////////////////////////////////////////////////////////////

#pragma once
#include "SQLCommonDef.h"
#include "core/database/src/CommonType.h"
#include <boost/lexical_cast.hpp>
#include <string>

NAMESPACE_BEGIN(TA_Base_Core)

class SQLTypeAdapter
{
public:

    /// \brief Default constructor; empty string
    SQLTypeAdapter();

    template <typename T>
    SQLTypeAdapter(const T& x)
        : m_nDataCategory(enumBeginDbType),
          m_bEmpty(false)
    {
        m_strParams[0] = boost::lexical_cast<std::string>(x);
    }

    /// \brief Create a string representation of a SQLStatement struct
    ///
    /// \see assign(const SQLStatement&) for details
    SQLTypeAdapter(const SQLStatement& rhs);

    /// \brief Return pointer to raw data buffer
    const char* data(int nIndex) const { return (enumBeginDbType == m_nDataCategory ? m_strParams[enumBeginDbType].c_str() : m_strParams[nIndex].c_str()); }

    /// \brief Returns the character at a given position within the
    /// string buffer.
    char at(size_t i, size_t uIndex) const;

    /// \brief Return number of bytes in data buffer
    size_t length(size_t nIndex) const { return (enumBeginDbType == m_nDataCategory ? m_strParams[enumBeginDbType].length() : m_strParams[nIndex].length()); }
    size_t size(size_t nIndex) const { return length(nIndex); } ///< alias for length()

    bool is_null(size_t nIndex) const { return m_strParams[nIndex].empty(); }
    bool empty() const { return m_bEmpty; }

    /// \brief Compare the internal buffer to the given string
    ///
    /// Works just like string::compare(const std::string&).
    int compare(const SQLTypeAdapter& other) const;
    int getDataCategory() const { return m_nDataCategory; }

    /// \brief clear the internal buffer to the given string
    ///
    /// Works just like string::clear(const std::string&).
    void clear();

    /// \brief series of add functions for add data for parts of
    ///  the SQL statement
    /// Works just like string::append
    void add(int nDynamicKey);
    void add(const std::string& strVal);
    void add(const SQLTypeAdapter& other);
    void add(const std::string& strOracle, const std::string& strMysql);
    void add(const std::string& strColunm, const std::string& strVal, int nColType, int nOperKey, bool bIsAQ = false);
    void add(const std::string& strColunm, const std::string& strVal, int nColType, const char* strOperator = "", bool bIsAQ = false);

    void add(const Row& vecVal, int nDynKey, bool bIsLast);
    void add(int nFuncKey, const std::string& strColunm, int nOperKey);
    void add(const std::string& strColunm, const std::string& strVal, int nDynaKey,
             int nFuncKey, const char* strOperator, bool bIsAQ = false);
    void add(const std::string& strColunm, const std::string& strVal, int nDynaKey,
             int nFuncKey, int nOperKey, bool bIsAQ = false);

    void add(const std::string& strColunm, int nDynaKey, int nFuncKey, bool bIsAQ = false);

    // TO_DATE('" + toTimeString + "','Mon DD HH24:MI:SS YYYY')
    void add(int nDynaKey, int nFuncKey, const std::string& strVal, bool bIsAQ);

    // add for PA dynamic SQL
    void add(SQLStatement& rSql, const std::string& strWhere);

    void addDateFunc(const std::string& strColunmName, int nDateFormatKey, bool bIsAQ = false);

private:

    /// \brief copy the internal buffer to the given arrary
    ///
    /// only used in the class.
    void _CopyData(const SQLTypeAdapter& other);
    void _AddData(const SQLTypeAdapter& other);

private:

    //std::string m_str;
    bool m_bEmpty;
    int  m_nDataCategory;
    std::string m_strParams[defSupportDBCount];      // contains 4 items including: common, oracle, mysql, sqlite
};

NAMESPACE_END(TA_Base_Core)
