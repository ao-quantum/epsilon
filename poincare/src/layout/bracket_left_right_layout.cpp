#include "bracket_left_right_layout.h"
#include <poincare/expression_layout_cursor.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}

namespace Poincare {

BracketLeftRightLayout::BracketLeftRightLayout() :
  ExpressionLayout(),
  m_operandHeight(36) //TODO
{
}

bool BracketLeftRightLayout::moveLeft(ExpressionLayoutCursor * cursor) {
  assert(cursor->pointedExpressionLayout() == this);
  // Case: Right.
  // Go Left.
  if (cursor->position() == ExpressionLayoutCursor::Position::Right) {
    cursor->setPosition(ExpressionLayoutCursor::Position::Left);
    return true;
  }
  assert(cursor->position() == ExpressionLayoutCursor::Position::Left);
  // Case: Left.
  // Ask the parent.
  if (m_parent) {
    return m_parent->moveLeft(cursor);
  }
  return false;
}

KDSize BracketLeftRightLayout::computeSize() {
  //TODO: compute the operandHeight according to the brothers
  return KDSize(k_externWidthMargin + k_lineThickness + k_bracketWidth + k_widthMargin, m_operandHeight);
}

KDPoint BracketLeftRightLayout::positionOfChild(ExpressionLayout * child) {
  assert(false);
  return KDPointZero;
}

}
