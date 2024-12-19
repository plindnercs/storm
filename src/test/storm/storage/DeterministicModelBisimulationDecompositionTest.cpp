#include <storage/bisimulation-rework/BisimulationQuotienting.h>
#include "storm-config.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/FormulaParser.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/storage/bisimulation/DeterministicModelBisimulationDecomposition.h"
#include "test/storm_gtest.h"

TEST(DeterministicModelBisimulationDecomposition, Die) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel =
        storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/die.tra", STORM_TEST_RESOURCES_DIR "/lab/die.lab", "", "");

    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim(*dtmc);
    ASSERT_NO_THROW(bisim.computeBisimulationDecomposition());
    std::shared_ptr<storm::models::sparse::Model<double>> result;
    ASSERT_NO_THROW(result = bisim.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(13ul, result->getNumberOfStates());
    EXPECT_EQ(20ul, result->getNumberOfTransitions());

#ifdef WINDOWS
    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
#else
    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
#endif
    options.respectedAtomicPropositions = std::set<std::string>({"one"});

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim2(*dtmc, options);
    ASSERT_NO_THROW(bisim2.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim2.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(5ul, result->getNumberOfStates());
    EXPECT_EQ(8ul, result->getNumberOfTransitions());

    options.setType(storm::storage::BisimulationType::Weak);

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim3(*dtmc, options);
    ASSERT_NO_THROW(bisim3.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim3.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(5ul, result->getNumberOfStates());
    EXPECT_EQ(8ul, result->getNumberOfTransitions());

    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"one\"]");

    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options2(*dtmc, *formula);

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim4(*dtmc, options2);
    ASSERT_NO_THROW(bisim4.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim4.getQuotient());
    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(5ul, result->getNumberOfStates());
    EXPECT_EQ(8ul, result->getNumberOfTransitions());
}

TEST(DeterministicModelBisimulationDecompositionBySignatureRefinement, Die) {
  std::shared_ptr<storm::models::sparse::Model<double>> abstractModel =
          storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/die.tra", STORM_TEST_RESOURCES_DIR "/lab/die.lab", "", "");

  ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
  std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();

  // set refinement type to SIGNATURE
  typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
  options.setRefinementAlgorithm(storm::storage::RefinementAlgorithm::SIGNATURE);

  storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim(*dtmc, options);
  ASSERT_NO_THROW(bisim.computeBisimulationDecomposition());
  std::shared_ptr<storm::models::sparse::Model<double>> result;
  ASSERT_NO_THROW(result = bisim.getQuotient());

  EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
  EXPECT_EQ(13ul, result->getNumberOfStates());
  EXPECT_EQ(20ul, result->getNumberOfTransitions());

  // compute quotient using partition refinement and compare results
  options.setRefinementAlgorithm(storm::storage::RefinementAlgorithm::PARTITION);

  storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisimPart(*dtmc, options);
  std::shared_ptr<storm::models::sparse::Model<double>> resultPart;
  ASSERT_NO_THROW(bisimPart.computeBisimulationDecomposition());
  ASSERT_NO_THROW(resultPart = bisimPart.getQuotient());

  EXPECT_EQ(storm::models::ModelType::Dtmc, resultPart->getType());
  EXPECT_EQ(13ul, resultPart->getNumberOfStates());
  EXPECT_EQ(20ul, resultPart->getNumberOfTransitions());

  EXPECT_EQ(true, result->getTransitionMatrix() == resultPart->getTransitionMatrix());
  EXPECT_EQ(true, result->getBackwardTransitions() == resultPart->getBackwardTransitions());
  EXPECT_EQ(true, result->getStateLabeling() == resultPart->getStateLabeling());
}

TEST(DeterministicModelBisimulationDecompositionByBothRefinementAlgorithms, Crowds) {
  std::shared_ptr<storm::models::sparse::Model<double>> abstractModel =
          storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/crowds5_5.tra",
                                                  STORM_TEST_RESOURCES_DIR "/lab/crowds5_5.lab", "", "");

  ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
  std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();

  // set refinement type to SIGNATURE
  typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
  options.setRefinementAlgorithm(storm::storage::RefinementAlgorithm::SIGNATURE);

  storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim(*dtmc, options);
  std::shared_ptr<storm::models::sparse::Model<double>> result;
  ASSERT_NO_THROW(bisim.computeBisimulationDecomposition());
  ASSERT_NO_THROW(result = bisim.getQuotient());

  EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
  EXPECT_EQ(334ul, result->getNumberOfStates());
  EXPECT_EQ(546ul, result->getNumberOfTransitions());

  // compute quotient using partition refinement and compare results
  options.setRefinementAlgorithm(storm::storage::RefinementAlgorithm::PARTITION);

  storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisimPart(*dtmc, options);
  std::shared_ptr<storm::models::sparse::Model<double>> resultPart;
  ASSERT_NO_THROW(bisimPart.computeBisimulationDecomposition());
  ASSERT_NO_THROW(resultPart = bisimPart.getQuotient());

  EXPECT_EQ(storm::models::ModelType::Dtmc, resultPart->getType());
  EXPECT_EQ(334ul, resultPart->getNumberOfStates());
  EXPECT_EQ(546ul, resultPart->getNumberOfTransitions());

  EXPECT_EQ(true, result->getTransitionMatrix() == resultPart->getTransitionMatrix());
  EXPECT_EQ(true, result->getBackwardTransitions() == resultPart->getBackwardTransitions());
  EXPECT_EQ(true, result->getStateLabeling() == resultPart->getStateLabeling());
}

