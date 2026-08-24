/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/CallForwardingDetails.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This class contains the C++ representation of the CORBA CallForwardDetails structure
 *
 */

#if !defined(CALL_FORWARDING_DETAILS_H)
#define CALL_FORWARDING_DETAILS_H

#include <string>

namespace TA_IRS_App
{	
    struct CallForwardingDetails
    {
	    enum EForwardingType
	    {
		    NeverForward,
		    ForwardNotReachable,
		    ForwardAlways
        } forwardAction;

        std::string destinationTSI;
		
		std::string getForwardingActionAsString() const
		{
			switch(forwardAction)
			{
				case NeverForward:
					return "NeverForward";
					break;
				case ForwardNotReachable:
					return "ForwardNotReachable";
					break;
				case ForwardAlways:
					return "ForwardAlways";
					break;
				default:
					return "";
			}
		}
    };
}

#endif // !defined(CALL_FORWARDING_DETAILS_H)
