/*
** ETIB PROJECT, 2025
** maverik
** File description:
** ASoftware
*/

#pragma once

#include"maverik.hpp"
#include <iostream>
#include "AGraphicalContext.hpp"

#include "Version.hpp"

namespace maverik {
    /**
     * @brief Abstract base class representing a software application within the maverik namespace.
     *
     * This class provides a common interface for software applications, including access to
     * graphical context and versioning information for both the application and its engine.
     */
    class ASoftware {
        public:
            virtual ~ASoftware() = default;

            /**
             * @brief Retrieves the graphical context associated with the software application.
             *
             * This function returns a shared pointer to an AGraphicalContext instance,
             * which represents the graphical context used by the application.
             *
             * @return A shared pointer to the AGraphicalContext instance.
             */
            std::shared_ptr<AGraphicalContext> getGraphicalContext();

        protected:
            std::string _appName;       // Application name (defined by user)
            Version *_appVersion;       // Application version (major, minor, patch)
            std::string _engineName;    // Engine name (defined by us WIP)
            Version *_engineVersion;    // Engine version   (major, minor, patch)

            std::shared_ptr<AGraphicalContext> _graphicalContext;   // Graphical context
    };
}
