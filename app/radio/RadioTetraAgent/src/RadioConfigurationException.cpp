#include "RadioConfigurationException.h"

namespace TA_IRS_App
{
    RadioConfigurationException::RadioConfigurationException(const std::string& errorText) throw()
                        : RadioException(errorText)
    {

    }

    RadioConfigurationException::~RadioConfigurationException() throw()
    {

    }

}
