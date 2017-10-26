#include <poincare/trigonometry.h>
#include <poincare/simplification_root.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/complex.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/rational.h>
#include <poincare/multiplication.h>
#include <poincare/subtraction.h>
#include <ion.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression * Trigonometry::immediateSimplifyDirectFunction(Expression * e, Context& context, Expression::AngleUnit angleUnit) {
  assert(e->type() == Expression::Type::Sine || e->type() == Expression::Type::Cosine || e->type() == Expression::Type::Tangent);
  Expression * lookup = Trigonometry::table(e->operand(0), e->type(), context, angleUnit);
  if (lookup != nullptr) {
    return e->replaceWith(lookup, true);
  }
  Expression::Type correspondingType = e->type() == Expression::Type::Cosine ? Expression::Type::ArcCosine : (e->type() == Expression::Type::Sine ? Expression::Type::ArcSine : Expression::Type::ArcTangent);
  if (e->operand(0)->type() == correspondingType) {
    float trigoOp = e->operand(0)->operand(0)->approximate<float>(context, angleUnit);
    if (e->type() == Expression::Type::Tangent || (trigoOp >= -1.0f && trigoOp <= 1.0f)) {
      return e->replaceWith(const_cast<Expression *>(e->operand(0)->operand(0)), true);
    }
  }
  if (e->operand(0)->sign() < 0) {
    Expression * op = const_cast<Expression *>(e->operand(0));
    Expression * newOp = op->turnIntoPositive(context, angleUnit);
    newOp->immediateSimplify(context, angleUnit);
    if (e->type() == Expression::Type::Cosine) {
      return e->immediateSimplify(context, angleUnit);
    } else {
      const Expression * multOperands[2] = {new Rational(Integer(-1)), e->clone()};
      Multiplication * m = new Multiplication(multOperands, 2, false);
      ((Expression *)m->operand(1))->immediateSimplify(context, angleUnit);
      return e->replaceWith(m, true)->immediateSimplify(context, angleUnit);
    }
  }
  if (e->operand(0)->type() == Expression::Type::Multiplication && e->operand(0)->operand(1)->type() == Expression::Type::Symbol && static_cast<const Symbol *>(e->operand(0)->operand(1))->name() == Ion::Charset::SmallPi && e->operand(0)->operand(0)->type() == Expression::Type::Rational) {
    Rational * r = static_cast<Rational *>((Expression *)e->operand(0)->operand(0));
    int unaryCoefficient = 1; // store 1 or -1
    // Replace argument in [0, Pi/2[
    if (r->denominator().isLowerThan(Integer::Addition(r->numerator(), r->numerator()))) {
      IntegerDivision div = Integer::Division(r->numerator(), r->denominator());
      if (r->denominator().isLowerThan(Integer::Addition(div.remainder, div.remainder))) {
        div.remainder = Integer::Subtraction(r->denominator(), div.remainder);
        if (e->type() == Expression::Type::Cosine || e->type() == Expression::Type::Tangent) {
          unaryCoefficient *= -1;
        }
      }
      Rational * newR = new Rational(div.remainder, r->denominator());
      const_cast<Expression *>(e->operand(0))->replaceOperand(r, newR, true);
      const_cast<Expression *>(e->operand(0))->immediateSimplify(context, angleUnit);
      if (Integer::Division(div.quotient, Integer(2)).remainder.isOne() && e->type() != Expression::Type::Tangent) {
        unaryCoefficient *= -1;
      }
      Expression * simplifiedCosine = e->immediateSimplify(context, angleUnit); // recursive
      const Expression * multOperands[2] = {new Rational(Integer(unaryCoefficient)), simplifiedCosine->clone()};
      Multiplication * m = new Multiplication(multOperands, 2, false);
      return simplifiedCosine->replaceWith(m, true)->immediateSimplify(context, angleUnit);
    }
    assert(r->sign() > 0);
    assert(!r->denominator().isLowerThan(Integer::Addition(r->numerator(), r->numerator())));
  }
  return e;
}

