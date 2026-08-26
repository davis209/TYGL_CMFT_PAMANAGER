/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/Resource.h $
  * @author:  Julian Chevalley
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2023/10/16 18:31:43 $
  * Last modified by:  $Author: CM $
  *
  * Resource implements the IResource interface
  *
  */


#if !defined(AFX_RESOURCE_H__9000ADE0_D501_4E0F_A17F_4BC04F27CB23__INCLUDED_)
#define AFX_RESOURCE_H__9000ADE0_D501_4E0F_A17F_4BC04F27CB23__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceHelper.h"

namespace TA_Base_Core
{
    class Resource  : public IResource
    {
    public:
        /**
         * Constructor
         *
         * Construct a complete Resource
         *
         * @param key The key of this Resource
         */
        Resource(const ta_uint32 key);

        /**
         * Constructor
         *
         * Construct a Resource class based around dataset
         */
        Resource(const ta_uint32 row, TA_Base_Core::IData& data);

        /**
         * Destructor
         */
        virtual ~Resource();

        /**
         * getKey
         *
         * Returns the key for this Resource.
         *
         * @return The key for this Resource as an ta_uint32 .
         */
        virtual ta_uint32 getKey();

        /**
         *  getSubsystem
         *
         *  Returns the subsystem associated with this Resource.
         *
         *  @return The subsystem for this Resource as an ta_uint32 .
         */
        virtual ta_uint32 getSubsystem();

    private:

        // Assignment operator not used so it is made private
        Resource& operator=(const Resource&);
        Resource(const Resource&);

        ResourceHelper* m_helper;
    };
};

#endif // !defined(AFX_RESOURCE_H__9000ADE0_D501_4E0F_A17F_4BC04F27CB23__INCLUDED_)
