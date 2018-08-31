#include <poincare/matrix_trace.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <cmath>

namespace Poincare {

MatrixTraceNode * MatrixTraceNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<MatrixTraceNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

Expression MatrixTraceNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return MatrixTrace(this).shallowReduce(context, angleUnit, futureParent);
}

LayoutRef MatrixTraceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixTrace(this), floatDisplayMode, numberOfSignificantDigits, name());
}

int MatrixTraceNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

template<typename T>
Evaluation<T> MatrixTraceNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, angleUnit);
  Complex<T> result = Complex<T>(input.trace());
  return result;
}

Expression MatrixTrace::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefinedOrAllocationFailure()) {
    return e;
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix m = static_cast<Matrix&>(c);
    if (m.numberOfRows() != m.numberOfColumns()) {
      return Undefined();
    }
    int n = m.numberOfRows();
    Addition a = Addition();
    for (int i = 0; i < n; i++) {
      a.addChildAtIndexInPlace(m.childAtIndex(i+n*i), i, a.numberOfChildren());
    }
    return a.shallowReduce(context, angleUnit);
  }
  if (!c.recursivelyMatches(Expression::IsMatrix)) {
    return c;
  }
  return *this;
#else
  if (c.type() != ExpressionNode::Type::Matrix) {
    return c;
  }
  return *this;
#endif
}

}
