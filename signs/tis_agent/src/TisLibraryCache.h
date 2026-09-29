/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_agent/src/TisLibraryCache.h $
 *
 * Reads the train CLD/TPA library version files directly from the agent's
 * local filesystem (the agent runs on Linux where /u01/transactive/data/...
 * is mounted) and exposes thread-safe getters. This decouples the TIS agent
 * (and, through it, the TTIS / TrainBorne managers) from the legacy
 * train-library-gateway process for version queries.
 *
 * The cache is refreshed on startup (refresh()) and may be refreshed on
 * demand. Individual getters return cached values without I/O so they are
 * cheap to call from the CORBA servant.
 */

#pragma once

#include <memory>
#include <string>

namespace TA_IRS_App::tisagent::libcache::detail
{
    struct TisLibraryCache
    {
        static TisLibraryCache& instance();

        // Reads the CLD/TPA directories from disk and refreshes the cached
        // current/next version numbers. Safe to call repeatedly. Never throws.
        void refresh();

        // Cached CLD (train message library) versions.
        unsigned short current_cld_version() const;
        unsigned short next_cld_version() const;

        // Cached TPA (train PA library) versions.
        unsigned short current_tpa_version() const;
        unsigned short next_tpa_version() const;

        // Returns true once a successful refresh has completed and both
        // current and next CLD versions could be read.
        bool train_library_synchronisation_complete() const;

        // Raw XML blob of the current CLD library file, as read from disk.
        // Empty if no successful refresh has happened yet, or if the file
        // could not be read. Returned by value (string copy) so callers can
        // safely use it without holding the cache lock.
        std::string current_cld_blob() const;

        // Filename (basename, no directory) of the cached CLD blob.
        std::string current_cld_filename() const;

        struct Impl;
        std::shared_ptr<Impl> m_impl;

    private:
        TisLibraryCache();
    };
}

namespace TA_IRS_App
{
    using tisagent::libcache::detail::TisLibraryCache;
}
