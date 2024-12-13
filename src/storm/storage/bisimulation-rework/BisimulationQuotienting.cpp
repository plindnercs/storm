#include <models/sparse/Dtmc.h>
#include "BisimulationQuotienting.h"
#include "Block.h"
#include "Partition.h"

namespace storm {
    namespace storage {
        namespace bisimulation {
            namespace rework {
                template<typename ModelType>
                BisimulationQuotienting<ModelType>::BisimulationQuotienting(const ModelType &model) : model(model),
                        partition() {
                  // Create initial partition based on state labels
                  uint64_t numberOfStates = model.getNumberOfStates();

                  std::chrono::high_resolution_clock::time_point initialPartitionStart = std::chrono::high_resolution_clock::now();

                  // create initial block which contains all states
                  rework::Block<ValueType> blockOfAllStates(storm::storage::BitVector(model.getNumberOfStates(), true));
                  partition.addBlock(blockOfAllStates);

                  // Iterate over all labels in the model
                  for (const auto& label : model.getStateLabeling().getLabels()) {
                    if (label == "init") {
                      continue;
                    }
                    // Get the states associated with the current label
                    const storm::storage::BitVector& currentLabelStates = model.getStates(label);

                    if (currentLabelStates.empty()) {
                      continue;
                    }

                    // iterate over all existing blocks
                    auto endOfIterator = partition.getBlocksEnd();
                    for (size_t i = 0, initialBlockCount = partition.getBlockCount(); i < initialBlockCount; ++i) {
                      // if there is a state which is already in an existing block and also satisfies the current label,
                      // then we have to create a new block, as it satisfies both labels, while also removing it
                      // from the existing block
                      // std::cout << "Bitvector of current block: " << it->getStates() << std::endl;
                      // std::cout << "Bitvector of current label: " << currentLabelStates << std::endl;
                      storm::storage::BitVector overlappingStates = partition.getBlockByIndex(i).getStates() & currentLabelStates;

                      // std::cout << "Bitvector of overlapping states" << overlappingStates << std::endl;

                      if (!overlappingStates.empty()) {
                        // keep all original states and remove the overlapping ones by bitwise operation
                        storm::storage::BitVector newStatesForExistingBlock = partition.getBlockByIndex(i).getStates() & ~overlappingStates;
                        // std::cout << "New bitvector of current block: " << newStatesForExistingBlock << std::endl;
                        // std::cout << partition.getBlockByIndex(i).getStates() << std::endl;
                        if (newStatesForExistingBlock.empty()) {
                          // reuse current partition if it would end up empty
                          partition.getBlockByIndex(i).setStates(overlappingStates);
                        } else {
                          // set new states for current block and create new one
                          partition.getBlockByIndex(i).setStates(newStatesForExistingBlock);
                          rework::Block<ValueType> newBlock(overlappingStates);
                          // std::cout << "Size of bitvector of new block: " << newBlock.getStates().empty() << std::endl;
                          partition.addBlock(newBlock);
                        }
                      }
                    }

                    std::cout << "Created block for label: " << label << std::endl;
                  }

                  std::chrono::high_resolution_clock::duration initialPartitionTime = std::chrono::high_resolution_clock::now() - initialPartitionStart;
                  std::chrono::milliseconds initialPartitionTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(initialPartitionTime);

                  std::cout << "Initial partitioning required " << initialPartitionTimeInMilliseconds.count() << "ms" << std::endl;

                  // Output initial partitioning for debugging
                  std::cout << "Initial partitioning:" << std::endl;
                  partition.print();

                  // Debugging state labels
                  // for (uint64_t state = 0; state < numberOfStates; state++) {
                  //   std::cout << "Labels for state " << state << ": ";
                  //   auto labels = model.getStateLabeling().getLabelsOfState(state);
                  //   for (const auto& label : labels) {
                  //     std::cout << label << " ";
                  //   }
                  //   std::cout << std::endl;
                  // }
                }

                template<typename ModelType>
                void BisimulationQuotienting<ModelType>::computeBisimulationQuotient() {
                  // Implementation goes here
                }

                // Explicit instantiation for Dtmc
                template
                class BisimulationQuotienting<storm::models::sparse::Dtmc<double>>;
            }
        } // namespace bisimulation
    } // namespace storage
}
