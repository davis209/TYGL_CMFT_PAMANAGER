/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IAtsPowerData.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * IAtsPowerData is an interface to an AtsPowerData object.
 * Its purpose is to give access to information about a given Ats Electrical Subsection.
 *
 */

#if !defined(AFX_IATSPOWERDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
#define AFX_IATSPOWERDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

namespace TA_Base_Core
{
    class IAtsPowerData
    {
    public:

        virtual ~IAtsPowerData() {};

        /**
         *  getElectricalSubsectionId()
         *
         *  Returns the id of the Electrical SubSection within the main line
         *
         *  @return     subsection id
         *
         */
        virtual ta_uint32 getElectricalSubSectionId() = 0;

        /**
         *  getTractionPowerDataPointEntityKey()
         *
         *  Returns the entity key of the ISCS datapoint which corresponds to the electrical
         *  subsection within the main line
         *
         *  @return     entity key
         *
         */
        virtual ta_uint32 getTractionPowerDataPointEntityKey() = 0;

        /**
         *  getLocationId()
         *
         *  Returns the location key of the location at which this Electrical Subsection exists
         *
         *  @return     key of the location
         *
         */
        virtual ta_uint32 getLocationId() = 0;

        /**
         *  getMSSZone()
         *
         *  Returns the MSS Zone within which this Electrical SubSection can be found.
         *
         *  @return     name of the MSS Zone
         *
         */
        virtual std::string getMSSZone() = 0;

        /**
         *  getKey()
         *
         *  Returns the primary key of the record holding this Ats Power Data
         *
         *  @return     entity key of the record
         *
         */
        virtual ta_uint32 getKey() = 0;
    };

} // TA_Base_Core

#endif // !defined(AFX_IATSPOWERDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
