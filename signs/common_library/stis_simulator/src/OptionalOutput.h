#pragma once
#include <iostream>
#include <boost/thread/executors/serial_executor.hpp>
#include <boost/thread/executors/basic_thread_pool.hpp>

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::optionaloutput::detail
{
    using namespace boost::executors;
    using Manipulator = std::ostream& (*)(std::ostream&);

    struct OptionalOutput
    {
        OptionalOutput(std::ostream& os = std::cout)
            : m_os(os)
        {
        }

        void disable()
        {
            enable(false);
        }

        void enable(bool b = true)
        {
            m_enabled = b;
        }

        template <class T>
        OptionalOutput& output(T&& x)
        {
            if (m_enabled)
            {
                m_executor.submit([=]
                {
                    if (m_enabled)
                    {
                        m_os << x;
                    }
                });
            }

            return *this;
        }

        OptionalOutput& output(Manipulator manip)
        {
            if (m_enabled)
            {
                m_executor.submit([=]
                {
                    if (m_enabled)
                    {
                        m_os << manip;
                    }
                });
            }

            return *this;
        }

        bool m_enabled = true;
        std::ostream& m_os;
        basic_thread_pool m_executor{1};
    };

    template <class T>
    inline OptionalOutput& operator<<(OptionalOutput& os, T&& x)
    {
        return os.output(std::forward<T>(x));
    }

    inline OptionalOutput& operator<<(OptionalOutput& os, Manipulator manip)
    {
        return os.output(manip);
    }

    static inline OptionalOutput s_cout;
}

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR
{
    using optionaloutput::detail::s_cout;
    using optionaloutput::detail::OptionalOutput;
}