Expression * Trigonometry::immediateSimplifyInverseFunction(Expression * e, Context& context, Expression::AngleUnit angleUnit) {
  assert(e->type() == Expression::Type::ArcCosine || e->type() == Expression::Type::ArcSine || e->type() == Expression::Type::ArcTangent);
  if (e->type() != Expression::Type::ArcTangent) {
  float approxOp = e->operand(0)->approximate<float>(context, angleUnit);
  if (approxOp > 1.0f || approxOp < -1.0f) {
    return e->replaceWith(new Undefined(), true);
  }
  }
  Expression::Type correspondingType = e->type() == Expression::Type::ArcCosine ? Expression::Type::Cosine : (e->type() == Expression::Type::ArcSine ? Expression::Type::Sine : Expression::Type::Tangent);
  if (e->operand(0)->type() == correspondingType) {
    float trigoOp = e->operand(0)->operand(0)->approximate<float>(context, angleUnit);
    if ((e->type() == Expression::Type::ArcCosine && trigoOp >= 0.0f && trigoOp <= M_PI) ||
        (e->type() == Expression::Type::ArcSine && trigoOp >= -M_PI/2.0f && trigoOp <= M_PI/2.0f) ||
        (e->type() == Expression::Type::ArcTangent && trigoOp >= -M_PI/2.0f && trigoOp <= M_PI/2.0f)) {
      return e->replaceWith(const_cast<Expression *>(e->operand(0)->operand(0)), true);
    }
  }
  Expression * lookup = Trigonometry::table(e->operand(0), e->type(), context, angleUnit);
  if (lookup != nullptr) {
    return e->replaceWith(lookup, true);
  }
  // arccos(-x) = Pi-arcos(x), arcsin(-x) = -arcsin(x), arctan(-x)=-arctan(x)
  if (e->operand(0)->sign() < 0 || (e->operand(0)->type() == Expression::Type::Multiplication && e->operand(0)->operand(0)->type() == Expression::Type::Rational && static_cast<const Rational *>(e->operand(0)->operand(0))->isMinusOne())) {
    Expression * op = const_cast<Expression *>(e->operand(0));
    if (e->operand(0)->sign() < 0) {
      Expression * newOp = op->turnIntoPositive(context, angleUnit);
      newOp->immediateSimplify(context, angleUnit);
    } else {
      ((Multiplication *)op)->removeOperand(const_cast<Expression *>(op->operand(0)), true);
      op->immediateSimplify(context, angleUnit);
    }
    if (e->type() == Expression::Type::ArcCosine) {
      Expression * pi = angleUnit == Expression::AngleUnit::Radian ? static_cast<Expression *>(new Symbol(Ion::Charset::SmallPi)) : static_cast<Expression *>(new Rational(Integer(180)));
      const Expression * subOperands[2] = {pi, e->clone()};
      Subtraction * s = new Subtraction(subOperands, false);
      ((Expression *)s->operand(1))->immediateSimplify(context, angleUnit);
      return e->replaceWith(s, true)->immediateSimplify(context, angleUnit);
    } else {
      const Expression * multOperands[2] = {new Rational(Integer(-1)), e->clone()};
      Multiplication * m = new Multiplication(multOperands, 2, false);
      ((Expression *)m->operand(1))->immediateSimplify(context, angleUnit);
      return e->replaceWith(m, true)->immediateSimplify(context, angleUnit);
    }
  }

  return e;
}

