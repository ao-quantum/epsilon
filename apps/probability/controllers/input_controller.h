#ifndef APPS_PROBABILITY_CONTROLLERS_INPUT_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_INPUT_CONTROLLER_H

#include <apps/i18n.h>
#include <apps/shared/button_with_separator.h>
#include <apps/shared/cell_with_separator.h>
#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/text_field_delegate.h>

#include "probability/abstract/button_delegate.h"
#include "probability/gui/layout_cell_with_editable_text_with_message.h"
#include "probability/gui/message_table_cell_with_separator.h"
#include "probability/gui/page_controller.h"
#include "probability/models/data.h"
#include "probability/models/statistic/statistic.h"
#include "probability/text_helpers.h"
#include "results_controller.h"

namespace Probability {

class InputController : public FloatParameterPage {
public:
  InputController(Escher::StackViewController * parent,
                  ResultsController * resultsController,
                  Statistic * statistic,
                  Escher::InputEventHandlerDelegate * handler);
  int numberOfRows() const override { return m_statistic->numberOfParameters() + 1 /* button */; }
  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override;
  int typeAtIndex(int i) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  void buttonAction() override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  void openPage(ViewController * nextPage,
                KDColor backgroundColor = s_titleColor,
                KDColor separatorColor = s_titleColor,
                KDColor textColor = KDColorWhite) {
    FloatParameterPage::openPage(nextPage, backgroundColor, separatorColor, textColor);
  }

protected:
  float parameterAtIndex(int i) override { return m_statistic->paramAtIndex(i); }
  bool isCellEditing(Escher::HighlightCell * cell, int index) override;
  void setTextInCell(Escher::HighlightCell * cell, const char * text, int index) override;

private:
  static KDColor s_titleColor;

  int reusableParameterCellCount(int type) override { return k_numberOfReusableCells; }
  Escher::HighlightCell * reusableParameterCell(int index, int type) override;
  bool setParameterAtIndex(int parameterIndex, float f) override;

  constexpr static int k_titleBufferSize = sizeof("H0:= Ha: α=") + 7 /* μ1-μ2 */ +
                                           3 * Constants::k_shortFloatNumberOfChars + 2 /* op */;
  char m_titleBuffer[k_titleBufferSize];
  Statistic * m_statistic;
  ResultsController * m_resultsController;

  constexpr static int k_numberOfReusableCells =
      Ion::Display::Height / Escher::TableCell::k_minimalLargeFontCellHeight + 1;
  constexpr static int k_significanceCellType = 2;

  LayoutCellWithEditableTextWithMessage m_parameterCells[k_numberOfReusableCells];
  MessageTableCellWithSeparator m_significanceCell;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_INPUT_CONTROLLER_H */
