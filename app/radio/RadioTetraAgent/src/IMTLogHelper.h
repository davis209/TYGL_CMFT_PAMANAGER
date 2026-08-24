
//
//////////////////////////////////////////////////////////////////////

#if !defined(IMTLOGHELPER_INCLUDED)
#define IMTLOGHELPER_INCLUDED


#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"

#include "core/message/src/NameValuePair.h"
#include "core/message/types/MessageTypes.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/types/src/ta_types.h"

#include <map>


namespace TA_Base_Core 
{ 
    class AuditMessageSender;
	class MessageType;
}

#if defined(WIN32) && defined(_CONSOLE) && defined(_DEBUG)
    #define LOGOUT(x)                   \
    {                                   \
        std::cout << x << std::endl;    \
    }                                   
#else
    #define LOGOUT(x)
#endif

namespace TA_IRS_App
{
    class AbstractBroadcast;
}

class IMTLogHelper  
{

public:

    IMTLogHelper();
    ~IMTLogHelper();

    /**
     * getInstance
	 *
	 * Returns an instance of the class
     *
     * @return the pointer to this singleton instance
     *
     */
	static IMTLogHelper* getInstance();

    /**
     * removeInstance
	 *
	 * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
     * used upon program termination (e.g. from main()) so that Purify does not consider this class
     * and its members to be leaks.
     *
     */
    static void removeInstance();


    ////////////////////////////////////////////////////////
    //
    //            Miscellaneous Utilities
    //
    ////////////////////////////////////////////////////////

    void tokenise(const std::string& str,
                  std::vector<std::string>& tokens,
                  const std::string& delimiters = " ");

    // formats the time to the display format the same as the event viewer
    std::string formatTime(time_t time);

	// trainagent usage
	void log(std::string profileName, std::string transactionID, TA_IRS_Bus::CommonTypes::TrainIdType trainId, std::string funcGroup, std::string funcName, std::string stepName, std::string log, std::string result);
	void log(std::string profileName, std::string transactionID, std::string trainId, std::string funcGroup, std::string funcName, std::string stepName, std::string log, std::string result);

	// radiogateway usage
	void log(std::string resourceName, std::string resourceKey, std::string direction, ta_uint32 transactionID, std::string data);
	void log(std::string resourceName, std::string resourceKey, std::string direction, ta_uint32 transactionID, ta_uint32 partNo, std::string data);
	void logMerge(std::string resourceName, std::string resourceKey, ta_uint32 transactionID);
	
protected:


    static TA_Base_Core::AuditMessageSender*         m_auditSender;
    static TA_Base_Core::NonReEntrantThreadLockable  m_lock;
    static IMTLogHelper*                          m_me;

	ta_uint32 m_key;
	ta_uint32 m_agentName;
	ta_uint32 m_locationKey;
	   
	
};

#endif // !defined(IMTLOGHELPER_INCLUDED)

