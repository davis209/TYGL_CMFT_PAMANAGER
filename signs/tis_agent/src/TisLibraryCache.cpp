/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_agent/src/TisLibraryCache.cpp $
 */

#include "pch.h"
#include "TisLibraryCache.h"

#include "bus/trains/TrainLibraryCommonLibrary/src/Types.h"
#include "bus/trains/TrainLibraryCommonLibrary/src/VersionFile.inl"

#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"

#include <boost/filesystem/path.hpp>
#include <atomic>
#include <mutex>
#include <string>

namespace TA_IRS_App::tisagent::libcache::detail
{
    using namespace TA_Base_Ex;
    using namespace TA_Base_Core;

    namespace
    {
        constexpr char DEFAULT_CLD_DIR[] = "/u01/transactive/data/trains/cct_files/cld";
        constexpr char DEFAULT_TPA_DIR[] = "/u01/transactive/data/trains/cct_files/tpa";
    }

    struct TisLibraryCache::Impl
    {
        std::atomic<unsigned short> current_cld{0};
        std::atomic<unsigned short> next_cld{0};
        std::atomic<unsigned short> current_tpa{0};
        std::atomic<unsigned short> next_tpa{0};
        std::atomic<bool> cld_loaded{false};
        std::mutex refresh_lock;

        // Cached current-CLD XML body and its filename. Guarded by blob_lock
        // (separate from refresh_lock so CORBA readers don't block while a
        // refresh is in progress).
        mutable std::mutex blob_lock;
        std::string cld_blob;
        std::string cld_filename;

        // Reads the current and next version numbers from a single library
        // directory. On any error logs and leaves the targets unchanged.
        template <class Parser, class Struct>
        void refresh_one(const std::string& key,
                         const char* default_dir,
                         std::atomic<unsigned short>& cur,
                         std::atomic<unsigned short>& nxt,
                         const char* label)
        {
            using namespace TA_IRS_Bus::TRAINS;
            using namespace TA_IRS_Bus::TRAINS::versionfile::detail;

            try
            {
                auto dir = RunParamsEx::get_or(key, std::string{default_dir});
                using Container = BasicCurrentNextFileContainer<LocalFileContainer<Parser>, Struct>;
                Container container{boost::filesystem::path{dir}};

                cur.store(static_cast<unsigned short>(container.current_version()));
                nxt.store(static_cast<unsigned short>(container.next_version()));

                LOG_INFO("TisLibraryCache: %s current=%u next=%u dir=%s",
                         label,
                         static_cast<unsigned>(cur.load()),
                         static_cast<unsigned>(nxt.load()),
                         dir.c_str());
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("TisLibraryCache: failed to read %s versions: %s", label, e.what());
            }
            catch (...)
            {
                LOG_ERROR("TisLibraryCache: failed to read %s versions: unknown exception", label);
            }
        }

        // Reads the current CLD library file content into the blob cache.
        // Logged and swallowed on failure so the agent always starts.
        void refresh_cld_blob()
        {
            using namespace TA_IRS_Bus::TRAINS;
            using namespace TA_IRS_Bus::TRAINS::versionfile::detail;

            try
            {
                auto dir = RunParamsEx::get_or("cld-dir", std::string{DEFAULT_CLD_DIR});
                using Container = BasicCurrentNextFileContainer<LocalFileContainer<CLDFileNameParser>, CLDFileNameStruct>;
                Container container{boost::filesystem::path{dir}};

                auto current = container.current_filename_struct();
                if (current.empty())
                {
                    LOG_ERROR("TisLibraryCache: no current CLD library file in %s", dir.c_str());
                    return;
                }

                auto filename = current.filename();
                auto content = container.get_content(filename);
                if (content.empty())
                {
                    LOG_ERROR("TisLibraryCache: CLD library file %s is empty", filename.c_str());
                    return;
                }

                {
                    std::lock_guard<std::mutex> g{blob_lock};
                    cld_filename = filename;
                    cld_blob.assign(content.begin(), content.end());
                }

                LOG_INFO("TisLibraryCache: cached CLD blob filename=%s size=%zu",
                         filename.c_str(),
                         content.size());
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("TisLibraryCache: failed to cache CLD blob: %s", e.what());
            }
            catch (...)
            {
                LOG_ERROR("TisLibraryCache: failed to cache CLD blob: unknown exception");
            }
        }
    };

    TisLibraryCache::TisLibraryCache()
        : m_impl(std::make_shared<Impl>())
    {
    }

    TisLibraryCache& TisLibraryCache::instance()
    {
        static TisLibraryCache s_instance;
        return s_instance;
    }

    void TisLibraryCache::refresh()
    {
        LOG_CALLSTACK("TisLibraryCache::refresh");
        FUNCTION_ENTRY("TisLibraryCache::refresh");

        using namespace TA_IRS_Bus::TRAINS::versionfile::detail;

        std::lock_guard<std::mutex> guard{m_impl->refresh_lock};

        m_impl->refresh_one<CLDFileNameParser, CLDFileNameStruct>(
            "cld-dir", DEFAULT_CLD_DIR,
            m_impl->current_cld, m_impl->next_cld, "CLD");

        m_impl->refresh_one<TPAFileNameParser, TPAFileNameStruct>(
            "tpa-dir", DEFAULT_TPA_DIR,
            m_impl->current_tpa, m_impl->next_tpa, "TPA");

        // Also cache the raw CLD XML body so the CORBA getLibrary() override
        // can serve it to managers without touching the filesystem again.
        m_impl->refresh_cld_blob();

        m_impl->cld_loaded.store(m_impl->current_cld.load() != 0 ||
                                 m_impl->next_cld.load() != 0);

        FUNCTION_EXIT;
    }

    unsigned short TisLibraryCache::current_cld_version() const { return m_impl->current_cld.load(); }
    unsigned short TisLibraryCache::next_cld_version() const    { return m_impl->next_cld.load(); }
    unsigned short TisLibraryCache::current_tpa_version() const { return m_impl->current_tpa.load(); }
    unsigned short TisLibraryCache::next_tpa_version() const    { return m_impl->next_tpa.load(); }

    bool TisLibraryCache::train_library_synchronisation_complete() const
    {
        return m_impl->cld_loaded.load();
    }

    std::string TisLibraryCache::current_cld_blob() const
    {
        std::lock_guard<std::mutex> g{m_impl->blob_lock};
        return m_impl->cld_blob;
    }

    std::string TisLibraryCache::current_cld_filename() const
    {
        std::lock_guard<std::mutex> g{m_impl->blob_lock};
        return m_impl->cld_filename;
    }
}
