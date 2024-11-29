//
// Created by peter on 26.11.24.
//

#ifndef STORM_BLOCK_H
#define STORM_BLOCK_H

#include <storage/BitVector.h>

#include <utility>

namespace storm {
    namespace storage {
        namespace bisimulation {
            namespace rework {
                template<typename DataType>
                class Block {
                public:
                    Block() : states() {}
                    explicit Block(storm::storage::BitVector states) : states(std::move(states)) {}

                    const storm::storage::BitVector& getStates() const { // Marked as const
                      return states;
                    }

                    void setStates(storm::storage::BitVector& newStates) {
                      states = newStates;
                    }

                    // Friend declaration for operator<<
                    friend std::ostream& operator<<(std::ostream& os, const Block<DataType>& block) {
                      os << "Block states: ";
                      for (size_t i = 0; i < block.states.size(); ++i) {
                        if (block.states.get(i)) {
                          os << i << " ";
                        }
                      }
                      return os;
                    }

                private:
                    storm::storage::BitVector states;
                };

                template class Block<double>;
            }
        }
    }
}

#endif //STORM_BLOCK_H
