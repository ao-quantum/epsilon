#include <quiz.h>
#include <poincare/test/helper.h>
#include <float.h>
#include "../finance/data.h"

using namespace Solver;

constexpr double k_precision = 1e-3;
constexpr double k_reference = FLT_EPSILON;

void assert_simple_interest_solves(const double expectedValues[SimpleInterestData::k_numberOfDoubleValues], bool yearConvention, SimpleInterestData * data, double reference = k_reference) {
  // Set all parameters
  data->m_yearConventionIs360 = yearConvention;
  for (uint8_t paramIndex = 0; paramIndex < SimpleInterestData::k_numberOfDoubleValues; paramIndex++) {
    data->setValue(static_cast<SimpleInterestParameter>(paramIndex), expectedValues[paramIndex]);
  }
  // Test each parameters
  for (uint8_t paramIndex = 0; paramIndex < SimpleInterestData::k_numberOfUnknowns; paramIndex++) {
    SimpleInterestParameter previousUnknownParam = data->getUnknown();
    // Set new unknown parameter, previous unknown parameter's value is resetted
    data->setUnknown(static_cast<SimpleInterestParameter>(paramIndex));
    // Restore previous unknown parameter's value
    uint8_t previousUnknownParamIndex = static_cast<uint8_t>(previousUnknownParam);
    if (previousUnknownParamIndex < SimpleInterestData::k_numberOfDoubleValues && previousUnknownParamIndex != paramIndex) {
      data->setValue(previousUnknownParam, expectedValues[previousUnknownParamIndex]);
    }
    // Compute and check unknown parameter's value
    double expectedValue = expectedValues[paramIndex];

    quiz_assert(IsApproximatelyEqual(data->computeUnknownValue(), expectedValue, k_precision, k_reference));
  }
  data->resetValues();
}

void assert_compound_interest_solves(const double expectedValues[CompoundInterestData::k_numberOfDoubleValues], bool payment, CompoundInterestData * data, double reference = k_reference) {
  // Set all parameters
  data->m_paymentIsBeginning = payment;
  for (uint8_t paramIndex = 0; paramIndex < CompoundInterestData::k_numberOfDoubleValues; paramIndex++) {
    data->setValue(static_cast<CompoundInterestParameter>(paramIndex), expectedValues[paramIndex]);
  }
  // Test each parameters
  for (uint8_t paramIndex = 0; paramIndex < CompoundInterestData::k_numberOfUnknowns; paramIndex++) {
    CompoundInterestParameter previousUnknownParam = data->getUnknown();
    // Set new unknown parameter, previous unknown parameter's value is resetted
    data->setUnknown(static_cast<CompoundInterestParameter>(paramIndex));
    // Restore previous unknown parameter's value
    uint8_t previousUnknownParamIndex = static_cast<uint8_t>(previousUnknownParam);
    if (previousUnknownParamIndex < CompoundInterestData::k_numberOfDoubleValues && previousUnknownParamIndex != paramIndex) {
      data->setValue(previousUnknownParam, expectedValues[previousUnknownParamIndex]);
    }
    // Compute and check unknown parameter's value
    double expectedValue = expectedValues[paramIndex];

    quiz_assert(IsApproximatelyEqual(data->computeUnknownValue(), expectedValue, k_precision, reference));
  }
  data->resetValues();
}


QUIZ_CASE(equation_finance_simple_interest) {
  // n, rPct, P, I
  SimpleInterestData data;
  {
    const double values[SimpleInterestData::k_numberOfDoubleValues] = {
      100.0, 1.0, -1000.0, 2.777777778};
    assert_simple_interest_solves(values, true, &data);
  }
  {
    const double values[SimpleInterestData::k_numberOfDoubleValues] = {
      365.0, 10.0, 1000.0, -100.0};
    assert_simple_interest_solves(values, false, &data);
  }
}

QUIZ_CASE(equation_finance_compound_interest) {
  // N, rPct, PV, Pmt, FV, PY, CY
  CompoundInterestData data;
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      72.0, 12.55741064, 12600.0, -250.0, 0.0, 12.0, 12.0};
    assert_compound_interest_solves(values, false, &data, 10.0);
  }
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      8.0, 6.4, -37000.0, 0.0, 42009.9, 4.0, 4.0};
    assert_compound_interest_solves(values, false, &data, 10.0);
  }
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      22.0, 0.0, -24000.0, 1000.0, 2000.0, 4.0, 4.0};
    assert_compound_interest_solves(values, false, &data, 0.1);
  }
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      18.9692, 6.4, -37000.0, 0.0, 50000.0, 4.0, 4.0};
    assert_compound_interest_solves(values, false, &data, 1.0);
  }
}