static_assert('\x89' == Ion::Charset::SmallPi, "Unicode error");
constexpr const char * cheatTable[Trigonometry::k_numberOfEntries][4] =
{{"\x89*(-2)^(-1)",    "",                                   "-1",                                 "undef"},
 {"\x89*(-5)*12^(-1)", "",                                   "(-1)*6^(1/2)*4^(-1)-2^(1/2)*4^(-1)", "-(3^(1/2)+2)"},
 {"\x89*2*(-5)^(-1)",  "",                                   "-(5/8+5^(1/2)/8)^(1/2)",             "-(5+2*5^(1/2))^(1/2)"},
 {"\x89*(-3)*8^(-1)",  "",                                   "-(2+2^(1/2))^(1/2)*2^(-1)",          "-(2+2^(1/2))^(1/2)*(2-2^(1/2))^(-1/2)"},
 {"\x89*(-3)^(-1)",    "",                                   "-3^(1/2)*2^(-1)",                    "-3^(1/2)"},
 {"\x89*(-4)^(-1)",    "",                                   "(-1)*(2^(-1/2))",                    "-1"},
 {"\x89*(-5)^(-1)",    "",                                   "-(5/8-5^(1/2)/8)^(1/2)",             "-(5-2*5^(1/2))^(1/2)"},
 {"\x89*(-6)^(-1)",    "",                                   "-0.5",                               "-3^(-1/2)"},
 {"\x89*(-8)^(-1)",    "",                                   "(2-2^(1/2))^(1/2)*(-2)^(-1)",        "-(2-2^(1/2))^(1/2)*(2+2^(1/2))^(-1/2)"},
 {"\x89*(-12)^(-1)",   "",                                   "-6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",     "3^(1/2)-2"},
 {"0",                 "1",                                  "0",                                  "0"},
 {"\x89*12^(-1)",      "6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",      "6^(1/2)*4^(-1)+2^(1/2)*(-4)^(-1)",   "-(3^(1/2)-2)"},
 {"\x89*8^(-1)",       "(2+2^(1/2))^(1/2)*2^(-1)",           "(2-2^(1/2))^(1/2)*2^(-1)",           "(2-2^(1/2))^(1/2)*(2+2^(1/2))^(-1/2)"},
 {"\x89*6^(-1)",       "3^(1/2)*2^(-1)",                     "0.5",                                "3^(-1/2)"},
 {"\x89*5^(-1)",       "(5^(1/2)+1)*4^(-1)",                 "(5/8-5^(1/2)/8)^(1/2)",              "(5-2*5^(1/2))^(1/2)"},
 {"\x89*4^(-1)",       "2^(-1/2)",                            "2^(-1/2)",                          "1"},
 {"\x89*3^(-1)",       "0.5",                                "3^(1/2)*2^(-1)",                     "3^(1/2)"},
 {"\x89*3*8^(-1)",     "(2-2^(1/2))^(1/2)*2^(-1)",           "(2+2^(1/2))^(1/2)*2^(-1)",           "(2+2^(1/2))^(1/2)*(2-2^(1/2))^(-1/2)"},
 {"\x89*2*5^(-1)",     "(5^(1/2)-1)*4^(-1)",                 "(5/8+5^(1/2)/8)^(1/2)",              "(5+2*5^(1/2))^(1/2)"},
 {"\x89*5*12^(-1)",    "6^(1/2)*4^(-1)+2^(1/2)*(-4)^(-1)",   "6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",      "3^(1/2)+2"},
 {"\x89*2^(-1)",       "0",                                  "1",                                  "undef"},
 {"\x89*7*12^(-1)",    "-6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",     "",                                   ""},
 {"\x89*3*5^(-1)",     "(1-5^(1/2))*4^(-1)",                 "",                                   ""},
 {"\x89*5*8^(-1)",     "(2-2^(1/2))^(1/2)*(-2)^(-1)",        "",                                   ""},
 {"\x89*2*3^(-1)",     "-0.5",                               "",                                   ""},
 {"\x89*3*4^(-1)",     "(-1)*(2^(-1/2))",                    "",                                   ""},
 {"\x89*4*5^(-1)",     "(-5^(1/2)-1)*4^(-1)",                "",                                   ""},
 {"\x89*5*6^(-1)",     "-3^(1/2)*2^(-1)",                    "",                                   ""},
 {"\x89*7*8^(-1)",     "-(2+2^(1/2))^(1/2)*2^(-1)",          "",                                   ""},
 {"\x89*11*12^(-1)",   "(-1)*6^(1/2)*4^(-1)-2^(1/2)*4^(-1)", "",                                   ""},
 {"\x89",              "-1",                                 "0",                                  "0"}};

Expression * Trigonometry::table(const Expression * e, Expression::Type type, Context & context, Expression::AngleUnit angleUnit) {
  assert(type == Expression::Type::Sine || type == Expression::Type::Cosine || type == Expression::Type::Tangent || type == Expression::Type::ArcCosine || type == Expression::Type::ArcSine || type == Expression::Type::ArcTangent);
  int trigonometricFunctionIndex = type == Expression::Type::Cosine || type == Expression::Type::ArcCosine ? 1 : (type == Expression::Type::Sine || type == Expression::Type::ArcSine ? 2 : 3);
  int inputIndex = type == Expression::Type::ArcCosine || type == Expression::Type::ArcSine || type == Expression::Type::ArcTangent ? trigonometricFunctionIndex : 0;
  int outputIndex = type == Expression::Type::ArcCosine || type == Expression::Type::ArcSine || type == Expression::Type::ArcTangent ? 0 : trigonometricFunctionIndex;
  for (int i = 0; i < k_numberOfEntries; i++) {
    Expression * input = Expression::parse(cheatTable[i][inputIndex]);
    if (input == nullptr) {
      continue;
    }
    SimplificationRoot inputRoot(input);
    inputRoot.simplify(context, angleUnit); // input expression does not change, no root needed and we can use entry after
    if (inputRoot.operand(0)->compareTo(e) == 0) {
      Expression * output = Expression::parse(cheatTable[i][outputIndex]);
      if (output == nullptr) {
        return nullptr;
      }
      SimplificationRoot outputRoot(output);
      return (Expression *)(outputRoot.simplify(context, angleUnit))->operand(0);
    }
  }
  return nullptr;
}

}
