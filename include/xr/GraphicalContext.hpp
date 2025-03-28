/*
** ETIB PROJECT, 2025
** maverik
** File description:
** GraphicalContext
*/

#pragma once

#include "xr/Openxr-include.hpp"
#include "AGraphicalContext.hpp"

namespace maverik {
    namespace xr {
        class GraphicalContext : public AGraphicalContext {
            public:
                GraphicalContext();
                ~GraphicalContext();

                std::vector<std::string> getInstanceExtensions() override;

            protected:
            private:
        };
    }
}
