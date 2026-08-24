
#ifndef RADIO_CONFIGURATION_EXCEPTION_H
#define RADIO_CONFIGURATION_EXCEPTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/radio/RadioTetraAgent/src/RadioException.h"

namespace TA_IRS_App
{
    class RadioConfigurationException : public RadioException
    {
        public:
            RadioConfigurationException(const std::string& errorText) throw();
            virtual ~RadioConfigurationException() throw();
    };
    
};


#endif // #ifndef RADIO_CONFIGURATION_EXCEPTION_H
