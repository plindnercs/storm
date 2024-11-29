//
// Created by peter on 26.11.24.
//

#ifndef STORM_PARTITION_H
#define STORM_PARTITION_H

#include <iostream>
#include "Block.h"

namespace storm {
    namespace storage {
        namespace bisimulation {
            namespace rework {
                template<typename ValueType>
                class Partition {
                public:
                    // Add a block to the partition
                    void addBlock(const Block<ValueType>& block) {
                      blocks.push_back(block);

                      // Update state-to-block mapping
                      for (uint64_t state : block.getStates()) {
                        stateToBlock[state] = blocks.size() - 1;
                      }
                    }

                    // Retrieve the block a state belongs to
                    const Block<ValueType>& getBlockOfState(uint64_t state) const {
                      auto it = stateToBlock.find(state);
                      if (it != stateToBlock.end()) {
                        return blocks[it->second];
                      }
                      throw std::runtime_error("State not found in any block.");
                    }

                    // Refine the partition (e.g., split blocks)
                    void refineBlock(size_t blockIndex, const std::vector<Block<ValueType>>& newBlocks) {
                      // Replace the original block with the first new block
                      blocks[blockIndex] = newBlocks[0];

                      // Add additional blocks to the partition
                      for (size_t i = 1; i < newBlocks.size(); ++i) {
                        blocks.push_back(newBlocks[i]);
                      }

                      // Update state-to-block mapping
                      for (const auto& newBlock : newBlocks) {
                        for (uint64_t state : newBlock.getStates()) {
                          stateToBlock[state] = blocks.size() - 1;
                        }
                      }
                    }

                    // Print the partition
                    void print() const {
                      for (size_t i = 0; i < blocks.size(); ++i) {
                        std::cout << "Block " << i << ": " << blocks[i] << std::endl;
                      }
                    }

                    typename std::vector<Block<ValueType>>::iterator getFirstBlock() {
                      return blocks.begin();
                    }

                    typename std::vector<Block<ValueType>>::iterator getBlocksEnd() {
                      return blocks.end();
                    }

                    Block<ValueType>& getBlockByIndex(size_t index) {
                      return blocks.at(index); // Safe access with bounds checking
                    }

                    size_t getBlockCount() const {
                      return blocks.size();
                    }

                private:
                    std::vector<Block<ValueType>> blocks; // List of blocks
                    std::unordered_map<uint64_t, size_t> stateToBlock; // State-to-block mapping
                };
            }
        }
    }
}




#endif //STORM_PARTITION_H
