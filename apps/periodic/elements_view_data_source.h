#ifndef PERIODIC_ELEMENTS_VIEW_DATA_SOURCE_H
#define PERIODIC_ELEMENTS_VIEW_DATA_SOURCE_H

#include "elements_data_base.h"
#include "elements_view_delegate.h"
#include "palette.h"

namespace Periodic {

class ElementsViewDataSource {
public:
  ElementsViewDataSource() : m_delegate(nullptr), m_field(&ElementsDataBase::GroupField), m_textFilter(nullptr), m_selectedElement(1) {}

  void setDelegate(ElementsViewDelegate * delegate) { m_delegate = delegate; }
  AtomicNumber selectedElement() const { return m_selectedElement; }
  void setSelectedElement(AtomicNumber z);
  const DataField * field() const { return m_field; }
  void setField(const DataField * field);
  void setTextFilter(const char * filter) { m_textFilter = filter; }

private:
  ElementsViewDelegate * m_delegate;
  const DataField * m_field;
  const char * m_textFilter;
  AtomicNumber m_selectedElement;
};

}

#endif
