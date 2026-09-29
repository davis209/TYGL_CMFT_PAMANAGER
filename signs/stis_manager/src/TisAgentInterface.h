/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/TisAgentInterface.h $
 * @author:  Ripple
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */
// CachedConfig.h: interface for the TisAgentInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TisAgentInterface_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_TisAgentInterface_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "app/signs/common_library/src/stis_protocol/STISClient.h"

using TA_IRS_App::STIS_PROTOCOL::INTERFACES::STISClient;

class TisAgentInterface
{
public:

    /**
     * getInstance
     *
     * Returns an instance of the class
     *
     * @return the pointer to this singleton instance
     *
     */
    static TisAgentInterface* getInstance();

    /**
     * removeInstance
     *
     * Removes the instance of the class (if already created) and cleanup the members.  Primarily
     * used upon program termination (e.g. from main()) so that Purify does not consider this class
     * and its members to be leaks.
     *
     */
    static void removeInstance();

    /**
     * getSTISClient
     *
     * @return a reference to the TTIS Interface
     */
    STISClient& getSTISClient();

private:

    TisAgentInterface();
    virtual ~TisAgentInterface();

    static TisAgentInterface* m_me;

    static TA_Base_Core::NonReEntrantThreadLockable m_lock;

    STISClient m_stisClient;
};

#endif
