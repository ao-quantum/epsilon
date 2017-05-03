#ifndef SHARED_GO_TO_PARAMETER_CONTROLLER_H
#define SHARED_GO_TO_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "float_parameter_controller.h"
#include "curve_view_cursor.h"
#include "interactive_curve_view_range.h"

namespace Shared {

class GoToParameterController : public FloatParameterController {
public:
  GoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol);
  int numberOfRows() override;
protected:
  constexpr static float k_maxDisplayableFloat = 1E8f;
  CurveViewCursor * m_cursor;
private:
  void buttonAction() override;
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override;
  View * loadView() override;
  void unloadView(View * view) override;
  char m_draftTextBuffer[MessageTableCellWithEditableText::k_bufferLength];
  MessageTableCellWithEditableText * m_abscisseCell;
  InteractiveCurveViewRange * m_graphRange;
  I18n::Message m_abscissaSymbol;
};

}

#endif
