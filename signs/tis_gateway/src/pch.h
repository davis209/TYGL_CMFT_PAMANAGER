#pragma once

#ifdef _MSC_VER
    #pragma warning(disable: 4018 4146 4244 4503 4786)
#endif

#if TA_PP_USE_PCH
    #include <boost/format.hpp>
    #include <boost/algorithm/string.hpp>
    #include <boost/range.hpp>
    #include <boost/assign.hpp>
    #include <boost/serialization/serialization.hpp>
    #include <boost/serialization/vector.hpp>
    #include <boost/serialization/throw_exception.hpp>
    #include <boost/serialization/map.hpp>
    #include <boost/serialization/string.hpp>
    #include <boost/archive/text_iarchive.hpp>
    #include <boost/archive/text_oarchive.hpp>
    #include <boost/scope_exit.hpp>
    #include <boost/thread.hpp>
    #include <string>
    #include <vector>
    #include <map>
    #include <queue>
    #include <sstream>
    #include <iostream>
    #include <iomanip>
    #include <thread>
    #include <future>
    #include <functional>
    #include <mutex>
    #include <algorithm>
    #include <type_traits>
    #include <memory>
    #include <chrono>
    #include <bitset>
    #include <condition_variable>
    #include <cstdint>
    #include <cstddef>
    #include <climits>
#endif
