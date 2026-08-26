/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IResource.h $
  * @author:  Julian Chevalley
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2023/10/16 18:31:43 $
  * Last modified by:  $Author: CM $
  *
  * IResource interface declaration. The IResource interface represents
  * any Access Controlled resource.
  *
  */


#if !defined(AFX_IRESOURCE_H__FD93CE24_225A_4A69_AC03_915D98770DAB__INCLUDED_)
#define AFX_IRESOURCE_H__FD93CE24_225A_4A69_AC03_915D98770DAB__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "core/types/src/ta_types.h"
#include <boost/shared_ptr.hpp>

namespace TA_Base_Core
{
    class IResource
    {
    public:
        virtual ~IResource() {};

        /**
          * getKey
          *
          * get the resource key
          *
          * @return ta_uint32
          *
          */
        virtual ta_uint32 getKey() = 0;

        /**
          * getSubsystem
          *
          * @return The subsystem associated with this Resource as an ta_uint32 .
          */
        virtual ta_uint32 getSubsystem() = 0;
    };

    typedef boost::shared_ptr<IResource> IResourcePtr;
};


#endif // !defined(AFX_IRESOUCE_H__FD93CE24_225A_4A69_AC03_915D98770DAB__INCLUDED_)
