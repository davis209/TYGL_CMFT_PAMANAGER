// RadioStatusSummary.h: interface for the RadioStatusSummary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RadioStatusSummary_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_RadioStatusSummary_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include <string>
#include <map>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "app/radio/RadioTetraAgent/src/RadioCorbaTypeConverter.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"


class RadioStatusSummary  
{

public:

    // Observable descriptors

    /**
     * getInstance
	 *
	 * Returns an instance of the class
     *
     * @return the pointer to this singleton instance
     *
     */
	static RadioStatusSummary* getInstance();

    /**
     * removeInstance
	 *
	 * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
     * used upon program termination (e.g. from main()) so that Purify does not consider this class
     * and its members to be leaks.
     *
     */
    static void removeInstance();

    void setSessionId(const std::string& sessionId);
	void setSDSStatus(bool isConnected);
	void setConnectionStatus(bool isConnected);
	void setRadioSessionStatus(std::string sessionId, std::string sessionName, bool isLoggedIn);

	//void getRadioStatusSummary();
	TA_Base_Bus::IRadioCorbaDef::RadioStatusSummary* getRadioStatusSummary();

private:

    RadioStatusSummary();
	virtual ~RadioStatusSummary();

   	static RadioStatusSummary* m_me;

    static TA_Base_Core::NonReEntrantThreadLockable m_lockForStatusUpdate;

	bool m_SDSStatus;
	bool m_connectionStatus;
    
	struct RadioSession
	{
		std::string sessionId;
		std::string sessionName;
		bool isLoggedIn;
	};

	std::vector<RadioSession> m_RadioSessionList;

};


#endif // !defined(AFX_RadioStatusSummary_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
