/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/StdAfx.h $
 * @author:  Ripple
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 */
 // stdafx.h : include file for standard system include files,
 //  or project specific include files that are used frequently, but
 //      are changed infrequently
 //

#if !defined(AFX_STDAFX_H__A15ADE1E_653C_4E3F_963F_673F6787D0D6__INCLUDED_)
#define AFX_STDAFX_H__A15ADE1E_653C_4E3F_963F_673F6787D0D6__INCLUDED_

//#define WINVER 0x0500

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4290 )
#pragma warning ( disable : 4503 )
#pragma warning ( disable : 4018 )

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#include <afxpriv.h>

#include <boost/format.hpp>
#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>
#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include <boost/type_index.hpp>
#include <boost/variant.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/hof.hpp>
#include <boost/hof/result.hpp>
#include <boost/predef.h>
#include <boost/algorithm/string.hpp>
#include <boost/range.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/thread/future.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/algorithm/cxx11/one_of.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/thread/futures/wait_for_all.hpp>
#include <boost/thread/futures/wait_for_any.hpp>
#include <boost/thread/future.hpp>
#include <boost/thread/executors/basic_thread_pool.hpp>
#include <boost/thread/executors/thread_executor.hpp>
#include <boost/thread/executors/inline_executor.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/core/demangle.hpp>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/operators.hpp>
#include <boost/scope_exit.hpp>

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <thread>
#include <future>
#include <functional>
#include <mutex>
#include <algorithm>
#include <type_traits>
#include <memory>
#include <chrono>
#include <bitset>
#include <condition_variable>
#include <cstdint>
#include <cstddef>
#include <climits>

#endif // !defined(AFX_STDAFX_H__A15ADE1E_653C_4E3F_963F_673F6787D0D6__INCLUDED_)
