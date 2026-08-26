/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IAtsTWPData.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * IAtsTWPData is an interface to an AtsTWPData object.
 *
 */

#if !defined(AFX_IATSTWPDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
#define AFX_IATSTWPDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

namespace TA_Base_Core
{
    class IAtsTWPData
    {
    public:

        virtual ~IAtsTWPData() {};

        /**
         *  getTWPDataPointEntityKey()
         *
         *  Returns the ISCS data point entity key which corresponds to given ATS TWP
         *
         *  @return     entity key of the ISCS data point
         *
         */
        virtual ta_uint32 getTWPDataPointEntityKey() = 0;

        /**
         *  getTWPDataPointType()
         *
         *  Returns the type of the TWP datapoint. Either "Input" or "Output"
         *
         *  @return     data point type as string
         *
         */
        virtual std::string getTWPDataPointType() = 0;

        /**
         *  getTWPCommand()
         *
         *  Returns the TWP command corresponding to the data point entity
         *
         *  @return     TWP command as string
         *
         */
        virtual std::string getTWPCommand() = 0;

        /**
         *  getKey()
         *
         *  Returns the primary key of the record holding this TWP Data
         *
         *  @return     entity key of the record
         *
         */
        virtual ta_uint32 getKey() = 0;
    };

} // TA_Base_Core

#endif // !defined(AFX_IATSTWPDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
