#pragma once

namespace storm {
namespace storage {
    namespace bisimulation {
        enum class BisimulationType { Strong, Weak, Interval, Approximate };
        enum class BisimulationTypeChoice { Strong, Weak, Interval, Approximate, FromSettings };
    }
}  // namespace storage
}  // namespace storm
