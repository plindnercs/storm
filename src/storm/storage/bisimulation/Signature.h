//
// Created by peter on 07.12.24.
//

#ifndef STORM_SIGNATURE_H
#define STORM_SIGNATURE_H

namespace storm {
namespace storage {
namespace bisimulation {

    template<typename DataType>
    class Signature {
    public:
        std::vector<std::pair<std::size_t, DataType>> blockProbabilities; // Block ID and aggregated probability

        // Default constructor
        Signature() = default;

        // Equality operator
        bool operator==(const Signature &other) const {
          return blockProbabilities == other.blockProbabilities;
        }

        // Less-than operator for ordering (optional, for sorting or map keys)
        bool operator<(const Signature &other) const {
          return blockProbabilities < other.blockProbabilities;
        }

        // Sort blockProbabilities for deterministic comparison
        void normalize() {
          std::sort(blockProbabilities.begin(), blockProbabilities.end());
        }

        void addBlockProbability(size_t blockId, DataType probability) {
          blockProbabilities.emplace_back(std::make_pair(blockId, probability));
        }
    };

}
}
}


#endif //STORM_SIGNATURE_H