TEST(DeterministicModelBisimulationDecompositionBySignatureRefinement, Leader4_8) {
  std::shared_ptr<storm::models::sparse::Model<double>> abstractModel =
          storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/leader4_8.tra",
                                                  STORM_TEST_RESOURCES_DIR "/lab/leader4_8.lab", "", "");

  ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
  std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();

  // set refinement type to SIGNATURE
  typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
  options.setRefinementAlgorithm(storm::storage::RefinementAlgorithm::SIGNATURE);

  storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim(*dtmc, options);
  std::shared_ptr<storm::models::sparse::Model<double>> result;
  ASSERT_NO_THROW(bisim.computeBisimulationDecomposition());
  ASSERT_NO_THROW(result = bisim.getQuotient());

  EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
  // std::cout << "Number of states: " << result->getNumberOfStates() << std::endl;
  // std::cout << "Number of transitions: " << result->getNumberOfTransitions() << std::endl;
  EXPECT_EQ(10ul, result->getNumberOfStates());
  EXPECT_EQ(11ul, result->getNumberOfTransitions());

  // compute quotient using partition refinement and compare results
  options.setRefinementAlgorithm(storm::storage::RefinementAlgorithm::PARTITION);

  storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisimPart(*dtmc, options);
  std::shared_ptr<storm::models::sparse::Model<double>> resultPart;
  ASSERT_NO_THROW(bisimPart.computeBisimulationDecomposition());
  ASSERT_NO_THROW(resultPart = bisimPart.getQuotient());

  EXPECT_EQ(storm::models::ModelType::Dtmc, resultPart->getType());
  EXPECT_EQ(10ul, resultPart->getNumberOfStates());
  EXPECT_EQ(11ul, resultPart->getNumberOfTransitions());

  EXPECT_EQ(true, result->getTransitionMatrix() == resultPart->getTransitionMatrix());
  EXPECT_EQ(true, result->getBackwardTransitions() == resultPart->getBackwardTransitions());
  EXPECT_EQ(true, result->getStateLabeling() == resultPart->getStateLabeling());
}

TEST(DeterministicModelBisimulationDecompositionByPartitionRefinement, Crowds) {
  std::shared_ptr<storm::models::sparse::Model<double>> abstractModel =
          storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/crowds5_5.tra",
                                                  STORM_TEST_RESOURCES_DIR "/lab/crowds5_5.lab", "", "");

  ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
  std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();

  // set refinement type to SIGNATURE
  typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
  options.setRefinementAlgorithm(storm::storage::RefinementAlgorithm::PARTITION);

  storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim(*dtmc, options);
  std::shared_ptr<storm::models::sparse::Model<double>> result;
  ASSERT_NO_THROW(bisim.computeBisimulationDecomposition());
  ASSERT_NO_THROW(result = bisim.getQuotient());

  EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
  EXPECT_EQ(334ul, result->getNumberOfStates());
  EXPECT_EQ(546ul, result->getNumberOfTransitions());
}

TEST(DeterministicModelBisimulationDecompositionBySignatureRefinement, SigRefTest) {
  std::shared_ptr<storm::models::sparse::Model<double>> abstractModel =
          storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/sigref_test.tra",
                                                  STORM_TEST_RESOURCES_DIR "/lab/sigref_test.lab", "", "");

  ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
  std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();

  // set refinement type to SIGNATURE
  typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
  options.setRefinementAlgorithm(storm::storage::RefinementAlgorithm::SIGNATURE);

  storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim(*dtmc, options);
  std::shared_ptr<storm::models::sparse::Model<double>> result;
  ASSERT_NO_THROW(bisim.computeBisimulationDecomposition());
  ASSERT_NO_THROW(result = bisim.getQuotient());

  EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
  EXPECT_EQ(3ul, result->getNumberOfStates());
  EXPECT_EQ(4ul, result->getNumberOfTransitions());

  // compute quotient using partition refinement and compare results
  options.setRefinementAlgorithm(storm::storage::RefinementAlgorithm::PARTITION);

  storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisimPart(*dtmc, options);
  std::shared_ptr<storm::models::sparse::Model<double>> resultPart;
  ASSERT_NO_THROW(bisimPart.computeBisimulationDecomposition());
  ASSERT_NO_THROW(resultPart = bisimPart.getQuotient());

  EXPECT_EQ(storm::models::ModelType::Dtmc, resultPart->getType());
  EXPECT_EQ(3ul, resultPart->getNumberOfStates());
  EXPECT_EQ(4ul, resultPart->getNumberOfTransitions());

  EXPECT_EQ(true, result->getTransitionMatrix() == resultPart->getTransitionMatrix());
  EXPECT_EQ(true, result->getBackwardTransitions() == resultPart->getBackwardTransitions());
  EXPECT_EQ(true, result->getStateLabeling() == resultPart->getStateLabeling());
}

