#include "Signature.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
namespace storage {
namespace bisimulation {

template<typename ValueType>
void Signature<ValueType>::addBlockProbability(size_t blockId, ValueType probability) {
    auto it = blockProbabilities.find(blockId);
    if (it != blockProbabilities.end()) {
      it->second += probability;
    } else {
      blockProbabilities.emplace(blockId, probability);
    }
}

template<typename ValueType>
size_t Signature<ValueType>::computeHash() const {
    std::size_t seed = 0;
    for (const auto& [blockId, prob] : blockProbabilities) {
      boost::hash_combine(seed, blockId);
      boost::hash_combine(seed, prob);
    }
    return seed;
}

template class Signature<double>;

#ifdef STORM_HAVE_CARL
template class Signature<storm::RationalNumber>;
template class Signature<storm::RationalFunction>;
#endif

}  // namespace bisimulation
}  // namespace storage
}  // namespace storm