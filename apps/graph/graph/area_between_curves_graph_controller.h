#ifndef GRAPH_AREA_BETWEEN_CURVES_GRAPH_CONTROLLER_H
#define GRAPH_AREA_BETWEEN_CURVES_GRAPH_CONTROLLER_H

#include "integral_graph_controller.h"

namespace Graph {

class AreaBetweenCurvesGraphController : public IntegralGraphController {
public:
  AreaBetweenCurvesGraphController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, GraphView * graphView, Shared::InteractiveCurveViewRange * graphRange, Shared::CurveViewCursor * cursor) : IntegralGraphController(parentResponder, inputEventHandlerDelegate, graphView, graphRange, cursor) {}
  const char * title() override;
private:
  Poincare::Layout createFunctionLayout(Shared::ExpiringPointer<Shared::Function> function) override;
  Ion::Storage::Record m_secondRecord;
};

}

#endif