TEST(BisimulationQuotienting, Die) {
// Parse the model
  std::shared_ptr<storm::models::sparse::Model<double>> abstractModel =
          // storm::parser::AutoParser<>::parseModel(
          //         STORM_TEST_RESOURCES_DIR "/tra/die.tra",
          //         STORM_TEST_RESOURCES_DIR "/lab/die.lab",
          //         "", "");

           storm::parser::AutoParser<>::parseModel(
                   STORM_TEST_RESOURCES_DIR "/tra/crowds5_5.tra",
                   STORM_TEST_RESOURCES_DIR "/lab/crowds5_5.lab",
                   "", "");

  // Ensure the parsed model is of the correct type (DTMC in this case)
  ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
  std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc =
          abstractModel->as<storm::models::sparse::Dtmc<double>>();

  // Instantiate the new BisimulationQuotienting class
  storm::storage::bisimulation::rework::BisimulationQuotienting<storm::models::sparse::Dtmc<double>> bisimulation(*dtmc);

  // Compute the bisimulation quotient
  ASSERT_NO_THROW(bisimulation.computeBisimulationQuotient());

  // Additional checks for correctness
  // For example, you could compare the number of states or transitions in the quotient model
  // std::shared_ptr<storm::models::sparse::Dtmc<double>> quotient = bisimulation.getQuotient();
  // ASSERT_EQ(quotient->getNumberOfStates(), 13ul); // Expected number of states in the quotient
  // ASSERT_EQ(quotient->getNumberOfTransitions(), 20ul); // Expected number of transitions in the quotient
}

TEST(DeterministicModelBisimulationDecomposition, Crowds) {
    std::shared_ptr<storm::models::sparse::Model<double>> abstractModel =
        storm::parser::AutoParser<>::parseModel(STORM_TEST_RESOURCES_DIR "/tra/crowds5_5.tra", STORM_TEST_RESOURCES_DIR "/lab/crowds5_5.lab", "", "");

    ASSERT_EQ(abstractModel->getType(), storm::models::ModelType::Dtmc);
    std::shared_ptr<storm::models::sparse::Dtmc<double>> dtmc = abstractModel->as<storm::models::sparse::Dtmc<double>>();

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim(*dtmc);
    std::shared_ptr<storm::models::sparse::Model<double>> result;
    ASSERT_NO_THROW(bisim.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(334ul, result->getNumberOfStates());
    EXPECT_EQ(546ul, result->getNumberOfTransitions());

#ifdef WINDOWS
    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
#else
    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options;
#endif
    options.respectedAtomicPropositions = std::set<std::string>({"observe0Greater1"});

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim2(*dtmc, options);
    ASSERT_NO_THROW(bisim2.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim2.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(65ul, result->getNumberOfStates());
    EXPECT_EQ(105ul, result->getNumberOfTransitions());

    options.setType(storm::storage::BisimulationType::Weak);

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim3(*dtmc, options);
    ASSERT_NO_THROW(bisim3.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim3.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(43ul, result->getNumberOfStates());
    EXPECT_EQ(83ul, result->getNumberOfTransitions());

    storm::parser::FormulaParser formulaParser;
    std::shared_ptr<storm::logic::Formula const> formula = formulaParser.parseSingleFormulaFromString("P=? [F \"observe0Greater1\"]");

    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options3(*dtmc, *formula);

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim5(*dtmc, options3);
    ASSERT_NO_THROW(bisim5.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim5.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(64ul, result->getNumberOfStates());
    EXPECT_EQ(104ul, result->getNumberOfTransitions());

    formula = formulaParser.parseSingleFormulaFromString("P=? [true U<=50 \"observe0Greater1\"] ");

    typename storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>>::Options options4(*dtmc, *formula);

    storm::storage::DeterministicModelBisimulationDecomposition<storm::models::sparse::Dtmc<double>> bisim6(*dtmc, options4);
    ASSERT_NO_THROW(bisim6.computeBisimulationDecomposition());
    ASSERT_NO_THROW(result = bisim6.getQuotient());

    EXPECT_EQ(storm::models::ModelType::Dtmc, result->getType());
    EXPECT_EQ(65ul, result->getNumberOfStates());
    EXPECT_EQ(105ul, result->getNumberOfTransitions());
}
