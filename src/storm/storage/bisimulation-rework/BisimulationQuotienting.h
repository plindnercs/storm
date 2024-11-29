#ifndef STORM_BISIMULATIONQUOTIENTING_H
#define STORM_BISIMULATIONQUOTIENTING_H

#include <storage/SparseMatrix.h>
#include <utility/ConstantsComparator.h>
#include <storage/bisimulation/Partition.h>
#include "BisimulationType.h"
#include "settings/SettingsManager.h"
#include "settings/modules/BisimulationSettings.h"
#include "Block.h"
#include "Partition.h"

namespace storm {
    namespace logic {
        class Formula;
    }

    namespace storage {

        namespace bisimulation {
            namespace rework {
                inline BisimulationType resolveBisimulationTypeChoice(BisimulationTypeChoice c) {
                  switch (c) {
                    case BisimulationTypeChoice::Strong:
                      return BisimulationType::Strong;
                    case BisimulationTypeChoice::Weak:
                      return BisimulationType::Weak;
                    case BisimulationTypeChoice::Interval:
                      return BisimulationType::Interval;
                    case BisimulationTypeChoice::Approximate:
                      return BisimulationType::Approximate;
                    case BisimulationTypeChoice::FromSettings:
                      if (storm::settings::getModule<storm::settings::modules::BisimulationSettings>().isWeakBisimulationSet()) {
                        return BisimulationType::Weak;
                      } else {
                        return BisimulationType::Strong;
                      }
                  }
                  return BisimulationType::Strong;
                }

                template<typename ModelType>
                class BisimulationQuotienting {
                public:
                    typedef typename ModelType::ValueType ValueType;

                    explicit BisimulationQuotienting(ModelType const& model);

                    void printInitialPartition() const {
                      std::cout << "Initial Partition:" << std::endl;
                      this->partition.print(); // Call the print method of Partition
                    }

                    void computeBisimulationQuotient();

                    std::shared_ptr<ModelType> getQuotient() const;
                protected:
                    // The model to decompose.
                    ModelType const& model;

                    // The backward transitions of the model.
                    storm::storage::SparseMatrix<ValueType> backwardTransitions;

                    // The current partition (used by partition refinement).
                    storm::storage::bisimulation::rework::Partition<ValueType> partition;

                    // A comparator used for comparing the distances of constants.
                    storm::utility::ConstantsComparator<ValueType> comparator;

                    // The quotient, if it was build. Otherwhise a null pointer.
                    std::shared_ptr<ModelType> quotient;
                private:
                    void refinePartition();
                };
            }
        }
    }
}

#endif //STORM_BISIMULATIONQUOTIENTING_H
